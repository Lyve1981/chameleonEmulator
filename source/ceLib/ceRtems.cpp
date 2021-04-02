#include "ceRtems.h"


#include <attr.h>
#include <cassert>
#include <modes.h>
#include <tasks.h>
#include <types.h>

#define LOCK std::lock_guard<std::mutex> lock(m_lock)

namespace ceLib
{
	Rtems::Rtems() = default;
	static uint32_t g_nextTaskId = 1;

	rtems_status_code Rtems::taskCreate(rtems_name name, rtems_task_priority initial_priority, rtems_unsigned32 stack_size, rtems_mode initial_modes, rtems_attribute attribute_set, rtems_id *id)
	{
		assert(initial_modes == RTEMS_DEFAULT_MODES);
		assert(attribute_set == RTEMS_DEFAULT_ATTRIBUTES);
		
		LOCK;

		if(m_tasks.find(name) != m_tasks.end())
			return RTEMS_INVALID_NAME;

		m_tasks.insert(std::make_pair(g_nextTaskId++, std::make_unique<Task>()));

		return RTEMS_SUCCESSFUL;
	}

	rtems_status_code Rtems::taskStart(rtems_id id, rtems_task_entry entry_point, unsigned32 argument)
	{
		LOCK;
		auto it = m_tasks.find(id);
		if(it == m_tasks.end())
			return RTEMS_INVALID_ID;

		Task* task = it->second.get();
		task->m_thread.reset(new std::thread([&]
		{
			entry_point(argument);
		}));

		return RTEMS_SUCCESSFUL;
	}

	rtems_status_code Rtems::taskDelete(rtems_id id)
	{
		// TODO: We need a native thread cancel as many threads are endless loops

		if(id == RTEMS_SELF)
		{
			const auto myId = std::this_thread::get_id();
			LOCK;

			for (auto& task : m_tasks)
			{
				if( task.second->m_thread->get_id() == myId)
				{
					id = task.first;
				}
			}
		}

		if(id == RTEMS_SELF)
			return RTEMS_ALREADY_SUSPENDED;

		LOCK;

		auto it = m_tasks.find(id);
		if(it == m_tasks.end())
			return RTEMS_INVALID_ID;

		auto* t = it->second.get();
		t->m_thread->detach();			// join would be better but if id == self this does not work.
		m_tasks.erase(it);

		return RTEMS_SUCCESSFUL;
	}
}

static ceLib::Rtems rtems;

extern "C"
{
	rtems_status_code rtems_task_create(rtems_name name, rtems_task_priority initial_priority, rtems_unsigned32 stack_size, rtems_mode initial_modes, rtems_attribute attribute_set, rtems_id *id)
	{
		return rtems.taskCreate(name, initial_priority, stack_size, initial_modes, attribute_set, id);
	}

	rtems_status_code rtems_task_start(rtems_id id, rtems_task_entry entry_point, rtems_unsigned32 argument)
	{
		return rtems.taskStart(id, entry_point, argument);
	}
	rtems_status_code rtems_task_delete(rtems_id id)
	{
		return rtems.taskDelete(id);
	}
}
