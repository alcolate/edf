/*****************************************************************************
* MIT License:
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*
* Contact information:
* <9183399@qq.com>
*****************************************************************************/
#pragma once

#include <string.h>

#include "edf.h"

#pragma warning(disable : 4996)

class CTestEvent : public Event
{
public:
	CTestEvent(Signal Sig, const char *Str) : Event(Sig, true)
	{
		m_Name = new char[strlen(Str) + 1];
		strcpy(m_Name, Str);
	}
	virtual ~CTestEvent()
	{
		LOG_DEBUG("Recycle: %s, %s \r\n", __FUNCTION__,  m_Name);
		delete [] m_Name;
	}

	char* m_Name;
};

class CHello : public CActive
{
public:
	CHello() : CActive((char*)"Hello"), m_Time(TIMEOUT_SIG, this)
	{

	}

	static CHello* Instance()
	{
		static CHello hello;
		return &hello;
	}

	virtual void Initial()
	{

		Edf::Subscribe(TEST_SIG, this);
		INIT_TRANS(&CHello::State1);
	}

	void State1(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
			m_Time.Trigger(100, 0U);
			{
				CTestEvent* de = new CTestEvent(TEST_SIG, __FUNCTION__);
				Publish(de);
			}
			break;
		case EXIT_SIG:
			break;

		case TIMEOUT_SIG:
			m_Time.Trigger(MilliSecond(500 + (((uint32_t)this) % 500)), 0U);
			
			TRANS(&CHello::State2);
			break;
		case TEST_SIG:
		{
			CTestEvent const* te = static_cast<CTestEvent const*>(e);
			LOG_DEBUG("msg: %s in %s\r\n", te->m_Name, __FUNCTION__);
			break;
		}
		default:
			break;
		}
	}

	void State2(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
			m_Time.Trigger(100, 0U);
			{
				CTestEvent* de = new CTestEvent(TEST_SIG, __FUNCTION__);
				Publish(de);
			}
			break;
		case EXIT_SIG:
			break;

		case TIMEOUT_SIG:
			m_Time.Trigger(MilliSecond(500 + (((uint32_t)this) % 500)), 0U);
			TRANS(&CHello::State1);
			break;

		case TEST_SIG:
		{
			CTestEvent const* te = static_cast<CTestEvent const*>(e);
			LOG_DEBUG("msg: %s in %s\r\n", te->m_Name, __FUNCTION__);
			break;
		}

		default:
			break;
		}
	}

public:

	CTimeEvent m_Time;

public:
	DEF_STATEMACHINE(CHello);
};


class CWorld : public CActive
{
public:
	CWorld() : CActive((char*)"World")
	{

	}

	static CWorld* Instance()
	{
		static CWorld world;
		return &world;
	}

	virtual void Initial()
	{

		Edf::Subscribe(TEST_SIG, this);
		INIT_TRANS(&CWorld::State1);
	}

	void State1(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:

			break;
		case EXIT_SIG:
			break;

		case TEST_SIG:
		{
			CTestEvent const* te = static_cast<CTestEvent const*>(e);
			LOG_DEBUG("msg: %s in %s\r\n", te->m_Name, __FUNCTION__);
			TRANS(&CWorld::State2);
			break;
		}

		default:
			break;
		}
	}

	void State2(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:

			break;
		case EXIT_SIG:
			break;

		case TEST_SIG:
		{
			CTestEvent const* te = static_cast<CTestEvent const*>(e);
			LOG_DEBUG("msg: %s in %s\r\n", te->m_Name, __FUNCTION__);
			TRANS(&CWorld::State1);
			break;
		}

		default:
			break;
		}
	}

public:
	DEF_STATEMACHINE(CWorld);
};

