#pragma once

#include "../dsp56300/source/dsp56kEmu/peripherals.h"

namespace std
{
	class mutex;
}

namespace dsp56k
{
	class DSP;
	class Essi;
}

namespace ceLib
{
	class DspPeripherals final : public dsp56k::PeripheralsDefault
	{
	public:
		void initialize(dsp56k::DSP& _dsp);

	private:
		bool isValidAddress( dsp56k::TWord _addr ) const override;
		dsp56k::TWord read(dsp56k::TWord _addr) override;
		void write(dsp56k::TWord _addr, dsp56k::TWord _value) override;
		void exec() override;

		dsp56k::DSP* m_dsp = nullptr;
	};
}
