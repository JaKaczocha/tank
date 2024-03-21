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

float map(float x, float in_min, float in_max,float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void PowerControl(float X, float Y)
{

	OLED_Draw_Circle(OLED_WIDTH/2, OLED_HEIGHT/2, OLED_HEIGHT/2);

	uint8_t x1 =OLED_WIDTH/4,  y1 = 0,x2 =OLED_WIDTH/4 * 3, y2 = OLED_HEIGHT-1;
	//OLED_Draw_Rect(x1, 0, OLED_WIDTH/4 * 3, OLED_HEIGHT-1, 1);

	//float	Xval= map(X,0,65536,32,96); //swap
	//float	Yval= map(Y,0,65536,0,63); //swap
	float	Xval= ((X/(32768.0))-1);
	float	Yval= ((Y/32768.0)-1);


	float Xnew = Xval * sqrt(1-Yval * Yval /2);
	float Ynew = Yval * sqrt(1-Xval * Xval / 2);

	Xval= map(Xnew,-1,1,32,96); //swap
    Yval= map(Ynew,-1,1,0,63); //swap



	//char sbuff[32];
	//sprintf(sbuff, "X:%5.2f Y:%5.2f", Xval, Yval);

	//OLED_Puts(0, 0, sbuff);
	//OLED_Draw_Point(Xval, Yval, 1);
	OLED_Draw_Fill_Rect(Xval-2, Yval-2, Xval+2, Yval + 2, 1);
	OLED_Refresh_Gram();
	OLED_Clear_Screen(0);

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
