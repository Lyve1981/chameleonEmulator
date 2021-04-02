#include "cePanel.h"

#include <rtems.h>
#include <chameleon.h>
#include <types.h>

#include "../dsp56300/source/dsp56kEmu/logging.h"

namespace ceLib
{
}

extern "C"
{

	int panel_init(void)
	{
		return 0;
	}
	rtems_boolean panel_exit(int ref)
	{
		return false;
	}

	rtems_boolean panel_out_lcd_clear(int ref)
	{
		return false;
	}
	rtems_boolean panel_out_lcd_print(int ref, rtems_unsigned8 row, rtems_unsigned8 col, char *text)
	{
		LOG(text);
		return true;
	}
	rtems_boolean panel_out_lcd_redefine(int ref, rtems_unsigned8 code, const rtems_unsigned8 *data)
	{
		return false;
	}

	rtems_boolean panel_out_led(int ref, rtems_unsigned32 led_bits)
	{
		return false;
	}

	rtems_boolean panel_in_new_event(int ref, rtems_boolean wait)
	{
		return false;
	}
	rtems_boolean panel_in_potentiometer(int ref, rtems_unsigned8 *potentiometer, rtems_unsigned8 *value)
	{
		return false;
	}
	rtems_boolean panel_in_keypad(int ref, rtems_unsigned32 *key_bits)
	{
		return false;
	}
	rtems_boolean panel_in_encoder(int ref, rtems_unsigned8 *encoder, rtems_signed8 *increment)
	{
		return false;
	}
}