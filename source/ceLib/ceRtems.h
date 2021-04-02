#pragma once
#include <map>
#include <modes.h>
#include <mutex>
#include <status.h>
#include <tasks.h>
#include <types.h>

namespace ceLib
{
	class Rtems
	{
	public:
		Rtems();

		rtems_status_code taskCreate(rtems_name name, rtems_task_priority initial_priority, rtems_unsigned32 stack_size, rtems_mode initial_modes, rtems_attribute attribute_set, rtems_id *id);
		rtems_status_code taskStart(rtems_id id, rtems_task_entry entry_point, unsigned32 argument);
		rtems_status_code taskDelete(rtems_id id);

	private:
		struct Task
		{
			std::unique_ptr<std::thread> m_thread;
		};

		std::map<uint32_t, std::unique_ptr<Task>> m_tasks;
		std::mutex m_lock;
	};
}
