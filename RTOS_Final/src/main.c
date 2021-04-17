/***************************************************************************//**
 * @file
 * @brief Simple LED Blink Demo for SLSTK3402A
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_int.h"


#include "bsp.h"
#include "main.h"
#include "app.h"
#include "math.h"

#include "gpio.h"

#include  <bsp_os.h>

#include  <cpu/include/cpu.h>
#include  <common/include/common.h>
//#include  <kernel/source/os_task.c>
#include  <kernel/include/os_trace.h>
#include  <kernel/include/os.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>

#include "textdisplay.h"
//#include "textdisplay.c"
#include "retargettextdisplay.h"
//#include "retargettextdisplay.c"
#include "capsense.h"

#include "display.c"
#include "display.h"
#include "displayconfigall.h"


#define  MAIN_START_TASK_PRIO              21u
#define  MAIN_START_TASK_STK_SIZE         512u
static  CPU_STK  MainStartTaskStk[MAIN_START_TASK_STK_SIZE];                /* Start Task TCB.                                      */
static  OS_TCB   MainStartTaskTCB;

#define LED_TASK_PRIO						15u
#define LED_TASK_STK_SIZE					512u
static  CPU_STK  LEDTaskStk[LED_TASK_STK_SIZE];                /* Start Task TCB.                                      */
static  OS_TCB   LEDTaskTCB;

#define PB_TASK_PRIO  						19u
#define PB_TASK_STK_SIZE 512u
static  CPU_STK  PBTaskStk[PB_TASK_STK_SIZE];                /* Start Task TCB.                                      */
static  OS_TCB   PBTaskTCB;

#define SLIDER_TASK_PRIO 					18u
#define SLIDER_TASK_STK_SIZE 512u
static  CPU_STK  SliderTaskStk[SLIDER_TASK_STK_SIZE];                /* Start Task TCB.                                      */
static  OS_TCB   SliderTaskTCB;

#define IDLE_TASK_PRIO 						23u
#define IDLE_TASK_STK_SIZE 512u
static  CPU_STK  IdleTaskStk[IDLE_TASK_STK_SIZE];                /* Start Task TCB.                                      */
static  OS_TCB   IdleTaskTCB;

#define LCD_TASK_PRIO 						22u
#define LCD_TASK_STK_SIZE 512u
static  CPU_STK  LCDTaskStk[LCD_TASK_STK_SIZE];                /* Start Task TCB.                                      */
static  OS_TCB   LCDTaskTCB;


#define MONITOR_TASK_PRIO 						16u
#define MONITOR_TASK_STK_SIZE 512u
static  CPU_STK  MonitorTaskStk[MONITOR_TASK_STK_SIZE];                /* Start Task TCB.                                      */
static  OS_TCB   MonitorTaskTCB;

static void MainStartTask (void  *p_arg);
static void LED2Task(void *p_arg);
static void LED1Task(void *p_arg);
static void CapTask(void *p_arg);
static void PBTask(void *p_arg);
static void LCDTask(void *p_arg);
static void PhysicsTask(void *p_arg);
static void MyCallback (OS_TMR *p_tmr, void *p_arg);

int main(void) {
	EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
	CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

	/* Chip errata */
	CHIP_Init();

	/* Init DCDC regulator and HFXO with kit specific parameters */
	/* Initialize DCDC. Always start in low-noise mode. */
	EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
	EMU_DCDCInit(&dcdcInit);
	em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
	EMU_EM23Init(&em23Init);
	CMU_HFXOInit(&hfxoInit);

	/* Switch HFCLK to HFRCO and disable HFRCO */
	CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
	CMU_OscillatorEnable(cmuOsc_HFXO, false, false);

	/* Enable GPIO clock */
	CMU_ClockEnable(cmuClock_GPIO, true);

	/* Call application program to open / initialize all required peripheral */
	app_peripheral_setup();

	  RTOS_ERR  err;


	      BSP_SystemInit();                                           /* Initialize System.                                   */
	      CPU_Init();                                                 /* Initialize CPU.                                      */

	      OS_TRACE_INIT();
	      OSInit(&err);                                               /* Initialize the Kernel.                               */
	                                                                  /*   Check error code.                                  */
	      APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);


	      GPIO_IntConfig(BUTTON0_port, BUTTON0_pin, false, true, true);
	      GPIO_IntConfig(BUTTON1_port, BUTTON1_pin, false, true, true);

	        GPIO_IntEnable(GPIO_ODD_IRQn);
	        GPIO_IntEnable(GPIO_EVEN_IRQn);
	        NVIC_EnableIRQ(GPIO_ODD_IRQn);
	        NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	      OSTaskCreate(&MainStartTaskTCB,                          /* Create the Start Task.                               */
	                   "Main Start Task",
	                    MainStartTask,
	                    DEF_NULL,
	                    MAIN_START_TASK_PRIO,
	                   &MainStartTaskStk[0],
	                   (MAIN_START_TASK_STK_SIZE / 10u),
	                    MAIN_START_TASK_STK_SIZE,
	                    0u,
	                    0u,
	                    DEF_NULL,
	                   (OS_OPT_TASK_STK_CLR),
	                   &err);
	                                                                  /*   Check error code.                                  */
	      APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);


	      OSStart(&err);                                              /* Start the kernel.                                    */
	                                                                  /*   Check error code.                                  */
	      APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);


	        /* Infinite  loop */


	        while (1) {

	      	  EMU_EnterEM1();

	        }

	      return 1;


}


