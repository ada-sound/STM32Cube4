/**
  ******************************************************************************
  * @file    USB_Host/AUDIO_Standalone/Src/audio.c 
  * @author  MCD Application Team
  * @brief   This file provides the Audio Out (playback) interface API
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright � 2017 STMicroelectronics International N.V. 
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
/* Includes ------------------------------------------------------------------ */
#include "main.h"

/* Private define ------------------------------------------------------------ */
#define AUDIO_BLOCK_SIZE  512
#define AUDIO_BLOCK_NBR   33

/* Private macro ------------------------------------------------------------- */
/* Private typedef ----------------------------------------------------------- */
typedef struct
{
  uint8_t buff[AUDIO_BLOCK_SIZE * AUDIO_BLOCK_NBR];
  int32_t out_ptr;
  uint32_t in_ptr;
} Audio_BufferTypeDef;

/* Private variables --------------------------------------------------------- */
static WAV_InfoTypedef WavInfo;
static FIL WavFile;
static Audio_BufferTypeDef BufferCtl;
static int16_t FilePos = 0;

/* Private function prototypes ----------------------------------------------- */
static AUDIO_ErrorTypeDef AUDIO_GetFileInfo(uint16_t file_idx,
                                            WAV_InfoTypedef * info);

/* Private functions --------------------------------------------------------- */

/**
  * @brief  Starts Audio streaming.    
  * @param  idx: File index
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_Start(uint8_t idx)
{
  uint32_t bytesread;

  if ((FileList.ptr > idx) && (BSP_SD_IsDetected()))
  {
    f_close(&WavFile);

    AUDIO_GetFileInfo(idx, &WavInfo);

    if (WavInfo.AudioFormat != 0x01)
    {
      audio_state = AUDIO_STATE_ERROR;
      return AUDIO_ERROR_NONE;
    }
    else
    {
      audio_state = AUDIO_STATE_CONFIG;
    }

    /* Set Frequency */
    USBH_AUDIO_SetFrequency(&hUSBHost,
                            WavInfo.SampleRate,
                            WavInfo.NbrChannels, WavInfo.BitPerSample);

    /* Fill whole buffer at first time */
    if (f_read(&WavFile,
               &BufferCtl.buff[0],
               AUDIO_BLOCK_SIZE * AUDIO_BLOCK_NBR, (void *)&bytesread) == FR_OK)
    {
      if (bytesread != 0)
      {
        return AUDIO_ERROR_NONE;
      }
    }

    BufferCtl.in_ptr = 0;
  }
  return AUDIO_ERROR_IO;
}

