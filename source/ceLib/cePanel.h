#pragma once

#include <array>
#include <cstdint>
#include <panel01.h>
#include <vector>


#include "semaphore.h"

namespace ceLib
{
	class Panel
	{
	public:
		Panel();
		
		static size_t getParameterCount();
		static const char* getParameterName(size_t _index);

		bool setParameter(size_t _param, float _value);

		bool	lcdClear();
		bool	lcdPrint(int row, int col, const char* _text);
		bool	lcdRedefine(uint8_t _code, const uint8_t* _data);
		bool	setLeds(uint32_t _ledBits);
		bool	hasEvents(bool _wait);
		bool	readPotentiometer(uint8_t* _potentiometer, uint8_t* _value);
		bool	readKeys(uint32_t* _keyBits);
		bool	readEncoder(uint8_t* _encoder, int8_t* _increment);
		float	getParameter(size_t _index) const { return m_parameterValues[_index]; }

	private:
		std::vector<float> m_parameterValues;

		std::array<uint8_t, 4> m_potentiometerValues = {};
		uint32_t m_potentiometersChanged = 0;

		uint32_t m_keysPressed = 0;
		bool m_keysChanged = false;

		float m_encoderValue = 0.0f;
		float m_encoderIncrement = 0.0f;
		bool m_encoderChanged = true;

		Semaphore m_changedEvent;
	};
}
