#include "ceDsp.h"

#include <types.h>

#include "ceRtems.h"
#include "cePlugin.h"

namespace ceLib
{
	constexpr size_t g_memorySize = 0x800000;

	constexpr uint8_t g_cmdMemoryP	= 0x00;
	constexpr uint8_t g_cmdMemoryX	= 0x01;
	constexpr uint8_t g_cmdMemoryY	= 0x02;
	constexpr uint8_t g_cmEnd		= 0x03;

	Dsp::Dsp() : m_memory(&m_peripherals, &m_peripherals, this, g_memorySize), m_dsp(m_memory)
	{
	}

	int Dsp::create(int _dspIndex, const uint8_t* _code)
	{
		if(_dspIndex != 1)
			return 0;

		if(!loadCode(_code))
			return 0;

		return _dspIndex;
	}

	bool Dsp::destroy(int _ref)
	{
		return (_ref == 1);
	}

	bool Dsp::writeData(int _ref, const int32_t* _data, size_t _count)
	{
		return false;
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
				m_memory.set(_area, _address + i, value);
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
