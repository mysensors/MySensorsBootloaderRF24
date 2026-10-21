# MYSBootloader 1.3.0-rc.1
MySensors bootloader supporting over-the-air firmware updates

<h1>Technical details to write your own controller</h1>
<p>
All initialization the bootloader does (finding parent / requesting nodeId on
first start) uses the same packets as a normal MySensors sketch. There is no
need for the controller to distinguish between packets from the bootloader and
packets from normal sketch execution. The controller only needs to care about
two additional request/response communications. All communication is binary.
</p>

<h2>FirmwareConfig</h2>
<ul>
<li>The bootloader sends a <code>RequestFirmwareConfig</code> packet to the
gateway to request information about the firmware it should execute:
<p>
typedef struct {<br/>
&nbsp;uint16_t type;<br/>
&nbsp;uint16_t version;<br/>
&nbsp;uint16_t blocks;<br/>
&nbsp;uint16_t crc;<br/>
&nbsp;uint16_t BLVersion;<br/>
} RequestFirmwareConfig;<br/>
</p>
<li>The gateway (the controller) responds with a <code>NodeFirmwareConfig</code>
including details about the firmware the sensor should execute:
<p>
typedef struct {<br/>
&nbsp;uint16_t type;<br/>
&nbsp;uint16_t version;<br/>
&nbsp;uint16_t blocks;<br/>
&nbsp;uint16_t crc;<br/>
} NodeFirmwareConfig;<br/>
</p>
</ul>

<h2>Firmware</h2>
<ul>
<li>The bootloader sends a <code>RequestFirmwareBlock</code> packet to the
gateway to request a specific subset (block) of the compiled firmware:
<p>
typedef struct {<br/>
&nbsp;uint16_t type;<br/>
&nbsp;uint16_t version;<br/>
&nbsp;uint16_t block;<br/>
} RequestFirmwareBlock;<br/>
</p>
<li>The gateway (the controller) responds with a <code>ResponseFirmwareBlock</code>
including the specific block of the compiled firmware:
<p>
typedef struct {<br/>
&nbsp;uint16_t type;<br/>
&nbsp;uint16_t version;<br/>
&nbsp;uint16_t block;<br/>
&nbsp;uint8_t data[FIRMWARE_BLOCK_SIZE];<br/>
} ResponseFirmwareBlock;<br/>
</p>
</ul>

<h1>Supported nRF24 wiring configurations</h1>
<p>
The bootloader supports several CE/CSN pin combinations. Select exactly one
<code>SPI_PINS_*</code> define in <code>MYSBootloader.c</code> before
compiling, and uncomment the matching line in the <i>SPI bus setting</i>
block. Only one define must be active at a time. If none is defined, the
compiler will fail with an error about undefined <code>CE_PORT</code> /
<code>CSN_PORT</code> macros, which is the intended behaviour: the wiring
must be explicit.
</p>
<table border="1" cellpadding="4" cellspacing="0">
<tr><th>Define</th><th>CE pin</th><th>CSN pin</th><th>Board example</th></tr>
<tr><td><code>SPI_PINS_CE9_CSN10</code></td><td>D9 (PB1)</td><td>D10 (PB2)</td><td>Default MySensors</td></tr>
<tr><td><code>SPI_PINS_CSN7_CE8</code></td><td>D8 (PB0)</td><td>D7 (PD7)</td><td></td></tr>
<tr><td><code>SPI_PINS_CE7_CSN8</code></td><td>D7 (PD7)</td><td>D8 (PB0)</td><td>RF-Nano Red</td></tr>
<tr><td><code>SPI_PINS_CE4_CSN10</code></td><td>D4 (PD4)</td><td>D10 (PB2)</td><td></td></tr>
<tr><td><code>SPI_PINS_CE7_CSN10</code></td><td>D7 (PD7)</td><td>D10 (PB2)</td><td>NRF24Duino</td></tr>
<tr><td><code>SPI_PINS_CE10_CSN9</code></td><td>D10 (PB2)</td><td>D9 (PB1)</td><td>RF-Nano Black</td></tr>
<tr><td><code>SPI_PINS_CE4_CSN5</code></td><td>D4 (PD4)</td><td>D5 (PD5)</td><td>RFduino, some RF-Nano variants</td></tr>
</table>

<h2>Note on RF-Nano boards</h2>
<p>
"RF-Nano" is a generic name used by several manufacturers for Arduino
Nano-sized boards with an integrated nRF24L01+. The CE/CSN wiring is
<b>not guaranteed</b> to be the same across vendors or even across batches
from the same vendor. Common variants include:
</p>
<ul>
<li><b>RF-Nano Red</b> (red PCB, V1): usually <code>SPI_PINS_CE7_CSN8</code>
(CE=D7, CSN=D8).</li>
<li><b>RF-Nano Black</b> (black PCB, V1/V3): usually
<code>SPI_PINS_CE10_CSN9</code> (CE=D10, CSN=D9).</li>
</ul>
<p>
If your board does not match either variant, identify the actual CE and CSN
pins by following the PCB traces from the nRF24L01+ module to the
ATmega328P. The nRF24L01+ pinout is: pin 1 = GND, pin 2 = VCC, pin 3 = CE,
pin 4 = CSN, pin 5 = SCK, pin 6 = MOSI, pin 7 = MISO, pin 8 = IRQ. Once you
know which ATmega328P pin each signal goes to, select the matching
<code>SPI_PINS_*</code> define, or add a new one in <code>HW.h</code> if no
existing define matches.
</p>

<h1>Build and flash</h1>
<p>
The bootloader is built with <code>avr-gcc</code> and flashed with
<code>avrdude</code>. The default <code>Makefile</code> targets an ATmega328P
with a 2 KB bootloader at <code>0x7800</code>.
</p>
<p>
The fuses depend on the clock source. The recommended values for
MYSBootloader are:
</p>
<table border="1" cellpadding="4" cellspacing="0">
<tr><th>Clock source</th><th>lfuse</th><th>hfuse</th><th>efuse</th></tr>
<tr><td>External crystal 8-16 MHz</td><td>0xFF</td><td>0xD2</td><td>0x06</td></tr>
<tr><td>Internal RC 8 MHz</td><td>0xE2</td><td>0xD2</td><td>0x06</td></tr>
<tr><td>Internal RC 1 MHz</td><td>0x62</td><td>0xD2</td><td>0x06</td></tr>
</table>
<p>
<code>hfuse=0xD2</code> is constant: it selects the 2048-byte boot section
(<code>BOOTSZ=01</code>) and enables <code>BOOTRST</code>, which places the
bootloader at <code>0x7800</code>. Verify the <code>lfuse</code> against your
board's actual clock source before flashing.
</p>