//check effect
#include <sunhu_cc1101.h>
#include <Arduino.h>

// ******************************************************************************
#define     WRITE_BURST     0x40
#define     READ_SINGLE    0x80
#define     READ_BURST      0xC0
#define     BYTES_IN_FIFO   0x7F    //used to detect FIFO underflow or overflow

/*********************SS_PIN as global variable****************************** */
/*                         sunhu_cc1101                                       */
/******************************************************************************/
int SS_PIN;

sunhu_cc1101::sunhu_cc1101(int ss_pin, int gdo0_pin, int gdo2_pin){
    pinMode(gdo0_pin,OUTPUT);//GDO0 as asynchronous serial mode input
    pinMode(gdo2_pin,INPUT);//GDO2 as asynchronous serial mode output
    pinMode(ss_pin,OUTPUT);
    SS_PIN=ss_pin;
}
//******************************************************************************
//SpiInit
/******************************************************************************/
void sunhu_cc1101::SpiInit(void){
    //initialize spi pins
    pinMode(SCK_PIN,OUTPUT);
    pinMode(MOSI_PIN,OUTPUT);
    pinMode(MISO_PIN,OUTPUT);
    // pinMode(SS_PIN,OUTPUT);

    //Enable spi master, MSB, SPI mode 0, FOSC/4
    SpiMode(0);
}

void sunhu_cc1101::SpiEnd(void){
	SPCR = ((0<<SPE) |               	// SPI Enable
        (0<<SPIE)|              		// SPI Interupt Enable
        (0<<DORD)|              		// Data Order (0:MSB first / 1:LSB first)
        (1<<MSTR)|              		// Master/Slave select
        (0<<SPR1)|(0<<SPR0)|   		// SPI Clock Rate ( 0 0 = osc/4; 0 1 = osc/16; 1 0 = osc/64; 1 1= 0sc/128)
        (0<<CPOL)|             		// Clock Polarity (0:SCK low / 1:SCK hi when idle)
        (0<<CPHA));             		// Clock Phase (0:leading / 1:trailing edge sampling)

	//SPSR =  (0<<SPI2X);             		// Double Clock Rate
}
/******************************************************************************
Function: SpiMode
 *INPUT        :        config               mode
			   (0<<CPOL) | (0 << CPHA)		 0
			   (0<<CPOL) | (1 << CPHA)		 1
			   (1<<CPOL) | (0 << CPHA)		 2
			   (1<<CPOL) | (1 << CPHA)		 3
*OUTPUT       :none
******************************************************************************/
  void sunhu_cc1101::SpiMode(byte config)
{
  byte tmp;
  // enable SPI master with configuration byte specified
  SPCR = 0;
  SPCR = (config & 0x7F) | (1<<SPE) | (1<<MSTR);
  tmp = SPSR;
  tmp = SPDR;
}
 /****************************************************************
*FUNCTION NAME:SpiTransfer
*FUNCTION     :spi transfer
*INPUT        :value: data to send
*OUTPUT       :data to receive
****************************************************************/
byte sunhu_cc1101::SpiTransfer(byte value)
{
  SPDR = value;
  while (!(SPSR & (1<<SPIF))) ;
  return SPDR;
}

 /****************************************************************
*FUNCTION NAME:SpiWriteReg
*FUNCTION     :CC1101 write data to register
*INPUT        :addr: register address; value: register value
*OUTPUT       :none
****************************************************************/
void sunhu_cc1101::SpiWriteReg(byte addr, byte value)
{
	digitalWrite(SS_PIN, LOW);
	while(digitalRead(MISO_PIN));
	SpiTransfer(addr);
	SpiTransfer(value);
	digitalWrite(SS_PIN, HIGH);
}

