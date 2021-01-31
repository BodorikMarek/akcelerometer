#include "iic.h"

#define MMA8451_ADDR 0x1D

#define ADDR_STATUS 0x00
#define ADDR_XYZ_DATA 0x01
#define ADDR_XYZ_DATA_CFG 0x0E
#define ADDR_CTRL_REG1 0x2A
#define ADDR_CTRL_REG4 0x2D
#define ADDR_CTRL_REG5 0x2E
#define ADDR_FF_MT_CFG 0x15
#define ADDR_FF_MT_SRC 0x16
#define ADDR_FF_MT_TH 0x17
#define ADDR_PULSE_CFG 0x21
#define ADDR_PULSE_THSX 0x23
#define ADDR_PULSE_THSY 0x24
#define ADDR_PULSE_THSZ 0x25
#define ADDR_PULSE_TMLT 0x26
#define ADDR_PULSE_LTCY 0x27

void mma8451_init(){
	iic_write(MMA8451_ADDR, ADDR_CTRL_REG1, 0x00);
	iic_write(MMA8451_ADDR, ADDR_XYZ_DATA_CFG, 0x01);
	iic_write(MMA8451_ADDR, ADDR_CTRL_REG4, 0x09);
	iic_write(MMA8451_ADDR, ADDR_CTRL_REG5, 0x08);

	iic_write(MMA8451_ADDR, ADDR_PULSE_THSY, 15);
	iic_write(MMA8451_ADDR, ADDR_PULSE_TMLT, 5);
	iic_write(MMA8451_ADDR, ADDR_PULSE_CFG, 0x04);
	iic_write(MMA8451_ADDR, ADDR_PULSE_LTCY, 20);

	iic_write(MMA8451_ADDR, ADDR_CTRL_REG1, 0x1D);
}

void mma8451_get_accel(int16_t *accel_buff){
	uint8_t buff[6];

	iic_read(MMA8451_ADDR, ADDR_XYZ_DATA, buff, 6);

	*(accel_buff) = ((int16_t)(((buff[0] * 256U) | buff[1]))) / 4U;
	*(accel_buff+1) = ((int16_t)(((buff[2] * 256U) | buff[3]))) / 4U;
	*(accel_buff+2) = ((int16_t)(((buff[4] * 256U) | buff[5]))) / 4U;
}

void mma8451_wait_ready(){
	uint8_t status0_value = 0;

	while (status0_value != 0xff){
		iic_read(MMA8451_ADDR, ADDR_STATUS, &status0_value, 1);
	}
}
