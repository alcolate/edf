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

#include "yahdlc.h"
#include "Edf.h"
#include "Serial.h"


class CAppEvent : public Event
{
public:
	CAppEvent(Signals Sig, const uint8_t* Data, uint32_t Len) : Event(Sig, true)
	{
		m_DataLen = Len;
		m_Data = NULL;
		if (Len)
		{
			m_Data = new uint8_t[Len];
			ASSERT(m_Data);
			memcpy(m_Data, Data, Len);
		}
		
	}

	~CAppEvent()
	{
		if (m_Data)
			delete[] m_Data;
	}

public:

	uint8_t* m_Data;
	uint32_t m_DataLen;
};


class CMacEvent : public Event
{
public:
	enum Type
	{
		SEND_ERROR,
		SEND_BUSY,
		GET_DATA,
	};
public:
	CMacEvent(Signals Sig, CMacEvent::Type Type, const uint8_t* Data, uint32_t Len) : Event(Sig, true)
	{
		m_Type = Type;
		m_DataLen = Len;
		m_Data = NULL;
		if (Len)
		{
			m_Data = new uint8_t[Len];
			ASSERT(m_Data);
			memcpy(m_Data, Data, Len);
		}
		
	}

	~CMacEvent()
	{
		if (m_Data)
			delete[] m_Data;
	}

public:
	//UART_HANDLE	Device;
	Type m_Type;
	uint8_t* m_Data;
	uint32_t m_DataLen;
};



class CMacLayer;

class CHdlc
{
public:
	CHdlc(CMacLayer* MacLayer);

    enum
    {
        DELIMETER = YAHDLC_FLAG_SEQUENCE
    };

	void PacketData(const uint8_t* Data, uint16_t Len);

	void Ack(yahdlc_control_t* Control);

	bool Parser(const uint8_t* Data, uint32_t Len);

private:
    yahdlc_control_t m_ControlSend;
    yahdlc_control_t m_ControlRecv;

    CMacLayer *m_MacLayer;

};

class CMacLayer : public CActive
{
public:
	CMacLayer();
	~CMacLayer();
	static bool MacCall(uint8_t* Buff, uint16_t& BuffSize, uint16_t& BuffCount, uint8_t AByte);

	void Initial();

	void S_Idle(Event const* const e);
	void S_WaitResponse(Event const* const e);

	void S_ReSend(Event const* const e);

	inline bool IsMe(Event const* const e);

	void Request(const uint8_t* Data, uint32_t Len);

public:
	uint32_t m_Retries;
	// lower layer
	CUart *m_Uart;
	CHdlc *m_Hdlc;
	CTimeEvent m_Timer;
	// upper layer


public:
	DEF_STATEMACHINE(CMacLayer);
};
