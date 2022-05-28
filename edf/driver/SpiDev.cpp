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

CSpiEvent::CSpiEvent(Signals Sig, DEV_HANDLE SpiHandle, 	
						uint8_t *Tx, uint32_t TxLen, uint32_t RxLen, bool Releasable)
	: CDeviceEvent(Sig, SpiHandle, RxLen, Releasable)
{
	m_TxLen = TxLen;
	m_RxLen = RxLen;
	m_Tx = nullptr;
	if (TxLen)
	{
		m_Tx = new uint8_t[TxLen];
		ASSERT(m_Tx);
		memcpy(m_Tx, Tx, TxLen);
	}
}


CSpiEvent::~CSpiEvent()
{
	if (m_TxLen)
	{
		delete [] m_Tx;
	}
}

CSPI::CSPI(char* Name, DEV_HANDLE Spi, DEV_HANDLE CS)
	: CDevice(Name, Spi, EDeviceType::SPI, 3)
{
	m_CS = CS;
	GPIO_Set(m_CS, GPIO_HIGH);

	CDevKeeper::Instance()->RegDevice(this);
}
CSPI::~CSPI()
{

}

void CSPI::Initial(CActive* Owner)
{
	//bool result = Spi_Init(m_HwHandle, &m_Config);
	//ASSERT(result);
	m_Tx = nullptr;
	m_Rx = nullptr;
	m_IrqRecvEvent = nullptr;

	m_RecvQ = OS_QueueCreate(1, sizeof(void *));
    configASSERT(m_RecvQ);

	CDevice::Initial(Owner);
}

bool CSPI::Send(Event const* const e)
{
	CSpiEvent const *se = EventCast(CSpiEvent);
	bool result = false;

	if (m_Tx) delete [] m_Tx;

	if (m_Rx) delete [] m_Rx;

	m_Tx = new uint8_t[se->m_TxLen];
	ASSERT(m_Tx);
	m_Rx = new uint8_t[se->m_RxLen + se->m_TxLen];
	ASSERT(m_Rx);

	m_TxLen = se->m_TxLen;
	m_RxLen = se->m_RxLen;

	memcpy(m_Tx, se->m_Tx, se->m_TxLen);

	m_IrqRecvEvent = new CSpiEvent(SPI_RSP_SIG, m_HwHandle, 0, 0, se->m_RxLen, true);

	GPIO_Set(m_CS, GPIO_LOW);

	result = Spi_TransmitReceive(m_HwHandle, m_Tx, m_Rx, se->m_TxLen + se->m_RxLen);

	ASSERT(result);

//	if (se->m_TxLen && se->m_RxLen)
//	{
//		result = Spi_TransmitReceive(m_HwHandle, const_cast<uint8_t *>(se->m_Tx), se->m_TxLen,
//				const_cast<uint8_t *>(se->m_Rx), se->m_RxLen);
//	}
//	else if (se->m_TxLen)
//	{
//		//result = Spi_Transmit(m_HwHandle, const_cast<uint8_t *>(se->m_Tx), se->m_TxLen);
//	}
//	else if (se->m_RxLen)
//	{
//		result = Spi_Receive(m_HwHandle, const_cast<uint8_t *>(se->m_Rx), se->m_RxLen);
//	}

	return result;
}

void CSPI::Release()
{
	if (m_Tx)
	{
		delete [] m_Tx;
		m_Tx = nullptr;
	}

	if (m_Rx)
	{
		delete [] m_Rx;
		m_Rx = nullptr;
	}
}

void CSPI::Send(uint8_t *Tx, uint8_t TxLen, uint8_t RxLen)
{
	m_Mode == CDevice::MODE::MODE_ASYNC;

	CSpiEvent *se = new CSpiEvent(MAC_REQ_SIG, m_HwHandle, Tx, TxLen, RxLen);

	Publish(se);
}

bool CSPI::SendSync(uint8_t *Tx, uint8_t TxLen, uint8_t *Rx, uint8_t RxLen)
{
	bool result = false;

	m_Mode = CDevice::MODE::MODE_SYNC;

	uint8_t *RxBuff = new uint8_t[RxLen + TxLen];
	ASSERT(RxBuff);

	m_IrqRecvEvent = new CSpiEvent(SPI_RSP_SIG, m_HwHandle, 0, 0, RxLen);
	ASSERT(m_IrqRecvEvent);

	CSpiEvent *e;

	GPIO_Set(m_CS, GPIO_LOW);

	result = Spi_TransmitReceive(m_HwHandle, Tx, RxBuff, TxLen + RxLen);

	ASSERT(result);

	result = OS_QueueReceive(m_RecvQ, &e, MAX_DELAY);

	ASSERT(result);
	if (RxLen)
	{
		memcpy(Rx, RxBuff + TxLen, RxLen);
	}

	delete [] RxBuff;
	delete m_IrqRecvEvent;

	return result;
}

void CSPI::PostIrqRecvEvent()
{
	if (m_Mode == CDevice::MODE::MODE_ASYNC)
	{
		m_IrqRecvEvent->Copy(m_Rx + m_TxLen, m_RxLen);
		Publish(m_IrqRecvEvent, true);
	}
	else
	{
		OS_QueueSend(m_RecvQ, m_IrqRecvEvent, true);
	}
}

bool CSPI::MacCall(uint8_t *Data, uint32_t Len)
{
	GPIO_Set(m_CS, GPIO_HIGH);

	return true;
}
} // namespace Edf

void Spi_SendComplete(DEV_HANDLE Spi)
{
	Edf::CDevKeeper::Instance()->SendComplete(Spi);
}
void Spi_Recv(DEV_HANDLE Spi)
{
	Edf::CDevKeeper::Instance()->Receive(Spi, 0, 0);
}


