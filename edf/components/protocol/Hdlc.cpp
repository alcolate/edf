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
#include "Hdlc.h"

CHdlcImp::CHdlcImp(CHdlc* MacLayer) : m_MacLayer(MacLayer)
{
	m_ControlSend.seq_no = 0;
	m_ControlRecv.seq_no = 0;
}

void CHdlcImp::Ack(uint8_t SeqNo)
{
	yahdlc_control_t Control;

	Control.seq_no = SeqNo;
	Control.seq_no++;
	Control.frame = YAHDLC_FRAME_ACK;

	CDeviceEvent *ue = new CDeviceEvent(MAC_REQ_SIG, m_MacLayer->m_Uart->m_Device, 10);

	yahdlc_frame_data(&Control, NULL, 0, (char *)ue->m_Data, &ue->m_Count);

	Publish(ue);
}

void CHdlcImp::PacketData(const uint8_t* Data, uint16_t Len)
{
	CDeviceEvent* ue = new CDeviceEvent(MAC_REQ_SIG, m_MacLayer->m_Uart->m_Device, Len * 2, true);

	m_ControlSend.frame = YAHDLC_FRAME_DATA;
	m_ControlSend.seq_no++;
	yahdlc_frame_data(&m_ControlSend, (char*)Data, Len, (char*)ue->m_Data, &ue->m_Count);

	Publish(ue);
}

bool CHdlcImp::Parser(const uint8_t* Data, uint32_t Len)
{
	int ret;

	yahdlc_control_t RcvControl;

	CMacEvent* he = new CMacEvent(MAC_RSP_SIG, CMacEvent::GET_DATA, Data, Len);

	ret = yahdlc_get_data(&RcvControl, (char*)Data, Len, (char*)he->m_Data, &he->m_DataLen);

	if (ret < 0)
	{
		delete he;
		return false;
	}

	switch (RcvControl.frame)
	{
	case YAHDLC_FRAME_ACK:
	{
		delete he;

		if (((m_ControlSend.seq_no + 1) == RcvControl.seq_no)
			|| (m_ControlSend.seq_no == 7 && RcvControl.seq_no == 0))
		{
			return true;
		}
	}
	break;
	case YAHDLC_FRAME_NACK:
		break;
	case YAHDLC_FRAME_DATA:
	{
		Ack(RcvControl.seq_no);

		// this is a new frame
		if (m_ControlRecv.seq_no != RcvControl.seq_no)
		{
			m_ControlRecv = RcvControl;
			Publish(he);
		}
	}
	break;

	default:
		delete he;
		ASSERT(false);
		break;
	}

	return false;
}




CHdlc::CHdlc() : CMacLayer((char*)"Hdlc", 0), m_Timer(TIMEOUT_SIG, this)
{

}
CHdlc::~CHdlc()
{
	delete m_Uart;
	delete m_Hdlc;

}
bool CHdlc::MacCall(uint8_t* Buff, uint16_t& BuffSize, uint16_t& BuffCount, uint8_t* Data, uint32_t Len)
{
	Buff[BuffCount++] = *Data;
	// get a frame?
	if (*Data == CHdlcImp::DELIMETER && BuffCount > 2)
	{
		return true;
	}
	else
	{
		// discard all data
		if (BuffCount == BuffSize)
		{
			BuffCount = 0;
		}
		return false;
	}
}

void CHdlc::Initial()
{
	m_Uart = new CSerial((char*)"AppUart",
		UART_0, 9600, Parity_None, StopBit_1Bit,
		128, CHdlc::MacCall);
	ASSERT(m_Uart);

	m_Hdlc = new CHdlcImp(this);
	ASSERT(m_Hdlc);

	m_DQ = new CEventQ(2);
	ASSERT(m_DQ);

	Subscribe(APP_REQ_SIG, this);
	Subscribe(Serial_RSP_SIG, this);

	INIT_TRANS(&CHdlc::S_Idle);
}

void CHdlc::S_Idle(Event const* const e)
{
	switch (e->Sig)
	{

	case APP_REQ_SIG:

		m_DQ->Defer(e);

		TRANS(&CHdlc::S_Sending);
		break;

	case Serial_RSP_SIG:
		if (IsToMe(e))
		{
			if (m_Hdlc->Parser(EventCast(CDeviceEvent)->m_Data, EventCast(CDeviceEvent)->m_Count))
			{

			}
		}

		break;
	default:
		break;
	}
}
void CHdlc::S_Sending(Event const* const e)
{
	switch (e->Sig)
	{
	case ENTRY_SIG:
		m_Timer.Trigger(100, 100);
		m_SendingEvent = static_cast<CAppEvent const *>(m_DQ->Fetch());
		Request(m_SendingEvent);
		m_Retries = MAX_RETRIES;
		break;
	case EXIT_SIG:
		(const_cast<CAppEvent *>(m_SendingEvent))->DecRef();
		m_Timer.UnTrigger();
		break;

	case Serial_RSP_SIG:
		if (IsToMe(e))
		{
			if (m_Hdlc->Parser(EventCast(CDeviceEvent)->m_Data, EventCast(CDeviceEvent)->m_Count))
			{
				TRANS(&CHdlc::S_Idle);
			}
		}
		break;

	case TIMEOUT_SIG:
		if (m_Retries == 0)
		{
			TRANS(&CHdlc::S_Idle);
		}
		else
		{
			m_Retries --;
			Request(m_SendingEvent);
		}

		break;

	case APP_REQ_SIG:
		LOG_ERROR("Mac Layer is busy\r\n");
		break;

	default:
		break;
	}
}



inline bool CHdlc::IsToMe(Event const* const e)
{
	return EventCast(CDeviceEvent)->m_Device == m_Uart->m_Device;
}

void CHdlc::Request(CAppEvent const * const e)
{
	m_Hdlc->PacketData(e->m_Data, e->m_DataLen);
}


