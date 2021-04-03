#include "ceRtems.h"

#include <attr.h>
#include <cassert>
#include <cstdint>
#include <map>
#include <modes.h>
#include <mutex>
#include <tasks.h>
#include <thread>
#include <types.h>

#include "cePlugin.h"

namespace ceLib
{
	using Guard = std::lock_guard<std::mutex>;

	struct Task
	{
		std::unique_ptr<std::thread> thread;
		Plugin& plugin;

		explicit Task(Plugin& _plugin) : plugin(_plugin) {}
	};

	static uint32_t g_nextTaskId = 1;
	static std::mutex g_lock;

	static std::map<uint32_t, std::unique_ptr<Task>> g_tasks;
	static std::map<std::thread::id, uint32_t> g_threadToTask;

	rtems_status_code Rtems::taskCreate(rtems_name name, rtems_task_priority _initialPriority, rtems_unsigned32 _stackSize, rtems_mode _initialModes, rtems_attribute _attributeSet, rtems_id *_id)
	{
		assert(_initialModes == RTEMS_DEFAULT_MODES);
		assert(_attributeSet == RTEMS_DEFAULT_ATTRIBUTES);
		
		Guard g(g_lock);

		if(g_tasks.find(name) != g_tasks.end())
			return RTEMS_INVALID_NAME;

		*_id = g_nextTaskId++;

		g_tasks.insert(std::make_pair(*_id, std::make_unique<Task>(Plugin::getCurrentPlugin())));
		
		return RTEMS_SUCCESSFUL;
	}

	rtems_status_code Rtems::taskStart(rtems_id _id, rtems_task_entry _entryPoint, unsigned32 _argument)
	{
		Guard g(g_lock);

		const auto it = g_tasks.find(_id);
		if(it == g_tasks.end())
			return RTEMS_INVALID_ID;

		auto* task = it->second.get();
		task->thread.reset(new std::thread([_entryPoint, _argument]
		{
			try
			{
				LOG("Thread starting");
				_entryPoint(_argument);
			}
			catch (const std::runtime_error& e)
			{
				LOG("Thread terminated: " << e.what());
			}
		}));

		g_threadToTask.insert(std::make_pair(task->thread->get_id(), _id));

		return RTEMS_SUCCESSFUL;
	}

	rtems_status_code Rtems::taskDelete(rtems_id _id)
	{
		if(_id == RTEMS_SELF)
		{
			const auto myId = std::this_thread::get_id();

			Guard g(g_lock);

			for (auto& task : g_tasks)
			{
				if( task.second->thread->get_id() == myId)
				{
					_id = task.first;
					break;
				}
			}
		}

		if(_id == RTEMS_SELF)
			return RTEMS_ALREADY_SUSPENDED;

		Guard g(g_lock);

		const auto it = g_tasks.find(_id);
		if(it == g_tasks.end())
			return RTEMS_INVALID_ID;

		auto* t = it->second.get();
		t->thread->join();
		g_tasks.erase(it);

		// Note: There won't be an entry if the task was never started
		for (auto it2 = g_threadToTask.begin(); it2 != g_threadToTask.end(); ++it2)
		{
			if(it2->second == _id)
			{
				g_threadToTask.erase(it2);
				break;
			}
		}

		return RTEMS_SUCCESSFUL;
	}

	void Rtems::endThreads(Plugin* _plugin)
	{
		std::list<rtems_id> tasksToEnd;
		{
			Guard g(g_lock);

			for (auto& task : g_tasks)
			{
				auto& plug = task.second->plugin;

				if(&plug == _plugin)
					tasksToEnd.push_back(task.first);
			}			
		}

		for (auto id : tasksToEnd)
			rtems_task_delete(id);
	}

	Plugin& Rtems::findInstance()
	{
		Guard g(g_lock);

		const auto it = g_threadToTask.find(std::this_thread::get_id());

		if(it != g_threadToTask.end())
		{
			const auto it2 = g_tasks.find(it->second);
			if(it2 != g_tasks.end())
				return it2->second->plugin;
		}

		return Plugin::getCurrentPlugin();
	}

	void Rtems::terminateEndlessLoop()
	{
		throw std::runtime_error("quit");
	}
}

extern "C"
{
	rtems_status_code rtems_task_create(rtems_name name, rtems_task_priority initial_priority, rtems_unsigned32 stack_size, rtems_mode initial_modes, rtems_attribute attribute_set, rtems_id *id)
	{
		return ceLib::Rtems::taskCreate(name, initial_priority, stack_size, initial_modes, attribute_set, id);
	}

	rtems_status_code rtems_task_start(rtems_id id, rtems_task_entry entry_point, rtems_unsigned32 argument)
	{
		return ceLib::Rtems::taskStart(id, entry_point, argument);
	}
	rtems_status_code rtems_task_delete(rtems_id id)
	{
		return ceLib::Rtems::taskDelete(id);
	}
}
