#include "cePanel.h"

#include "rtems.h"
#include <chameleon.h>
#include <types.h>


#include "ceRtems.h"
#include "cePlugin.h"

#include "panel01.h"

namespace ceLib
{
	constexpr uint32_t PANEL01_ROTARY_ENCODER = PANEL01_POT_CTRL3+1;

	static uint32_t g_parameters[] =
	{
		PANEL01_POT_VOLUME,
		PANEL01_POT_CTRL1,
		PANEL01_POT_CTRL2,
		PANEL01_POT_CTRL3,
		
		PANEL01_ROTARY_ENCODER,

		PANEL01_KEY_GROUP_UP,
		PANEL01_KEY_PAGE_UP,
		PANEL01_KEY_GROUP_DOWN,
		PANEL01_KEY_PAGE_DOWN,
		PANEL01_KEY_PARAM_UP,
		PANEL01_KEY_VALUE_UP,
		PANEL01_KEY_PARAM_DOWN,
		PANEL01_KEY_VALUE_DOWN,
		PANEL01_KEY_EDIT,
		PANEL01_KEY_PART_UP,
		PANEL01_KEY_SHIFT,
		PANEL01_KEY_PART_DOWN,
	};

	static const char* g_paramNames[] =
	{
		"PotVol",
		"PotCtrl1",
		"PotCtrl2",
		"PotCtrl3",

		"Encoder",

		"KGroupUp",
		"KPageUp",
		"KGroupDn",
		"KPageDn",
		"KParamUp",
		"KValUp",
		"KParamDn",
		"KValDn",
		"KEdit",
		"KPartUp",
		"KShift",
		"KPartDn"
	};

	constexpr int g_paramCount = sizeof(g_parameters) / sizeof(g_parameters[0]);

	Panel::Panel()
	{
		m_parameterValues.resize(g_paramCount, 0.0f);
	}

	size_t Panel::getParameterCount()
	{
		return g_paramCount;
	}

	const char* Panel::getParameterName(size_t _index)
	{
		if(_index >= g_paramCount)
			return g_paramNames[g_paramCount-1];

		return g_paramNames[_index];
	}

	bool Panel::setParameter(size_t _param, float _value)
	{
		if(_param >= g_paramCount || m_parameterValues[_param] == _value)
			return false;

		m_parameterValues[_param] = _value;

		const auto param = g_parameters[_param];
		
		switch (param)
		{
			case PANEL01_POT_VOLUME:
			case PANEL01_POT_CTRL1:
			case PANEL01_POT_CTRL2:
			case PANEL01_POT_CTRL3:
				{
					const auto pot = param - PANEL01_POT_VOLUME;
					const auto val = static_cast<uint8_t>(std::round(_value * 127.0f));
					if(val == m_potentiometerValues[pot])
						return false;
					m_potentiometerValues[pot] = val;
					m_potentiometersChanged |= (1<<pot);
				}
				break;
			case PANEL01_ROTARY_ENCODER:
				m_encoderIncrement = _value - m_encoderValue;
				m_encoderValue = _value;
				m_encoderChanged = true;
				break;
			case PANEL01_KEY_GROUP_UP:
			case PANEL01_KEY_PAGE_UP:
			case PANEL01_KEY_GROUP_DOWN:
			case PANEL01_KEY_PAGE_DOWN:
			case PANEL01_KEY_PARAM_UP:
			case PANEL01_KEY_VALUE_UP:
			case PANEL01_KEY_PARAM_DOWN:
			case PANEL01_KEY_VALUE_DOWN:
			case PANEL01_KEY_EDIT:
			case PANEL01_KEY_PART_UP:
			case PANEL01_KEY_SHIFT:
			case PANEL01_KEY_PART_DOWN:
				if(_value >= 0.5f)
					m_keysPressed |= param;
				else
					m_keysPressed &= ~_param;
				m_keysChanged = true;
				break;
			default:
				return false;
		}

		m_changedEvent.notify();

		return true;
	}

	bool Panel::lcdClear()
	{
		return true;
	}

	bool Panel::lcdPrint(int row, int col, const char* _text)
	{
		LOG("LCD PRINT: [x" << row << " y" << col << "] " << _text);
		return true;
	}

