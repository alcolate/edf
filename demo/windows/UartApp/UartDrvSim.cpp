/*****************************************************************************
Copyright 2021 The Edf Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Contact information:
<9183399@qq.com>
*****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "Serial.h"
#include "Edf.h"

UART_HANDLE  UART_0 = (UART_HANDLE)1;
UART_HANDLE  UART_1 = (UART_HANDLE)2;
UART_HANDLE  UART_2 = (UART_HANDLE)3;

// the three functions are mocks for uart

static void SendGet(UART_HANDLE Uart);

static void SendResponse(UART_HANDLE Uart, uint8_t* Data, uint16_t Len);

class CUartDrvSim : public CActive
{
public:

	virtual void Initial()
	{
		Edf::Subscribe(UART_TEST_SIG, this);
		INIT_TRANS(&CUartDrvSim::S_Idle);
	}

	void S_Idle(Event const* const e)
	{
		switch (e->Sig)
		{
		case UART_TEST_SIG:
		{
			CUartEvent const* ue = static_cast<CUartEvent const*>(e);

			memcpy(m_Buff, ue->Data, ue->DataLen);
			m_BuffCount = ue->DataLen;

			SendGet(ue->UartDevice);

			TRANS(&CUartDrvSim::S_SendResponse);

		}
		break;

		default:
			break;
		}
	}

	void S_SendResponse(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
		{
			m_Time.Trigger(MilliSecond(10), 0);
			break;
		}

		case TIMEOUT_SIG:
		{
			uint8_t rsp[] = "i got you\r\n";
			SendResponse(m_Device, rsp, strlen((char*)rsp));
			TRANS(&CUartDrvSim::S_Idle);
		}
		break;

		default:
			break;
		}
	}


public:
	CUartDrvSim(UART_HANDLE Uart) : CActive((char*)"UartDrvSim"), m_Time(TIMEOUT_SIG, this)
	{
		m_Device = Uart;
	}

public:
	CTimeEvent m_Time;
	UART_HANDLE m_Device;

	uint8_t  m_Buff[128];
	uint16_t m_BuffCount;

public:
	DEF_STATEMACHINE(CUartDrvSim);
};

bool Uart_Init(UART_HANDLE Uart, UartConfig* Config)
{
	CUartDrvSim* uartsim = new CUartDrvSim(Uart);

	uartsim->Start();

	return true;
}

bool Uart_Send(UART_HANDLE Uart, uint8_t* Data, uint16_t DataLen)
{
	CUartEvent* e = new CUartEvent(UART_TEST_SIG, Uart, Data, DataLen);

	Publish(e);

	return true;
}

void SendGet(UART_HANDLE Uart)
{
	Uart_SendComplete(Uart);
}

void SendResponse(UART_HANDLE Uart, uint8_t* Data, uint16_t Len)
{
	for (uint16_t i = 0; i < Len; i++)
	{
		Uart_Recv(Uart, Data[i]);
	}
}
