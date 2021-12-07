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

class CRfEvent : public Event
{
public:
	CRfEvent(Signal Sig, const char *Str) : Event(Sig, true)
	{
		m_Name = new char[strlen(Str) + 1];
		strcpy(m_Name, Str);
	}
	virtual ~CRfEvent()
	{
		LOG_DEBUG("%s, %s \r\n", __FUNCTION__,  m_Name);
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

		Edf::Subscribe(RF_WTR_SIG, this);
		INIT_TRANS(&CHello::S_Silence);
	}

	void S_Silence(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
			m_Time.Trigger(100, 0U);
			LOG_DEBUG("enter %s\r\n", __FUNCTION__);
			{
				Event* de = new CRfEvent(RF_WTR_SIG, __FUNCTION__);
				Publish(de);
			}
			break;
		case EXIT_SIG:
			LOG_DEBUG("exit %s\r\n", __FUNCTION__);
			break;

		case TIMEOUT_SIG:
			m_Time.Trigger(MilliSecond(500 + (((uint32_t)this) % 500)), 0U);
			
			TRANS(&CHello::S_Noise);
			break;
		case RF_WTR_SIG:
			LOG_DEBUG("RF %s\r\n", __FUNCTION__);
			break;

		default:
			break;
		}
	}

	void S_Noise(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
			m_Time.Trigger(100, 0U);
			LOG_DEBUG("enter %s\r\n", __FUNCTION__);
			{
				Event* de = new CRfEvent(RF_WTR_SIG, __FUNCTION__);
				Publish(de);
			}
			break;
		case EXIT_SIG:
			LOG_DEBUG("exit %s\r\n", __FUNCTION__);
			break;

		case TIMEOUT_SIG:
			m_Time.Trigger(MilliSecond(500 + (((uint32_t)this) % 500)), 0U);
			TRANS(&CHello::S_Silence);
			break;

		case RF_WTR_SIG:
			LOG_DEBUG("RF %s\r\n", __FUNCTION__);
			break;

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

		Edf::Subscribe(RF_WTR_SIG, this);
		INIT_TRANS(&CWorld::S_Silence);
	}

	void S_Silence(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:

			LOG_DEBUG("enter %s\r\n", __FUNCTION__);

			break;
		case EXIT_SIG:
			LOG_DEBUG("exit %s\r\n", __FUNCTION__);
			break;

		case RF_WTR_SIG:
			LOG_DEBUG("RF %s\r\n", __FUNCTION__);
			break;

		default:
			break;
		}
	}

public:
	DEF_STATEMACHINE(CWorld);
};

