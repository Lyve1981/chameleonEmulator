#pragma once

#include <modes.h>
#include <status.h>
#include <tasks.h>
#include <types.h>

namespace ceLib
{
	class Plugin;

	class Rtems
	{
	public:
		static rtems_status_code taskCreate(rtems_name name, rtems_task_priority _initialPriority, rtems_unsigned32 _stackSize, rtems_mode _initialModes, rtems_attribute _attributeSet, rtems_id *_id);
		static rtems_status_code taskStart(rtems_id _id, rtems_task_entry _entryPoint, unsigned32 _argument);
		static rtems_status_code taskDelete(rtems_id _id);
		static void endThreads(Plugin* _plugin);

		static Plugin& findInstance();
		static void terminateEndlessLoop();
	};
}
