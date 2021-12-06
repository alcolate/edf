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

#include <iostream>

#include "edf.h"


class CHello : public CActive
{
public:
	CHello() : CActive((char*)"AT"), m_Time(TIMEOUT_SIG, this)
	{

	}

	static CHello* Instance()
	{
		static CHello at;
		return &at;
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
			std::cout << "enter " << __FUNCTION__ << std::endl;
			{
				Event* de = new Event(RF_WTR_SIG, true);
				Publish(de);
			}
			break;
		case EXIT_SIG:
			std::cout <<  "exit " << __FUNCTION__ << std::endl;
			break;

		case TIMEOUT_SIG:
			m_Time.Trigger(MilliSecond(500 + (((uint32_t)this) % 500)), 0U);
			
			TRANS(&CHello::S_Noise);
			break;
		case RF_WTR_SIG:
			std::cout << "RF" << std::endl;
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
			std::cout << "enter " << __FUNCTION__ << std::endl;
			{
				Event* de = new Event(RF_WTR_SIG, true);
				Publish(de);
			}
			break;
		case EXIT_SIG:
			std::cout << "exit " << __FUNCTION__ << std::endl;
			break;

		case TIMEOUT_SIG:
			m_Time.Trigger(MilliSecond(500 + (((uint32_t)this) % 500)), 0U);
			TRANS(&CHello::S_Silence);
			break;
		case RF_WTR_SIG:

			std::cout << "RF" << std::endl;
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

