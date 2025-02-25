/**
  ******************************************************************************
  * @file    USB_Host/DynamicSwitch_Standalone/Src/audio_menu.c
  * @author  MCD Application Team
  * @brief   This file implements Audio Menu Functions
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
uint32_t audio_flag = 0;
FILELIST_FileTypeDef        file_list;
extern uint8_t              prev_select;
AUDIO_DEMO_SelectMode       audio_select_mode;
AUDIO_DEMO_StateMachine     audio_demo;
AUDIO_PLAYBACK_StateTypeDef audio_state;

uint8_t *AUDIO_main_menu[] =
{
  (uint8_t *)"      1 - Explore audio file                                         ",
  (uint8_t *)"      2 - Start audio Player                                         ",
  (uint8_t *)"      3 - Re-Enumerate                                               ",
};

/* Private function prototypes -----------------------------------------------*/
static uint8_t Audio_ShowWavFiles(void);
static void LCD_ClearTextZone(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Manages Audio Menu Process.
  * @param  None
  * @retval None
  */
void AUDIO_MenuProcess(void)
{
  switch(audio_demo.state)
  {
  case AUDIO_DEMO_IDLE:
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    BSP_LCD_DisplayStringAtLine(14, (uint8_t *)"                                                 ");
    BSP_LCD_DisplayStringAtLine(15, (uint8_t *)"Use [Joystick Left/Right] to scroll up/down       ");
    BSP_LCD_DisplayStringAtLine(16, (uint8_t *)"Use [Joystick Up/Down] to scroll audio menu      ");
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    AUDIO_MenuSelectItem(AUDIO_main_menu, 0);
    audio_demo.state = AUDIO_DEMO_WAIT;
    audio_demo.select = 0;
    break;

  case AUDIO_DEMO_WAIT:
    if(audio_demo.select != prev_select)
    {
      prev_select = audio_demo.select;
      AUDIO_MenuSelectItem(AUDIO_main_menu, audio_demo.select & 0x7F);

      /* Handle select item */
      if(audio_demo.select & 0x80)
      {
        audio_demo.select &= 0x7F;

        switch(audio_demo.select)
        {
        case 0:
          audio_demo.state = AUDIO_DEMO_EXPLORE;
          break;

        case 1:
          /* Display HMI messages */
          BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
          BSP_LCD_DisplayStringAtLine(14, (uint8_t *)"                                             ");
          BSP_LCD_DisplayStringAtLine(15, (uint8_t *)"                                             ");
          BSP_LCD_DisplayStringAtLine(16, (uint8_t *)"Use [User Tamper] To Stop and return from player");
          BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

          /* Set PLAYBACK state and start playing 1st file */
          audio_state = AUDIO_STATE_IDLE;
          audio_demo.state = AUDIO_DEMO_PLAYBACK;
          Audio_ChangeSelectMode(AUDIO_PLAYBACK_CONTROL);
          break;

        case 2:
          audio_demo.state = AUDIO_REENUMERATE;
          break;

        default:
          break;
        }
      }
    }
    break;

  case AUDIO_DEMO_EXPLORE:
    if(Appli_state == APPLICATION_AUDIO)
    {
      if(Audio_ShowWavFiles() > 0)
      {
        LCD_ErrLog("There is no WAV file on the microSD.\n");
        Audio_ChangeSelectMode(AUDIO_SELECT_MENU);
      }
      else
      {
        audio_demo.state = AUDIO_DEMO_WAIT;
      }
    }
    break;

  case AUDIO_DEMO_PLAYBACK:
    if(Appli_state == APPLICATION_AUDIO)
    {
      if(audio_state == AUDIO_STATE_IDLE)
      {
        /* Start Playing...*/
        audio_state = AUDIO_STATE_INIT;
        if(AUDIO_Start(0) == AUDIO_ERROR_IO)
        {
          Audio_ChangeSelectMode(AUDIO_SELECT_MENU);
        }
        else
        {
          BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
          BSP_LCD_DisplayStringAtLine(10, (uint8_t *)"[  UP   ] : Volume +");
          BSP_LCD_DisplayStringAtLine(11, (uint8_t *)"[ DOWN  ] : Volume -");
          BSP_LCD_DisplayStringAtLine(12, (uint8_t *)"[ LEFT  ] : Previous");
          BSP_LCD_DisplayStringAtLine(13, (uint8_t *)"[ RIGHT ] : Next");
          BSP_LCD_DisplayStringAtLine(14, (uint8_t *)"[  SEL  ] : Pause/Resume");
          BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        }
      }
      else /* Not idle */
      {
        if(AUDIO_Process() == AUDIO_ERROR_IO)
        {
          Audio_ChangeSelectMode(AUDIO_SELECT_MENU);
        }
      }
    }
    break;

  case AUDIO_REENUMERATE:
    /* Force Audio Device to re-enumerate */
    USBH_ReEnumerate(&hUSBHost);
    audio_demo.state = AUDIO_DEMO_WAIT;
    break;

  default:
    break;
  }
}

/**
  * @brief  Manages the menu on the screen.
  * @param  menu: Menu table
  * @param  item: Selected item to be highlighted
  * @retval None
  */
void AUDIO_MenuSelectItem(uint8_t **menu, uint8_t item)
{
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

  switch(item)
  {
  case 0:
    BSP_LCD_SetBackColor(LCD_COLOR_MAGENTA);
    BSP_LCD_DisplayStringAtLine(17, menu[0]);
    BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
    BSP_LCD_DisplayStringAtLine(18, menu[1]);
    BSP_LCD_DisplayStringAtLine(19, menu[2]);
    break;

  case 1:
    BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
    BSP_LCD_DisplayStringAtLine(17, menu[0]);
    BSP_LCD_SetBackColor(LCD_COLOR_MAGENTA);
    BSP_LCD_DisplayStringAtLine(18, menu[1]);
    BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
    BSP_LCD_DisplayStringAtLine(19, menu[2]);
    break;

  case 2:
    BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
    BSP_LCD_DisplayStringAtLine(17, menu[0]);
    BSP_LCD_DisplayStringAtLine(18, menu[1]);
    BSP_LCD_SetBackColor(LCD_COLOR_MAGENTA);
    BSP_LCD_DisplayStringAtLine(19, menu[2]);
    break;

  default:
    BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
    BSP_LCD_DisplayStringAtLine(17, menu[0]);
    BSP_LCD_DisplayStringAtLine(18, menu[1]);
    BSP_LCD_DisplayStringAtLine(19, menu[2]);
    break;
  }
  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
}

/**
  * @brief  Probes the Audio joystick state.
  * @param  state: Joystick state
  * @retval None
  */
void AUDIO_MenuProbeKey(JOYState_TypeDef state)
{
  /* Handle Joystick inputs */
  if(audio_select_mode == AUDIO_SELECT_MENU)
  {
    /* Handle Menu inputs */
    if((state == JOY_UP) && (audio_demo.select > 0))
    {
      audio_demo.select--;
    }
    else if((audio_demo.select < 2) && (state == JOY_DOWN))
    {
      audio_demo.select++;
    }
    else if(state == JOY_SEL)
    {
      audio_demo.select |= 0x80;
    }
  }
  else
  {
    AUDIO_PlaybackProbeKey(state);
  }
}

/**
  * @brief  Changes the selection mode.
  * @param  select_mode: Selection mode
  * @retval None
  */
void Audio_ChangeSelectMode(AUDIO_DEMO_SelectMode select_mode)
{
  if(select_mode == AUDIO_SELECT_MENU)
  {
    AUDIO_MenuSelectItem(AUDIO_main_menu, 0x00);
    LCD_ClearTextZone();
    LCD_LOG_UpdateDisplay();
    audio_demo.state = AUDIO_DEMO_IDLE;
    AUDIO_Stop();
  }
  else if(select_mode == AUDIO_PLAYBACK_CONTROL)
  {
    LCD_ClearTextZone();
    AUDIO_MenuSelectItem(AUDIO_main_menu, 0xFF);
  }
  audio_select_mode = select_mode;
  audio_demo.select = 0;
}

/**
  * @brief  Shows audio file (*.wav) on the root
  * @param  None
  * @retval None
  */
static uint8_t Audio_ShowWavFiles(void)
{
  uint8_t i;
  uint8_t line_idx = 0;

  if((FileList.ptr > 0) && (BSP_SD_IsDetected()))
  {
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    LCD_UsrLog("audio file(s) [ROOT]:\n\n");

    for( i = 0; i < FileList.ptr; i++)
    {
      line_idx++;
      if(line_idx > 9)
      {
        line_idx = 0;
        LCD_UsrLog("> Press [Tamper] To Continue.\n");

        /* Tamper Button in polling */
        while(BSP_PB_GetState(BUTTON_TAMPER) != SET)
        {
          /* Wait for User Input */
        }
      }
      LCD_DbgLog("   |__");
      LCD_DbgLog((char *)FileList.file[i].name);
      LCD_DbgLog("\n");
    }
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    LCD_UsrLog("\nEnd of files list.\n");
    return 0;
  }
  else
  {
    return 1;
  }
}

/**
  * @brief  Clear the Text Zone
  * @param  None
  * @retval None
  */
static void LCD_ClearTextZone(void)
{
  uint8_t i = 0;

  for(i = 0; i < 12; i++)
  {
    BSP_LCD_ClearStringLine(i + 3);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
