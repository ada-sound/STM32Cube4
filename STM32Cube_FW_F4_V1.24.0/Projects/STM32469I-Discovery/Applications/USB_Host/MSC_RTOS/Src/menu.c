/**
  ******************************************************************************
  * @file    USB_Host/MSC_RTOS/Src/menu.c
  * @author  MCD Application Team
  * @brief   This file implements Menu Functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
MSC_DEMO_StateMachine msc_demo;
osSemaphoreId MenuEvent;
__IO uint8_t ExploreDisk_State = 0;

/* Private function prototypes -----------------------------------------------*/
static void MSC_MenuThread(void const *argument);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Demo state machine.
  * @param  None
  * @retval None
  */
void Menu_Init(void)
{
  /* Create Menu Semaphore */
  osSemaphoreDef(osSem);

  MenuEvent = osSemaphoreCreate(osSemaphore(osSem), 1);

  /* Force menu to show Item 0 by default */
  osSemaphoreRelease(MenuEvent);

  /* Menu task */
  osThreadDef(Menu_Thread, MSC_MenuThread, osPriorityHigh, 0, 8 * configMINIMAL_STACK_SIZE);
  osThreadCreate(osThread(Menu_Thread), NULL);

  msc_demo.state = MSC_DEMO_IDLE;
  USBH_UsrLog("Starting MSC Demo");
}

/**
  * @brief  User task
  * @param  argument not used
  * @retval None
  */
void MSC_MenuThread(void const *argument)
{
  for(;;)
  {
    if(osSemaphoreWait(MenuEvent , osWaitForever) == osOK)
    {
      switch(msc_demo.state)
      {
      case MSC_DEMO_IDLE:
        msc_demo.state = MSC_DEMO_START;
        osSemaphoreRelease(MenuEvent);
        break;

      case MSC_DEMO_START:
        if(Appli_state == APPLICATION_READY)
        {
          BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
          BSP_LCD_DisplayStringAtLine(19, (uint8_t *)"Press User button to start read and write operations");

          msc_demo.state = MSC_DEMO_FILE_OPERATIONS;
	  osSemaphoreRelease(MenuEvent);
        }
        break;

      case MSC_DEMO_FILE_OPERATIONS:
        /* Read and Write File Here */
        if(Appli_state == APPLICATION_READY)
        {
          BSP_LCD_ClearStringLine(19);
          MSC_File_Operations();

          BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
          BSP_LCD_DisplayStringAtLine(19, (uint8_t *)"Press User button to display disk content");
          msc_demo.state = MSC_DEMO_EXPLORER;
	}
        break;

      case MSC_DEMO_EXPLORER:
        /* Display disk content */
        if(Appli_state == APPLICATION_READY)
        {
          BSP_LCD_ClearStringLine(19);
          ExploreDisk_State = 1;

          Explore_Disk("0:/", 1);
          msc_demo.state = MSC_DEMO_START;

          ExploreDisk_State = 0;
          BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
          BSP_LCD_DisplayStringAtLine(19, (uint8_t *)"Press User button to start read and write operations");
        }
        break;

      default:
        break;
      }

      if(Appli_state == APPLICATION_DISCONNECT)
      {
        Appli_state = APPLICATION_IDLE;
        LCD_LOG_ClearTextZone();
        BSP_LCD_ClearStringLine(19);
        LCD_ErrLog("MSC device disconnected!\n");
        msc_demo.state = MSC_DEMO_IDLE;
      }
    }
  }
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == WAKEUP_BUTTON_PIN)
  {
    if(ExploreDisk_State == 0)
    {
      osSemaphoreRelease(MenuEvent);
    }
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
