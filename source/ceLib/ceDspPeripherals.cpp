#include "ceDspPeripherals.h"

#include "ceDsp.h"
#include "rtems.h"
#include "chameleon.h"

#include "../dsp56300/source/dsp56kEmu/dsp.h"

namespace ceLib
{
	bool DspPeripherals::isValidAddress(dsp56k::TWord _addr) const
	{
		return PeripheralsDefault::isValidAddress(_addr);
	}

	dsp56k::TWord DspPeripherals::read(dsp56k::TWord _addr)
	{
          // HACK: inject max volume
		if(_addr == 0xFFFFC3)
		    return 0xffffff;
		if(_addr == 0xffffc6)
		    return 0x7fffff;

		const dsp56k::TWord res = PeripheralsDefault::read(_addr);

		if(_addr == dsp56k::Essi::ESSI0_RX)
		{
			assert(m_dsp);

			// if DSP reads the receive register, clear "Receive Register Full" bit in ESSI SR
			m_dsp->getEssi().toggleStatusRegisterBit(dsp56k::Essi::Essi0, dsp56k::Essi::SSISR_RDF, 0);
		}

		return res;
	}

	void DspPeripherals::write(dsp56k::TWord _addr, dsp56k::TWord _value)
	{
		PeripheralsDefault::write(_addr, _value);

		if(_addr == dsp56k::Essi::ESSI0_TX0)
		{
			assert(m_dsp);

			// if DSP writes to transmit register, clear "Transmit Register Empty" bit in ESSI SR
			m_dsp->getEssi().toggleStatusRegisterBit(dsp56k::Essi::Essi0, dsp56k::Essi::SSISR_TDE, 0);
		}
	}

	void DspPeripherals::initialize(dsp56k::DSP& _dsp)
	{
		m_dsp = &_dsp;

		auto& essi = _dsp.getEssi();

		const dsp56k::TWord cra =	dsp56k::Essi::CRA_WL0 | dsp56k::Essi::CRA_WL1;	// word length 24 bits
		const dsp56k::TWord crb =	dsp56k::Essi::CRB_TE0 |							// Transmit 0 Enable
									dsp56k::Essi::CRB_RE |							// Receive Enable
									dsp56k::Essi::CRB_TIE |							// Enable Transmit Interrupts
									dsp56k::Essi::CRB_RIE;							// Enable Receive Interrupts

		// CRB: Enable Receive & Transmit 0, enable receive & transmit interrupts
		essi.setControlRegisters(dsp56k::Essi::Essi0, cra, crb);
	}

	void DspPeripherals::process(std::mutex& _dspLock, float* _inputs, float* _outputs)
	{	
		auto& essi = m_dsp->getEssi();;

		const auto waitCycles = 10000000;
		
		for(size_t c=0; c<2; ++c)
		{
			// toggle frame sync to inform DSP which channel is being transmitted, 1 = left, 0 = right
			{
//				const Dsp::Guard g(_dspLock);
				essi.toggleStatusRegisterBit(dsp56k::Essi::Essi0, dsp56k::Essi::SSISR_RFS, 1 - c);
			}
		
			// wait for receive register to be free
			for(size_t r=0; r<waitCycles; ++r)
			{
//				const Dsp::Guard g(_dspLock);
				const auto receiveFull = essi.testStatusRegisterBit(dsp56k::Essi::Essi0, dsp56k::Essi::SSISR_RDF);
				if(!receiveFull)
					break;
			}

			// write input to receive register
			{
//				const Dsp::Guard g(_dspLock);
				write(dsp56k::Essi::ESSI0_RX, float_to_fix(_inputs[c]) & 0xffffff);
				essi.toggleStatusRegisterBit(dsp56k::Essi::Essi0, dsp56k::Essi::SSISR_RDF, 1);
			}

			// wait for transmit register to be full
			for(size_t r=0; r<waitCycles; ++r)
			{
//				const Dsp::Guard g(_dspLock);
				const auto transmitEmpty = essi.testStatusRegisterBit(dsp56k::Essi::Essi0, dsp56k::Essi::SSISR_TDE);
				if(!transmitEmpty)
					break;
			}

			// read output register
			{
//				const Dsp::Guard g(_dspLock);
				_outputs[c] = fix_to_float(read(dsp56k::Essi::ESSI0_TX0));
				essi.toggleStatusRegisterBit(dsp56k::Essi::Essi0, dsp56k::Essi::SSISR_TDE, 1);
			}
		}
	}
}
