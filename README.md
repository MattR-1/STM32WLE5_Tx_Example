# STM32WLE5_Tx_Example

THIS CODE DOESN'T WORK YET!!!

The goal of this project is to send some data from one STM32WLE5 to anotherone using LoRa. It is supposed to be as simple as possible. Therefore the roles and all the parameters are fixed.

I am using the Wio-E5-LE module by Seeed Studio as the rf-module on the devellopment boards 'Wio-E5-LE Dev Kit' and 'Wio-E5-LE mini Dev Board'. The Wio-E5 modules use the STM32WLE5JC.

The Wio-E5 comes with an AT-command software wich has to be cleared first. After that it is programmed with the STM32CubeIDE.

Sadly Seeed Studio doesn't supply any example and STMicroelectronics only provides one example which sets the chip up so send an empty package on an unspecified frequency. This code however is based on this example and expands it by an data input and a full configuration of the chip. There is another example in my repository wich provides code for the reciever.

Apart from the example code by STM (STMicroelectronics/STM32CubeWL/Projects/NUCLEO-WL55JC/Examples/SUBGHZ/SUBGHZ_Tx_Mode) I used documents RM0461 (Reference manual for STM32WLEx) and UM2642 (Description of STM32WL HAL and low-layer drivers).

The code is structured after chapter 4.9.1 from RM0461 and is configured to use a frequency allowed in Europ an otherwise default, easy to use or parameters that increase stability. Details are explained in the chapters describing the code that defines them. 

There is another reference manual: RM0453, which seems to have the same content as RM0461 for the STM32WL5x. The SUB-GHZ-radio chapters look like they are the same.



## Code structure from RM0461, chapter 4.9.1
The sub-GHz radio can be set in LoRa, (G)MSK or (G)FSK transmit operation mode with
the following steps:

1. Define the location of the transmit payload data in the data buffer, with
    Set_BufferBaseAddress().
2. Write the payload data to the transmit data buffer with Write_Buffer().
3. Select the packet type (generic or LoRa) with Set_PacketType().
4. Define the frame format with Set_PacketParams().
5. Define synchronization word in the associated packet type SUBGHZ_xSYNCR(n) with
    Write_Register( ).
6. Define the RF frequency with Set_RfFrequency().
7. Define the PA configuration with Set_PaConfig().
8. Define the PA output power and ramping with Set_TxParams().
9. Define the modulation parameters with Set_ModulationParams().
10. Enable TxDone and timeout interrupts by configuring IRQ with Cfg_DioIrq().
11. Start the transmission by setting the sub-GHz radio in TX mode with Set_Tx(). After
    the transmission is finished, the sub-GHz radio enters automatically the Standby mode.
12. Wait for sub-GHz radio IRQ interrupt and read interrupt status with
    Get_IrqStatus():
    a) On a TxDone interrupt, the packet is successfully sent
    b) On a timeout interrupt, the transmission is timeout.
13. Clear interrupt with Clr_IrqStatus().
14. Optionally, send a Set_Sleep() command to force the sub-GHz radio in Sleep mode.



### 1. Define the location of the transmit payload data in the data buffer

#### code

RadioParam[0] = 0x80U;
RadioParam[1] = 0x00U;

if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_BUFFERBASEADDRESS, &RadioParam, 2) != HAL_OK)
{
	Error_Handler();
}



#### explanation

Optcode: 0x8fU



**Set_BufferBaseAddress() command**
Set_BufferBaseAddress(TxBaseAddr, RxBaseAddr) sets the data buffer base
address for the packet handling in TX and RX.

byte 0 (w) bits 7:0 Opcode: 0x8F
byte 1 (w) bits 7:0 TxBaseAddr[7:0]: Tx base address offset relative to the sub-GHz RAM base address
byte 2 (w) bits 7:0 RxBaseAddr[7:0]: Rx base address offset relative to the sub-GHz RAM base address



From RM0461 - Chapter 4.6:

In order to retrieve data after Sleep mode retention, the default values must be used (TxBaseAddr = 0x80 and RxBaseAddr = 0x00), or RxPayloadLength and RxBufferPointer must be stored in the CPU memory.

