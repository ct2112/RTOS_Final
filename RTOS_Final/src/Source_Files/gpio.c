//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"
#include "capsense.h"
#include "em_gpio.h"


//***********************************************************************************
// defined files
//***********************************************************************************

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

//***********************************************************************************
// functions
//***********************************************************************************
/***************************************************************************//**
 * @brief Unified GPIO Interrupt handler (pushbuttons)
 *        PB0 Switches between analog and digital clock modes
 *        PB1 Increments the time by one minute
 *****************************************************************************/

void gpio_open(void){
    RTOS_ERR  err;

    OSSemCreate(&PBSem, (CPU_CHAR*) "push button sem", 10, &err);
    OSFlagCreate(&ledFlags, (CPU_CHAR*) "led flags", LED1_FLAG | LED0_FLAG, &err);
    OSFlagCreate(&speedDirFlags, (CPU_CHAR*) "speed/dir flags", SPEED_FLAG|DIR_FLAG, &err);
    //while(OSTmrStart(&myTimer, &err) != DEF_TRUE);
}
static void unifiedHandler(void);
static void unifiedHandler(void){
	GPIO_IntClear(GPIO_IntGet());
	pb0_status = !GPIO_PinInGet(BUTTON0_port, BUTTON0_pin);
	pb1_status = !GPIO_PinInGet(BUTTON1_port, BUTTON1_pin);
	OSSemPost(&PBSem, OS_OPT_POST_ALL, &err);
}
/***************************************************************************//**
 * @brief GPIO Interrupt handler for even pins
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void) {
	unifiedHandler();
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler for odd pins
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void) {
	unifiedHandler();
}



/*
 * Sets the global variable cap_status to the state of the capacitive sensor, where 0 means the left side is pressed, 1 means that
 * the right side is pressed, and -1 means that either no side is pressed or both are.
 */
void sample_cap() {

	CAPSENSE_Sense();
	bool left   =  CAPSENSE_getPressed(LEFT_CHANNEL);
	bool cleft  = CAPSENSE_getPressed(LEFT_C_CHANNEL);
	bool right  = CAPSENSE_getPressed(RIGHT_CHANNEL);
	bool cright = CAPSENSE_getPressed(RIGHT_C_CHANNEL);
	if(left && !cleft && !cright && !right){
		myDir.dir = HARD_LEFT;
	}
	else if(cleft && !left && !cright && !right){
		myDir.dir = SOFT_LEFT;
	}
	else if(right && !left && !cleft && !cright){
		myDir.dir = HARD_RIGHT;
	}
	else if(cright && !left && !cleft && !right){
		myDir.dir = SOFT_LEFT;
	}
	else{
		myDir.dir = NO_DIR;
	}

}
