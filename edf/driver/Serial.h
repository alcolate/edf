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
#include "Edf.h"

namespace Edf
{


class CSerialEvent : public Event
{
public:
	CSerialEvent(Signals Sig, UART_HANDLE Uart_H, uint32_t BuffSize, bool Dynamic = true);

	CSerialEvent(Signals Sig, UART_HANDLE Uart_H, const uint8_t* Data, uint16_t Len);
	virtual ~CSerialEvent();

	void SetSig(Signals Sig);
	void CopyData(uint8_t* Data, uint16_t Len);

	UART_HANDLE	m_Device;
	uint8_t*	m_Data;
	uint32_t 	m_DataCount;
	uint32_t	m_DataSize;

};



using MACCALLBACK = bool (*)(uint8_t *Buff, uint16_t &BuffSize, uint16_t &BuffCount, uint8_t AByte);

class CUart
{
public:
	CUart(char *Name, UART_HANDLE Uart, 
			UART_Baudrate Baudrate, UART_Parity Parity, UART_StopBit Stopbit,
			uint16_t MaxFrameLen, 
			MACCALLBACK MacCall, uint32_t DQSize = 2);
	~CUart();

	void ResetBuff();

	void Dispatcher(Event const* const e);

	void Initial();

	void S_Idle(Event const* const e);
	void S_WaitComplete(Event const* const e);

	void PostIrqEvent(Signals Sig);

	bool DeferEvent(Event const* const e);

	void FetchDeferedEvent();

public:

	char m_Name[10];

	UART_HANDLE m_Device;			// the handle. of uart hardware
	UartConfig 	m_Config;
	uint8_t* 	m_Buff4MacCall;
	uint16_t 	m_BuffSize;
	uint16_t 	m_BuffCount;
	MACCALLBACK m_MacCall;
	CSerialEvent *m_IrqEvent;
	uint32_t	m_IrqEventIndex;

	CEventQ* m_DQ;
	CActive* m_Owner;
	USE_LINK(CUart);

public:
	DEF_STATEMACHINE(CUart);
};

class CUartKeeper : public CActive
{
public:

	static CUartKeeper* Instance();

	void RegUart(CUart* Uart);

	virtual void Initial();

	void SendComplete(UART_HANDLE UartH);

	void Receive(UART_HANDLE UartH, uint8_t AByte);

private:
	void S_Run(Event const* const e);

	CUart* GetUart(UART_HANDLE UartH);

	void AddUart(CUart* Uart);

	CUartKeeper() : CActive((char*)"UartKeeper")
	{

	}

	CList<CUart> m_Uart;

public:
	DEF_STATEMACHINE(CUartKeeper);
};

} // namespace Edf

