#include "ceDspPeripherals.h"

#include "../dsp56300/source/dsp56kEmu/dsp.h"

namespace ceLib
{
	void DspPeripherals::initialize()
	{
		auto& essi = getEssi();

		const dsp56k::TWord cra =	(1<<dsp56k::Essi::CRA_WL0) | (1<<dsp56k::Essi::CRA_WL1);	// word length 24 bits
		const dsp56k::TWord crb =	(1<<dsp56k::Essi::CRB_TE0) |								// Transmit 0 Enable
									(1<<dsp56k::Essi::CRB_RE);/* |								// Receive Enable
									(1<<dsp56k::Essi::CRB_TIE) |								// Enable Transmit Interrupts
									(1<<dsp56k::Essi::CRB_RIE);									// Enable Receive Interrupts*/

		// CRB: Enable Receive & Transmit 0, enable receive & transmit interrupts
		essi.setControlRegisters(dsp56k::Essi::Essi0, cra, crb);
	}
}
