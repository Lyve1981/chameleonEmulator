#pragma once

#include "../dsp56300/source/dsp56kEmu/peripherals.h"

namespace ceLib
{
	class DspPeripherals final : public dsp56k::Peripherals56303
	{
	public:
		void initialize();
	};
}
