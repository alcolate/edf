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
************************************s*****************************************/
#include "Active.h"


namespace Edf
{

static CTimeEvent *gTimer = NULL;
static uint32_t gTimerNum = 0; 

CTimeEvent::CTimeEvent(Signal Sig, CActive *Act):Event(Sig)
{
	ASSERT(Act);
	this->m_Act = Act;
	this->m_Timeout = 0U;
	this->m_Interval = 0U;

	m_Next = gTimer;
	gTimer = this;
	++gTimerNum;
}

/*..........................................................................*/
void CTimeEvent::Trigger(uint32_t Timeout, uint32_t Interval)
{
	OS_EnterCritical();
	this->m_Timeout = Timeout;
	this->m_Interval = Interval;
	OS_ExitCritical();
}

/*..........................................................................*/
void CTimeEvent::UnTrigger()
{
	OS_EnterCritical();
	this->m_Timeout = 0U;
	OS_ExitCritical();
}

void CTimeEvent::Touch()
{
	m_Act->Post(this, true);
}

void CTimeEvent::Tick()
{
	CTimeEvent* timer, * p;
	for (p = gTimer; p; p = p->m_Next)
	{
		timer = NULL;
		uint32_t flag = OS_EnterCritical(true);
		if (p->m_Timeout > 0U)
		{
			if (--p->m_Timeout == 0U)
			{
				timer = p;
				p->m_Timeout = p->m_Interval;
			}
		}
		OS_ExitCritical(flag, true);
		if (timer != NULL)
		{
			timer->Touch();
		}
	}
}
} // namespace Edf

void TimeEvent_tickFromISR()
{
	Edf::CTimeEvent::Tick();
}
