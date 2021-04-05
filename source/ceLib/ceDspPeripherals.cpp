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
		if(_addr == dsp56k::Essi::ESSI0_RX)
		{
			assert(m_dsp);

			if(m_audioInput.empty())
				return 0;

			const auto res = m_audioInput.pop_front();

			return res;
		}

		const auto res = PeripheralsDefault::read(_addr);

		return res;
	}

	void DspPeripherals::write(const dsp56k::TWord _addr, dsp56k::TWord _value)
	{
		PeripheralsDefault::write(_addr, _value);

		if(_addr == dsp56k::Essi::ESSI0_TX0)
		{
			assert(m_dsp);

			m_audioOutput.push_back(_value);
		}
	}

	void DspPeripherals::exec()
	{
		if(!m_dsp)
			return;
		
		auto& essi = m_dsp->getEssi();

		// set Receive Register Full flag if there is input
		essi.toggleStatusRegisterBit(dsp56k::Essi::Essi0, dsp56k::Essi::SSISR_RDF, m_audioInput.empty() ? 0 : 1);

		// set Transmit Register Empty flag if there is space left in the output
		essi.toggleStatusRegisterBit(dsp56k::Essi::Essi0, dsp56k::Essi::SSISR_TDE, m_audioOutput.full() ? 0 : 1);

		// Toggle Frame Sync flag. We do not need it as we ensure proper channel ordering anyway, but the DSP needs it to sync to the left/right channel
		essi.toggleStatusRegisterBit(dsp56k::Essi::Essi0, dsp56k::Essi::SSISR_RFS, (++m_frameSync)&1);

		PeripheralsDefault::exec();
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

	void DspPeripherals::process(float** _inputs, float** _outputs, size_t _sampleFrames)
	{
		if(!_sampleFrames)
			return;

		// write input data
		for(size_t i=0; i<_sampleFrames; ++i)
		{
			for(size_t c=0; c<2; ++c)
			{
				m_audioInput.push_back(dsp56k::Essi::float2Dsdp(_inputs[c][i]));
			}
		}

		// read output
		for(size_t i=0; i<_sampleFrames; ++i)
		{
			for(size_t c=0; c<2; ++c)
			{
				const auto v = m_audioOutput.pop_front();

				_outputs[c][i] = dsp56k::Essi::dsp2Float(v);
			}
		}
	}
}