#### parameter bytes

| **byte** | **hex** | **bin**     | **note**        |
| -------- | ------- | ----------- | --------------- |
| 1        | 0x80    | 0b1000 0000 | Tx base address |
| 2        | 0x00    | 0b0000 0000 | Rx base address |



### 2. Write Payload to buffer

#### code

uint8_t payload[64] = "Hello World!";

if (HAL_SUBGHZ_WriteBuffer(&hsubghz, 0, &payload, sizeof(payload)) != HAL_OK)
{
	Error_Handler();
}

#### explanation

The second parameter (0) is the offset in the buffer.



### 3. Select packet type

#### code

RadioParam[0] = 0x01U;

if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETTYPE, &RadioParam, 1) != HAL_OK)
{
	Error_Handler();
}



#### explanation

Set_PacketType(PktType) allows the selection of packet frame format. This command must be the first command of a sub-GHz radio configuration sequence. Changing from one sub-GHz radio configuration to another is done using Set_PacketType(). The parameters from the previous sub-GHz radio configuration are lost. The switch from one configuration mode to another is only accepted in Standby mode.

byte 0 (w)        bits 7:0 Opcode: 0x8A.
byte 1 (w)	bits 7:2 Reserved, must be kept at reset value.
			 bits 1:0 PktType[1:0]: Packet type definition

​			 0: FSK generic packet type
​			 1: LoRa packet type
​			 2: BPSK packet type
​			 3: MSK generic packet type
​			 Other: reserved

#### parameter bytes

| **byte** | **hex** | **bin**     | **note**    |
| -------- | ------- | ----------- | ----------- |
| 1        | 0x01    | 0b0000 0001 | packet type |



### 4. Define frame format

#### code

RadioParam[0] = 0x00U;
RadioParam[1] = 0x0CU;
RadioParam[2] = 0x00U;
RadioParam[3] = 0x40U;
RadioParam[4] = 0x01U;
RadioParam[5] = 0x00U;

if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETPARAMS, &RadioParam, 6) != HAL_OK)
{
	Error_Handler();
}



#### explanation

**LoRa Set_PacketParams() command**
Set_PacketParams(PbLength, HeaderType, PayloadLength, CrcType,
InvertIQ) is used to configure the packet handling for the sub-GHz radio. Depending on
the selected packet type in Set_PacketType() sent prior to this function, the parameters
are interpreted as below.

byte 0           bits 7:0       Opcode: 0x8C.
bytes 2:1      bits 15:0     PbLength[15:0]: Preamble length in number of symbols
                                             0x0000: reserved
                                             0x0001 - 0xFFFF: 1 to 65535 symbols
byte 3           bits 7:1       Reserved, must be kept at reset value.
                      bit 0            HeaderType: Header type definition
                                             0: explicit header for variable length payload
                                             1: implicit header for fixed length payload
byte 4           bits 7:0       PayloadLength[7:0]: Payload length in number of bytes
                                             0x00- 0xFF: 0 to 255 bytes
byte 5           bits 7:1       Reserved, must be kept at reset value.
                      bit 0            CrcType CRC enable
                                             0: CRC disabled
                                             1: CRC enabled
byte 6           bits 7:1       Reserved, must be kept at reset value.
                      bit 0            InvertIQ: IQ setup
                                             0: standard IQ setup
                                             1: inverted IQ setup

#### parameter bytes

| **byte** | hex  | bin        | note                             |
| -------- | ---- | ---------- | -------------------------------- |
| 1        | 0x00 | 0b00000000 | 12-symbol-long preamble sequence |
| 2        | 0x0C | 0b00001100 | 12-symbol-long preamble sequence |
| 3        | 0x00 | 0b00000000 | explicit header mode             |
| 4        | 0x40 | 0b01000000 | 64 bit packet length.            |
| 5        | 0x01 | 0b00000001 | CRC enabled                      |
| 6        | 0x00 | 0b00000000 | standard IQ setup                |

explanation in RM0461, chapter 4.5.2

