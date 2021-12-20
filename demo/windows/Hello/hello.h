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
#include <atomic>
#include <random>

#include "Edf.h"

#pragma warning(disable : 4996)

std::atomic<uint32_t> create_c(0);
std::atomic<uint32_t> recycle_c(0);

enum
{
	TEST_SIG = Edf::USER_SIG,
	TEST2_SIG,
	TEST3_SIG,
	TEST4_SIG,
	TEST5_SIG,
	MAX_SIG
};

class CTestEvent : public Edf::Event
{
public:
	CTestEvent(Edf::Signal Sig, const char *Str) : Event(Sig, true)
	{
		m_Name = new char[1 * 1024 * 1024];
		strcpy(m_Name, Str);
		uint32_t c = create_c++;
		LOG_INFO("Create: %d, sig = %d from %s \r\n", c, Sig, m_Name);
	}
	virtual ~CTestEvent()
	{
		uint32_t c = recycle_c++;
		LOG_INFO("Recycle: %d, sig = %d from %s \r\n", c, Sig, m_Name);
		delete [] m_Name;
	}

	char* m_Name;
};

class CHello : public Edf::CActive
{
public:
	CHello() : Edf::CActive((char*)"Hello", 10), m_Time(Edf::TIMEOUT_SIG, this)
	{
		static int ObjCount = 0;
		char* name = new char[50];
		sprintf(name, "%s%3d", m_Name, ObjCount ++);
	}

	static CHello* Instance()
	{
		static CHello hello;
		return &hello;
	}

	virtual void Initial()
	{

		Edf::Subscribe(TEST_SIG, this);
		Edf::Subscribe(TEST5_SIG, this);
		INIT_TRANS(&CHello::State1);
	}

	void State1(Edf::Event const* const e)
	{
		switch (e->Sig)
		{
		case Edf::ENTRY_SIG:
			m_Time.Start(MilliSecond(200 + (rand() % 10)), 0U);
			{
				CTestEvent* de = new CTestEvent(TEST_SIG, __FUNCTION__);
				Publish(de);
			}
			break;
		case Edf::EXIT_SIG:
			break;

		case Edf::TIMEOUT_SIG:
			{
				CTestEvent* de = new CTestEvent(TEST2_SIG, __FUNCTION__);
				Publish(de);
			}			
			
			TRANS(&CHello::State2);
			break;
		case TEST_SIG:
		{
			CTestEvent const* te = static_cast<CTestEvent const*>(e);
			//LOG_DEBUG("msg: %s in %s\r\n", te->m_Name, __FUNCTION__);
			break;
		}
		default:
			break;
		}
	}

	void State2(Edf::Event const* const e)
	{
		switch (e->Sig)
		{
		case Edf::ENTRY_SIG:
			m_Time.Start(MilliSecond(200 + (rand() % 10)), 0U);
			{
				CTestEvent* de = new CTestEvent(TEST_SIG, __FUNCTION__);
				Publish(de);
			}
			break;
		case Edf::EXIT_SIG:
			break;

		case Edf::TIMEOUT_SIG:
			TRANS(&CHello::State1);
			break;

		case TEST_SIG:
		{
			CTestEvent* de = new CTestEvent(TEST3_SIG, __FUNCTION__);
			Publish(de);
		}

		default:
			break;
		}
	}

public:

	Edf::CTimeEvent m_Time;

public:
	DEF_STATEMACHINE(CHello);
};


class CWorld : public Edf::CActive
{
public:
	CWorld() : Edf::CActive((char*)"World", 10)
	{
		static int ObjCount = 0;
		char* name = new char[50];
		sprintf(name, "%s%3d", m_Name, ObjCount ++);
	}

	static CWorld* Instance()
	{
		static CWorld world;
		return &world;
	}

	virtual void Initial()
	{

		Edf::Subscribe(TEST_SIG, this);
		Edf::Subscribe(TEST2_SIG, this);
		Edf::Subscribe(TEST3_SIG, this);
		INIT_TRANS(&CWorld::State1);
	}

	void State1(Edf::Event const* const e)
	{
		switch (e->Sig)
		{
		case Edf::ENTRY_SIG:

			break;
		case Edf::EXIT_SIG:
			break;

		case TEST_SIG:
		{
			DeferEvent(e);
			TRANS(&CWorld::State2);
			break;
		}

		case TEST2_SIG:
		{
			CTestEvent* de = new CTestEvent(TEST4_SIG, __FUNCTION__);
			Publish(de);
		}
		{
			CTestEvent const* te = static_cast<CTestEvent const*>(e);
			//LOG_DEBUG("msg: %s in %s\r\n", te->m_Name, __FUNCTION__);
			TRANS(&CWorld::State2);
			break;
		}

		case TEST3_SIG:
		{
			CTestEvent const* te = static_cast<CTestEvent const*>(e);
			//LOG_DEBUG("msg: %s in %s\r\n", te->m_Name, __FUNCTION__);

			break;
		}		

		default:
			break;
		}
	}

	void State2(Edf::Event const* const e)
	{
		switch (e->Sig)
		{
		case Edf::ENTRY_SIG:
			FetchDeferedEvent();
			break;
		case Edf::EXIT_SIG:
			break;

		case TEST_SIG:
		{
			CTestEvent* de = new CTestEvent(TEST5_SIG, __FUNCTION__);
			Publish(de);
		}
		{
			CTestEvent const* te = static_cast<CTestEvent const*>(e);
			//LOG_DEBUG("msg: %s in %s\r\n", te->m_Name, __FUNCTION__);
			TRANS(&CWorld::State1);
			break;
		}
		case TEST2_SIG:
		{
			CTestEvent* de = new CTestEvent(TEST5_SIG, __FUNCTION__);
			Publish(de);
		}
		{
			CTestEvent const* te = static_cast<CTestEvent const*>(e);
			//LOG_DEBUG("msg: %s in %s\r\n", te->m_Name, __FUNCTION__);

			break;
		}

		case TEST3_SIG:
		{
			CTestEvent const* te = static_cast<CTestEvent const*>(e);
			//LOG_DEBUG("msg: %s in %s\r\n", te->m_Name, __FUNCTION__);
			TRANS(&CWorld::State2);
			break;
		}	
		default:
			break;
		}
	}

public:
	DEF_STATEMACHINE(CWorld);
};

