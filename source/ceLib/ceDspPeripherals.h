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
		void initialize();
	};
}
