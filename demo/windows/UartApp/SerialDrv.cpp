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
#include <windows.h>
#include "Serial.h"
#include "../../../third_party/serial/include/serial/serial.h"

DEV_HANDLE  UART_0 = (DEV_HANDLE)"COM1";
DEV_HANDLE  UART_1 = (DEV_HANDLE)"COM2";
DEV_HANDLE  UART_2 = (DEV_HANDLE)"COM3";

static serial::Serial *Serial0;
static serial::Serial* Serial1;
static serial::Serial* Serial2;

DEV_HANDLE SerialToHandle(serial::Serial* Serial)
{
    if (Serial == Serial0)
    {
        return UART_0;
    }
    else if (Serial == Serial1)
    {
        return UART_1;
    }
    else if (Serial == Serial2)
    {
        return UART_2;
    }
    else
    {
        ASSERT(false);
        return 0;
    }
}

serial::Serial** HandleToSerial(DEV_HANDLE Uart)
{
    if (Uart == UART_0)
    {
        return &Serial0;
    }
    else if (Uart == UART_1)
    {
        return &Serial1;
    }
    else if (Uart == UART_2)
    {
        return &Serial2;
    }
    else
    {
        ASSERT(false);
        return NULL;
    }
}

static DWORD WINAPI ThreadExe(LPVOID p)
{
    uint8_t data;
    size_t result;
    serial::Serial* serial = static_cast<serial::Serial*>(p);

    for (;;)
    {
        result = serial->read(&data, 1);

        if (result)
        {
            Uart_Recv(SerialToHandle(serial), data);
        }
    }

    return 0;
}

bool Uart_Init(DEV_HANDLE Uart, UartConfig* Config)
{
    bool isOpen;
    char* port = (char*)Uart;

    serial::Serial** serial;
    
    serial = HandleToSerial(Uart);

    *serial = new serial::Serial(port, Config->Baudrate, serial::Timeout::simpleTimeout(1000));

    isOpen = ( *serial)->isOpen();

    if (isOpen)
    {
        LOG_ERROR("open serial port %s ok\r\n", port);
    }
    else
    {
        LOG_ERROR("open serial port %s error\r\n", port);
        exit(-1);
    }

    DWORD id;

    CreateThread(
        NULL,
        1024,
        &ThreadExe,
        *serial,
        0,
        &id);

    return true;

}
bool Uart_Send(DEV_HANDLE Uart, uint8_t* Data, uint16_t DataLen)
{
    (* HandleToSerial(Uart))->write(Data, DataLen);

    return true;
}




