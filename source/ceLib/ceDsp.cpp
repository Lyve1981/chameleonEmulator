#include "ceDsp.h"

#include <types.h>

#include "ceRtems.h"
#include "cePlugin.h"

#include <chrono>

namespace ceLib
{
	constexpr size_t g_memorySize = 0x800000;

	constexpr uint8_t g_cmdMemoryP	= 0x00;
	constexpr uint8_t g_cmdMemoryX	= 0x01;
	constexpr uint8_t g_cmdMemoryY	= 0x02;
	constexpr uint8_t g_cmEnd		= 0x03;

	constexpr uint32_t g_dspId		= 1;

	Dsp::Dsp() : m_runThread(false)
	{
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

		Guard g(m_lock);

		if(m_dsp)
			return 0;

		if(!createDSP())
			return 0;
		
		if(!loadCode(_code))
		{
			destroyDSP();
			return 0;			
		}

		m_runThread = true;
		m_runnerThread.reset(new std::thread([this]
		{
			threadFunc();
		}));

		return _dspIndex;
	}

	void Dsp::destroyThread()
	{
		if(m_runnerThread)
		{
			m_runThread = false;

			Guard g(m_lock);
			m_runnerThread->join();
		}
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
		Guard g(m_lock);
		m_peripherals->writeHI8Data(_data, _count);
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
		m_memory.reset(new dsp56k::Memory(this, g_memorySize));
		m_dsp.reset(new dsp56k::DSP(*m_memory, m_peripherals.get(), m_peripherals.get()));

		m_peripherals->initialize();

		return true;
	}

	bool Dsp::destroyDSP()
	{
		if(!m_dsp)
			return false;

		m_dsp.reset();
		m_memory.reset();
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

	bool Dsp::memTranslateAddress(dsp56k::EMemArea& _area, dsp56k::TWord& _offset) const
	{
		if(_offset >= 0x400000)
		{
			_area = dsp56k::MemArea_X;
			return true;
		}
		return false;
	}

	bool Dsp::memValidateAccess(dsp56k::EMemArea _area, dsp56k::TWord _addr, bool _write) const
	{
		return true;
	}

	void Dsp::threadFunc()
	{
		{
			Guard g(m_lock);
			m_dsp->setPC(0x100);
		}

		size_t instructions = 0;

		using clock = std::chrono::high_resolution_clock;

		auto t = clock::now();

		const size_t ipsStep = 0x400000;

		while(m_runThread)
		{
			Guard g(m_lock);
			m_dsp->exec();

			++instructions;

			if((instructions & (ipsStep-1)) == 0)
			{
				const auto t2 = clock::now();
				const auto d = t2 - t;

				const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(d);

				const auto ips = ipsStep * 1000 / ms.count();

				t = t2;

				LOG("IPS: " << ips);
			}
		}
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
