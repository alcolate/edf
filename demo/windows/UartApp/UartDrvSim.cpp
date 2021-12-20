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
#include <memory>
#include "Serial.h"
#include "Edf.h"
#include "yahdlc.h"

DEV_HANDLE  UART_0 = (DEV_HANDLE)1;
DEV_HANDLE  UART_1 = (DEV_HANDLE)2;
DEV_HANDLE  UART_2 = (DEV_HANDLE)3;


class CUartDrvSim : public CActive
{
public:

	virtual void Initial()
	{
		Edf::Subscribe(UART_SIM_SIG, this);
		INIT_TRANS(&CUartDrvSim::S_Idle);
	}

	void S_Idle(Event const* const e)
	{
		switch (e->Sig)
		{
		case UART_SIM_SIG:
		{
			CSerialEvent const* ue = static_cast<CSerialEvent const*>(e);

			SendGet(ue->m_Device);

			std::shared_ptr<char> Buff (new char[ue->m_DataCount * 2]);
			uint32_t len;

			yahdlc_control_t control;

			int ret = yahdlc_get_data(&control, (char*)ue->m_Data, ue->m_DataCount, Buff.get(), &len);

			if (control.frame == YAHDLC_FRAME_DATA)
			{
				control.seq_no++;

				yahdlc_frame_data(&control, Buff.get(), len, (char*)m_Buff, &m_BuffCount);

				TRANS(&CUartDrvSim::S_SendAck);
			}
		}
		break;

		default:
			break;
		}
	}

	void S_SendAck(Event const* const e)
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
			SendAck(m_Device);
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
			SendResponse(m_Device, m_Buff, m_BuffCount);
			TRANS(&CUartDrvSim::S_Idle);
		}
		break;

		default:
			break;
		}
	}
	void SendGet(DEV_HANDLE Uart)
	{
		Uart_SendComplete(Uart);
	}



	void SendAck(DEV_HANDLE Uart)
	{
		static yahdlc_control_t control = { YAHDLC_FRAME_ACK, 1 };
		control.seq_no += 1;
		control.frame = YAHDLC_FRAME_ACK;
		uint8_t AckData[20];
		uint32_t DataCount;

		yahdlc_frame_data(&control, NULL, 0, (char*)AckData, &DataCount);

		for (uint32_t i = 0; i < DataCount; i++)
		{
			Uart_Recv(Uart, AckData[i]);
		}
	}
	void SendResponse(DEV_HANDLE Uart, uint8_t* Data, uint16_t Len)
	{
		for (uint16_t i = 0; i < Len; i++)
		{
			Uart_Recv(Uart, Data[i]);
		}
	}


public:
	CUartDrvSim(DEV_HANDLE Uart) : CActive((char*)"UartDrvSim"), m_Time(TIMEOUT_SIG, this)
	{
		m_Device = Uart;
	}

public:
	CTimeEvent m_Time;
	DEV_HANDLE m_Device;

	uint8_t  m_Buff[128];
	uint32_t m_BuffCount;

public:
	DEF_STATEMACHINE(CUartDrvSim);
};

bool Uart_Init(DEV_HANDLE Uart, UartConfig* Config)
{
	CUartDrvSim* uartsim = new CUartDrvSim(Uart);

	uartsim->Start();

	return true;
}

bool Uart_Send(DEV_HANDLE Uart, uint8_t* Data, uint16_t DataLen)
{
	CSerialEvent* e = new CSerialEvent(UART_SIM_SIG, Uart, DataLen);

	memcpy(e->m_Data, Data, DataLen);

	Publish(e);

	return true;
}