static  void  MainStartTask (void  *p_arg)
{
    PP_UNUSED_PARAM(p_arg);                                     /* Prevent compiler warning.                            */
#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();                                /* Initialize interrupts disabled measurement.          */
#endif

    Common_Init(&err);                                          /* Call common module initialization example.           */
   // APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

    BSP_OS_Init();                                              /* Initialize the BSP. It is expected that the BSP ...  */
                                                                /* ... will register all the hardware controller to ... */
                                                                /* ... the platform manager at this moment.             */
    BSP_LedsInit();
    /* Initialize the display module. */
     DISPLAY_Init();

     /* Retarget stdio to a text display. */
     if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK) {
       while (1) ;
     }
     gpio_open();
     static OS_TMR myTimer;
     OSTmrCreate(&myTimer, (CPU_CHAR *) "SLIDER TIMER", 10, 10, OS_OPT_TMR_PERIODIC, MyCallback, NULL, &err);
     while(OSTmrStart(&myTimer, &err) != DEF_TRUE);
    OSTaskCreate(&LEDTaskTCB,                          /* Create the LED Task.  */
                  "LED Task",
                   LED1Task,
                   DEF_NULL,
                   LED_TASK_PRIO,
                  &LEDTaskStk[0],
                  (LED_TASK_STK_SIZE / 10u),
                   LED_TASK_STK_SIZE,
                   0u,
                   0u,
                   DEF_NULL,
                  (OS_OPT_TASK_STK_CLR),
                  &err);


    OSTaskCreate(&PBTaskTCB,                          // Create the PB Task.
                  "PB Task",
                   PBTask,
                   DEF_NULL,
                   PB_TASK_PRIO,
                  &PBTaskStk[0],
                  (PB_TASK_STK_SIZE / 10u),
                   PB_TASK_STK_SIZE,
                   0u,
                   0u,
                   DEF_NULL,
                  (OS_OPT_TASK_STK_CLR),
                  &err);



    OSTaskCreate(&SliderTaskTCB,                          /* Create the Slider Task.                               */
                  "CAP Task",
                   CapTask,
                   DEF_NULL,
                   SLIDER_TASK_PRIO,
                  &SliderTaskStk[0],
                  (SLIDER_TASK_STK_SIZE / 10u),
                   SLIDER_TASK_STK_SIZE,
                   0u,
                   0u,
                   DEF_NULL,
                  (OS_OPT_TASK_STK_CLR),
                  &err);

    OSTaskCreate(&IdleTaskTCB,                          /* Create the Slider Task.                               */
                  "Idle Task",
                   LED2Task,
                   DEF_NULL,
                   IDLE_TASK_PRIO,
                  &IdleTaskStk[0],
                  (IDLE_TASK_STK_SIZE / 10u),
                   IDLE_TASK_STK_SIZE,
                   0u,
                   0u,
                   DEF_NULL,
                  (OS_OPT_TASK_STK_CLR),
                  &err);

    OSTaskCreate(&LCDTaskTCB,                          /* Create the Slider Task.                               */
                      "LCD Task",
                       LCDTask,
                       DEF_NULL,
                       LCD_TASK_PRIO,
                      &LCDTaskStk[0],
                      (LCD_TASK_STK_SIZE / 10u),
                       LCD_TASK_STK_SIZE,
                       0u,
                       0u,
                       DEF_NULL,
                      (OS_OPT_TASK_STK_CLR),
                      &err);
    OSTaskCreate(&MonitorTaskTCB,                          /* Create the LED Task.  */
                   "LED Task",
                    PhysicsTask,
                    DEF_NULL,
                    MONITOR_TASK_PRIO,
                   &MonitorTaskStk[0],
                   (MONITOR_TASK_STK_SIZE / 10u),
                    MONITOR_TASK_STK_SIZE,
                    0u,
                    0u,
                    DEF_NULL,
                   (OS_OPT_TASK_STK_CLR),
                   &err);
}
#define LED_DELAY 10
#define PB_DELAY LED_DELAY
#define CAP_DELAY LED_DELAY

