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
#include "SpiDev.h"
#include "GPIODrv.h"

namespace Edf
{

CSpiEvent::CSpiEvent(Signal Sig, DEV_HANDLE SpiHandle,
						uint8_t *Tx, uint16_t TxLen, uint16_t RxLen, bool Releasable)
	: CDeviceEvent(Sig, SpiHandle, 0, Releasable)
{
	m_TxLen = TxLen;
	m_RxLen = RxLen;
	m_Tx = nullptr;
	m_Rx = nullptr;
	if (TxLen)
	{
		m_Tx = new uint8_t[TxLen];
		ASSERT(m_Tx);
		memcpy(m_Tx, Tx, TxLen);

		m_Rx = new uint8_t[RxLen + TxLen];
		ASSERT(m_Rx);
	}
}

uint8_t * CSpiEvent::Rx()
{
	return m_Rx + m_TxLen;
}


CSpiEvent::~CSpiEvent()
{
	if (m_TxLen)
	{
		delete [] m_Tx;
		delete [] m_Rx;
	}
}

Q_HANDLE CSPI::m_SyncQ = nullptr;

CSPI::CSPI(const char* Name, DEV_HANDLE Spi, DEV_HANDLE CS)
	: CDevice(Name, Spi, EDeviceType::SPI, 3)
{
	m_CS = CS;
	GPIO_Set(m_CS, GPIO_HIGH);

	CreateIrqSendCompleteEvent();

	SetCallback(CSPI::MacCall, CSPI::PostIrqRecvEvent);

	CDevKeeper::Instance()->RegDevice(this);
}
CSPI::~CSPI()
{

}

void CSPI::Initial(CActive* Owner)
{
	//bool result = Spi_Init(m_HwHandle, &m_Config);
	//ASSERT(result);
	if (CSPI::m_SyncQ == nullptr)
	{
		CSPI::m_SyncQ = OS_QueueCreate(1, 0);
		ASSERT(m_SyncQ);
		OS_QueueSend(m_SyncQ, nullptr);
	}

	m_IrqRecvEvent = nullptr;

	m_RecvQ = OS_QueueCreate(1, 0);
    configASSERT(m_RecvQ);

	CDevice::Initial(Owner);
}

bool CSPI::Send(Event const* const e)
{
	CSpiEvent const *se = EventCast(CSpiEvent);
	bool result = false;

	m_Mode == CDevice::MODE::MODE_ASYNC;

	m_IrqRecvEvent = new CSpiEvent(SPI_RSP_SIG, m_HwHandle, se->m_Tx, se->m_TxLen, se->m_RxLen);

	GPIO_Set(m_CS, GPIO_LOW);

	result = Spi_TransmitReceive(m_HwHandle, m_IrqRecvEvent->m_Tx, m_IrqRecvEvent->m_Rx, m_IrqRecvEvent->m_TxLen + m_IrqRecvEvent->m_RxLen);

	ASSERT(result);

	return result;
}


void CSPI::Send(uint8_t *Tx, uint16_t TxLen, uint16_t RxLen)
{
	CSpiEvent *se = new CSpiEvent(MAC_REQ_SIG, m_HwHandle, Tx, TxLen, RxLen);

	Publish(se);
}

bool CSPI::SendSync(uint8_t *Tx, uint16_t TxLen, uint8_t *Rx, uint16_t RxLen)
{
	bool result = false;

	OS_QueueReceive(m_SyncQ, nullptr, MAX_DELAY);

	m_Mode = CDevice::MODE::MODE_SYNC;

	uint8_t *RxBuff = new uint8_t[RxLen + TxLen];
	ASSERT(RxBuff);

	GPIO_Set(m_CS, GPIO_LOW);

	result = Spi_TransmitReceive(m_HwHandle, Tx, RxBuff, TxLen + RxLen);

	if (false == result)
	{
		goto DONE;
	}

	result = OS_QueueReceive(m_RecvQ, nullptr, 100);

	if (false == result)
	{
		goto DONE;
	}

	if (RxLen)
	{
		memcpy(Rx, RxBuff + TxLen, RxLen);
	}

DONE:
	delete [] RxBuff;
	OS_QueueSend(m_SyncQ, nullptr);

	return result;
}

void CSPI::PostIrqRecvEvent(CDevice *Device)
{
	CSPI *me = static_cast<CSPI*> (Device);
	if (me->m_Mode == CDevice::MODE::MODE_ASYNC)
	{
		Publish(me->m_IrqRecvEvent, true);
	}
	else
	{
		OS_QueueSend(me->m_RecvQ, nullptr, true);
	}
}

bool CSPI::MacCall(CDevice *Device, uint8_t *Data, uint32_t Len)
{
	CSPI *me = static_cast<CSPI*> (Device);
	GPIO_Set(me->m_CS, GPIO_HIGH);

	return true;
}
} // namespace Edf

void Spi_SendComplete(DEV_HANDLE Spi)
{
	Edf::CSPI *dev = static_cast<Edf::CSPI *>(Edf::CDevKeeper::Instance()->GetDevice(Spi));
	if (dev->m_Mode == Edf::CDevice::MODE::MODE_ASYNC)
	{
		Edf::CDevKeeper::Instance()->SendComplete(Spi);
	}
}
void Spi_Recv(DEV_HANDLE Spi)
{
	Edf::CDevKeeper::Instance()->Receive(Spi, 0, 0);
}


