/* 
 MYSBootloader 1.3pre3
 OTA RF24 bootloader for MySensors: http://www.mysensors.org
 Based on MySensors library 2.1
 Developed and maintained by tekka 2016
*/

#ifndef RF24_H
#define RF24_H

#include "RF24_registers.h"

extern nodeConfig_t _eepromNodeConfig;

#define RF24_SETUP ( ( (RF24_DATARATE & 0x02 ) << 4) | ((RF24_DATARATE & 0x01 ) << 3) | (RF24_PA_LEVEL << 1) ) + 1

typedef enum { 
	RF24_PA_MIN = 0, 
	RF24_PA_LOW, 
	RF24_PA_HIGH, 
	RF24_PA_MAX
} rf24_pa_t;

typedef enum { 
	RF24_1MBPS = 0, 
	RF24_2MBPS, 
	RF24_250KBPS 
} rf24_datarate_t;

#define RF24_CONFIG _BV(CRCO) | _BV(EN_CRC)

#define _write_register(reg) ( (reg) | W_REGISTER)
#define _read_register(reg) ( (reg) | R_REGISTER)

uint8_t base_addr[RF24_ADDR_WIDTH] = { RF24_BASE_RADIO_ID };


static uint8_t SPIBytes(uint8_t addr, uint8_t* buf, uint8_t len, bool aReadMode) {
	CSN_LOW();
	uint8_t status = SPItransfer( addr );
	
	while ( len-- ) {
		if (aReadMode) {		
			status = SPItransfer(NOP);
			if(buf!=NULL) *buf++ = status;
		} else status = SPItransfer(*buf++);
	}

	CSN_HIGH();
	return status;
}

static uint8_t burstWriteAddress(uint8_t addr, uint8_t* buf, uint8_t len) {
	return SPIBytes(addr,buf,len,false);
}


static uint8_t writeAddress(uint8_t addr, uint8_t buf) {
	return SPIBytes(addr,&buf,1,false);
}


#define _readAddress(addr) SPIBytes(addr,NULL,1,true)
#define _burstWriteRegister(reg,buf,len) burstWriteAddress(_write_register(reg), buf, len)
#define _burstReadRegister(reg,buf,len) SPIBytes(_read_register(reg), buf, len, true)
#define _writeRegister(reg,val) writeAddress(_write_register(reg), val)
#define _readRegister(reg) _readAddress(_read_register(reg))
#define _getStatus() SPIBytes(NOP, NULL, 0,true)
#define _dataAvailable() !(_readRegister(FIFO_STATUS) & ( _BV(RX_EMPTY) ))
#define _resetInterrupts() _writeRegister(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) ) 
#define _flushTX() burstWriteAddress(FLUSH_TX, NULL, 0)
#define _flushRX() burstWriteAddress(FLUSH_RX, NULL, 0)


static void Flush_RXTX_CLI(void) {
	// flush RX and TX buffer
	_flushRX();
	_flushTX();
	// clear interrupts
	_resetInterrupts();
}

static bool writeBuf(uint8_t* buf, uint8_t len ) {
	uint8_t status;
	// write payload to FIFO, broadcasts do not require ACK
	//_burstWriteRegister( broadcast ? W_TX_PAYLOAD_NO_ACK : W_TX_PAYLOAD, buf, len) ;
	_burstWriteRegister( W_TX_PAYLOAD, buf, len) ;
	// CE pulse to start transmission
	CE_HIGH();
	do {
		// get status byte
		CSN_LOW();
		status = SPItransfer(NOP);
		CSN_HIGH();
		
	} while( !( status  & ( _BV(TX_DS) | _BV(MAX_RT) )) );
	
	CE_LOW();
	
	_writeRegister(STATUS, _BV(TX_DS) | _BV(MAX_RT) ); 
	
	// max retries exceeded, flush tx buffer
	if(status & _BV(MAX_RT)) {
		// flush RX FIFO
		_flushTX();
	}
	
	return (status  & _BV(TX_DS) ); 
}

static uint8_t getDynamicPayloadSize(void) {
	uint8_t result = _readAddress(R_RX_PL_WID);
	// discard payloads with length > 32
	if(result > 32) {
		// flush RX FIFO
		_flushRX();
		result = 0;
	}
	return result;
}



static uint8_t readMessage(uint8_t* buf) {
	const uint8_t len = getDynamicPayloadSize();
	// read payload
	_burstReadRegister(R_RX_PAYLOAD,buf,len);
	// reset RX interrupt
	Flush_RXTX_CLI();
	//_writeRegister(STATUS, _BV(RX_DR) ); 
	//#warning maybe clear IRQ only
	return len;
}


static bool writeMessage(const uint8_t recipient, uint8_t* buf, const uint8_t len) {	
	//stop listening
	CE_LOW();
	// switch to TX mode
	_writeRegister(CONFIG, RF24_CONFIG | _BV(PWR_UP) ) ;
	// flush FIFO and interrupts
	Flush_RXTX_CLI();
	// set pipe 0 RX/TX address		
	base_addr[0] = recipient;
	_burstWriteRegister(RX_ADDR_P0, base_addr, RF24_ADDR_WIDTH);
	_burstWriteRegister(TX_ADDR, base_addr, RF24_ADDR_WIDTH);
	// write payload
	const bool result = writeBuf(buf, len);
	// start listening	
	_writeRegister(CONFIG, RF24_CONFIG | _BV(PWR_UP) | _BV(PRIM_RX) ) ;
	// set pipe0 RX address
	_writeRegister(RX_ADDR_P0, _eepromNodeConfig.nodeId);
	// flush FIFO and interrupts
	Flush_RXTX_CLI();
	// go!
	CE_HIGH();

	return result;
}

static bool initRadio(){		 
	CE_LOW();
	CSN_HIGH();
	// set address width
	_writeRegister(SETUP_AW, RF24_ADDR_WIDTH - 2 );
	// auto retransmit delay 1500us, auto retransmit count 0 (BROADCASTS)
	//_writeRegister(SETUP_RETR, 5 << ARD | 15 << ARC);
	// set channel
	_writeRegister(RF_CH, RF24_CHANNEL);
	// set data rate and pa level, +1 for Si24R1
	_writeRegister(RF_SETUP, RF24_SETUP);
	// unlock features (certain nRF24 clones and non-P)
	writeAddress(ACTIVATE, 0x73);
	// enable dynamic payload length
	_writeRegister(FEATURE, _BV(EN_DPL) );	
	// enable pipe 0
	_writeRegister(EN_RXADDR, _BV(ERX_P0));
	// enable autoACK on pipe 0
	_writeRegister(EN_AA, _BV(ENAA_P0));
	// Enable dynamic payload length on pipe 0
	_writeRegister(DYNPD, _BV(DPL_P0));	
	// flush FIFOs and clear IRQs
	//Flush_RXTX_CLI();
	return _readRegister(RF_SETUP)==RF24_SETUP;	// sanity check
}

#endif // RF24_H