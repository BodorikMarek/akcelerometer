#include "board.h"
#include "fsl_i2c.h"
#include "fsl_gpio.h"
#include "fsl_port.h"

#define I2C_RELEASE_BUS_COUNT 100U
#define I2C_RELEASE_SDA_PORT PORTE
#define I2C_RELEASE_SCL_PORT PORTE
#define I2C_RELEASE_SDA_GPIO GPIOE
#define I2C_RELEASE_SDA_PIN 25U
#define I2C_RELEASE_SCL_GPIO GPIOE
#define I2C_RELEASE_SCL_PIN 24U

#define I2C_FREQ 100000U


i2c_master_handle_t g_m_handle;
volatile bool completionFlag = false;
volatile bool nakFlag = false;

static void i2c_master_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData){
	if (status == kStatus_Success){
		completionFlag = true;
	}
	if ((status == kStatus_I2C_Nak) || (status == kStatus_I2C_Addr_Nak)){
		nakFlag = true;
	}
}

static void i2c_release_bus_delay(void){
	uint32_t i = 0;
	for (i = 0; i < I2C_RELEASE_BUS_COUNT; i++){
		__NOP();
	}
}

void BOARD_I2C_ReleaseBus(void){
	uint8_t i = 0;
	gpio_pin_config_t pin_config;
	port_pin_config_t i2c_pin_config = {0};

	/* Config pin mux as gpio */
	i2c_pin_config.pullSelect = kPORT_PullUp;
	i2c_pin_config.mux = kPORT_MuxAsGpio;

	pin_config.pinDirection = kGPIO_DigitalOutput;
	pin_config.outputLogic = 1U;
	CLOCK_EnableClock(kCLOCK_PortE);
	PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SCL_PIN, &i2c_pin_config);
	PORT_SetPinConfig(I2C_RELEASE_SDA_PORT, I2C_RELEASE_SDA_PIN, &i2c_pin_config);

	GPIO_PinInit(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, &pin_config);
	GPIO_PinInit(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, &pin_config);

	/* Drive SDA low first to simulate a start */
	GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
	i2c_release_bus_delay();

	/* Send 9 pulses on SCL and keep SDA low */
	for (i = 0; i < 9; i++){
		GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
		i2c_release_bus_delay();

		GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
		i2c_release_bus_delay();

		GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
		i2c_release_bus_delay();
		i2c_release_bus_delay();
	}

	/* Send stop */
	GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
	i2c_release_bus_delay();

	GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
	i2c_release_bus_delay();

	GPIO_WritePinOutput(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
	i2c_release_bus_delay();

	GPIO_WritePinOutput(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
	i2c_release_bus_delay();
}

void iic_init(){
	BOARD_I2C_ReleaseBus();
	BOARD_I2C_ConfigurePins();
	I2C_MasterTransferCreateHandle(BOARD_ACCEL_I2C_BASEADDR, &g_m_handle, i2c_master_callback, NULL);

	i2c_master_config_t masterConfig;
	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = I2C_FREQ;
	uint32_t sourceClock = CLOCK_GetFreq(I2C0_CLK_SRC);
	I2C_MasterInit(BOARD_ACCEL_I2C_BASEADDR, &masterConfig, sourceClock);
}

bool iic_write(uint8_t device, uint8_t wr_addr, uint8_t wr_data){
	i2c_master_transfer_t masterXfer;

	memset(&masterXfer, 0, sizeof(masterXfer));
	masterXfer.slaveAddress = device;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = wr_addr;
	masterXfer.subaddressSize = 1;
	masterXfer.data = &wr_data;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	I2C_MasterTransferNonBlocking(BOARD_ACCEL_I2C_BASEADDR, &g_m_handle, &masterXfer);

	/*  wait for transfer completed. */
	while ((!nakFlag) && (!completionFlag)){
	}

	nakFlag = false;

	if (completionFlag == true){
		completionFlag = false;
		return true;
	}
	else{
		return false;
	}
}

bool iic_read(uint8_t device, uint8_t rd_addr, uint8_t *rd_data, size_t len){
	i2c_master_transfer_t masterXfer;

	memset(&masterXfer, 0, sizeof(masterXfer));
	masterXfer.slaveAddress = device;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = rd_addr;
	masterXfer.subaddressSize = 1;
	masterXfer.data = rd_data;
	masterXfer.dataSize = len;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	I2C_MasterTransferNonBlocking(BOARD_ACCEL_I2C_BASEADDR, &g_m_handle, &masterXfer);

	/*  wait for transfer completed. */
	while ((!nakFlag) && (!completionFlag)){
	}

	nakFlag = false;

	if (completionFlag == true){
		completionFlag = false;
		return true;
	}
	else
	{
		return false;
	}
}
