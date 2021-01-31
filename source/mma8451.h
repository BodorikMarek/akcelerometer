#ifndef MMA8451_H_
#define MMA8451_H_

#include <stdint.h>

void mma8451_init();
void mma8451_get_accel(int16_t *accel_buff);
void mma8451_wait_ready();

#endif /* MMA8451_H_ */
