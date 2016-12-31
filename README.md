# MYSBootloader 1.3-beta3
MySensors bootloader supporting over-the-air firmware updates 

<h1>Technical details to write your own controller</h1>
All initialization the bootloader does (finding parent / requesting nodeId on first start) uses the same packets as a normal MySensors sketch. There is no need for the controller to distinguish between packets from the bootloader and packets from normal sketch execution. The controller only needs to care about two additional request/response communications. All communication is binary.
<h2>FirmwareConfig</h2>
<ul>
<li>the bootloader sends a RequestFirmwareConfig packet to the gateway to request information about the firmware it should execute:
<p>
typedef struct {<br/>
&nbsp;uint16_t type;<br/>
&nbsp;uint16_t version;<br/>
&nbsp;uint16_t blocks;<br/>
&nbsp;uint16_t crc;<br/>
&nbsp;uint16_t BLVersion;<br/>
} RequestFirmwareConfig;<br/>
</p>
<li>the gateway (the controller) responds with a NodeFirmwareConfig including details about the firmware the sensor should execute:
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
<li>the bootloader sends a RequestFirmwareBlock packet to the gateway to request a specific subset (block) of the compiled firmware:
<p>
typedef struct {<br/>
&nbsp;uint16_t type;<br/>
&nbsp;uint16_t version;<br/>
&nbsp;uint16_t block;<br/>
} RequestFirmwareBlock;<br/>
</p>
<li>the gateway (the controller) responds with a ResponseFirmwareBlock including the specific block of the compiled firmware:
<p>
typedef struct {<br/>
&nbsp;uint16_t type;<br/>
&nbsp;uint16_t version;<br/>
&nbsp;uint16_t block;<br/>
&nbsp;uint8_t data[FIRMWARE_BLOCK_SIZE];<br/>
} ResponseFirmwareBlock;<br/>
</p> 
</ul>
