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

#include <string.h>

#include "Edf.h"
#include "Serial.h"
#include "Led.h"
#include "Hdlc.h"
#pragma warning(disable : 4996)

uint32_t MAX_SIG = USER_SIG;

class CSession : public CActive
{
public:
	virtual void Initial()
	{
		m_Led = new CLed();
		m_Led->Initial();
		m_MacLayer = new CHdlc();
		m_MacLayer->Start();
		
		Edf::Subscribe(MAC_RSP_SIG, this);
		INIT_TRANS(&CSession::S_Idle);
	}

	void S_Idle(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
		{
			m_Time.Trigger(MilliSecond(50), 0);
			Event e(CLed::OFF);
			//m_Led->Dispatcher(&e);
			break;
		}
		case MAC_RSP_SIG:
		{
			CMacEvent const* ue = static_cast<CMacEvent const*>(e);

			LOG_INFO("(%s) get : %s\r\n", __FUNCTION__, ue->m_Data);

			if (ue->m_Type == CMacEvent::GET_DATA)
			{
				
			}
			break;
		}
		case TIMEOUT_SIG:
		{
			Request();

			TRANS(&CSession::S_WaitReponse);
		}
		break;

		default:
			break;
		}
	}

	void S_WaitReponse(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
		{
			Event e(CLed::ON);
			//m_Led->Dispatcher(&e);
			m_Time.Trigger(MilliSecond(1000), 0);
			break;
		}
		case MAC_RSP_SIG:
		{
			CMacEvent const* ue = static_cast<CMacEvent const*>(e);			

			LOG_INFO("(%s) get : %s\r\n", __FUNCTION__, ue->m_Data);

			if (ue->m_Type == CMacEvent::GET_DATA)
			{
				if (strstr((char*)ue->m_Data, "hello") != NULL)
				{
					LOG_DEBUG("get response\r\n");
					TRANS(&CSession::S_Idle);
				}				
			}			
			break;
		}
		case TIMEOUT_SIG:
		{
			LOG_DEBUG("Wait Response Timeout \r\n");
			TRANS(&CSession::S_Idle);
		}
		break;

		default:
			break;
		}
	}


	void Request()
	{
		static uint32_t times = 0;
		char Data[32];
		sprintf(Data, "hello world %06d", ++times);
		LOG_INFO("(%s) send: %s\r\n", __FUNCTION__, Data);
		CAppEvent*he = new CAppEvent(APP_REQ_SIG, (uint8_t *)Data, strlen(Data) + 1);
		Publish(he);
	}

public:
	CSession() : CActive((char*)"Session"), m_Time(TIMEOUT_SIG, this)
	{
		m_MacLayer = 0;
	}
	~CSession()
	{
		delete m_MacLayer;
		delete m_Led;
	}

public:
	CHdlc* m_MacLayer;
	CTimeEvent m_Time;
	CLed* m_Led;

public:
	DEF_STATEMACHINE(CSession);
};
