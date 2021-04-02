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
		std::lock_guard<std::mutex> lock(g_lockCurrentPlugin);
		// This is needed to make multiple instances work
		g_currentPlugin = this;
		rtems_main(0);
		g_currentPlugin = nullptr;
	}

	Plugin& Plugin::getCurrentPlugin()
	{
		assert(g_currentPlugin);
		return *g_currentPlugin;
	}
}
