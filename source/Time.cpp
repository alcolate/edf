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
#include "Active.h"


namespace Edf
{

CTimeEvent *l_tevt[10]; /* all TimeEvents in the application */
uint_fast8_t l_tevtNum; /* current number of TimeEvents */

CTimeEvent::CTimeEvent(Signal Sig, CActive *Act):Event(Sig)
{
	/* no critical section because it is presumed that all TimeEvents
	 * are created *before* multitasking has started.
	 */
	this->m_Act = Act;
	this->m_Timeout = 0U;
	this->m_Interval = 0U;

	/* register one more TimeEvent with the application */
	ASSERT(l_tevtNum < sizeof(l_tevt)/sizeof(l_tevt[0]));
	l_tevt[l_tevtNum] = this;
	++l_tevtNum;
}

/*..........................................................................*/
void CTimeEvent::Trigger(uint32_t timeout, uint32_t interval)
{
	//taskENTER_CRITICAL();
	this->m_Timeout = timeout;
	this->m_Interval = interval;
	//taskEXIT_CRITICAL();
}

/*..........................................................................*/
void CTimeEvent::UnTrigger()
{
	//taskENTER_CRITICAL();
	this->m_Timeout = 0U;
	//taskEXIT_CRITICAL();
}


} // namespace Edf

void TimeEvent_tickFromISR()
{
	uint_fast8_t i;
	for (i = 0U; i < Edf::l_tevtNum; ++i)
	{
		Edf::CTimeEvent * t = Edf::l_tevt[i];
		ASSERT(t); 
		if (t->m_Timeout > 0U) 
		{
			if (--t->m_Timeout == 0U)  
			{
				QueueSend(t->m_Act->Q(), t, true );
				t->m_Timeout = t->m_Interval;
			}
		}
	}
}
