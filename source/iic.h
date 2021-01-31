#ifndef IIC_H_
#define IIC_H_

#include "fsl_i2c.h"

//static void i2c_master_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData);
void iic_init();
bool iic_write(uint8_t device, uint8_t wr_addr, uint8_t wr_data);
bool iic_read(uint8_t device, uint8_t rd_addr, uint8_t *rd_data, size_t len);

#endif /* IIC_H_ */
