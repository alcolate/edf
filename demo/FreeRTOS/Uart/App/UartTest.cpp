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
// Hello.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "UartTest.h"
#include "Edf.h"
#include "Serial.h"
#include "Led.h"

class CAppUart : public CUart
{
public:
	CAppUart() : CUart((char*)"AppUart")
	{
		m_Uart_H = UART_0;
		m_Config.Baudrate = 9600;
		m_Config.Parity = Parity_None;
		m_Config.StopBits = StopBit_1Bit;
		m_BuffSize = CUartEvent::FRAME_MAX_LEN + 1;
		m_Buff4MacCall = new uint8_t [1 * 1024  + 1];
		ASSERT(m_Buff4MacCall);
		m_BuffCount = 0;
		m_IrqEvent = new CUartEvent(m_Uart_H);
	}
	~CAppUart()
	{
		if (m_Buff4MacCall)
			delete [] m_Buff4MacCall;
	}
	virtual bool MacCall(uint8_t AByte)
	{
		if (m_BuffCount < m_BuffSize)
		{
			m_Buff4MacCall[m_BuffCount++] = AByte;
			if (AByte == '\n')
			{
				m_Buff4MacCall[m_BuffCount++] = 0;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			// frame error occurs, reset it
			m_BuffCount = 0;
			return false;
		}

	}
private:
	UartConfig m_Config;
};

class CAPP : public CActive
{
public:
	virtual void Initial()
	{
		m_Led = new CLed();
		m_Led->Initial();
		m_Uart = new CAppUart();
		CUartKeeper::Instance()->RegUart(m_Uart);
		Edf::Subscribe(SERIAL_IN_SIG, this);
		INIT_TRANS(&CAPP::S_Idle);
	}

	void S_Idle(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
		{
			m_Time.Trigger(MilliSecond(50), 0);
			Event e(CLed::OFF);
			m_Led->Dispatcher(&e);
			break;
		}

		case TIMEOUT_SIG:
		{
			Request();

			TRANS(&CAPP::S_WaitReponse);

		}
		break;


		default:
			break;
		}
	}

	void S_WaitReponse(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
		{
			Event e(CLed::ON);
			m_Led->Dispatcher(&e);
			m_Time.Trigger(MilliSecond(1000), 0);
			break;
		}
		case SERIAL_IN_SIG:
		{
			CUartEvent const* ue = static_cast<CUartEvent const*>(e);
			if (ue->Uart_H == m_Uart->m_Uart_H)
			{
				Response(ue->Data, ue->DataLen);

				m_Time.UnTrigger();
				TRANS(&CAPP::S_Idle);
			}
			break;
		}
		case TIMEOUT_SIG:
		{
			LOG_DEBUG("Wait Response Timeout \r\n");
			TRANS(&CAPP::S_RetryRequest);
		}
		break;

		default:
			break;
		}
	}

	void S_RetryRequest(Event const* const e)
	{
		switch (e->Sig)
		{
		case ENTRY_SIG:
			m_Time.Trigger(MilliSecond(20), 0);
			break;

		case TIMEOUT_SIG:
		{
			Request();

			TRANS(&CAPP::S_WaitReponse);
		}
		break;


		default:
			break;
		}
	}

	void Request(void)
	{
		char tosend[100];
		static uint32_t cc = 0;
		sprintf(tosend, "hello uart %d \r\n", cc++);
		uint16_t len = strlen(tosend) + 1;
		LOG_DEBUG("app send: %s\r\n", tosend);
		CUartEvent* ue = new CUartEvent(SERIAL_OUT_SIG, m_Uart->m_Uart_H, (uint8_t*)tosend, len);
		Publish(ue);
	}

	void Response(const uint8_t* Data, uint16_t Len)
	{
		const uint8_t* response = Data;
		LOG_DEBUG("app get: %s\r\n", response);
		// do your work
	}

public:
	CAPP() : CActive((char*)"APP"), m_Time(TIMEOUT_SIG, this)
	{
		m_Uart = 0;
	}

public:
	CUart* m_Uart;
	CTimeEvent m_Time;
	CLed* m_Led;

public:
	DEF_STATEMACHINE(CAPP);
};


void App_Start(void)
{
    std::cout << "Hello Uart!\n";

    CUartKeeper::Instance()->Start();

    CAPP app;

    app.Start();


    Edf::EdfStart();
}



// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
