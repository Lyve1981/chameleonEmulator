#pragma once

#include <mutex>

#include "ceDsp.h"
#include "cePanel.h"
#include "ceRtems.h"

namespace ceLib
{
	class Plugin
	{
	public:
		Plugin();
		virtual ~Plugin();

		void process(float** _inputs, float** _outputs, size_t _sampleFrames);

		float getParameter(size_t _index);
		void setParameter(size_t _index, float _value);

		static Plugin& getCurrentPlugin();

		Rtems& getRtems()		{ return m_rtems; }
		Dsp& getDsp()			{ return m_dsp; }
		Panel& getPanel()		{ return m_panel; }

		std::mutex& getMutex()	{ return m_mutex; }

		using Guard = std::lock_guard<std::mutex>;

	private:
		Rtems m_rtems;
		Dsp m_dsp;
		Panel m_panel;
		std::mutex m_mutex;
	};
}
