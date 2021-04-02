#include "ceDspPeripherals.h"

#include "rtems.h"
#include "chameleon.h"

#include "../dsp56300/source/dsp56kEmu/dsp.h"

namespace ceLib
{
	bool DspPeripherals::isValidAddress(dsp56k::TWord _addr) const
	{
		return true;
	}

	dsp56k::TWord DspPeripherals::read(dsp56k::TWord _addr)
	{
		return 0;
	}

	void DspPeripherals::write(dsp56k::TWord _addr, dsp56k::TWord _value)
	{
	}

	void DspPeripherals::initialize(dsp56k::DSP& _dsp)
	{
		auto& essi = _dsp.getEssi();

		const dsp56k::TWord cra =	dsp56k::Essi::CRA_WL0 | dsp56k::Essi::CRA_WL1;	// word length 24 bits
		const dsp56k::TWord crb =	dsp56k::Essi::CRB_TE0 |							// Transmit 0 Enable
									dsp56k::Essi::CRB_RE |							// Receive Enable
									dsp56k::Essi::CRB_TIE |							// Enable Transmit Interrupts
									dsp56k::Essi::CRB_RIE;							// Enable Receive Interrupts

		// CRB: Enable Receive & Transmit 0, enable receive & transmit interrupts
		essi.setControlRegisters(dsp56k::Essi::Essi0, cra, crb);
	}

	void DspPeripherals::process(dsp56k::Essi& _essi, float* _inputs, float* _outputs)
	{
		const uint32_t in = float_to_fix(_inputs[0]);
	}
}
