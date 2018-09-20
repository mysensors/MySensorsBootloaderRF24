/* 
 * MYSBootloader 1.3.0-rc.1
 * OTA RF24 bootloader for MySensors: https://www.mysensors.org
 * Based on MySensors library 2.2
 * Developed and maintained by tekka 2018
 */

#ifndef HW_H
#define HW_H

// hardware
# define UART_SRA UCSR0A
# define UART_SRB UCSR0B
# define UART_SRC UCSR0C
# define UART_SRL UBRR0L
# define UART_UDR UDR0

/* set the UART baud rate defaults */
#ifndef BAUD_RATE
	#if F_CPU >= 16000000L
		#define BAUD_RATE   115200L
	#elif F_CPU >= 8000000L
		#define BAUD_RATE   57600L
	#elif F_CPU >= 1000000L
		#define BAUD_RATE   9600L   // 19200 also supported, but with significant error
	#elif F_CPU >= 128000L
		#define BAUD_RATE   4800L   // Good for 128kHz internal RC
	#else
		#define BAUD_RATE 1200L     // Good even at 32768Hz
	#endif
#endif

#ifndef UART
	#define UART 0
#endif

#define BAUD_SETTING (( (F_CPU + BAUD_RATE * 4L) / ((BAUD_RATE * 8L))) - 1 )
#define BAUD_ACTUAL (F_CPU/(8 * ((BAUD_SETTING)+1)))
#if BAUD_ACTUAL <= BAUD_RATE
	#define BAUD_ERROR (( 100*(BAUD_RATE - BAUD_ACTUAL) ) / BAUD_RATE)
	#if BAUD_ERROR >= 5
		#error BAUD_RATE error greater than -5%
	#elif BAUD_ERROR >= 2
		#warning BAUD_RATE error greater than -2%
	#endif
#else
	#define BAUD_ERROR (( 100*(BAUD_ACTUAL - BAUD_RATE) ) / BAUD_RATE)
	#if BAUD_ERROR >= 5
		#error BAUD_RATE error greater than 5%
	#elif BAUD_ERROR >= 2
		#warning BAUD_RATE error greater than 2%
	#endif
#endif

#if (F_CPU + BAUD_RATE * 4L) / (BAUD_RATE * 8L) - 1 > 250
	#error Unachievable baud rate (too slow) BAUD_RATE
#endif // baud rate slow check
#if (F_CPU + BAUD_RATE * 4L) / (BAUD_RATE * 8L) - 1 < 3
	#if BAUD_ERROR != 0 // permit high bitrates (ie 1Mbps@16MHz) if error is zero
		#error Unachievable baud rate (too fast) BAUD_RATE
	#endif
#endif

// Watchdog definitions and functions
#define WATCHDOG_OFF    (0)
#define WATCHDOG_16MS   (_BV(WDE))
#define WATCHDOG_32MS   (_BV(WDP0) | _BV(WDE))
#define WATCHDOG_64MS   (_BV(WDP1) | _BV(WDE))
#define WATCHDOG_125MS  (_BV(WDP1) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_250MS  (_BV(WDP2) | _BV(WDE))
#define WATCHDOG_500MS  (_BV(WDP2) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_1S     (_BV(WDP2) | _BV(WDP1) | _BV(WDE))
#define WATCHDOG_2S     (_BV(WDP2) | _BV(WDP1) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_4S     (_BV(WDP3) | _BV(WDE))
#define WATCHDOG_8S     (_BV(WDP3) | _BV(WDP0) | _BV(WDE))


static inline void watchdogReset(void) {
	__asm__ __volatile__ ("wdr\n");
}

static void watchdogConfig(const uint8_t wdtConfig) {
	WDTCSR = _BV(WDCE) | _BV(WDE);
	WDTCSR = wdtConfig;
}


// SPI communication
#define SPI_PORT	PORTB	// 
#define SPI_DDR		DDRB	// 
#define	SPI_SCLK	PB5		// Arduino Pin 13 <-> Bit 5 of port B
#define	SPI_MISO	PB4		// Arduino Pin 12 <-> Bit 4 of port B
#define	SPI_MOSI	PB3		// Arduino Pin 11 <-> Bit 3 of port B

