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

#include "Driver.h"
#include "Edf.h"

namespace Edf
{

class CDeviceEvent : public Event
{
public:
	CDeviceEvent(Signal Sig, DEV_HANDLE HwHandle, uint32_t BuffSize, bool Releasable = true);
	CDeviceEvent(Signal Sig, DEV_HANDLE HwHandle, uint8_t *Data, uint32_t DataLen, bool Releasable = true);
	virtual ~CDeviceEvent();

	bool Copy(const uint8_t *Data, uint32_t Len);

	DEV_HANDLE	m_HwHandle;
	uint8_t* 	m_Data;
	uint32_t 	m_DataLen;
	uint32_t	m_Size;

};

class CSpiEvent : public CDeviceEvent
{
public:
	enum {MAX_SIZE = 256};
public:
	CSpiEvent(Signal Sig, DEV_HANDLE SpiHandle,
			uint8_t *Tx, uint16_t TxLen, uint16_t RxLen, bool Releasable = true);

	virtual ~CSpiEvent();

	uint8_t * Rx();

	uint8_t *m_Tx;
	uint8_t *m_Rx;
	uint16_t m_TxLen;
	uint16_t m_RxLen;
};


class CI2CEvent : public CDeviceEvent
{
public:
	enum MODE
	{
		WRITE,
		READ
	};
public:
	CI2CEvent(Signal Sig, DEV_HANDLE I2C, uint8_t *Data, uint32_t DataLen, bool Releasable = true);

	virtual ~CI2CEvent();

	uint8_t  m_SlaveAddress;
	uint8_t  m_RegAddress;
	MODE	m_Mode;
};

class CPWMEvent : public CDeviceEvent
{
public:
	CPWMEvent(Signal Sig, DEV_HANDLE PWM, uint32_t Steps, bool Releasable = true);

	virtual ~CPWMEvent();

	uint32_t m_Steps;
};

class CGPIOEvent : public CDeviceEvent
{
public:
	enum MODE
	{
		LOW,
		HIGH,
		TOGGLE,
	};
public:
	CGPIOEvent(Signal Sig, DEV_HANDLE GPIO, MODE Mode, bool Releasable = true);

	virtual ~CGPIOEvent();

	MODE m_Mode;
};

class CAdcEvent : public CDeviceEvent
{
public:
	enum MODE
	{
		INTERRUPT,
		DMA
	};
	enum {MAX_CHANNEL = 16};
public:
	CAdcEvent(Signal Sig, DEV_HANDLE ADC, bool Releasable = true);

	virtual ~CAdcEvent();

	uint16_t m_Result[MAX_CHANNEL];
	uint32_t m_ResLen;
};





class CDevice;
using LowMacCallBack = bool (*)(CDevice *Device, uint8_t *Data, uint32_t Len);
using LowPostIrqRecvEvent = void (*)(CDevice *Device);

enum class EDeviceType
{
	UART,
	SPI,
	CAN,
	I2C,
	GPIO,
	PWM,
	ADC
};

class CDevKeeper;

class CDevice
{
public:
	CDevice(const char *Name, DEV_HANDLE HwHandle, EDeviceType Type, uint32_t DQSize = 2);
	virtual ~CDevice();

	virtual bool Reset();

	virtual bool IsMe(DEV_HANDLE DeviceH);

	virtual bool IsMe(char* Name, EDeviceType Type);

private:
	void Dispatcher(Event const* const e);

	bool DeferEvent(Event const* const e);

	Event const * FetchDeferedEvent();

	void RecycleEvent(Event const* const e);

	void S_Idle(Event const* const e);
	void S_Sending(Event const* const e);

protected:
	virtual bool Send(Event const* const e) = 0;
	virtual void SendSuccess(Event const* const e);
	virtual void SendFail(Event const* const e);
	virtual void Initial(CActive *Owner);
	//virtual void PostIrqRecvEvent();
	//virtual bool MacCall(uint8_t *Data, uint32_t Len);
	void CreateIrqSendCompleteEvent();
	void PostIrqSendCompleteEvent();

	void SetCallback(LowMacCallBack MacCB, LowPostIrqRecvEvent IrqRecvCB);

public:
	enum class MODE
	{
		MODE_SYNC,
		MODE_ASYNC,
	};


	DEV_HANDLE  GetHwHandle() {return m_HwHandle;}

protected:

	DEV_HANDLE  m_HwHandle;			// the handle. of Device hardware

	char m_Name[10];

	EDeviceType m_Type;

	CDeviceEvent  *m_IrqSendCompleteEvent;

	Event const *m_SendingEvent;
	CEventQ *m_DQ;
	CActive* m_Owner;

	friend CDevKeeper;

private:
	LowMacCallBack m_MacCallback;

	LowPostIrqRecvEvent m_PostIrqRecvEvent;

private:
	DEF_STATEMACHINE(CDevice);
};

class CDevKeeper : public CActive
{
public:

	static CDevKeeper* Instance();

	virtual void Start() override;

	void RegDevice(CDevice* Device);

	virtual void Initial() override;

	void SendComplete(DEV_HANDLE DevHandle);

	void Receive(DEV_HANDLE DeviceH, uint8_t *Data, uint32_t Len);

	CDevice * GetDevice(char* Name, EDeviceType Type);

	CDevice* GetDevice(DEV_HANDLE DevHandle);

private:
	void S_Run(Event const* const e);

	void AddDevice(CDevice* Device);

	CDevKeeper();

	CList<CDevice> m_Device;

public:
	DEF_STATEMACHINE(CDevKeeper);
};

} // namespace Edf

