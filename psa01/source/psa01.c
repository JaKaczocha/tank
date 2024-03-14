#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC55S69_cm33_core0.h"
#include "fsl_debug_console.h"
#include "fsl_power.h"
#include "oled.h"
#include <math.h>


#define I2C_OLED ((I2C_Type *)I2C1_BASE)
#define BARPOS1 16
#define BARPOS2 24
#define BARSIZE 128

#define BARMODE PB_CENTER

float myAbs(float a) {
	return a > 0.0 ? a : -a;
}

void PowerControl(float X, float Y)
{
	OLED_Progressbar_Frame(0, BARPOS1, BARSIZE, BARMODE);
	OLED_Progressbar_Frame(0, BARPOS2, BARSIZE, BARMODE);

	float	Xval= -((X/(32768.0))-1);
	float	Yval= -((Y/32768.0)-1);

	{
		if( Xval < 0.03f && Xval > -0.03f ) {
			Xval = 0.0f;
		}
		if( Yval < 0.03f && Yval > -0.03f ) {
				Yval = 0.0f;
		}
	}

	float left = Xval, right = Yval;
	if(myAbs(Yval) <= 0.2 && myAbs(Xval) <= 0.2 ) { //w miejscu
		right = 0;
		left = 0;

	}
	else if(Yval >= 0.2) { //do przodu

		if(Xval <= -0.2) { //troche w lewo
			right = 1.0;
			left = 1.0 + 2*Xval;
			//OLED_Puts(0, 0, "//lewo i do przodu");

		}
		else if( Xval >= 0.2) {//troche w prawo
			left = 1.0;
			right = -1.0 + 2*Yval;
			//OLED_Puts(0, 0, "//prawo  i do przodu");

		}
		else { //prosto że prosto
			left = 1.0;
			right = 1.0;
			//OLED_Puts(0, 0, "//prosto że prosto     ");

		}

	}
	else if(Yval <= -0.2) { //do tyłu
		/*
		if(Xval <= -0.2) { //troche w lewo
			right = -1.0;
			left = -1.0 - 2*Yval;
			//OLED_Puts(0, 0, "//lewy tył        ");

		}
		else if( Xval >= 0.2) {//troche w prawo
			left = -1.0;
			right = -1.0 + 2*Xval;
			//OLED_Puts(0, 0, "//prawy tył        ");

		}*/
		if(Yval >= -0.3) {
			left = 0;
			right = 0;
		}
		else { //prosto że do tyłu
			left = -Yval;
			right = -Yval;
			//OLED_Puts(0, 0, "//prosto ze do tyłu       ");

		}

	}
	else if(Xval > 0.2) {

		//OLED_Puts(0, 0, "//prawo       >>>>>>>>>>");
		left = 1.0;
		right = -1.0;

	}
	else if(Xval < -0.2f) {

		//OLED_Puts(0, 0, "//lewo       <<<<<<<<<<<");

		left = -1.0;
		right = 1.0;
	}



	//some calculations;


	char sbuff[32];

	sprintf(sbuff, "X:%5.2f Y:%5.2f", Xval, Yval);
	//sprintf(sbuff, "l:%5.2f r:%5.2f", left, right);
	OLED_Puts(0, 0, sbuff);

	OLED_Progressbar_Value(0, BARPOS1, BARSIZE, BARMODE, left);
	OLED_Progressbar_Value(0, BARPOS2, BARSIZE, BARMODE, right);
	OLED_Refresh_Gram();


}



lpadc_conv_result_t g_LpadcResultConfigStruct;
uint16_t adcCh1, adcCh2;

/* ADC0_IRQn interrupt handler */
void ADC0_IRQHANDLER(void) {
	LPADC_GetConvResult(ADC0, &g_LpadcResultConfigStruct, 0U);
	adcCh1=g_LpadcResultConfigStruct.convValue;
	LPADC_GetConvResult(ADC0, &g_LpadcResultConfigStruct, 0U);
	adcCh2=g_LpadcResultConfigStruct.convValue;
}
/*
 * @brief Application entry point.
 */
int main(void) {
	/* Disable LDOGPADC power down */
	POWER_DisablePD(kPDRUNCFG_PD_LDOGPADC);
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif
	/* Initialize OLED */
	OLED_Init(I2C_OLED);

	while(1) {
		PowerControl(adcCh1, adcCh2);
	}
	return 0 ;
}
