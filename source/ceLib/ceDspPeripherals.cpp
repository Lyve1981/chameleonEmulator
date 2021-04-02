#include "ceDspPeripherals.h"

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

	void DspPeripherals::process(float* _inputs, float* _outputs)
	{
	}
}
