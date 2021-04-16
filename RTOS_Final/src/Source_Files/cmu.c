//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void cmu_open(void){

	//	uint32_t	temp_freq;
//Some of the CMU functions were getting caught at an assert statement  but it seems to work
		//CMU_ClockSelectSet(cmuClock_HFPER, cmuSelect_HFRCO);
		//CMU_ClockEnable(cmuClock_HFPER, true);

		// By default, LFRCO is enabled
		CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);	// using LFXO or ULFRCO


		  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
		  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

		// Route LF clock to the LF clock tree

//		CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
		CMU_OscillatorEnable(cmuOsc_LFXO, false, false);		// Disable LFXO
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);	// routing clock to LFA


//		CMU_ClockEnable(cmuClock_LFA, true);
	//	CMU_ClockEnable(cmuClock_CORELE, true);

		// Peripheral clocks enabled
		CMU_ClockEnable(cmuClock_GPIO, true);

}

