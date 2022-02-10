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


namespace Edf
{

CSpiEvent::CSpiEvent(Signals Sig, DEV_HANDLE SpiHandle, 	
						uint8_t *Tx, uint32_t TxLen, uint8_t *Rx, uint32_t RxLen, bool Dynamic)
	: CDeviceEvent(Sig, SpiHandle, 0, Dynamic)
{
	m_Tx = Tx;
	m_TxLen = TxLen;
	m_Rx = Rx;
	m_RxLen = RxLen;
}


CSpiEvent::~CSpiEvent()
{

}


CSPI::CSPI(char* Name, DEV_HANDLE Spi) 
	: CDevice(Name, Spi, 1)
{
	m_IrqRecvEvent = new  CDeviceEvent(HW_RSP_SIG, m_Device, 0, false);
	ASSERT(m_IrqRecvEvent);	

	CDevKeeper::Instance()->RegDevice(this);
}
CSPI::~CSPI()
{
	delete [] m_IrqRecvEvent;
}

void CSPI::Initial(CActive* Owner)
{
	bool result = Spi_Init(m_Device, &m_Config);
	ASSERT(result);

	CDevice::Initial(Owner);
}
bool CSPI::Send(Event const* const e)
{
	CSpiEvent const *se = EventCast(CSpiEvent);
	bool result = false;

	if (se->m_TxLen && se->m_RxLen)
	{
		result = Spi_TransmitReceive(m_Device, const_cast<uint8_t *>(se->m_Tx), se->m_TxLen,
				const_cast<uint8_t *>(se->m_Rx), se->m_RxLen);
	}
	else if (se->m_TxLen)
	{
		result = Spi_Transmit(m_Device, const_cast<uint8_t *>(se->m_Tx), se->m_TxLen);
	}
	else if (se->m_RxLen)
	{
		result = Spi_Receive(m_Device, const_cast<uint8_t *>(se->m_Rx), se->m_RxLen);
	}

	return result;
}
void CSPI::PostIrqRecvEvent()
{
	Publish(m_IrqRecvEvent, true);
}

bool CSPI::MacCall(uint8_t *Data, uint32_t Len)
{
	return true;
}
} // namespace Edf

void Spi_SendComplete(DEV_HANDLE Spi)
{
	CDevKeeper::Instance()->SendComplete(Spi);
}
void Spi_Recv(DEV_HANDLE Spi, uint8_t* Data, uint32_t Len)
{
	CDevKeeper::Instance()->Receive(Spi, Data, Len);
}


