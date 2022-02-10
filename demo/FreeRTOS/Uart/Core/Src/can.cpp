/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "CanDrv.h"
/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
CAN_HANDLE  CAN_0 = (CAN_HANDLE)&hcan;
DEV_HANDLE  CAN_1 = 0;
DEV_HANDLE  CAN_2 = 0;

typedef struct
{
	uint32_t mailbox;
	CAN_TxHeaderTypeDef hdr;
	uint8_t payload[8];
}CAN_TxPacketTypeDef;

typedef struct
{
	CAN_RxHeaderTypeDef hdr;
	uint8_t payload[8];
}CAN_RxPacketTypeDef;

#define CAN_BASE_ID 0						///< CAN标准ID，最大11位，也就是0x7FF

#define CAN_FILTER_MODE_MASK_ENABLE 1		///< CAN过滤器模式选择：=0：列表模式  =1：屏蔽模式

#define CAN_ID_TYPE_STD_ENABLE      1       ///< CAN过滤ID类型选择：=1：标准ID，=0：扩展ID

/// CAN过滤器寄存器位宽类型定义
typedef union
{
    __IO uint32_t value;
    struct
    {
        uint8_t REV : 1;			///< [0]    ：未使用
        uint8_t RTR : 1;			///< [1]    : RTR（数据帧或远程帧标志位）
        uint8_t IDE : 1;			///< [2]    : IDE（标准帧或扩展帧标志位）
        uint32_t EXID : 18;			///< [21:3] : 存放扩展帧ID
        uint16_t STID : 11;			///< [31:22]: 存放标准帧ID
    } Sub;
} CAN_FilterRegTypeDef;


void CAN_Filter_Config(CAN_HandleTypeDef *Can)
{
    CAN_FilterTypeDef sFilterConfig;
    CAN_FilterRegTypeDef IDH = {0};
    CAN_FilterRegTypeDef IDL = {0};

#if CAN_ID_TYPE_STD_ENABLE
    IDH.Sub.STID = (CAN_BASE_ID >> 16) & 0xFFFF;		// 标准ID高16位
    IDL.Sub.STID = (CAN_BASE_ID & 0xFFFF);				// 标准ID低16位
#else
    IDH.Sub.EXID = (CAN_BASE_ID >> 16) & 0xFFFF;		// 扩展ID高16位
    IDL.Sub.EXID = (CAN_BASE_ID & 0xFFFF);				// 扩展ID低16位
    IDL.Sub.IDE  = 1;									// 扩展帧标志位置位
#endif
    sFilterConfig.FilterBank           = 0;												// 设置过滤器组编号
#if CAN_FILTER_MODE_MASK_ENABLE
    sFilterConfig.FilterMode           = CAN_FILTERMODE_IDMASK;							// 屏蔽位模式
#else
    sFilterConfig.FilterMode           = CAN_FILTERMODE_IDLIST;							// 列表模式
#endif
    sFilterConfig.FilterScale          = CAN_FILTERSCALE_32BIT;							// 32位宽
    sFilterConfig.FilterIdHigh         = IDH.value;										// 标识符寄存器一ID高十六位，放入扩展帧位
    sFilterConfig.FilterIdLow          = IDL.value;										// 标识符寄存器一ID低十六位，放入扩展帧位
    sFilterConfig.FilterMaskIdHigh     = IDH.value;										// 标识符寄存器二ID高十六位，放入扩展帧位
    sFilterConfig.FilterMaskIdLow      = IDL.value;										// 标识符寄存器二ID低十六位，放入扩展帧位
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;									// 过滤器组关联到FIFO0
    sFilterConfig.FilterActivation     = ENABLE;										// 激活过滤器
    sFilterConfig.SlaveStartFilterBank = 14;											// 设置从CAN的起始过滤器编号，本单片机只有一个CAN，顾此参数无效
    if (HAL_CAN_ConfigFilter(Can, &sFilterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

bool Can_Init(CAN_HANDLE Can, CanConfig* Config)
{
	if (Can == CAN_0)
	{
		MX_CAN_Init();
	    CAN_Filter_Config((CAN_HandleTypeDef *)Can);
	    HAL_CAN_Start((CAN_HandleTypeDef *)Can);
	    HAL_CAN_ActivateNotification((CAN_HandleTypeDef *)Can, CAN_IT_RX_FIFO0_MSG_PENDING);					// 使能CAN接收中断
	}
	else
	{
		return false;
	}

    return true;
}

uint8_t CAN_Transmit(CAN_TxPacketTypeDef* packet)
{
	return HAL_CAN_AddTxMessage(&hcan, &packet->hdr, packet->payload, &packet->mailbox);
}

CAN_TxPacketTypeDef g_CanTxPacket;
bool Can_Send(DEV_HANDLE Can, uint8_t* Data, uint16_t DataLen)
{
	g_CanTxPacket.hdr.StdId = 0x321;			// 标准ID
//	g_CanTxPacket.hdr.ExtId = 0x10F01234;		// 扩展ID
	g_CanTxPacket.hdr.IDE = CAN_ID_STD;			// 标准ID类型
//	g_CanTxPacket.hdr.IDE = CAN_ID_EXT;			// 扩展ID类型
	g_CanTxPacket.hdr.DLC = 8;					// 数据长度
	g_CanTxPacket.hdr.RTR = CAN_RTR_DATA;		// 数据帧
//	g_CanTxPacket.hdr.RTR = CAN_RTR_REMOTE;		// 远程帧
	g_CanTxPacket.hdr.TransmitGlobalTime = DISABLE;

	if (DataLen > 8)
	{
		return false;
	}

	for(int i = 0; i < DataLen; i++)
	{
		g_CanTxPacket.payload[i] = i;
	}

    return (HAL_OK == CAN_Transmit(&g_CanTxPacket));
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{

}
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{

}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{

}
void HAL_CAN_TxMailbox0AbortCallback(CAN_HandleTypeDef *hcan)
{

}
void HAL_CAN_TxMailbox1AbortCallback(CAN_HandleTypeDef *hcan)
{

}
void HAL_CAN_TxMailbox2AbortCallback(CAN_HandleTypeDef *hcan)
{

}
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxPacketTypeDef packet;

    // CAN数据接收
    if (hcan == (CAN_HandleTypeDef *)CAN_0)
    {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &packet.hdr, packet.payload) == HAL_OK)		// 获得接收到的数据头和数据
        {
			Can_Recv(CAN_0, packet.payload, packet.hdr.DLC);
			HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);						// 再次使能FIFO0接收中断
        }
    }
}

void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)
{

}
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

}
void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan)
{

}
void HAL_CAN_SleepCallback(CAN_HandleTypeDef *hcan)
{

}
void HAL_CAN_WakeUpFromRxMsgCallback(CAN_HandleTypeDef *hcan)
{

}
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{

}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