/****************************************************************
*FUNCTION NAME:SpiWriteBurstReg
*FUNCTION     :CC1101 write burst data to register
*INPUT        :addr: register address; buffer:register value array; num:number to write
*OUTPUT       :none
****************************************************************/
void sunhu_cc1101::SpiWriteBurstReg(byte addr, byte *buffer, byte num)
{
	byte i, temp;

	temp = addr | WRITE_BURST;
    digitalWrite(SS_PIN, LOW);
    while(digitalRead(MISO_PIN));
    SpiTransfer(temp);
    for (i = 0; i < num; i++)
 	{
        SpiTransfer(buffer[i]);
    }
    digitalWrite(SS_PIN, HIGH);
}

/****************************************************************
*FUNCTION NAME:SpiStrobe
*FUNCTION     :CC1101 Strobe
*INPUT        :strobe: command; //refer define in CC1101.h//
*OUTPUT       :none
****************************************************************/
void sunhu_cc1101::SpiStrobe(byte strobe)
{
	digitalWrite(SS_PIN, LOW);
	while(digitalRead(MISO_PIN));
	SpiTransfer(strobe);
	digitalWrite(SS_PIN, HIGH);
}

/****************************************************************
*FUNCTION NAME:SpiReadReg
*FUNCTION     :CC1101 read data from register
*INPUT        :addr: register address
*OUTPUT       :register value
****************************************************************/
byte sunhu_cc1101::SpiReadReg(byte addr)
{
	byte temp, value;

    temp = addr|READ_SINGLE;
	digitalWrite(SS_PIN, LOW);
	while(digitalRead(MISO_PIN));
	SpiTransfer(temp);
	value=SpiTransfer(0);
	digitalWrite(SS_PIN, HIGH);

	return value;
}

/****************************************************************
*FUNCTION NAME:SpiReadBurstReg
*FUNCTION     :CC1101 read burst data from register
*INPUT        :addr: register address; buffer:array to store register value; num: number to read
*OUTPUT       :none
****************************************************************/
void sunhu_cc1101::SpiReadBurstReg(byte addr, byte *buffer, byte num)
{
	byte i,temp;

	temp = addr | READ_BURST;
	digitalWrite(SS_PIN, LOW);
	while(digitalRead(MISO_PIN));
	SpiTransfer(temp);
	for(i=0;i<num;i++)
	{
		buffer[i]=SpiTransfer(0);
	}
	digitalWrite(SS_PIN, HIGH);
}

/****************************************************************
*FUNCTION NAME:SpiReadStatus
*FUNCTION     :CC1101 read status register
*INPUT        :addr: register address
*OUTPUT       :status value
****************************************************************/
byte sunhu_cc1101::SpiReadStatus(byte addr)
{
	byte value,temp;

	temp = addr | READ_BURST;
	digitalWrite(SS_PIN, LOW);
	while(digitalRead(MISO_PIN));
	SpiTransfer(temp);
	value=SpiTransfer(0);
	digitalWrite(SS_PIN, HIGH);

	return value;
}

