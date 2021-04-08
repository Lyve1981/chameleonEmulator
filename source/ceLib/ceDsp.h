#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>

#include "ceDspPeripherals.h"
#include "../dsp56300/source/dsp56kEmu/memory.h"
#include "../dsp56300/source/dsp56kEmu/dsp.h"

namespace ceLib
{
	class Dsp final : dsp56k::IMemoryValidator
	{
	public:
		using Guard = std::lock_guard<std::mutex>;

		Dsp();
		virtual ~Dsp();

		int create(int _dspIndex, const uint8_t* _code);
		bool destroy(int _ref);
		bool writeData(int _ref, const int32_t* _data, size_t _count);
		void process(float** _inputs, float** _outputs, size_t _sampleFrames);

	private:
		bool createDSP();
		bool loadCode(const uint8_t* _code);
		bool destroyDSP();

		void destroyThread();

		bool memValidateAccess	(dsp56k::EMemArea _area, dsp56k::TWord _addr, bool _write ) const override ;

		void threadFunc();

		std::unique_ptr<DspPeripherals> m_peripherals;
		dsp56k::Memory* m_memory;
		dsp56k::DSP* m_dsp;

		std::atomic<bool> m_runThread;
		std::unique_ptr<std::thread> m_runnerThread;
		std::mutex m_lock;
		std::vector<uint8_t> m_buffer;
	};
}
