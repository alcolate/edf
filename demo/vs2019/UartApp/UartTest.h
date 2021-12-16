/*****************************************************************************
* MIT License:
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*
* Contact information:
* <9183399@qq.com>
*****************************************************************************/
#pragma once

#include <string.h>

#include "Edf.h"
#include "Serial.h"
#pragma warning(disable : 4996)


class CAppUart : public CUart
{
public:
	CAppUart(UARTDEV_H UartH, UartConfig *Config, uint8_t* Buff4MacCall, uint16_t BuffSize)
		: CUart(UartH, Config, Buff4MacCall, BuffSize)
	{

	}

	virtual bool MacCall(uint8_t AByte)
	{
		if (BuffCount < BuffSize)
		{
			Buff4MacCall[BuffCount++] = AByte;
			if (AByte == '\n')
			{
				Buff4MacCall[BuffCount++] = 0;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			// frame error occurs, reset it
			BuffCount = 0;
			return false;
		}

	}

};

class CAPP : public CActive
{
public:
	virtual void Initial()
	{
		m_Config.Baudrate = 9600;
		m_Uart = new CAppUart(UART_0, &m_Config, m_Buff4MacCall, sizeof(m_Buff4MacCall) - 1);
		CUartKeeper::Instance()->RegUart(m_Uart);
		Edf::Subscribe(SERIAL_IN_SIG, this);
		INIT_TRANS(&CAPP::S_Request);
	}

	void S_Request(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
			m_Time.Trigger(MilliSecond(50), 0);
			break;

		case TIMEOUT_SIG:
		{
			char tosend[100];
			static uint32_t cc = 0;
			sprintf(tosend,"hello uart %d \r\n", cc ++);
			uint16_t len = strlen(tosend) + 1;
			LOG_DEBUG("app send: %s\r\n", tosend);
			CUartEvent* ue = new CUartEvent(SERIAL_OUT_SIG, m_Uart->m_Uart_H, (uint8_t*)tosend, len);
			Publish(ue);

			TRANS(&CAPP::S_WaitReponse);

		}
		break;


		default:
			break;
		}
	}

	void S_WaitReponse(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
			break;
		case SERIAL_IN_SIG:
		{
			CUartEvent const* ue = static_cast<CUartEvent const*>(e);
			if (ue->Uart_H == m_Uart->m_Uart_H)
			{
				uint8_t* response = ue->Data;
				LOG_DEBUG("app get: %s\r\n", response);
				// do your work

				TRANS(&CAPP::S_Request);
			}
			break;
		}

		default:
			break;
		}
	}

public:
	CAPP() : CActive((char*)"APP"), m_Time(TIMEOUT_SIG, this)
	{
		m_Uart = 0;
	}
	CUart* m_Uart;
	UartConfig m_Config;
	uint8_t m_Buff4MacCall[17];

public:

	CTimeEvent m_Time;

public:
	DEF_STATEMACHINE(CAPP);
};
