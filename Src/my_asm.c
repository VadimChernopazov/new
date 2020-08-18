/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : my_function.c
  * @brief          : my_function
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include "my_function.h"
#include "math.h"
#include "cmsis_os.h"
#include "my_asm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
									
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */
														
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
__asm void my_strcpy(const char *src, char *dst)
{
loop
      LDRB  r2, [r0], #1		//[r0] Содержит адрес начала src
      STRB  r2, [r1], #1		//[r1] Содержит адрес начала dst
      CMP   r2, #0
      BNE   loop
      BX    lr
};

void hard_fault_handler_c(unsigned int * hardfault_args);

//__asm void my_asm_handler(void)
//{
//		TST lr, #4
//		ITE eq
//		MRSEQ r0, msp
//		MRSNE r0, psp 
//		LDR r1,=(hard_fault_handler_c)
//		BX r1 
////		[debugHardfault]: .word debugHardfault
//};
__asm void my_HardFault_Handler(void)
{
	MOVS   r0, #4
  MOV    r1, LR
  TST    r0, r1
  BEQ    stacking_used_MSP
  MRS    R0, PSP ; //first parameter - stacking was using PSP
  B      get_LR_and_branch
stacking_used_MSP
  MRS    R0, MSP ; //first parameter - stacking was using MSP
get_LR_and_branch
  MOV    R1, LR  ; //second parameter is LR current value
  LDR    R2,=__cpp(hard_fault_handler_c)
  BX     R2
}


//__asm void hard_fault_handler_asm(void)
//{
//TST lr, #4
//ITE eq
//MRSEQ r0, MSP
//MRSNE r0, PSP
//B hard_fault_handler_c
//}

void hard_fault_handler_c(unsigned int * hardfault_args)
{
unsigned int stacked_r0;
unsigned int stacked_r1;
unsigned int stacked_r2;
unsigned int stacked_r3;
unsigned int stacked_r12;
unsigned int stacked_lr;
unsigned int stacked_pc;
unsigned int stacked_psr;
 
stacked_r0 = ((unsigned long) hardfault_args[0]);
stacked_r1 = ((unsigned long) hardfault_args[1]);
stacked_r2 = ((unsigned long) hardfault_args[2]);
stacked_r3 = ((unsigned long) hardfault_args[3]);
stacked_r12 = ((unsigned long) hardfault_args[4]);
stacked_lr = ((unsigned long) hardfault_args[5]);
stacked_pc = ((unsigned long) hardfault_args[6]);
stacked_psr = ((unsigned long) hardfault_args[7]);
	
printf("[Hard fault handler]\n");
printf("R0 = %x\n", stacked_r0);
printf("R1 = %x\n", stacked_r1);
printf("R2 = %x\n", stacked_r2);
printf("R3 = %x\n", stacked_r3);
printf("R12 = %x\n", stacked_r12);
printf("LR = %x\n", stacked_lr);
printf("PC = %x\n", stacked_pc);
printf("PSR = %x\n", stacked_psr);
printf("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38))));
printf("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28))));
printf("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C))));
printf("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30))));
printf("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C))));
//exit(0); // terminate
return;
}

   //__ASM volatile("BKPT #01");



/* USER CODE END 0 */