/****************************************************************
*FUNCTION NAME:Reset
*FUNCTION     :CC1101 reset //details refer datasheet of CC1101/CC1100//
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void sunhu_cc1101::Reset (void)
{
	digitalWrite(SS_PIN, LOW);
	delay(1);
	digitalWrite(SS_PIN, HIGH);
	delay(1);
	digitalWrite(SS_PIN, LOW);
	while(digitalRead(MISO_PIN));
	SpiTransfer(CC1101_SRES);
	while(digitalRead(MISO_PIN));
	digitalWrite(SS_PIN, HIGH);
}
 /****************************************************************
*FUNCTION NAME:Init
*FUNCTION     :CC1101 initialization
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void sunhu_cc1101::Init(void)
{
    #ifdef CC1101_DEBUG
        Serial.println(F("Init SPI..."));
    #endif
	SpiInit();										//spi initialization
	digitalWrite(SS_PIN, HIGH);
	digitalWrite(SCK_PIN, HIGH);
	digitalWrite(MOSI_PIN, LOW);
    #ifdef CC1101_DEBUG
       Serial.println(F("Reset CC1101..."));
    #endif
	Reset();										//CC1101 reset

    #ifdef CC1101_DEBUG
        byte partnum, version;
        partnum=SpiReadStatus(CC1101_PARTNUM);
        version=SpiReadStatus(CC1101_VERSION);
        Serial.print(F("Partnum:0x"));
        Serial.print(partnum,HEX);
        Serial.println();
        Serial.print(F("Version:0x"));
        Serial.print(version,HEX);
        Serial.println();
    #endif

    #ifdef CC1101_DEBUG
       Serial.println(F("Init CC1101..."));
    #endif
	RegConfigSettings();							//CC1101 register config

    #ifdef CC1101_DEBUG
        Serial.println(F("Done!"));
    #endif


}
 /****************************************************************
*FUNCTION NAME:SetMod
*FUNCTION     :CC1101 modulation type
*INPUT        :byte mode
*OUTPUT       :none
****************************************************************/
void sunhu_cc1101::SetMod(byte mode){
    SpiWriteReg(CC1101_MDMCFG2,  mode);	//no sync/preamble; ASK/OOK only support up to -1dbm
    if (mode|0x30==ASK) {
        SpiWriteReg(CC1101_FREND0,   0x11);	//use first up to PATABLE(0)
        byte PaTabel[8] = {0x00 ,POWER,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
	    SpiWriteBurstReg(CC1101_PATABLE,PaTabel,8);//CC1101 PATABLE config
    }
    else {
        SpiWriteReg(CC1101_FREND0,   0x10);	//use first up to PATABLE(0)
        byte PaTabel[8] = {POWER,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
	    SpiWriteBurstReg(CC1101_PATABLE,PaTabel,8);//CC1101 PATABLE config
    }

    #ifdef CC1101_DEBUG
    switch(mode|0x30){
        case GFSK: {
            Serial.println(F("CC1101 Modulation: GFSK"));
            break;
            }
        case MSK:
            {
            Serial.println(F("CC1101 Modulation: MSK"));
            break;
            }
        case ASK:
            {
            Serial.println(F("CC1101 Modulation: ASK/OOK"));
            break;
            }
        case FSK2:
            {
            Serial.println(F("CC1101 Modulation: 2-FSK"));
            break;
            }
        case FSK4:
            {
            Serial.println(F("CC1101 Modulation: 4-FSK"));
            break;
            }
        default:                                //default to GFSK
            {
            Serial.println(F("Modulation mode not supported"));
            break;
            }
    }
    #endif

}
 /****************************************************************
*FUNCTION NAME:RegConfigSettings
*FUNCTION     :CC1101 register config //details refer datasheet of CC1101/CC1100//
*INPUT        :none
*OUTPUT       :none
****************************************************************/
void sunhu_cc1101::RegConfigSettings(void)
{
	SpiWriteReg(CC1101_FSCTRL1,  0x06);		//IF frequency
    SpiWriteReg(CC1101_FSCTRL0,  0x00);		//frequency offset before synthesizer

    SpiWriteReg(CC1101_MDMCFG4,  0xCC);		// RX filter bandwidth 100k(0xcc)
    SpiWriteReg(CC1101_MDMCFG3,  0x43);	//datarate config 512kBaud  for the purpose of fast rssi measurement
    SpiWriteReg(CC1101_MDMCFG1,  0x21);	//FEC preamble etc. last 2 bits for channel spacing
    SpiWriteReg(CC1101_MDMCFG0,  0xF8);		//100khz channel spacing
    //CC1101_CHANNR moved to SetChannel func

    //SpiWriteReg(CC1101_DEVIATN,  0x47);
    SpiWriteReg(CC1101_MCSM0 ,   0x18);	// calibrate when going from IDLE to RX or TX ; 149 - 155 μs timeout
    SpiWriteReg(CC1101_FOCCFG,   0x16);	//frequency compensation
    //SpiWriteReg(CC1101_BSCFG,    0x1C);	//bit synchronization config
    SpiWriteReg(CC1101_AGCCTRL2, 0x43);
	SpiWriteReg(CC1101_AGCCTRL1, 0x49);
    SpiWriteReg(CC1101_AGCCTRL0, 0x91);
	//freq synthesizer calibration
    SpiWriteReg(CC1101_FSCAL3,   0xEA);
	SpiWriteReg(CC1101_FSCAL2,   0x2A);
	SpiWriteReg(CC1101_FSCAL1,   0x00);
    SpiWriteReg(CC1101_FSCAL0,   0x1F);
    SpiWriteReg(CC1101_TEST2,    0x81);
    SpiWriteReg(CC1101_TEST1,    0x35);
    SpiWriteReg(CC1101_TEST0,    0x0B); //should be 0x0B for lower than 430.6MHz and 0x09 for higher

    //SpiWriteReg(CC1101_FREND1,   0x56);

    //SpiWriteReg(CC1101_IOCFG2,   0x0B); 	//serial clock.synchronous to the data in synchronous serial mode
    //SpiWriteReg(CC1101_IOCFG0,   0x06);  	//asserts when sync word has been sent/received, and de-asserts at the end of the packet
    SpiWriteReg(CC1101_IOCFG2,   0x0D);	//data output pin for asynchronous mode
	SpiWriteReg(CC1101_IOCFG0,   0x2E);		//High impedance (3-state), GDO0 configed as data input for asynchronous mode
    //SpiWriteReg(CC1101_PKTCTRL0, 0x05);		//whitening off;CRC Enable；variable length packets, packet length configured by the first byte after sync word
	SpiWriteReg(CC1101_PKTCTRL0, 0x33);		//whitening off; asynchronous serial mode; CRC diable；reserved
    //SpiWriteReg(CC1101_PKTLEN,   0x3D); 	//61 bytes max length
	SpiWriteReg(CC1101_FIFOTHR, 0x47);	//Adc_retention enabled for RX filter bandwidth less than 325KHz; defalut fifo threthold.

}
/****************************************************************
 *FUNCTION NAME:SetFreq
 *FUNCTION     :SetFreq
 *INPUT        :Freq2, Freq1, Freq0
 *OUTPUT       :none
 ****************************************************************/
void sunhu_cc1101::SetFreq(byte freq2, byte freq1, byte freq0) {
    SpiWriteReg(CC1101_FREQ2, freq2);
    SpiWriteReg(CC1101_FREQ1, freq1);
    SpiWriteReg(CC1101_FREQ0, freq0);
}
/****************************************************************
 *FUNCTION NAME:SetChannel
 *FUNCTION     :SetChannel
 *INPUT        :int channel
 *OUTPUT       :none
 ****************************************************************/
 void sunhu_cc1101::SetChannel(int channel){
	#ifdef CC1101_DEBUG
        Serial.print(F("Set CC1101 channel to:"));
		Serial.println(channel,DEC);
    #endif
	SpiWriteReg(CC1101_CHANNR,   (byte)channel);	//related to channel numbers
 }
/****************************************************************
 *FUNCTION NAME:SetReceive
 *FUNCTION     :SetReceive
 *INPUT        :none
 *OUTPUT       :none
 ****************************************************************/
void sunhu_cc1101::SetReceive(void)
{
    SpiStrobe(CC1101_SRX);
    while (SpiReadStatus(CC1101_MARCSTATE)^CC1101_STATUS_RX);
}
/****************************************************************
 *FUNCTION NAME:SetTransmit
 *FUNCTION     :
 *INPUT        :none
 *OUTPUT       :none
 ****************************************************************/
void sunhu_cc1101::SetTransmit(void)
{
    SpiStrobe(CC1101_STX);
    while (SpiReadStatus(CC1101_MARCSTATE)^CC1101_STATUS_TX);
}
//sunhu_cc1101 sunhu_cc1101;
