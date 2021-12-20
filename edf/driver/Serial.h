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
#include "SerialDrv.h"
#include "Device.h"

namespace Edf
{

class CSerial : public CDevice
{
public:
	using MACCALLBACK = bool (*)(uint8_t* Buff, const uint32_t& BuffSize, uint32_t& BuffCount, uint8_t *Data, uint32_t Len);

	enum class eMode
	{
		Asynch,
		Synch1Wire,
		Synch2Wire
	};

	CSerial(const char *Name, DEV_HANDLE Uart, eMode Mode, uint32_t DQSize = 2);
	~CSerial();

	bool Reset() override;

	// only used in asynchronous mode
	bool ResetBuff();

	bool Config(UART_Baudrate Baudrate, UART_Parity Parity, UART_StopBit Stopbit);

	// only used in asynchronous mode
	bool SetBuff(uint32_t RspSig, uint32_t Size);

	void SetMacCall(MACCALLBACK MacCB);

	// asynchronously communicate
	void Send(uint8_t *Tx, uint32_t TxLen);

	// the method is only used for master - slave synchronous communication.
	bool SendRecv(uint8_t *Tx, uint32_t TxLen, uint8_t *Rx, uint32_t RxLen, uint32_t Delay);
protected:
	void Initial(CActive *Owner) override;
	void SendSuccess(Event const* const e) override;
	void SendFail(Event const* const e) override;
	static void PostIrqRecvEvent(CDevice *Device);
	static bool MacCall(CDevice *Device, uint8_t *Data, uint32_t Len);
	bool Send(Event const* const e) override;
	uint32_t TimeSend(uint32_t DataLen);
	void EnterSend();
	void ExitSend();
	bool WaitComplete(uint32_t Timeout);

private:
	uint8_t* m_RxBuff;
	uint32_t m_RxSize;
	uint32_t m_RxCount;
	uint32_t m_Skip;

	uint8_t* m_Buff4MacCall;
	uint32_t 	m_BuffSize;
	uint32_t 	m_BuffCount;
	MACCALLBACK m_MacCall;

	CDeviceEvent* m_IrqRecvEvents;
	uint32_t	m_IrqRecvEventIndex;

	UartConfig 	m_Config;

	eMode	m_Mode;
	Q_HANDLE   m_RecvQ;
	Q_HANDLE   m_SyncQ;
};



} // namespace Edf

