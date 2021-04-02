#pragma once

#include "ceDsp.h"
#include "cePanel.h"
#include "ceRtems.h"

namespace ceLib
{
	class Plugin
	{
	public:
		Plugin();
		virtual ~Plugin() = default;

		static Plugin& getCurrentPlugin();

		Rtems& getRtems()		{ return m_rtems; }
		Dsp& getDsp()			{ return m_dsp; }
		Panel& getPanel()		{ return m_panel; }

	private:
		Rtems m_rtems;
		Dsp m_dsp;
		Panel m_panel;
	};
}
