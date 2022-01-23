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

class CMacLayer : public CActive
{
public:
	CMacLayer(char* Name, uint32_t DQSize = 0) : CActive(Name, DQSize)
	{

	}

	//CMacLayer * Create(char *Type)

};

class CProtocolStack
{
public:
	virtual CProtocolStack* Create()
	{
		return nullptr;
	}
};





