#pragma once

#include "../dsp56300/source/dsp56kEmu/peripherals.h"

namespace dsp56k
{
	class DSP;
	class Essi;
}

namespace ceLib
{
	class DspPeripherals final : public dsp56k::IPeripherals
	{
	public:
		void initialize(dsp56k::DSP& _dsp);
		void process(dsp56k::Essi& _essi, float* _inputs, float* _outputs);

		private:
		bool isValidAddress( dsp56k::TWord _addr ) const override;
		dsp56k::TWord read(dsp56k::TWord _addr) override;
		void write(dsp56k::TWord _addr, dsp56k::TWord _value) override;

	};
}
