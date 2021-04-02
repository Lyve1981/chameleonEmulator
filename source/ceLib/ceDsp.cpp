#include "ceDsp.h"

#include <types.h>

namespace ceLib
{
}

extern "C"
{
	int dsp_init(int dsp_index, const rtems_unsigned8 *code)
	{
		return 0;
	}
	rtems_boolean dsp_exit(int ref)
	{
		return false;
	}

	rtems_boolean dsp_write_data(int ref, const rtems_signed32 *data, rtems_unsigned32 count)
	{
		return false;
	}
}
