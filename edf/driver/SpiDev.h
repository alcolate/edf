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




class CSPI : public CDevice
{
public:
	CSPI(const char * Name, DEV_HANDLE Spi, DEV_HANDLE CS);
	~CSPI();

	void Send(uint8_t *Tx, uint16_t TxLen, uint16_t RxLen);
	bool SendSync(uint8_t *Tx, uint16_t TxLen, uint8_t *Rx, uint16_t RxLen);

protected:
	virtual void Initial(CActive* Owner) override;
	static void PostIrqRecvEvent(CDevice *Device);
	static bool MacCall(CDevice *Device, uint8_t *Data, uint32_t Len);
	virtual bool Send(Event const* const e) override;

public:
	MODE m_Mode;

private:
	CSpiEvent *m_IrqRecvEvent;

	uint16_t m_TxLen;
	uint16_t m_RxLen;

	DEV_HANDLE m_CS;

	SpiConfig 	m_Config;

	Q_HANDLE   m_RecvQ;

	static Q_HANDLE   m_SyncQ;



};


}