/**
  * @brief  Manages Audio process. 
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_Process(void)
{
  int32_t diff;
  uint32_t bytesread, elapsed_time;
  static uint32_t prev_elapsed_time = 0xFFFFFFFF;
  uint8_t str[16];
  AUDIO_ErrorTypeDef error_state = AUDIO_ERROR_NONE;

  switch (audio_state)
  {
  case AUDIO_STATE_PLAY:
    if ((BufferCtl.out_ptr = USBH_AUDIO_GetOutOffset(&hUSBHost)) < 0) /* End of 
                                                                       * file */
    {
      audio_state = AUDIO_STATE_NEXT;
    }
    else if (BufferCtl.out_ptr >= (AUDIO_BLOCK_SIZE * AUDIO_BLOCK_NBR)) /* End
                                                                         * of
                                                                         * buffer 
                                                                         */
    {
      USBH_AUDIO_ChangeOutBuffer(&hUSBHost, &BufferCtl.buff[0]);
    }
    else
    {
      diff = BufferCtl.out_ptr - BufferCtl.in_ptr;

      if (diff < 0)
      {
        diff = AUDIO_BLOCK_SIZE * AUDIO_BLOCK_NBR + diff;
      }

      if (diff >= (AUDIO_BLOCK_SIZE * AUDIO_BLOCK_NBR / 2))
      {
        BufferCtl.in_ptr += AUDIO_BLOCK_SIZE;

        if (BufferCtl.in_ptr >= (AUDIO_BLOCK_SIZE * AUDIO_BLOCK_NBR))
        {
          BufferCtl.in_ptr = 0;
        }

        if (f_read(&WavFile,
                   &BufferCtl.buff[BufferCtl.in_ptr],
                   AUDIO_BLOCK_SIZE, (void *)&bytesread) != FR_OK)
        {
          error_state = AUDIO_ERROR_IO;
        }
      }
    }

    /* Display elapsed time */
    elapsed_time = WavFile.fptr / WavInfo.ByteRate;
    if (prev_elapsed_time != elapsed_time)
    {
      prev_elapsed_time = elapsed_time;
      sprintf((char *)str, "[%02lu:%02lu]", elapsed_time / 60,
              elapsed_time % 60);
      BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
      BSP_LCD_DisplayStringAt(0, LINE(9), str, LEFT_MODE);
      BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    }
    break;

  case AUDIO_STATE_NEXT:
    if (++FilePos >= FileList.ptr)
    {
      FilePos = 0;
    }
    USBH_AUDIO_Stop(&hUSBHost);
    AUDIO_Start(FilePos);
    break;

  case AUDIO_STATE_PREVIOUS:
    if (--FilePos < 0)
    {
      FilePos = FileList.ptr - 1;
    }
    USBH_AUDIO_Stop(&hUSBHost);
    AUDIO_Start(FilePos);
    break;

  case AUDIO_STATE_PAUSE:
    BSP_LCD_DisplayStringAt(160, LINE(9), (uint8_t *) "  [PAUSE]", LEFT_MODE);
    USBH_AUDIO_Suspend(&hUSBHost);
    audio_state = AUDIO_STATE_WAIT;
    break;

  case AUDIO_STATE_RESUME:
    BSP_LCD_DisplayStringAt(160, LINE(9), (uint8_t *) "  [PLAY] ", LEFT_MODE);
    USBH_AUDIO_Resume(&hUSBHost);
    audio_state = AUDIO_STATE_PLAY;
    break;

  case AUDIO_STATE_VOLUME_UP:
    USBH_AUDIO_SetVolume(&hUSBHost, VOLUME_UP);
    audio_state = AUDIO_STATE_PLAY;
    break;

  case AUDIO_STATE_VOLUME_DOWN:
    USBH_AUDIO_SetVolume(&hUSBHost, VOLUME_DOWN);
    audio_state = AUDIO_STATE_PLAY;
    break;

  case AUDIO_STATE_ERROR:
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DisplayStringAt(178, LINE(8), (uint8_t *) "[Unsupported Format]",
                            LEFT_MODE);
    audio_state = AUDIO_STATE_INIT;
    break;

  case AUDIO_STATE_WAIT:
  case AUDIO_STATE_CONFIG:
  case AUDIO_STATE_IDLE:
  case AUDIO_STATE_INIT:
  default:
    break;
  }
  return error_state;
}

/**
  * @brief  Stops Audio streaming.
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_Stop(void)
{
  audio_state = AUDIO_STATE_IDLE;
  USBH_AUDIO_Stop(&hUSBHost);
  FilePos = 0;
  f_close(&WavFile);
  return AUDIO_ERROR_NONE;
}

/**
  * @brief  Probes the play back joystick state.
  * @param  GPIO_Pin: Joystick pin
  * @retval None
  */
