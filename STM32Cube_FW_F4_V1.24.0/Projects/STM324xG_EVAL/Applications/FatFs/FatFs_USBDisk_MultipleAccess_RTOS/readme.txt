/**
  @page FatFs_USBDisk_MultipleAccess_RTOS   FatFs with USB disk drive multiple access in RTOS mode application
 
  @verbatim
  ******************** (C) COPYRIGHT 2017 STMicroelectronics *******************
  * @file    FatFs/FatFs_USBDisk_MultipleAccess_RTOS/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the FatFs with USB disk drive multiple access in 
  *          RTOS mode application
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
  @endverbatim

@par Application Description

How to use STM32Cube firmware with FatFs middleware component as a generic FAT
file system module, FreeRTOS as an RTOS module based on using CMSIS-OS wrapping
layer common APIs, and also STM32 USB  On-The-Go (OTG) host library, in both 
Full Speed (FS) and High Speed (HS) modes. This example develops an application 
exploiting FatFs features with USB  disk drive in RTOS mode configuration.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock
(SYSCLK) to run at 168 MHz.
           
The application is based on writing two text files to a drive. Two threads, with
different priorities, are created to manage multiple access to the FAT volumes
through FatFs APIs as described in the following steps:  

 - StartThread with a normal priority executing steps below:           
   - Link the USB Host disk I/O driver;
   - Register the file system object (mount) to the FatFs module for the USB drive;
   - Create and Open new text file object with write access;
   - Write data to the text file;
   - Close the open text file.
 
 - ConcurrentThread with a high priority executing steps below:  
   - Create and Open new text file object with write access;
   - Write data to the text file;
   - Close the open text file;
   - Unlink the USB Host disk I/O driver.
 
It is worth noting that the application manages any error occurred during the 
access to FAT volume, when using FatFs APIs. Otherwise, user can check if the
written text files are available on the USB disk.

It is possible to fine tune needed FatFs features by modifying defines values 
in FatFs configuration file �ffconf.h� available under the project includes 
directory, in a way to fit the application requirements. 

STM32 Eval board's LEDs can be used to monitor the application status:
  - LED1 and LED4 are ON when the application runs successfully.
  - LED3 is ON when any error occurs.


@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in HAL time base ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the HAL time base interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the HAL time base is always set to 1 millisecond to have correct 
      HAL operation.

@note The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set according to the OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.

For more details about FatFs implementation on STM32Cube, please refer to UM1721 "Developing Applications 
on STM32Cube with FatFs".


@par Directory contents
 
  - FatFs/FatFs_USBDisk_MultipleAccess_RTOS/Inc/stm32f4xx_hal_conf.h           HAL configuration file
  - FatFs/FatFs_USBDisk_MultipleAccess_RTOS/Inc/stm32f4xx_it.h                 Interrupt handlers header file
  - FatFs/FatFs_USBDisk_MultipleAccess_RTOS/Inc/main.h                         Main program header file
  - FatFs/FatFs_USBDisk_MultipleAccess_RTOS/Inc/usbh_diskio_dma.h              FatFS usbh diskio driver header file
  - FatFs/FatFs_USBDisk_MultipleAccess_RTOS/Inc/usbh_conf.h                    USB Host low level config options
  - FatFs/FatFs_USBDisk_MultipleAccess_RTOS/Inc/FreeRTOSConfig.h               FreeRTOS configuration file
  - FatFs/FatFs_USBDisk_MultipleAccess_RTOS/Inc/ffconf.h                       FAT file system module configuration file
  - FatFs/FatFs_USBDisk_MultipleAccess_RTOS/Src/stm32f4xx_it.c                 Interrupt handlers
  - FatFs/FatFs_USBDisk_MultipleAccess_RTOS/Src/main.c                         Main program
  - FatFs/FatFs_USBDisk_MultipleAccess_RTOS/Src/usbh_diskio_dma.c              FatFS usbh diskio driver implementation
  - FatFs/FatFs_USBDisk_MultipleAccess_RTOS/Src/usbh_conf.c                    USB Host configuration file
  - FatFs/FatFs_USBDisk_MultipleAccess_RTOS/Src/system_stm32f4xx.c             STM32F4xx system clock configuration file
  - FatFs/FatFs_USBDisk_MultipleAccess_RTOS/Src/stm32f4xx_hal_timebase_tim.c   HAL TIM based timebase functions
 
@par Hardware and Software environment

  - This application runs on STM32F407xx/417xx devices.
    
  - This application has been tested with STMicroelectronics STM324xG-EVAL RevC 
    evaluation boards and can be easily tailored to any other supported device 
    and development board. 

  - STM324xG-EVAL RevC Set-up
    - Plug the USB key into the STM324xG-EVAL board through 'USB micro A-Male 
      to A-Female' cable to the connector:
      - CN9: to use USB High Speed (HS) with embedded PHY(U8)
             Please ensure that jumper JP31 is fitted.
      - CN8: to use USB Full Speed (FS)


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - In the workspace toolbar select the project configuration:
   - STM324xG-EVAL_USBH-HS: to configure the project for STM32F4xx devices using USB OTG HS peripheral
   - STM324xG-EVAL_USBH-FS: to configure the project for STM32F4xx devices using USB OTG FS peripheral
 - Run the application

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
