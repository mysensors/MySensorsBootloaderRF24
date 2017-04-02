/* 
 MYSBootloader 1.3.0-beta.4
 OTA RF24 bootloader for MySensors: http://www.mysensors.org
 Based on MySensors library 2.2
 Developed and maintained by tekka 2017
 
 Tested with MYSController (goo.gl/9DCWNo) 
 MCU: ATmega328p
  
 History:

 * Version 1.3.0-beta.4
 - Code optimization => reintroduction of bootloader commands: Akubi

 * Version 1.3.0-beta.3
 - Optimization / Refactorings
 
 * Version 1.3pre
 - redesign update process
 - implement STK500 protocol / serial upload
 - preferred parent with fallback option
 - save MCUSR, can be retrieved by application

 * Version 1.2
 - internal version, PoC
 
 * Version 1.1
 - use eeprom_update instead of eeprom_write to reduce wear out
 - bootloader commands: erase eeprom, set node id
 - verify incoming FW blocks for type & address
 - communicate over static parent (if set and found) else broadcast to find nearest node
 - adjusted timings 
 
 * Version 1.0
 - initial release
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */



// RF24 communication settings *****************************************************************************************
#define RF24_CHANNEL		(76)			// RF channel for the sensor net, 0-127; default 76
#define RF24_DATARATE		RF24_250KBPS	// RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
#define RF24_PA_LEVEL		RF24_PA_MAX		// PA level, RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBM, and RF24_PA_MAX=0dBm
#define RF24_ADDR_WIDTH		(5)
#define RF24_BASE_RADIO_ID	0x00,0xFC,0xE1,0xA8,0xA8
#define RF24_ARC			(15)			// Auto Retransmit Counts, see nRF24L01+ data sheet
#define RF24_ARD			(5)				// Auto Retransmit Delay, 5=1500us, see nRF24L01+ data sheet

// Options *************************************************************************************************************
#define WATCHDOG_ON_SKETCH_START			// WDT on when application starts
#define WDT_TIMEOUT			WATCHDOG_8S		// WDT timeout

// SPI bus setting *****************************************************************************************************
#define SPI_PINS_CE9_CSN10							
//define SPI_PINS_CSN7_CE8

// LED settings ********************************************************************************************************
#define LED_DDR     DDRB
#define LED_PORT    PORTB
#define LED_PIN		PINB5

// DEBUG settings ******************************************************************************************************
//#define DEBUG
#define DEBUG_PORT	PORTD
#define DEBUG_DDR	DDRD


#include "Core.h"

// prototype
int main(void) __attribute__ ((OS_main)) __attribute__ ((section (".init9")));

// implementation
int main(void) {
	asm volatile ("clr __zero_reg__");
	_save_MCUSR = MCUSR;				// save the status register for the reset cause
	MCUSR = 0;
	watchdogConfig(WDT_TIMEOUT);		// enable watchdog
	#ifdef DEBUG
		DEBUG_DDR = 0xFF;
		DEBUG_PORT = DEBUG_INIT;
	#endif

	blinkLed();

	// STK500_bootloader runs only if reset reason was EXTERNAL RESET/POWER ON
	if (_save_MCUSR & _BV(EXTRF) ) {
		STK500Bootloader();
	}

	MySensorsBootloader();

}
