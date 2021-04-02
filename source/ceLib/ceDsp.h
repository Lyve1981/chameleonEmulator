#pragma once
#include <cstdint>


#include "ceDspPeripherals.h"
#include "../dsp56300/source/dsp56kEmu/memory.h"
#include "../dsp56300/source/dsp56kEmu/dsp.h"

namespace ceLib
{
	class Dsp : dsp56k::IMemoryMap
	{
	public:
		Dsp();
		
		int create(int _dspIndex, const uint8_t* _code);
		bool destroy(int _ref);
		bool writeData(int _ref, const int32_t* _data, size_t _count);

	private:
		bool loadCode(const uint8_t* _code);
		bool memTranslateAddress(dsp56k::EMemArea& _area, dsp56k::TWord& _offset ) const override;
		bool memValidateAccess	(dsp56k::EMemArea _area, dsp56k::TWord _addr, bool _write ) const override ;

		DspPeripherals m_peripherals;
		dsp56k::Memory m_memory;
		dsp56k::DSP m_dsp;
	};
}
