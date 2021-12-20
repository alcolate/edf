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
#include <string.h>
#include <stdio.h>
#include "Serial.h"

namespace Edf
{


CSerial::CSerial(const char *Name, DEV_HANDLE Uart, eMode Mode, uint32_t DQSize) :
		CDevice(Name, Uart, EDeviceType::UART, DQSize)
{
	m_MacCall = nullptr;
	m_IrqRecvEvents = nullptr;
	m_RxBuff = nullptr;

	m_Mode = Mode;

	SetCallback(CSerial::MacCall, CSerial::PostIrqRecvEvent);

	CreateIrqSendCompleteEvent();

	CDevKeeper::Instance()->RegDevice(this);

}

CSerial::~CSerial()
{
	if (m_IrqRecvEvents)
	{
		delete [] m_IrqRecvEvents;
	}
}

void CSerial::Initial(CActive *Owner)
{
	bool result = Uart_Init(m_HwHandle, &m_Config);
	ASSERT(result);

	if (m_Mode != eMode::Asynch)
	{
		m_SyncQ = OS_QueueCreate(1, 0);
		ASSERT(m_SyncQ);
		OS_QueueSend(m_SyncQ, nullptr);

		m_RecvQ = OS_QueueCreate(1, 0);
	    configASSERT(m_RecvQ);
	}

	CDevice::Initial(Owner);
}

bool CSerial::Reset()
{
	bool result = Uart_Init(m_HwHandle, &m_Config);
	ASSERT(result);
	return result;
}

void CSerial::SetMacCall(MACCALLBACK MacCB)
{
	m_MacCall = MacCB;
}

bool CSerial::ResetBuff()
{
	m_BuffCount = 0;
	return true;
}

bool CSerial::Config(UART_Baudrate Baudrate, UART_Parity Parity, UART_StopBit Stopbit)
{
	m_Config.Baudrate = Baudrate;
	m_Config.Parity = Parity;
	m_Config.StopBits = Stopbit;

	return Reset();
}

bool CSerial::SetBuff(uint32_t RspSig, uint32_t Size)
{
	if (m_Mode != eMode::Asynch)
	{
		return false;
	}

	if (m_IrqRecvEvents)
	{
		delete [] m_IrqRecvEvents;
	}

	m_IrqRecvEvents = new  CDeviceEvent[2]{
		CDeviceEvent((Signal)RspSig, m_HwHandle, Size, false),
		CDeviceEvent((Signal)RspSig, m_HwHandle, Size, false)
	};
	ASSERT(m_IrqRecvEvents);
	m_IrqRecvEventIndex = 0;

	m_BuffSize = Size;
	m_BuffCount = 0;
	m_Buff4MacCall = m_IrqRecvEvents[m_IrqRecvEventIndex].m_Data;

	return true;
}

void CSerial::SendSuccess(Event const* const e)
{
	if (m_Mode == eMode::Asynch)
	{
		return;
	}
	
	OS_QueueSend(m_RecvQ, nullptr, true);
}

void CSerial::SendFail(Event const* const e)
{
	if (m_Mode == eMode::Asynch)
	{
		return;
	}
	OS_QueueSend(m_RecvQ, nullptr, true);
}


uint32_t CSerial::TimeSend(uint32_t DataLen)
{
	uint32_t ByteBits = 10;
	if (m_Config.Parity != Parity_None)
	{
		ByteBits ++;
	}
	if (m_Config.StopBits == StopBit_2Bit)
	{
		ByteBits ++;
	}

	uint32_t DataBits = DataLen * ByteBits;

	uint32_t Time = 1000 * DataBits / m_Config.Baudrate;

	return (Time? 2 * Time: 2);
}

void CSerial::EnterSend()
{
	OS_QueueReceive(m_SyncQ, nullptr, MAX_DELAY);
}

void CSerial::ExitSend()
{
	OS_QueueSend(m_SyncQ, nullptr);
}

bool CSerial::WaitComplete(uint32_t Timeout)
{
	return OS_QueueReceive(m_RecvQ, nullptr, Timeout);
}

void CSerial::Send(uint8_t *Tx, uint32_t TxLen)
{
	m_Owner->Post(new CDeviceEvent(MAC_REQ_SIG, m_HwHandle, Tx, TxLen));
}

bool CSerial::SendRecv(uint8_t *Tx, uint32_t TxLen, uint8_t *Rx, uint32_t RxLen, uint32_t Delay)
{
	bool result = false;
	
	if (m_Mode == eMode::Asynch)
	{
		return false;
	}

	EnterSend();

	m_Skip = (m_Mode == eMode::Synch1Wire)? TxLen: 0;

	if (TxLen > 0)
	{
		result = Uart_Send(m_HwHandle, Tx, TxLen);

		if (false == result)
		{
			goto DONE;
		}

		result = WaitComplete(TimeSend(TxLen));

		if (false == result)
		{
			goto DONE;
		}
	}

	if (RxLen > 0)
	{
		m_RxSize = RxLen;
		m_RxCount = 0;
		m_RxBuff = Rx;

		result = WaitComplete( TimeSend(RxLen) + Delay);

		m_RxSize = 0;

		m_Skip = 0;

		if (false == result)
		{
			goto DONE;
		}
	}

DONE:

	ExitSend();

	return result;
}

bool CSerial::Send(Event const* const e)
{
	return Uart_Send(m_HwHandle, const_cast<uint8_t *>(EventCast(CDeviceEvent)->m_Data), EventCast(CDeviceEvent)->m_DataLen);
}

void CSerial::PostIrqRecvEvent(CDevice *Device)
{
	CSerial *me = static_cast<CSerial*> (Device);
	
	if (me->m_Mode != eMode::Asynch)
	{
		OS_QueueSend(me->m_RecvQ, nullptr, true);
	}
	else
	{
		me->m_IrqRecvEvents[me->m_IrqRecvEventIndex].m_DataLen = me->m_BuffCount;
		Publish(&me->m_IrqRecvEvents[me->m_IrqRecvEventIndex], true);
		me->m_IrqRecvEventIndex ^= 0x01;
		me->m_BuffCount = 0;
		me->m_Buff4MacCall = me->m_IrqRecvEvents[me->m_IrqRecvEventIndex].m_Data;
	}
}

bool CSerial::MacCall(CDevice *Device, uint8_t *Data, uint32_t Len)
{
	CSerial *me = static_cast<CSerial*> (Device);
	
	if (me->m_Mode == eMode::Asynch)
	{
		return me->m_MacCall && me->m_MacCall(me->m_Buff4MacCall, me->m_BuffSize, me->m_BuffCount, Data, Len);
	}
	else
	{
		if (me->m_Skip > 0)
		{
			-- (me->m_Skip);
			return false;
		}

		if (me->m_RxSize > 0)
		{
			me->m_RxBuff[me->m_RxCount] = *Data;

			++ (me->m_RxCount);

			if (me->m_RxCount == me->m_RxSize)
			{
				return true;
			}

			return false;
		}
		return false;
	}


}

} // namespace Edf

void Uart_SendComplete(DEV_HANDLE Uart)
{
	Edf::CDevKeeper::Instance()->SendComplete(Uart);
}
void Uart_Recv(DEV_HANDLE Uart, uint8_t Data)
{
	Edf::CDevKeeper::Instance()->Receive(Uart, &Data, 1);
}

void Uart_Error(DEV_HANDLE Uart)
{
	Edf::CSerial *dev = static_cast<Edf::CSerial *>(Edf::CDevKeeper::Instance()->GetDevice(Uart));

	dev->ResetBuff();
}