#if defined(SPI_PINS_CE9_CSN10)
	#define CSN_PORT	PORTB	// port for CSN
	#define CSN_DDR		DDRB	// DDR for CSN
	#define	CSN_PIN		PB2		// Arduino Pin 10 <-> Bit 2 of port B

	#define CE_PORT		PORTB	// port for CE 
	#define CE_DDR		DDRB	// DDR for CE
	#define	CE_PIN		PB1		// Arduino Pin  9 <-> Bit 1 of port B
#elif defined(SPI_PINS_CSN7_CE8)
	#define CSN_PORT	PORTD	// port for CSN
	#define CSN_DDR		DDRD	// DDR for CSN
	#define	CSN_PIN		PD7		// Arduino Pin 7 <-> Bit 7 of port D

	#define CE_PORT		PORTB	// port for CE
	#define CE_DDR		DDRB	// DDR for CE
	#define	CE_PIN		PB0		// Arduino Pin  8 <-> Bit 0 of port B
#elif defined(SPI_PINS_CE4_CSN10)
	#define CSN_PORT	PORTB	// port for CSN
	#define CSN_DDR		DDRB	// DDR for CSN
	#define	CSN_PIN		PB2		// Arduino Pin 10 <-> Bit 2 of port B

	#define CE_PORT		PORTD	// port for CE
	#define CE_DDR		DDRD	// DDR for CE
	#define	CE_PIN		PD4		// Arduino Pin  4 <-> Bit 4 of port D
#elif defined(SPI_PINS_CE7_CSN10)
	// NRF24Duino Configuration
	#define CE_PORT		PORTD	// port for CE
	#define CE_DDR		DDRD	// DDR for CE
	#define	CE_PIN		PD7		// Arduino Pin 7 <-> Bit 7 of port D

	#define CSN_PORT	PORTB	// port for CSN
	#define CSN_DDR		DDRB	// DDR for CSN
	#define	CSN_PIN		PB2		// Arduino Pin  10 <-> Bit 2 of port B
	// NRF24Duino has LED on Pin 9 set the LED_PIN in MYSBootloader.c to PB1
#endif

#define CSN_LOW()	CSN_PORT &= ~_BV(CSN_PIN)
#define CSN_HIGH()	CSN_PORT |= _BV(CSN_PIN)
#define CE_LOW()	CE_PORT &= ~_BV(CE_PIN)
#define CE_HIGH()	CE_PORT |= _BV(CE_PIN)


