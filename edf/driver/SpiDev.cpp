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
						uint8_t *Tx, uint32_t TxLen, uint32_t RxLen, bool Dynamic)
	: CDeviceEvent(Sig, SpiHandle, RxLen, Dynamic)
{
	ASSERT(TxLen <= CSpiEvent::MAX_SIZE);
	ASSERT(RxLen <= CSpiEvent::MAX_SIZE);

	m_TxLen = TxLen;
	m_RxLen = RxLen;

	if (TxLen)
	{
		memcpy(m_Tx, Tx, TxLen);
	}
}


CSpiEvent::~CSpiEvent()
{

}

CSPI::CSPI(char* Name, DEV_HANDLE Spi, DEV_HANDLE CS)
	: CDevice(Name, Spi, EDeviceType::SPI, 1)
	, m_IrqRecvEvent(SPI_RSP_SIG, Spi, 0, 0, CSpiEvent::MAX_SIZE, false)
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

	CDevice::Initial(Owner);
}

bool CSPI::Send(Event const* const e)
{
	CSpiEvent const *se = EventCast(CSpiEvent);
	bool result = false;

	memcpy(m_Tx, se->m_Tx, se->m_TxLen);
	m_TxLen = se->m_TxLen;
	m_RxLen = se->m_RxLen;

	GPIO_Set(m_CS, GPIO_LOW);

	result = Spi_TransmitReceive(m_HwHandle, m_Tx, m_Rx, se->m_TxLen + se->m_RxLen);

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

void CSPI::Send(uint8_t *Tx, uint8_t TxLen, uint8_t RxLen)
{
	ASSERT(TxLen <= CSpiEvent::MAX_SIZE);
	ASSERT(RxLen <= CSpiEvent::MAX_SIZE);

	CSpiEvent *se = new CSpiEvent(MAC_REQ_SIG, m_HwHandle, Tx, TxLen, RxLen);

	Publish(se);
}

void CSPI::PostIrqRecvEvent()
{
	m_IrqRecvEvent.Copy(m_Rx + m_TxLen, m_RxLen);
	Publish(&m_IrqRecvEvent, true);
}

bool CSPI::MacCall(uint8_t *Data, uint32_t Len)
{
	GPIO_Set(m_CS, GPIO_HIGH);

	return true;
}
} // namespace Edf

void Spi_SendComplete(DEV_HANDLE Spi)
{
	CDevKeeper::Instance()->SendComplete(Spi);
}
void Spi_Recv(DEV_HANDLE Spi)
{
	CDevKeeper::Instance()->Receive(Spi, 0, 0);
}


