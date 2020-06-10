/*
 * i2c_driver.h
 *
 * Created: 2013/09/15 14:57:21
 *  Author: sazae7
 */ 


#ifndef I2C_DRIVER_H_
#define I2C_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define I2C_OK							(0)
#define I2C_ERROR_NACK					(uint8_t)(-1)
#define I2C_INVALID_MODE				(uint8_t)(-2)
#define I2C_INVALID_CLOCK				(uint8_t)(-3)
#define I2C_MODULE_IS_NOT_MASTER_MODE	(uint8_t)(-4)
#define I2C_NOT_IMPLEMENTED				(uint8_t)(-5)
#define I2C_BUFFER_IS_NULL_PTR			(uint8_t)(-6)

#define I2C_INVALUID_ARGS				(uint8_t)(-7)
#define I2C_BUS_ERROR					(uint8_t)(-8)
#define I2C_IS_IN_USED					(uint8_t)(-9)

/*---------------------------------------------------------------------------*/
#define I2C_IDLE						(0)
#define I2C_MASTER_READ					(1)
#define I2C_MASTER_WRITE				(2)

#define I2C_SLAVE_RESPONCE_NACK			(1)
#define I2C_SLAVE_RESPONCE_ACK			(0)


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
typedef uint8_t (*i2c_StartCallback)(uint8_t mode);
typedef uint8_t (*i2c_StopCallback)(void);
typedef uint8_t (*i2c_MasterRxCallback)(uint8_t ack);
typedef uint8_t (*i2c_MasterTxCallback)(uint8_t data);

typedef void    (*i2c_done_callback)(uint8_t status);

/*---------------------------------------------------------------------------*/
typedef struct 
{
	uint8_t ownAddress;
	i2c_StartCallback startCB;
	i2c_StopCallback stopCB;
	i2c_MasterRxCallback masterRxCB;
	i2c_MasterTxCallback masterTxCB;
} I2C_INIT_OPT;


/*---------------------------------------------------------------------------*/
typedef enum {
	I2C_NOT_INITIALIZED,
	I2C_SLAVE,
	I2C_MASTER
} I2C_MODE;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
uint8_t initialize_i2c(uint32_t clk, uint32_t sysclkHz, I2C_MODE mode, I2C_INIT_OPT *opt);
uint8_t i2c_txRxBytes(uint8_t slaveAddress, uint8_t *txBuf, uint32_t txLen, uint8_t *rxBuf, uint32_t rxLen);
uint8_t i2c_txRxBytes_cb(uint8_t slaveAddress, uint8_t *txBuf, uint32_t txLen, uint8_t *rxBuf, uint32_t rxLen, i2c_done_callback cb);
uint8_t i2c_checkDevice(uint8_t slaveAddress);

/*---------------------------------------------------------------------------*/
#define I2C_TX_BYTES(slvAdr, buf, len)			(i2c_txRxBytes(slvAdr, buf, len, NULL, 0))
#define I2C_RX_BYTES(slvAdr, buf, len)			(i2c_txRxBytes(slvAdr, NULL, 0, buf, len))


#ifdef __cplusplus
};
#endif

#endif /* I2C_DRIVER_H_ */