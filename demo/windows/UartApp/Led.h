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
#include "Edf.h"

class CLed
{
public:
	CLed()
	{

	}

	enum
	{
		ON = USER_SIG,
		OFF,
	};

	void Dispatcher(Event const* const e)
	{
		RunState(e);
	}

	void Initial()
	{
		INIT_TRANS(&CLed::S_On);
	}

	void S_On(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
			LOG_DEBUG("led on\r\n");
			break;

		case OFF:
			
			TRANS(&CLed::S_Off);
		break;

		default:
			break;
		}
	}
	void S_Off(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
			LOG_DEBUG("led off\r\n");
			break;

		case ON:
			
			TRANS(&CLed::S_On);
			break;

		default:
			break;
		}
	}
public:
	DEF_STATEMACHINE(CLed);
};

