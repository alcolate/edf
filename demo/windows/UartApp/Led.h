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
			//LOG_DEBUG("led on\r\n");
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
			//LOG_DEBUG("led off\r\n");
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

