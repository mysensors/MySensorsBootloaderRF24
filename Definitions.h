/* 
 MYSBootloader 1.3.0-beta.4
 OTA RF24 bootloader for MySensors: http://www.mysensors.org
 Based on MySensors library 2.2
 Developed and maintained by tekka 2017
*/

#ifndef Definitions_H
#define Definitions_H

#define FIRMWARE_BLOCK_SIZE	(16)
#define EEPROM_SIZE 1024			// 1024 bytes for ATMEGA328

#define false	(0)
#define true	(1)

typedef uint8_t bool;

typedef enum {
	BL_INIT,
	BL_READ_CONFIG,
	BL_FIND_PARENTS,
	BL_CHECK_ID,
	BL_CONFIGRUATION,
	BL_INIT_UPDATE,
	BL_DO_UPDATE,
	BL_VALIDATE_OTA,
	BL_RUN,
	BL_EXIT
} SM_BL_STATE;


typedef struct {
	union {
		uint16_t type;
		uint16_t bl_command;
	} type_command;
	union {
		uint16_t version;
		uint16_t bl_data;
	} version_data;
	uint16_t blocks;
	uint16_t crc;
} __attribute__((packed)) nodeFirmwareConfig_t;

typedef struct {
	uint16_t type;
	uint16_t version;
	uint16_t blocks;
	uint16_t crc;
	uint16_t BLVersion;
} __attribute__((packed)) requestFirmwareConfig_t;

typedef struct {
	uint16_t type;
	uint16_t version;
	uint16_t block;
} __attribute__((packed)) requestFirmwareBlock_t;

typedef struct {
	uint16_t type;
	uint16_t version;
	uint16_t block;
	uint8_t data[FIRMWARE_BLOCK_SIZE];
} __attribute__((packed)) responseFirmwareBlock_t;

typedef struct {
	uint8_t nodeId;			// Current node id
	uint8_t parentNodeId;	// Where this node sends its messages
	uint8_t distance;		// This nodes distance to sensor net gateway (number of hops)
} __attribute__((packed)) nodeConfig_t;


#endif // Definitions_H