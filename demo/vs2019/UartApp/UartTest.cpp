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
// Hello.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "UartTest.h"

UARTDEV_H  UART_0 = (UARTDEV_H)1;
UARTDEV_H  UART_1 = (UARTDEV_H)2;
UARTDEV_H  UART_2 = (UARTDEV_H)3;

// the three functions are mocks for uart
bool Uart_Init(UARTDEV_H Uart, UartConfig* Config)
{
    return true;
}
bool Uart_Send(UARTDEV_H Uart, uint8_t* Data, uint16_t DataLen)
{
    char msg[] = "yes, I am Uart\r\n";

    for (size_t i = 0; i < strlen(msg); i ++)
    {
        CUartKeeper::Instance()->Receive(Uart, msg[i]);
    }
    return true;
}

void Uart_Isr(void)
{
    CUartKeeper::Instance()->Receive(UART_0, 1);
}




int main()
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
