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
	CAppUart()
	{
		m_Uart_H = UART_0;
		m_Config.Baudrate = 9600;
		m_Config.Parity = Parity_None;
		m_Config.StopBits = StopBit_1Bit;
		m_BuffSize = 17;
		m_Buff4MacCall = new uint8_t [16];
		m_BuffCount = 0;
		ASSERT(m_Buff4MacCall);
	}
	~CAppUart()
	{
		if (m_Buff4MacCall)
			delete [] m_Buff4MacCall;
	}
	virtual bool MacCall(uint8_t AByte)
	{
		if (m_BuffCount < m_BuffSize)
		{
			m_Buff4MacCall[m_BuffCount++] = AByte;
			if (AByte == '\n')
			{
				m_Buff4MacCall[m_BuffCount++] = 0;
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
			m_BuffCount = 0;
			return false;
		}

	}
private:
	UartConfig m_Config;
};

class CAPP : public CActive
{
public:
	virtual void Initial()
	{
		
		m_Uart = new CAppUart();
		CUartKeeper::Instance()->RegUart(m_Uart);
		Edf::Subscribe(SERIAL_IN_SIG, this);
		INIT_TRANS(&CAPP::S_Idle);
	}

	void S_Idle(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
			m_Time.Trigger(MilliSecond(50), 0);
			break;

		case TIMEOUT_SIG:
		{
			Request();

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
			m_Time.Trigger(MilliSecond(1000), 0);
			break;
		case SERIAL_IN_SIG:
		{
			CUartEvent const* ue = static_cast<CUartEvent const*>(e);
			if (ue->Uart_H == m_Uart->m_Uart_H)
			{
				Response(ue->Data, ue->DataLen);


				TRANS(&CAPP::S_Idle);
			}
			break;
		}
		case TIMEOUT_SIG:
		{
			LOG_DEBUG("Wait Response Timeout \r\n");
			TRANS(&CAPP::S_RetryRequest);
		}
		break;

		default:
			break;
		}
	}

	void S_RetryRequest(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
			m_Time.Trigger(MilliSecond(20), 0);
			break;

		case TIMEOUT_SIG:
		{
			Request();

			TRANS(&CAPP::S_WaitReponse);
		}
		break;


		default:
			break;
		}
	}

	void Request(void)
	{
		char tosend[100];
		static uint32_t cc = 0;
		sprintf(tosend, "hello uart %d \r\n", cc++);
		uint16_t len = strlen(tosend) + 1;
		LOG_DEBUG("app send: %s\r\n", tosend);
		CUartEvent* ue = new CUartEvent(SERIAL_OUT_SIG, m_Uart->m_Uart_H, (uint8_t*)tosend, len);
		Publish(ue);
	}

	void Response(uint8_t* Data, uint16_t Len)
	{
		uint8_t* response = Data;
		LOG_DEBUG("app get: %s\r\n", response);
		// do your work
	}

public:
	CAPP() : CActive((char*)"APP"), m_Time(TIMEOUT_SIG, this)
	{
		m_Uart = 0;
	}

public:
	CUart* m_Uart;
	CTimeEvent m_Time;

public:
	DEF_STATEMACHINE(CAPP);
};
