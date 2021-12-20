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

#include "Active.h"

namespace Edf
{
class CTimeEvent : public Event
{
public:
	CTimeEvent(Signal Sig, CActive *Act);

	virtual ~CTimeEvent();

	void Start(uint32_t StartPoint, uint32_t Period);

	void Stop();

	uint32_t Pause();

	void Resume();

	void Resume(uint32_t HisPoint);

	void Touch(bool FromISR = false);

	static void Tick(bool FromISR);

	uint32_t GetTimeRemain();

	uint32_t GetTick();

private:
	CActive* m_Act;
	uint32_t m_StartPoint;
	uint32_t m_Period;
	bool m_Paused;
	static uint32_t m_Tick;
};

} // namespace Edf
