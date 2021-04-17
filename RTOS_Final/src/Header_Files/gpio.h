#ifndef GPIO_H
#define GPIO_H
//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_gpio.h"
#include  <kernel/include/os.h>
#include <string.h>
//***********************************************************************************
// defined files
//***********************************************************************************

// LED 0 pin is
#define	LED0_port		gpioPortF
#define LED0_pin		04u
#define LED0_default	false 	// Default false (0) = off, true (1) = on
#define LED0_GPIOMODE			gpioModePushPull


// LED 1 pin is
#define LED1_port		gpioPortF
#define LED1_pin		05u
#define LED1_default	false	// Default false (0) = off, true (1) = on
#define LED1_GPIOMODE			gpioModePushPull


// BUTTON 0 pin is
#define	BUTTON0_port	gpioPortF
#define BUTTON0_pin		06u
#define BUTTON0_default	true	// Default false (0) = off, true (1) = on
#define PB0_GPIOMODE			gpioModeInput

// BUTTON 1 pin is
#define BUTTON1_port	gpioPortF
#define BUTTON1_pin		07u
#define BUTTON1_default	true	// Default false (0) = off, true (1) = on
#define PB1_GPIOMODE			gpioModeInput

#define LED0_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
#define LED1_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
#define PB0_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
#define PB1_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak

// CAPSENSOR Channel definitions
#define LEFT_CHANNEL    0
#define LEFT_C_CHANNEL  1
#define RIGHT_CHANNEL   2
#define RIGHT_C_CHANNEL 3

//Defaults for global variables
#define PB0_STATUS_default false
#define PB1_STATUS_default false
#define CAP_STATUS_default -1

//***********************************************************************************
// global variables
//***********************************************************************************
bool pb0_status;
bool pb1_status;
int cap_status;

OS_SEM PBSem;
OS_FLAG_GRP ledFlags;
OS_FLAG_GRP speedDirFlags;
OS_MUTEX mutex1;
OS_MUTEX mutex2;
OS_TMR myTimer;

OS_SEM LCDSem;
OS_SEM PHYSem;
RTOS_ERR err;

#define LED0_FLAG 1
#define LED1_FLAG 2

#define SPEED_FLAG 1
#define DIR_FLAG   2

typedef struct speed_info{
	int speed;
	int increments;
	int decrements;

}speed_info;
enum Theta {
	ZERO, M_PI4, M_PI3, M_PI2, M_PI1
};
typedef struct physics{
	enum Theta theta;
	double acc;
	double vel;
	int weight;
	int position;
}physics;
physics physicState;

enum dir_opt {HARD_LEFT, SOFT_LEFT, NO_DIR, SOFT_RIGHT, HARD_RIGHT};
typedef struct dir_info{
	enum dir_opt dir;
	int time_elapsed;
	int left_turns;
	int right_turns;
}dir_info;

dir_info myDir;
speed_info mySpeed;



//***********************************************************************************
// function prototypes
//***********************************************************************************
void gpio_open(void);
void sample_cap(void);



#endif