	bool Panel::lcdRedefine(uint8_t _code, const uint8_t* _data)
	{
		LOG("LCD REDEFINE: code " << static_cast<int>(_code));
		return true;
	}

	bool Panel::setLeds(uint32_t _ledBits)
	{
		LOG("SET LEDS: " << std::hex << _ledBits);
		return true;
	}

	bool Panel::hasEvents(bool _wait)
	{
		if(_wait)
			m_changedEvent.wait();

		const bool hasEvents = m_potentiometersChanged != 0 || m_keysChanged || m_encoderChanged;

		if(!hasEvents)
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

		return hasEvents;
	}

	bool Panel::readPotentiometer(uint8_t* _potentiometer, uint8_t* _value)
	{
		for(size_t i=0; i<m_potentiometerValues.size(); ++i)
		{
			const auto bit = (1<<i);

			if(!(m_potentiometersChanged & bit))
				continue;

			*_potentiometer = static_cast<uint8_t>(i);
			*_value = m_potentiometerValues[i];
			m_potentiometersChanged &= ~bit;
			return true;
		}
		return false;
	}

	bool Panel::readKeys(uint32_t* _keyBits)
	{
		if(!m_keysChanged)
			return false;
		*_keyBits = m_keysPressed;
		m_keysChanged = false;
		return true;
	}

	bool Panel::readEncoder(uint8_t* _encoder, int8_t* _increment)
	{
		if(!m_encoderChanged)
			return false;
		const auto increment = static_cast<int>(m_encoderIncrement * 127.0f);
		*_increment = static_cast<int8_t>(std::min(std::max(increment, -128), 127));
		m_encoderChanged = false;
		return true;
	}
}

extern "C"
{
	constexpr int g_panelId = 1;
	ceLib::Panel* g_panel = nullptr;

	int panel_init(void)
	{
		return g_panelId;
	}
	rtems_boolean panel_exit(int ref)
	{
		if(ref != g_panelId)
			return false;
		return true;
	}

	rtems_boolean panel_out_lcd_clear(int ref)
	{
		if(g_panelId != ref)
			return false;

		auto& plug = ceLib::Rtems::findInstance();
		return plug.getPanel().lcdClear();
	}

	rtems_boolean panel_out_lcd_print(int ref, rtems_unsigned8 row, rtems_unsigned8 col, char *text)
	{
		if(ref != g_panelId)
			return false;
		auto& plug = ceLib::Rtems::findInstance();
		return plug.getPanel().lcdPrint(row, col, text);
	}
	rtems_boolean panel_out_lcd_redefine(int ref, rtems_unsigned8 code, const rtems_unsigned8 *data)
	{
		if(ref != g_panelId)
			return false;
		auto& plug = ceLib::Rtems::findInstance();
		return plug.getPanel().lcdRedefine(code, data);
	}

	rtems_boolean panel_out_led(int ref, rtems_unsigned32 led_bits)
	{
		if(ref != g_panelId)
			return false;
		auto& plug = ceLib::Rtems::findInstance();
		return plug.getPanel().setLeds(led_bits);
	}

	rtems_boolean panel_in_new_event(int ref, rtems_boolean wait)
	{
		if(ref != g_panelId)
			return false;
		auto& plug = ceLib::Rtems::findInstance();
		return plug.getPanel().hasEvents(wait);
	}
	rtems_boolean panel_in_potentiometer(int ref, rtems_unsigned8 *potentiometer, rtems_unsigned8 *value)
	{
		if(ref != g_panelId)
			return false;
		auto& plug = ceLib::Rtems::findInstance();
		return plug.getPanel().readPotentiometer(potentiometer, value);
	}
	rtems_boolean panel_in_keypad(int ref, rtems_unsigned32 *key_bits)
	{
		if(ref != g_panelId)
			return false;
		auto& plug = ceLib::Rtems::findInstance();
		return plug.getPanel().readKeys(key_bits);
	}
	rtems_boolean panel_in_encoder(int ref, rtems_unsigned8 *encoder, rtems_signed8 *increment)
	{
		if(ref != g_panelId)
			return false;
		auto& plug = ceLib::Rtems::findInstance();
		return plug.getPanel().readEncoder(encoder, increment);
	}
}