void AUDIO_PlaybackProbeKey(uint16_t GPIO_Pin)
{
  uint32_t init_tick = 0, timeout = 0;

  /* Handle File List Selection */
  if ((GPIO_Pin & UP_JOY_PIN) == UP_JOY_PIN)
  {
    if (audio_state == AUDIO_STATE_PLAY)
    {
      audio_state = AUDIO_STATE_VOLUME_UP;
    }
  }
  else if ((GPIO_Pin & DOWN_JOY_PIN) == DOWN_JOY_PIN)
  {
    if (audio_state == AUDIO_STATE_PLAY)
    {
      audio_state = AUDIO_STATE_VOLUME_DOWN;
    }
  }
  else if ((GPIO_Pin & RIGHT_JOY_PIN) == RIGHT_JOY_PIN)
  {
    audio_state = AUDIO_STATE_NEXT;
  }
  else if ((GPIO_Pin & LEFT_JOY_PIN) == LEFT_JOY_PIN)
  {
    /* Go to main menu if user press left key for more than 500ms */
    init_tick = HAL_GetTick();
    while ((HAL_GPIO_ReadPin(LEFT_JOY_GPIO_PORT, LEFT_JOY_PIN) == GPIO_PIN_SET)
           && (timeout < 500))
    {
      timeout = HAL_GetTick() - init_tick;
    }

    if (timeout < 500)
    {
      if (audio_state == AUDIO_STATE_WAIT)
      {
        audio_state = AUDIO_STATE_RESUME;
      }

      if (audio_state == AUDIO_STATE_PLAY)
      {
        audio_state = AUDIO_STATE_PAUSE;
      }
    }
    else
    {
      Audio_ChangeSelectMode(AUDIO_SELECT_MENU);
    }
  }
}

/**
  * @brief  Gets the file info.
  * @param  file_idx: File index
  * @param  info: Pointer to WAV file info
  * @retval Audio error
  */
static AUDIO_ErrorTypeDef AUDIO_GetFileInfo(uint16_t file_idx,
                                            WAV_InfoTypedef * info)
{
  uint32_t bytesread;
  uint32_t duration;
  uint8_t str[FILEMGR_FILE_NAME_SIZE + 20];

  if (f_open
      (&WavFile, (char *)FileList.file[file_idx].name,
       FA_OPEN_EXISTING | FA_READ) == FR_OK)
  {
    /* Fill the buffer to Send */
    if (f_read(&WavFile, info, sizeof(WAV_InfoTypedef), (void *)&bytesread) ==
        FR_OK)
    {
      BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
      sprintf((char *)str, "Playing file (%d/%d): %s",
              file_idx + 1, FileList.ptr, (char *)FileList.file[file_idx].name);
      BSP_LCD_ClearStringLine(4);
      BSP_LCD_DisplayStringAtLine(4, str);

      BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
      sprintf((char *)str, "Sample rate : %lu Hz", info->SampleRate);
      BSP_LCD_ClearStringLine(6);
      BSP_LCD_DisplayStringAtLine(6, str);

      sprintf((char *)str, "Channels number : %d", info->NbrChannels);
      BSP_LCD_ClearStringLine(7);
      BSP_LCD_DisplayStringAtLine(7, str);

      duration = info->FileSize / info->ByteRate;
      sprintf((char *)str, "File Size : %lu MB", info->FileSize / 1024 / 1024);
      BSP_LCD_ClearStringLine(8);
      BSP_LCD_DisplayStringAtLine(8, str);
      sprintf((char *)str, "        - [%02lu:%02lu]", duration / 60,
              duration % 60);
      BSP_LCD_ClearStringLine(9);
      BSP_LCD_DisplayStringAtLine(9, str);
      BSP_LCD_DisplayStringAt(0, LINE(9), (uint8_t *) "[00:00]", LEFT_MODE);
      BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
      return AUDIO_ERROR_NONE;
    }
    f_close(&WavFile);
  }
  return AUDIO_ERROR_IO;
}

/**
  * @brief  Informs user that settings have been changed.
  * @param  phost: Host Handle
  * @retval None
  */
void USBH_AUDIO_FrequencySet(USBH_HandleTypeDef * phost)
{
  if (audio_state == AUDIO_STATE_CONFIG)
  {
    /* Start first read */
    USBH_AUDIO_Play(&hUSBHost, &BufferCtl.buff[0], WavInfo.FileSize);
    BSP_LCD_DisplayStringAt(160, LINE(9), (uint8_t *) "  [PLAY] ", LEFT_MODE);
    audio_state = AUDIO_STATE_PLAY;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
