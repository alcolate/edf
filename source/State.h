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

#define DEF_STATE(Class) \
		void (Class::*m_State)(Event const *const e); \
		void (Class::*m_NextState)(Event const *const e); \
		virtual void RunState(Event const * const e)\
		{\
			(this->*m_State)(e);\
		}\
		virtual void ToNextState()\
		{\
			m_State = m_NextState;\
		}

#define STATE() (m_State)
#define NEXT_STATE() (m_NextState)
#define RUN_STATE()  RunState(e)
#define TO_NEXT_STATE() ToNextState()
#define INIT_TRANS(state) \
		do {\
			m_State = state; \
			static Event e_entry (ENTRY_SIG);\
			this->Post(&e_entry); \
		}while(0)

#define TRANS(to) \
		do {\
			static Event e_exit (EXIT_SIG);\
			this->Post(&e_exit); \
			m_NextState = to;\
			static Event e_entry (ENTRY_SIG);\
			this->Post(&e_entry); \
		}while(0)



namespace Edf
{

} // namespace edf
