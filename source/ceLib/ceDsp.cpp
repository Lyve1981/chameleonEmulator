#include "ceDsp.h"

#include <types.h>

#include "ceRtems.h"
#include "cePlugin.h"

#include <chrono>

#include "../dsp56300/source/dsp56kEmu/unittests.h"

namespace ceLib
{
	constexpr size_t g_memorySize		= 0x800000;
	constexpr uint8_t g_cmdMemoryP		= 0x00;
	constexpr uint8_t g_cmdMemoryX		= 0x01;
	constexpr uint8_t g_cmdMemoryY		= 0x02;
	constexpr uint8_t g_cmEnd			= 0x03;

	constexpr uint32_t g_dspId			= 1;

	constexpr size_t g_cacheLineSize	= 128;

	constexpr size_t alignedSize(size_t _size)
	{
		return (_size + g_cacheLineSize - 1) / g_cacheLineSize * g_cacheLineSize;
	}

	template<typename T>
	T* alignedAddress(T* _ptr)
	{
		static_assert(sizeof(size_t) == sizeof(T*), "pointer size invalid");
		const auto addr = reinterpret_cast<size_t>(_ptr);
		return reinterpret_cast<T*>((addr + g_cacheLineSize - 1) / g_cacheLineSize * g_cacheLineSize);
	}

	template<typename T> constexpr size_t alignedSize()
	{
		return alignedSize(sizeof(T));
	}
	
	constexpr size_t g_requiredMemSize	= alignedSize<dsp56k::DSP>() + alignedSize<dsp56k::Memory>() + g_memorySize * dsp56k::MemArea_COUNT * sizeof(uint32_t);

	Dsp::Dsp() : m_dsp(nullptr), m_memory(nullptr)
	{
		dsp56k::UnitTests tests;
		m_buffer.resize(alignedSize(g_requiredMemSize));
	}
	Dsp::~Dsp()
	{
		destroyThread();
		destroyDSP();
	}

	int Dsp::create(int _dspIndex, const uint8_t* _code)
	{
		if(_dspIndex != g_dspId)
			return 0;

		if(m_dsp)
			return 0;

		if(!createDSP())
			return 0;
		
		if(!loadCode(_code))
		{
			destroyDSP();
			return 0;			
		}

		m_dsp->setPC(0x100);

		m_runnerThread.reset(new dsp56k::DSPThread(*m_dsp));

		return _dspIndex;
	}

	void Dsp::destroyThread()
	{
		if(m_runnerThread)
			m_runnerThread.reset();
	}

	bool Dsp::destroy(int _ref)
	{
		if(_ref != g_dspId)
			return false;

		destroyThread();

		Guard g(m_lock);
		return destroyDSP();
	}

	bool Dsp::writeData(int _ref, const int32_t* _data, size_t _count)
	{
		if(_ref != g_dspId)
			return false;

		m_peripherals->getHI08().write(_data, _count);
		return true;
	}

	void Dsp::process(float** _inputs, float** _outputs, size_t _sampleFrames)
	{
		if(m_runnerThread)
			m_peripherals->getEssi().processAudioInterleavedTX0(_inputs, _outputs, _sampleFrames);
	}

	bool Dsp::createDSP()
	{
		m_peripherals.reset(new DspPeripherals());

		auto* buf = &m_buffer[0];
		buf = alignedAddress(buf);

		auto* bufDSP = buf;
		auto* bufMem = bufDSP + alignedSize<dsp56k::DSP>();
		auto* bufBuf = bufMem + alignedSize<dsp56k::Memory>();
		
		m_memory = new (bufMem)dsp56k::Memory(*this, g_memorySize, reinterpret_cast<dsp56k::TWord*>(bufBuf));
		m_dsp = new (buf)dsp56k::DSP(*m_memory, m_peripherals.get(), m_peripherals.get());

		m_memory->setExternalMemory(0x400000, true);

		m_peripherals->initialize();

		return true;
	}

	bool Dsp::destroyDSP()
	{
		if(!m_dsp)
			return false;

		m_dsp->~DSP();
		m_memory->~Memory();

		m_dsp = nullptr;
		m_memory = nullptr;

		m_peripherals.reset();
		
		return true;
	}

	bool Dsp::loadCode(const uint8_t* _code)
	{
		auto readWord = [&]()
		{
			uint32_t w = _code[0] << 16;
			w |= _code[1] << 8;
			w |= _code[2];
			_code += 3;
			return w;
		};

		auto readMemory = [&](dsp56k::EMemArea _area)
		{
			const auto _address = readWord();
			const auto _size = readWord();

			for(size_t i=0; i<_size; ++i)
			{
				const dsp56k::TWord value = readWord();
				m_memory->set(_area, _address + i, value);
			}
		};
		
		while(true)
		{
			const auto type = *_code++;

			switch (type)
			{
			case g_cmdMemoryP:	readMemory(dsp56k::MemArea_P);	break;
			case g_cmdMemoryX:	readMemory(dsp56k::MemArea_X);	break;
			case g_cmdMemoryY:	readMemory(dsp56k::MemArea_Y);	break;
			case g_cmEnd:		return true;
			default:			LOG("Invalid command " << static_cast<int>(type));		return false;
			}
		}
	}

	bool Dsp::memValidateAccess(dsp56k::EMemArea _area, dsp56k::TWord _addr, bool _write) const
	{
		return true;
	}
}

extern "C"
{
	int dsp_init(int dsp_index, const rtems_unsigned8 *code)
	{
		auto& plug = ceLib::Rtems::findInstance();
		ceLib::Plugin::Guard g(plug.getMutex());
		return plug.getDsp().create(dsp_index, code);
	}
	rtems_boolean dsp_exit(int ref)
	{
		auto& plug = ceLib::Rtems::findInstance();
		ceLib::Plugin::Guard g(plug.getMutex());
		return plug.getDsp().destroy(ref);
	}

	rtems_boolean dsp_write_data(int ref, const rtems_signed32 *data, rtems_unsigned32 count)
	{
		auto& plug = ceLib::Rtems::findInstance();
		ceLib::Plugin::Guard g(plug.getMutex());
		return plug.getDsp().writeData(ref, data, count);
	}
}