byte 1 und byte 2: By default, the packet is configured with a 12-symbol-long preamble sequence. 

byte 3: packets with header selected - explicit header mode

byte 4: since explicit header mode was selected, this should be redundant. But 64 is selected anyway since it is half of the Tx buffer and therefore two packets can be stored.

byte 5: CRC (Cyclic redundancy check) is enabled. I'm not quite sure, what it does bur error detection shouldn't hurt.

byte 6: This is some real dark magic rf stuff, that scares me a little. It is set to the default and I hope it doesn't come back to haunt me.



### 5. Define synchronization word

#### code

RadioParam[0] = 0x14U;
RadioParam[1] = 0x24U;

if (HAL_SUBGHZ_WriteRegisters(&hsubghz, (uint16_t) 0x740, &RadioParam, 2) != HAL_OK)
{
	Error_Handler();
}



#### explanation

This defines whether a private or a public network is used. I have no idea what kind of network this describes since i'm setting up a point to point transmission. Since private is the default, we're going to use it.



**4.10.33 Sub-GHz radio LoRa synchronization word MSB register (SUBGHZ_LSYNCRH)**

Address offset: 0x740
Reset value: 0x14

Bits 7:0 	SYNCWORD[15:8]: 	LoRa synchronization word MSB bits [15:8]
									0x14: LoRa private network
									0x34: LoRa public network
									Others: reserved

**4.10.34 Sub-GHz radio LoRa synchronization word LSB register (SUBGHZ_LSYNCRL)**

Address offset: 0x741
Reset value: 0x24

Bits 7:0 	SYNCWORD[7:0]: 	 LoRa synchronization word LSB bits [7:0]
									0x24: LoRa private network
									0x44: LoRa public network
									Others: reserved



#### parameter bytes

| **byte** | hex  | bin         | note                     |
| -------- | ---- | ----------- | ------------------------ |
| 1        | 0x14 | 0b0001 0100 | LoRa private network MSB |
| 2        | 0x24 | 0b0010 0100 | LoRa private network LSB |




### 6. Define RF frequency

#### code

RadioParam[0] = 0x33U;
RadioParam[1] = 0xBCU;
RadioParam[2] = 0xA1U;
RadioParam[3] = 0x00U;

if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_RFFREQUENCY, &RadioParam, 4) != HAL_OK)
{
	Error_Handler();
}

#### explanation

**Set_RfFrequency() command**
Set_RfFrequency(RfFreq) is used to lock the RF-PLL frequency to the transmit and receive frequency.

byte 0          bits 7:0       Opcode: 0x86
bytes 4:1     bits 31:0     RfFreq[31:0]: RF frequency
                                            RF-PLL frequency = 32e^6 x RFfreq / 2^25



I'm assuming the RF frequency is the carrier frequency. But it could be that the RF-PLL frequency is the carrier frequency. I want the carrier frequency to be 868000000 Hz (868MHz), since my aplication has to be legal in Europ. In hex that would be: 0x33BCA100, which is split up into four bytes.

RF-PLL frequency = 32e^6 x RFfreq / 2^25

RF-PLL frequency = 827789307 Hz = 32e^6 x 868000000Hz / 2^25

#### parameter bytes

| **byte** | hex  | bin         | note         |
| -------- | ---- | ----------- | ------------ |
| 1        | 0x33 | 0b0011 0011 | RF frequency |
| 2        | 0xBC | 0b1011 1100 | RF frequency |
| 3        | 0xA1 | 0b1010 0001 | RF frequency |
| 4        | 0x00 | 0b0000 0000 | RF frequency |



### 7. Define the PA configuration

#### code

RadioParam[0] = 0x04U;
RadioParam[1] = 0x00U;
RadioParam[2] = 0x01U;
RadioParam[3] = 0x01U;

if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACONFIG, &RadioParam, 4) != HAL_OK)
{
	Error_Handler();
}

#### explanation

**Set_PaConfig() command**
Set_PaConfig(PaDutyCycle, HpMax, PaSel, 0x01) is used to customize the maximum output power and PA efficiency.

