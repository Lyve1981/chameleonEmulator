#pragma once

#include "../dsp56300/source/dsp56kEmu/peripherals.h"

namespace ceLib
{
	class DspPeripherals : public dsp56k::IPeripherals
	{
		bool isValidAddress( dsp56k::TWord _addr ) const override;
		dsp56k::TWord read(dsp56k::TWord _addr) override;
		void write(dsp56k::TWord _addr, dsp56k::TWord _value) override;

	public:
		void process(float* _inputs, float* _outputs);
	};
}
