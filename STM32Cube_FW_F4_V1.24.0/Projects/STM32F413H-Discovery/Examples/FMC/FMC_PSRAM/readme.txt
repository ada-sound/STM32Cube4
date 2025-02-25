    /**
  @page FMC_SRAM FMC PSRAM memory basic functionalities use example

  @verbatim
  ******************** (C) COPYRIGHT 2017 STMicroelectronics *******************
  * @file    FMC/FMC_PSRAM/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the FMC PSRAM example.
  ******************************************************************************
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  @endverbatim

@par Example Description 

This example describes how to configure the FMC controller to access the PSRAM 
memory.

The PSRAM is IS66WV51216EBLL-55BLI.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 100 MHz.

The goal of this example is to explain the different steps to configure the FMC 
and make the PSRAM device ready for access, without using the MSP layer.          

In this example, the SRAM device is configured and initialized explicitly 
following all initialization sequence steps. After initializing the device, user 
can perform read/write operations on it. A data buffer is written to the PSRAM 
memory, then read back and checked to verify its correctness.

If the data is read correctly from PSRAM, the LED3 is ON, otherwise the LED4 is ON.   
In case of HAL initialization issue, LED4 will be ON.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Memory, FMC, PSRAM, Read, Write, Initialization, Access

@par Directory contents 

 - FMC/FMC_PSRAM/Inc/stm32f4xx_hal_conf.h        HAL Configuration file
 - FMC/FMC_PSRAM/Inc/main.h                      Header for main.c module 
 - FMC/FMC_PSRAM/Inc/stm32f4xx_it.h              Header for stm32f4xx_it.c
 - FMC/FMC_PSRAM/Src/main.c                      Main program
 - FMC/FMC_PSRAM/Src/stm32f4xx_hal_msp.c         HAL MSP module  
 - FMC/FMC_PSRAM/Src/stm32f4xx_it.c              Interrupt handlers
 - FMC/FMC_PSRAM/Src/system_stm32f4xx.c          STM32F4xx system clock configuration file 
    

@par Hardware and Software environment 

  - This example runs on STM32F413xx/STM32F423xx devices.
    
  - This example has been tested with STM32F413H-Discovery board and can be
    easily tailored to any other supported device and development board. 

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