byte 0 	bits 7:0 	Opcode: 0x95
byte 1 	bits 7:3 	Reserved, must be kept at reset value.
		    bits 2:0         PaDutyCycle[2:0]:       PA duty cycle (conduit angle) control Duty cycle = 0.2 + 0.04 x PaDutyCycle[2:0] (see Table 27 for settings)

Caution: The following restrictions must be observed to avoid over-stress on the PA:

- LP PA mode with synthesis frequency >400 MHz, PaDutyCycle must be < 0x7.
- LP PA mode with synthesis frequency < 400 MHz, PaDutyCycle must be < 0x4.
- HP PA mode, PaDutyCycle must be < 0x4.



byte 2 	bits 2:0 	HpMax[2:0]: HP PA output power (see Table 27 for settings)
		    bits 7:3 	Reserved, must be kept at reset value.
byte 3 	bits 7:1 	Reserved, must be kept at reset value.
		    bit 0 	     PaSel: PA selection.
						0: HP PA selected
						1: LP PA selected (default)
byte 4 	bits 7:0 	0x01



Since I'm using the Wio-E5-LE, which has the impedance matching on the RF output for the low power configuration (LP PA), the second line of Table 27 in RM0461 is selected. 



#### parameter bytes

| byte | hex  | bin         | note                            |
| ---- | ---- | ----------- | ------------------------------- |
| 1    | 0x04 | 0b0000 0100 | PaDutyCycle                     |
| 2    | 0x00 | 0b0000 0000 | HpMax                           |
| 3    | 0x01 | 0b0000 0001 | LP PA selected                  |
| 4    | 0x01 | 0b0000 0001 | predefined in RM0461 and RM0453 |



### 8. Define the PA output power and ramping

#### code

RadioParam[0] = 0x0EU;
RadioParam[1] = 0x04U;

if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TXPARAMS, &RadioParam, 2) != HAL_OK)
{
	Error_Handler();
}

#### explanation

**Set_TxParams() command**
Set_TxParams(Power, RampTime) is used to set the transmit output power and the PA ramp-up time.

byte 0 	bits 7:0 	Opcode: 0x8E
byte 1 	bits 7:0 	Power[7:0]: Output power setting
						LP PA selected in Set_PaConfig()
							0x0E:+ 14 dB
							...
							0x00: 0 dB
							...
							0xEF: - 17 dB
							Others: reserved
						HP PA selected in Set_PaConfig()
							0x16:+ 22 dB
							...
							0x00: 0 dB
							...
							0xF7: - 9 dB
							Others: reserved
byte 2 	bits 7:0 	RampTime[7:0]: PA ramp time for FSK, MSK and LoRa modulation
						0x00: 10 μs
						0x01: 20 μs
						0x02; 40 μs
						0x03: 80 μs
						0x04: 200 μs
						0x05: 800 μs
						0x06: 1700 μs
						0x07: 3400 μs
						Others: reserved
Note: In BPSK mode, the ramping time is specific and RampTime[7:0] is not used.



byte 1: Defines the power. See Table 27 in RM0461 and chapter 7 in this document for more details.

byte 2: A PA ramp time of 200 μs is selected. This is probably on the slow side but for this example there is no rush and a short time probably introduces weird RF black magic problems.


#### parameter bytes

| byte | hex  | bin         | note     |
| ---- | ---- | ----------- | -------- |
| 1    | 0x0E | 0b0000 1110 | Power    |
| 2    | 0x04 | 0b0000 0100 | RampTime |



### 9. Define the modulation parameters

#### code

RadioParam[0] = 0x07U;
RadioParam[1] = 0x09U;
RadioParam[2] = 0x01U;
RadioParam[3] = 0x00U;

if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_MODULATIONPARAMS, &RadioParam, 4) != HAL_OK)
{
	Error_Handler();
}



#### explanation

**LoRa Set_ModulationParams() command**
Set_ModulationParams(Sf, Bw, Cr, Ldro) is used to configure the LoRa
modulation parameters for the sub-GHz radio. Depending on the selected packet type in
Set_PacketType() sent prior to this function, the parameters for LoRa are interpreted as
below.

