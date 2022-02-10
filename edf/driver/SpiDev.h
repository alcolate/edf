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
#pragma once

#include <memory.h>
#include "SpiDrv.h"
#include "Device.h"

namespace Edf
{

class CSpiEvent : public CDeviceEvent
{
public:
	CSpiEvent(Signals Sig, SPI_HANDLE SpiHandle,
			uint8_t *Tx, uint32_t TxLen, uint8_t *Rx, uint32_t RxLen, bool Dynamic = true);

	virtual ~CSpiEvent();

	uint8_t *m_Tx;
	uint32_t m_TxLen;
	uint8_t *m_Rx;
	uint32_t m_RxLen;
};


class CSPI : public CDevice
{
public:
	CSPI(char* Name, SPI_HANDLE Spi);
	~CSPI();

	virtual void Initial(CActive* Owner) override;

	virtual void PostIrqRecvEvent() override;

	virtual bool MacCall(uint8_t *Data, uint32_t Len) override;
protected:
	virtual bool Send(Event const* const e) override;

public:

	CDeviceEvent* m_IrqRecvEvent;

	SpiConfig 	m_Config;

};


}
