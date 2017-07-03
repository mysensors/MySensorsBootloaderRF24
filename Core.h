/* 
 MYSBootloader 1.3.0-beta.4
 OTA RF24 bootloader for MySensors: http://www.mysensors.org
 Based on MySensors library 2.2
 Developed and maintained by tekka 2017
*/

#ifndef Core_H
#define Core_H

// version *************************************************************************************************************
#define MYSBOOTLOADER_MAJVER 1
#define MYSBOOTLOADER_MINVER 3
#define MYSBOOTLOADER_VERSION ((MYSBOOTLOADER_MINVER << 8) + MYSBOOTLOADER_MAJVER)

// size setting ********************************************************************************************************
#define BOOTLOADER_SIZE	(2048)
#define BOOTLOADER_START_ADDRESS (0x8000 - BOOTLOADER_SIZE)

// DEBUG led patterns  *************************************************************************************************

#define DEBUG_INIT				_BV(3)
#define DEBUG_READ_CONFIG		_BV(4)
#define DEBUG_FIND_PARENTS		_BV(5)
#define DEBUG_CHECK_ID			_BV(6)
#define DEBUG_CONFIGURATION		_BV(7)
#define DEBUG_CONFIG_RECEIVED	_BV(8)
#define DEBUG_INIT_UPDATE		_BV(6) | _BV(7)
#define DEBUG_DO_UPDATE			_BV(4) | _BV(5) | _BV(7)
#define DEBUG_PREPARE_RUN		_BV(4) | _BV(5) | _BV(7)
#define DEBUG_RUN				_BV(4) | _BV(5) | _BV(6) | _BV(7)

#include <string.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "Definitions.h"
#include "boot.h"
#include "HW.h"
#include "MyEepromAddresses.h"
#include "MyMessage.h"
#include "RF24.h"
#include "STK500Bootloader.h"
#include "MySensorsBootloader.h"


// global variables
MyMessage _outMsg;
MyMessage _inMsg;
nodeConfig_t _eepromNodeConfig;
uint8_t _configuredParentID;
uint8_t _save_MCUSR;


#endif // Core_H