byte 0          bits 7:0           Opcode: 0x8B
byte 1          bits 7:4           Reserved, must be kept at reset value.
                     bits 3:0           Sf[3:0]: Spreading factor
                                                0x5: Spreading factor 5
                                                0x6: Spreading factor 6
                                                0x7: Spreading factor 7
                                                0x8: Spreading factor 8
                                                0x9: Spreading factor 9
                                                0xA: Spreading factor 10
                                                0xB: Spreading factor 11
                                                0xC: Spreading factor 12
                                                Others: reserved
byte 2          bits 7:0           Bw[7:0]: Bandwidth
                                                0x00: bandwidth 7 (7.81 kHz)
                                                0x08: bandwidth 10 (10.42 kHz)
                                                0x01: bandwidth 15 (15.63 kHz)
                                                0x09: bandwidth 20 (20.83 kHz)
                                                0x02: bandwidth 31 (31.25 kHz)
                                                0x0A: bandwidth 41 (41.67 kHz)
                                                0x03: bandwidth 62 (62.50 kHz)
                                                0x04: bandwidth 125 (125 kHz)
                                                0x05: bandwidth 250 (250 kHz)
                                                0x06: bandwidth 500 (500 kHz)
                                                Others: reserved
byte 3          bits 7:3           Reserved, must be kept at reset value.
                     bits 2:0           Cr[2:0]: Forward error correction coding rate
                                                0x0: no forward error correction coding rate 4/4
                                                0x1: forward error correction coding rate 4/5
                                                0x2: forward error correction coding rate 4/6
                                                0x3:forward error correction coding rate 4/7
                                                0x4: forward error correction coding rate 4/8
                                                Others: reserved
byte 4          bits 7:1           Reserved, must be kept at reset value.
                     bit 0                Ldro: low data rate optimization enable
                                                0: low data rate optimization disabled
                                                1: low data rate optimization enabled

byte 1: SF (Spreading factor) Since 7 is the default, that's what we're going to use. But a higher SF would lead to a more stable transmission. (RM0461, Chapter 4.5.1)

byte 2: BW (Bandwidth) Since there is no default for the bandwidth, i picked a pretty low one without going into the extreme: 20.83kHz -> 0x09. The lower the BW, the lower the link budget but the higher the range. My project needs more range than linkbudget, thats why it was picked like that.

byte 3: CR (Forward error correction coding rate) The default setting for CR is 4/6 but RM0461, chapter 4.5.1 says that 4/5 is the best trade off between immunity to interference (lower CR) and longer
transmission time (higher CR), so 4/5 was picked.

byte 4: LDRO (Low data rate optimization) This would help at high BW and low SF. We have neither in this example, so it's turned off.




#### parameter bytes

| byte | hex  | bin         | note                                            |
| ---- | ---- | ----------- | ----------------------------------------------- |
| 1    | 0x07 | 0b0000 0111 | SF (Spreading factor) - 7 (default)             |
| 2    | 0x09 | 0b0000 1001 | BW (Bandwidth) - bandwidth 20 (20.83kHz)        |
| 3    | 0x01 | 0b0000 0001 | CR (Forward error correction coding rate) - 4/5 |
| 4    | 0x00 | 0b0000 0000 | LDRO (Low data rate optimization) - off         |



### 10. Interrupts configuration

#### code

RadioParam[0] = 0x01U;
RadioParam[1] = 0x01U;
RadioParam[2] = 0x00U;
RadioParam[3] = 0x01U;
RadioParam[4] = 0x01U;
RadioParam[5] = 0x00U;
RadioParam[6] = 0x00U;
RadioParam[7] = 0x00U;

if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_CFG_DIOIRQ, &RadioParam, 8) != HAL_OK)
{
	Error_Handler();
}

#### explanation

**Cfg_DioIrq() command**
Cfg_DioIrq(IrqMask, Irq1Mask, Irq2Mask, Irq3Mask) allows interrupts to be
masked and mapped on the IRQ lines.