static void initSPI(void) {
	// Initialize the SPI pins: SCK, MOSI, CE, CSN as outputs, MISO as input
	#if defined(SPI_PINS_CE9_CSN10)
		// CSN_PIN (=PB2) is SS pin and set as output
		SPI_DDR = _BV(SPI_MOSI) | _BV(SPI_SCLK) | _BV(CE_PIN) | _BV(CSN_PIN);
	#elif defined(SPI_PINS_CSN7_CE8)
		// PB2 is SS pin has to be defined as OUTPUT, else SPI goes to slave mode
		SPI_DDR = _BV(SPI_MOSI) | _BV(SPI_SCLK) | _BV(CE_PIN) | _BV(PB2);	
		CSN_DDR = _BV(CSN_PIN);
	#elif defined(SPI_PINS_CE4_CSN10)
		// PB2 is SS pin has to be defined as OUTPUT, else SPI goes to slave mode
		SPI_DDR = _BV(SPI_MOSI) | _BV(SPI_SCLK) | _BV(PB2) | _BV(CSN_PIN);
		CE_DDR = _BV(CE_PIN);
	#elif defined(SPI_PINS_CE7_CSN10)
		// PB2 is SS pin has to be defined as OUTPUT, else SPI goes to slave mode
		SPI_DDR = _BV(SPI_MOSI) | _BV(SPI_SCLK) | _BV(PB2) | _BV(CSN_PIN);
		CE_DDR = _BV(CE_PIN);
	#endif
	
	// SPE	=	SPI enable
	// SPIE	=	SPI interrupt enable
	// DORD	=	data order (0:MSB first, 1:LSB first)
	// MSTR	=	Master/Slave select
	// SPR1	=	SPI clock rate bit 1
	// SPR0	=	SPI clock rate bit 0; 0,0=osc/4, 0,1=osc/16, 1,0=osc/64, 1,1=osc/128
	// CPOL	=	clock polarity idle (0:low, 1:high)
	// CPHA	=	clock phase edge sampling (0:leading, 1:trailing)
	
	// SPI speed setting, nRF24L01P max. 10Mhz	
	#if (F_CPU >= 16000000)
		// DIV 16 = 1 Mhz
		SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
	#elif (F_CPU >= 8000000)
		// DIV 8 = 1 Mhz
		SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
		SPSR = _BV(SPI2X);  
	#elif (F_CPU >= 4000000)
		// DIV 4 = 1Mhz
		SPCR = _BV(SPE) | _BV(MSTR);
	#elif (F_CPU >= 2000000)
		// DIV 2 = 1 Mhz
		SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
		SPSR = _BV(SPI2X);
	#else
		// DIV 2 <= 0.5 Mhz
		SPCR = _BV(SPE) | _BV(MSTR);
		SPSR = _BV(SPI2X); 
	#endif          
	
}
static uint8_t SPItransfer(const uint8_t value) {
	SPDR = value;
	while(!(SPSR & _BV(SPIF)));		// wait until transmitted
	return SPDR;
}
static inline void SPIclose(void) {
	// disable hardware SPI
	SPCR = 0;	
}


// UART
static void initUART(void) {
	UART_SRA = _BV(U2X0); //Double speed mode USART0
	UART_SRB = _BV(RXEN0) | _BV(TXEN0);
	UART_SRC = _BV(UCSZ00) | _BV(UCSZ01);
	UART_SRL = (uint8_t)( (F_CPU + BAUD_RATE * 4L) / (BAUD_RATE * 8L) - 1 );
}

void putch(const uint8_t ch) {
	while (!(UART_SRA & _BV(UDRE0)));
	UART_UDR = ch;
}

static uint8_t getch(void) {
	// wait until char received
	while(!(UART_SRA & _BV(RXC0)));
	// 10 bytes
	// framing error?
	if (!(UART_SRA & _BV(FE0))) {
		watchdogReset();
	}
	return UART_UDR;
}

static inline void writeTemporaryBuffer(const uint16_t address, const uint16_t data) {
	// fill temporary page buffer
	__boot_page_fill_short(address, data);
}

static void programPage(const uint16_t page){
	__boot_page_erase_short(page);	// erase page
	boot_spm_busy_wait();
	__boot_page_write_short(page);	// program page
	boot_spm_busy_wait();
	__boot_rww_enable_short();		// re-enable RWW
}

static uint16_t crc16_update(uint16_t crc, const uint8_t data) {
	crc ^= data;
	for (uint8_t i = 0; i < 8; ++i) {
		crc = (crc >> 1) ^ (-(int16_t)(crc & 1) & 0xA001);
	}
	return crc;
}

static uint16_t calcCRCrom (const uint16_t len) {
	// init CRC
	uint16_t _internal_crc = 0xFFFF;	
	// start address for CRC calculation
	uint16_t address = 0x0000;
	// calc and prevent overflow
	while (address < len && address < BOOTLOADER_START_ADDRESS) {
		uint8_t _rom_byte;
		// read a flash byte and increment the address
		__asm__ ("lpm %0,Z+\n" : "=r" (_rom_byte), "=z" (address): "1" (address));
		_internal_crc = crc16_update(_internal_crc,_rom_byte);
	}
	return _internal_crc;
}

static void blinkLed(void) {
	LED_DDR |= _BV(LED_PIN);
	//300ms total
	for (uint8_t i = 0; i < 6; i++) {
		LED_PORT ^= _BV(LED_PIN);
		_delay_ms(50);		
	}
}


#endif // HW_H
