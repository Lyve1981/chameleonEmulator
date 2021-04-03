#include "cePlugin.h"

#include <cassert>
#include <mutex>

#include "rtems.h"

extern "C"
{
	rtems_task rtems_main(rtems_task_argument arg);
}

namespace ceLib
{
	Plugin* g_currentPlugin = nullptr;
	static std::mutex g_lockCurrentPlugin;

	Plugin::Plugin()
	{
		Guard lock(g_lockCurrentPlugin);
		// This is needed to make multiple instances work
		g_currentPlugin = this;
		rtems_main(0);
		g_currentPlugin = nullptr;
	}

	Plugin::~Plugin()
	{
		m_panel.destroy();
		Rtems::endThreads(this);
	}

	void Plugin::process(float** _inputs, float** _outputs, size_t _sampleFrames)
	{
		m_dsp.process(_inputs, _outputs, _sampleFrames);
	}

	float Plugin::getParameter(size_t _index)
	{
		return m_panel.getParameter(_index);
	}

	void Plugin::setParameter(size_t _index, float _value)
	{
		m_panel.setParameter(_index, _value);
	}

	Plugin& Plugin::getCurrentPlugin()
	{
		assert(g_currentPlugin);
		return *g_currentPlugin;
	}
}
