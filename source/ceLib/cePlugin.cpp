#include "cePlugin.h"

#include "rtems.h"

extern "C"
{
	rtems_task rtems_main(rtems_task_argument arg);
}

namespace ceLib
{
	Plugin::Plugin()
	{
		rtems_main(0);
	}
}
