#include <stdint.h>
#include <math.h>

#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_gpio.h"
#include "board.h"

#include "mma8451.h"
#include "iic.h"
#include "lpsci.h"

#define ALFA 0.059
#define M_PI 3.1415926535
volatile bool ready_to_send = false;

void PORTA_IRQHandler(){
	if(GPIO_GetPinsInterruptFlags(GPIOA) & (1<<15)){
		GPIO_ClearPinsInterruptFlags(GPIOA, (1 << 15));
		ready_to_send = true;
	}
	if(GPIO_GetPinsInterruptFlags(GPIOA) & (1<<14)){
		GPIO_ClearPinsInterruptFlags(GPIOA, (1 << 14));
		LED_RED_TOGGLE();
	}
}

void acc_normalize(int16_t *raw, float *norm){
	*(norm+0) = (float) *(raw+0) / 2048;
	*(norm+1) = (float) *(raw+1) / 2048;
	*(norm+2) = (float) *(raw+2) / 2048;
}

void filter_iir(float *raw, float *filtered){
	*(filtered+0) = (*(raw+0) * ALFA) + (*(filtered+0) * (1 - ALFA));
	*(filtered+1) = (*(raw+1) * ALFA) + (*(filtered+1) * (1 - ALFA));
	*(filtered+2) = (*(raw+2) * ALFA) + (*(filtered+2) * (1 - ALFA));
}

int main(void){
	BOARD_InitPins();
	BOARD_BootClockRUN();
	lpsci_init();
	iic_init();
	mma8451_init();

	EnableIRQ(PORTA_IRQn);

	int16_t acc_raw[3];
	float acc_norm[3];
	float acc_fill[3] = {0};

	while(1){
		while(ready_to_send == false);
		ready_to_send = false;
		mma8451_get_accel(acc_raw);

		acc_normalize(acc_raw, acc_norm);
		filter_iir(acc_norm, acc_fill);

		int16_t axises[2];
		axises[0] = atan2(acc_fill[1], sqrt((acc_fill[0] * acc_fill[0])+(acc_fill[2]*acc_fill[2]))) * (180.0 / M_PI);
		axises[1] = atan2(acc_fill[0], sqrt((acc_fill[1] * acc_fill[1])+(acc_fill[2]*acc_fill[2]))) * (180.0 / M_PI);
		lpsci_send_vals(axises, 2);
	}
}