byte 0 		bits 7:0 		Opcode: 0x08
bytes 2:1 	   bits 15:0 	      IrqMask[15:0]: Global interrupt enable
							  See Table 29 for interrupt bit map definition. For each bit:
								0: IRQ disabled
								1: IRQ enabled
bytes 4:3 	   bits 15:0 	      Irq1Mask[15:0]: IRQ1 line Interrupt enable
								0: interrupt on IRQ1 line disable
								1: interrupt on IRQ1 line enabled
bytes 6:5 	   bits 15:0 	      Irq2Mask[15:0]: IRQ2 line Interrupt enable
								0: interrupt on IRQ2 line disable
								1: interrupt on IRQ2 line enabled
bytes 8:7 	   bits 15:0 	      Irq3Mask[15:0]: IRQ3 line Interrupt enable
								0: interrupt on IRQ3 line disable
								1: interrupt on IRQ3 line enabled

I am very unsure about the way this works. The way I understood this is: bytes 1 and 2 activate the interrupts in general and bytes 3 to 8 map them onto an interrupt line. There are three lines. If four or more interrupts are needed, some interrupts have to share a line. 

I imagine, there are hardware limits on how many interrupt lines there are. I don't quite understand, why this is important to me since the status of the interrupts is read back in one message. Mabey one interrupt activates the whole line and if there are multiple interrupts mapped to one line you can't tell which one fired, but this is just a guess.

The only two things I want to know in this example is whether the transmission was successful or if it timed out. So using Table 29 in RM0461 Bit 0 (TxDone) and Bit 9 (Timeout) are activated. After that the TxDone interrupt is mapped to line 1 and the Timeout interrupt is mapped to line 2. Line 3 remains deactivated.


#### parameter bytes


| byte | hex  | bin        | notes              |
| ---- | ---- | ---------- | ------------------ |
| 1    | 0x01 | 0b00000001 | IRQ Mask MSB       |
| 2    | 0x01 | 0b00000001 | IRQ Mask LSB       |
| 3    | 0x00 | 0b00000000 | IRQ1 Line Mask MSB |
| 4    | 0x01 | 0b00000001 | IRQ1 Line Mask LSB |
| 5    | 0x01 | 0b00000001 | IRQ2 Line Mask MSB |
| 6    | 0x00 | 0b00000000 | IRQ2 Line Mask LSB |
| 7    | 0x00 | 0b00000000 | IRQ3 Line Mask MSB |
| 8    | 0x00 | 0b00000000 | IRQ3 Line Mask LSB |







### 11. Start the transmission

#### code

RadioParam[0] = 0x09U;
RadioParam[1] = 0xC4U;
RadioParam[2] = 0x00U;

if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TX, &RadioParam, 3) != HAL_OK)
{
	Error_Handler();
}

#### explanation

Set_Tx() command
Set_Tx(Timeout) is used to set the sub-GHz radio in TX mode.

byte 0         bits 7:0       Opcode: 0x83
bytes 3:1    bits 23:0     Timeout[23:0]: Transmit packet timeout
                                            0x000000: Timeout disabled
                                            0x000001 - 0xFFFFFF: Timeout enabled, resolution 15.625 μs

Time-out duration is computed as follows:
Time-out duration = Timeout x 15.625 μs (maximum time-out duration = 262.14 s)
When Set_Tx(Timeout) is sent in Standby or Receive mode, the sub-GHz radio passes
through the FS mode (no need to send Set_Fs()). In this case, the RF-PLL frequency
must be set by Set_RfFrequency() prior sending Set_Tx(Timeout).

A timeout of 10 seconds is set to prevent the chip from overheating.

Timeout = 10s/15.625us = 640000 = 0x9C400



#### parameter bytes

| byte | hex  | bin         | notes   |
| ---- | ---- | ----------- | ------- |
| 1    | 0x09 | 0b0000 1001 | Timeout |
| 2    | 0xC4 | 0b1100 0100 | Timeout |
| 3    | 0x00 | 0b0000 0000 | Timeout |



### Rest of the code

The rest of the code is here for debugging purposes. Pin PB5 is wired to a LED via open drain. UART1 is connected to a serial port, which I'm monitoring with Putty.