static void LED1Task(void *p_arg){

	GPIO_DriveStrengthSet(LED0_port, LED0_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED0_port, LED0_pin, LED0_GPIOMODE, LED0_default);

	GPIO_DriveStrengthSet(LED1_port, LED1_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED1_port, LED1_pin, LED1_GPIOMODE, LED1_default);
    RTOS_ERR  err;
static OS_FLAGS flags;
	while(1){
		flags = OSFlagPend(&ledFlags, LED0_FLAG | LED1_FLAG, 0, OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME|OS_OPT_PEND_BLOCKING , NULL, &err);
		//flags = ledFlags.Flags;
		if(flags & LED0_FLAG)
			GPIO_PinOutSet(LED0_port, LED0_pin);
		else
			GPIO_PinOutClear(LED0_port, LED0_pin);

		if(flags & LED1_FLAG)
			GPIO_PinOutSet(LED1_port, LED1_pin);
		else
			GPIO_PinOutClear(LED1_port, LED1_pin);

        OSTimeDly( LED_DELAY,                                        /*   1000 OS Ticks                                      */
                   OS_OPT_TIME_DLY,                             /*   from now.                                          */
                  &err);
	}

}
static void LED2Task(void *p_arg){
	GPIO_DriveStrengthSet(BUTTON1_port, PB1_DRIVE_STRENGTH);
	GPIO_PinModeSet(BUTTON1_port, BUTTON1_pin, PB1_GPIOMODE, BUTTON1_default);

	GPIO_DriveStrengthSet(BUTTON0_port, PB0_DRIVE_STRENGTH);
	GPIO_PinModeSet(BUTTON0_port, BUTTON0_pin, PB0_GPIOMODE, BUTTON0_default);

	pb0_status = PB0_STATUS_default;
	pb1_status = PB1_STATUS_default;

    RTOS_ERR  err;


	while(1){
		OSSemPend(&PBSem, 0, OS_OPT_PEND_BLOCKING, NULL, &err);

			if(pb0_status){
				mySpeed.speed += 5;
				mySpeed.increments++;
			}
			if(pb1_status){
				mySpeed.speed -= 5;
				mySpeed.decrements++;
			}
			if(mySpeed.speed < 0)
				mySpeed.speed = 0;
			OSFlagPost(&speedDirFlags, SPEED_FLAG, OS_OPT_POST_FLAG_SET, &err);
		}

			OSFlagPost(&speedDirFlags, SPEED_FLAG, OS_OPT_POST_FLAG_CLR, &err);



        OSTimeDly( PB_DELAY,                                        /*   1000 OS Ticks                                      */
                   OS_OPT_TIME_DLY,                             /*   from now.                                          */
                  &err);

}
static void CapTask(void *p_arg){
	CAPSENSE_Init();
	/*CAPSENSOR Setup*/

	cap_status = CAP_STATUS_default;

    static RTOS_ERR  err;
static enum dir_opt prev;
	while(1){
		sample_cap();

		if(prev != myDir.dir){
			myDir.time_elapsed = 0;
			OSFlagPost(&speedDirFlags, DIR_FLAG, OS_OPT_POST_FLAG_SET, &err);
		}
		else{
			OSFlagPost(&speedDirFlags, DIR_FLAG, OS_OPT_POST_FLAG_CLR, &err);
		}
		prev = myDir.dir;
        OSTimeDly( CAP_DELAY,                                        /*   1000 OS Ticks                                      */
                   OS_OPT_TIME_DLY,                             /*   from now.                                          */
                  &err);
	}
}
#define WEIGHT_INCR 5
static void PBTask(void *p_arg){
	static bool stateChanged;
	while(1){
		pb0_status = !GPIO_PinInGet(BUTTON0_port, BUTTON0_pin);
		pb1_status = !GPIO_PinInGet(BUTTON1_port, BUTTON1_pin);
		if(pb1_status){
			physicState.weight += WEIGHT_INCR;
			stateChanged = 1;
		}
		if(pb0_status){
			physicState.weight -= WEIGHT_INCR;
			stateChanged = 1;
		}
		if(stateChanged){
			OSSemPost(&PHYSem, 0, &err);
			stateChanged = 0;
		}
	}
}
static void PhysicsTask(void *p_arg){
	static bool stateChanged;
    static CPU_TS       ts;
    OSSemCreate(&PHYSem, (CPU_CHAR*) "LCD Semaphore", 10, &err);
	while(1){
		OSSemPend(&PHYSem, 0, OS_OPT_PEND_BLOCKING, NULL, &err);

		if(stateChanged){
			OSSemPost(&LCDSem, 0, &err);
			stateChanged = 0;
		}
	}
}
static void LCDTask(void *p_arg){
	OSSemCreate(&LCDSem, (CPU_CHAR*) "LCD Semaphore", 10, &err);
	while(1){
		OSSemPend(&LCDSem, 0, OS_OPT_PEND_BLOCKING, NULL, &err);
		switch(physicState.theta){

		case M_PI1:
			break;
		case M_PI2:
			break;
		case M_PI3:
			break;
		case M_PI4:
			break;
		case ZERO:
			RETARGET_WriteString("YOU LOSE");
			break;
		default:
			break;
		}
		static char temp[40];
		sprintf(temp, "Accelerating at: %f m/s^2\n Traveling at: %f m/s", physicState.acc, physicState.vel);
		RETARGET_WriteString(temp);

	}
}

static void MyCallback (OS_TMR *p_tmr, void *p_arg){
	myDir.time_elapsed++;
}

