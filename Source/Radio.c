
#include <stdint.h>
#include <string.h>

#include "cc430x513x.h"

#include "Utils\Utils.h"
#include "RFCounter433.h"
#include "Board\Board.h"

#include "Radio\hal_pmm.h"

#include "Radio.h"

uint8_t tx_data_len, tx_data_index;;
extern char urx_buf[];
uint8_t rf_packet_len;

void Set_RF_freq_Flash(void)
{
TSmodem_settings *settings_ee;
settings_ee=(TSmodem_settings *)&ee_modem_settings;
if ((settings_ee->rf_freq[0]!=0xFF)||		//change
    (settings_ee->rf_freq[1]!=0xFF)||
    (settings_ee->rf_freq[2]!=0xFF))
	{
	halRfWriteReg(FREQ0, settings_ee->rf_freq[0]);   // Carrier frequency = 433.82
	halRfWriteReg(FREQ1, settings_ee->rf_freq[1]);   //Frequency Control Word, Middle Byte
	halRfWriteReg(FREQ2, settings_ee->rf_freq[2]);   //Frequency Control Word, Low Byte
	}
}


void RadioInit( void )
{
// Stop watchdog timer to prevent time out reset
//WDTCTL = WDTPW + WDTHOLD;

// Increase PMMCOREV level to 2 for proper radio operation
SetVCore(3);

ResetRadioCore();

PMMCTL0_H = 0xA5;
PMMCTL0_L |= PMMHPMRE_L; 
PMMCTL0_H = 0x00; 

WriteRfSettings();
Set_RF_freq_Flash();

WriteSinglePATable(rfPOWER10);

RF1AIE = 0;	//new
rf_packet_len = ee_modem_settings.rf_length;
RF_transmitting = 0;
ReceiveOn();
}



// *****************************************************************************
// @fn          ReadSingleReg
// @brief       Read a single byte from the radio register
// @param       uint8_t addr      Target radio register address
// @return      uint8_t data_out  Value of byte that was read
// *****************************************************************************
uint8_t ReadSingleReg(uint8_t addr)
{
uint8_t data_out;
// Check for valid configuration register address, 0x3E refers to PATABLE
if ((addr <= 0x2E) || (addr == 0x3E))
	// Send address + Instruction + 1 dummy byte (auto-read)
	RF1AINSTR1B = (addr | RF_SNGLREGRD);
else
	// Send address + Instruction + 1 dummy byte (auto-read)
	RF1AINSTR1B = (addr | RF_STATREGRD);
uint16_t timeout=0xffff;
while (!(RF1AIFCTL1 & RFDOUTIFG) ) if (timeout--==0) return 0;
data_out = RF1ADOUTB;                    // Read data and clears the RFDOUTIFG
return data_out;
}



// *****************************************************************************
// @fn          WriteSingleReg
// @brief       Write a single byte to a radio register
// @param       uint8_t addr      Target radio register address
// @param       uint8_t value     Value to be written
// @return      none
// *****************************************************************************
void WriteSingleReg(uint8_t addr, uint8_t value)
{
timeout = 100;
while (!(RF1AIFCTL1 & RFINSTRIFG)) if (timeout==0) return;       // Wait for the Radio to be ready for next instruction
RF1AINSTRB = (addr | RF_SNGLREGWR);	    // Send address + Instruction
RF1ADINB = value; 			    // Write data in
}


// *****************************************************************************
// @fn          ReadBurstReg
// @brief       Read multiple bytes to the radio registers
// @param       uint8_t addr      Beginning address of burst read
// @param       uint8_t *buffer   Pointer to data table
// @param       uint8_t count     Number of bytes to be read
// @return      none
// *****************************************************************************
void ReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t cnt)
{
uint8_t i;
if(cnt > 0)
	{
	timeout = 100;
	while (!(RF1AIFCTL1 & RFINSTRIFG)) if (timeout==0) return;       // Wait for INSTRIFG
	RF1AINSTR1B = (addr | RF_REGRD);          // Send addr of first conf. reg. to be read
	// ... and the burst-register read instruction
	for (i = 0; i < (cnt-1); i++)
		{
		timeout = 100;
		while (!(RFDOUTIFG&RF1AIFCTL1)) if (timeout==0) return;        // Wait for the Radio Core to update the RF1ADOUTB reg
		buffer[i] = RF1ADOUT1B;                 // Read DOUT from Radio Core + clears RFDOUTIFG
		// Also initiates auo-read for next DOUT byte
		}
	buffer[cnt-1] = RF1ADOUT0B;             // Store the last DOUT from Radio Core
	}
}



// *****************************************************************************
// @fn          WriteBurstReg
// @brief       Write multiple bytes to the radio registers
// @param       uint8_t addr      Beginning address of burst write
// @param       uint8_t *buffer   Pointer to data table
// @param       uint8_t count     Number of bytes to be written
// @return      none
// *****************************************************************************
void WriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t cnt)
{
uint8_t i;

if(cnt > 0)
	{
	timeout = 100;
	while (!(RF1AIFCTL1 & RFINSTRIFG)) if (timeout==0) return;       // Wait for the Radio to be ready for next instruction
	RF1AINSTRW = ((addr | RF_REGWR)<<8 ) + buffer[0]; // Send address + Instruction
	for (i = 1; i < cnt; i++)
		{
		RF1ADINB = buffer[i];                   // Send data
		timeout = 100;
		while (!(RFDINIFG & RF1AIFCTL1)) if (timeout==0) return;       // Wait for TX to finish
		};
	i = RF1ADOUTB;                            // Reset RFDOUTIFG flag which contains status byte
	};
}



// *****************************************************************************
// @fn          Strobe
// @brief       Send a command strobe to the radio. Includes workaround for RF1A7
// @param       uint8_t strobe        The strobe command to be sent
// @return      uint8_t statusByte    The status byte that follows the strobe
// *****************************************************************************
uint8_t Strobe(uint8_t strobe)
{
__disable_interrupt();
    uint8_t statusByte = 0;
    uint16_t   gdo_state;
// Check for valid strobe command
    if((strobe == 0xBD) || ((strobe >= RF_SRES) && (strobe <= RF_SNOP)))
    {
// Clear the Status read flag
        RF1AIFCTL1 &= ~(RFSTATIFG);
// Wait for radio to be ready for next instruction
		timeout = -1;
        while( !(RF1AIFCTL1 & RFINSTRIFG)) if (--timeout==0) return 0;
// Write the strobe instruction
        if ((strobe > RF_SRES) && (strobe < RF_SNOP))
        {
            gdo_state = ReadSingleReg(IOCFG2);    // buffer IOCFG2 state
            WriteSingleReg(IOCFG2, 0x29);         // chip-ready to GDO2
            RF1AINSTRB = strobe;
            if ( (RF1AIN&0x04)== 0x04 )           // chip at sleep mode
            {
                if ( (strobe == RF_SXOFF) || (strobe == RF_SPWD) || (strobe == RF_SWOR) ) { }
                else
                {
					timeout = -1;
                    while ((RF1AIN&0x04)== 0x04) if (--timeout==0) return 0;     // chip-ready ?
// Delay for ~810usec at 1.05MHz CPU clock, see erratum RF1A7
                    __delay_cycles(850);
                }
            }
            WriteSingleReg(IOCFG2, gdo_state);    // restore IOCFG2 setting
			timeout = -1;
            while( !(RF1AIFCTL1 & RFSTATIFG) ) if (--timeout==0) return 0;
        }
        else		                    // chip active mode (SRES)
        {
            RF1AINSTRB = strobe;
        }
        statusByte = RF1ASTATB;
    }
    __enable_interrupt();
    return statusByte;
}



// *****************************************************************************
// @fn          ResetRadioCore
// @brief       Reset the radio core using RF_SRES command
// @param       none
// @return      none
// *****************************************************************************
void ResetRadioCore (void)
{
Strobe(RF_SRES);                          // Reset the Radio Core
Strobe(RF_SNOP);                          // Reset Radio Pointer
}



// *************************************************************************************************
// @fn          radio_powerdown
// @brief       Put radio to SLEEP mode. 
// @param       none
// @return      none
// *************************************************************************************************
void Radio_PowerDown(void)
{
  /* Chip bug: Radio does not come out of this SLEEP when put to sleep
   * using the SPWD cmd. However, it does wakes up if SXOFF was used to
   * put it to sleep.
   */
	// Powerdown radio
	Strobe(RF_SIDLE);
	Strobe(RF_SPWD);	
}



// *************************************************************************************************
// @fn          radio_sxoff
// @brief       Put radio to SLEEP mode (XTAL off only). 
// @param       none
// @return      none
// *************************************************************************************************
volatile char cdata;
void Radio_SxOff(void)
{
  /* Chip bug: Radio does not come out of this SLEEP when put to sleep
   * using the SPWD cmd. However, it does wakes up if SXOFF was used to
   * put it to sleep.
   */
	// Powerdown radio
	Strobe(RF_SIDLE);
	Strobe(RF_SXOFF);
	
	// Очистка буфера
	RFRxBufferLength = ReadSingleReg( RXBYTES ) & 0x7F;	// Без учёта флага переполнения
	timeout = 100;
	while (!(RF1AIFCTL1 & RFINSTRIFG))if (timeout==0) return;       // Wait for INSTRIFG
	RF1AINSTR1B = (RF_RXFIFORD | RF_REGRD);          // Send addr of first conf. reg. to be read
	// ... and the burst-register read instruction
	while(--RFRxBufferLength)
	{
		timeout = 100;
		while (!(RFDOUTIFG&RF1AIFCTL1))if (timeout==0) return;        // Wait for the Radio Core to update the RF1ADOUTB reg
		cdata = RF1ADOUT1B;                 // Read DOUT from Radio Core + clears RFDOUTIFG
		// Also initiates auo-read for next DOUT byte
	}
	cdata = RF1ADOUT0B;
}



void ReceiveOn(void)
{
	ReceiveOff();	//new
//	if (ee_modem_settings.rf_length)
	if (rf_packet_len)
	{
		WriteSingleReg(PKTCTRL0, 0); //  Fix packet length mode No CRC white off
//		WriteSingleReg(PKTLEN,  ee_modem_settings.rf_length);//new
		WriteSingleReg(PKTLEN,  rf_packet_len);//new
	}
	else
	{
		WriteSingleReg(PKTCTRL0, 1); //  Variable packet length mode No CRC white off
		WriteSingleReg(PKTLEN, 0xFF); //  Fix packet length mode No CRC white off
	}
	// Radio is in IDLE following a TX, so strobe SRX to enter Receive Mode
	Strobe( RF_SRX );
}



void ReceiveOff(void)
{
// It is possible that ReceiveOff is called while radio is receiving a packet.
// Therefore, it is necessary to flush the RX FIFO after issuing IDLE strobe 
// such that the RXFIFO is empty prior to receiving a packet.
Strobe( RF_SIDLE );
Strobe( RF_SFRX  );                       
}



// *****************************************************************************
// @fn          WritePATable
// @brief       Write data to power table
// @param       uint8_t value		Value to write
// @return      none
// *****************************************************************************
void WriteSinglePATable(uint8_t Power)
{
timeout = 100;
while( !(RF1AIFCTL1 & RFINSTRIFG))if (timeout==0) return;
RF1AINSTRW = 0x3E00 + Power;              // PA Table single write
timeout = 100;
while( !(RF1AIFCTL1 & RFINSTRIFG))if (timeout==0) return;
RF1AINSTRB = RF_SNOP;                     // reset PA_Table pointer
}



// *****************************************************************************
// @fn          WritePATable
// @brief       Write to multiple locations in power table
// @param       uint8_t *buffer	Pointer to the table of values to be written
// @param       uint8_t count	Number of values to be written
// @return      none
// *****************************************************************************
void WriteBurstPATable(uint8_t *buffer, uint8_t cnt)
{
uint8_t i = 0;
while( !(RF1AIFCTL1 & RFINSTRIFG));
RF1AINSTRW = 0x7E00 + buffer[i];          // PA Table burst write
for (i = 1; i < cnt; i++)
	{
	RF1ADINB = buffer[i];                   // Send data
	while (!(RFDINIFG & RF1AIFCTL1));       // Wait for TX to finish
	}
i = RF1ADOUTB;                            // Reset RFDOUTIFG flag which contains status byte
while( !(RF1AIFCTL1 & RFINSTRIFG));
RF1AINSTRB = RF_SNOP;                     // reset PA Table pointer
}



// *****************************************************************************
// @fn          WriteRfSettings
// @brief       Write the minimum set of RF configuration register settings
// @param       RF_SETTINGS *pRfSettings  Pointer to the structure that holds the rf settings
// @return      none
// *****************************************************************************
void WriteRfSettings(void) {
halRfWriteReg(IOCFG0,0x0E); // SYNC word
WriteSingleReg(IOCFG1, 0x29);	//RF_RDYn for WOR
//halRfWriteReg(IOCFG1,0x2E); // Tri-State
halRfWriteReg(IOCFG2,0x06); // Carrier Sence
halRfWriteReg(SYNC1,0xF6); // sync word, high byte
halRfWriteReg(SYNC0,0x8D); // sync word, low byte
//  RX Attenuation 0 dB
//  bytes in  FIFO 33-TX 32-RX
halRfWriteReg(FIFOTHR,0x47);
halRfWriteReg(PKTCTRL1,0x40); //  No Status
halRfWriteReg(PKTCTRL0,0); //  Fix packet length mode No CRC white off
halRfWriteReg(PKTLEN, ee_modem_settings.rf_length); //new //  Fixed Packet Length

halRfWriteReg(ADDR,   0x00);
halRfWriteReg(CHANNR, 0x00);
halRfWriteReg(FSCTRL1,0x06); //
halRfWriteReg(FSCTRL0,0x00);    // 0x17

halRfWriteReg(FREQ2,0x10);   // Carrier frequency = 433.82
halRfWriteReg(FREQ1,0xAF);   //Frequency Control Word, Middle Byte
halRfWriteReg(FREQ0,0x75);   //Frequency Control Word, Low Byte

//
halRfWriteReg(MDMCFG4,0xF6); // RX filter BW = 58.035714
halRfWriteReg(MDMCFG3,0x83); // Data rate = 2.39897
halRfWriteReg(MDMCFG2,0x02); // 16/16 + carrier-sense above threshold
halRfWriteReg(MDMCFG1,0x51); //  12 byte preamble
halRfWriteReg(MDMCFG0,0x2E); // Channel spacing = 59.906006
halRfWriteReg(DEVIATN,0x16); // Deviation = 5.554199
halRfWriteReg(MCSM2,0x07); // Until end of packet
halRfWriteReg(MCSM1,0x2C); // CCA always (Unless currently receiving a packet)+set RX mode after Reciev and Idle after Transmit data
//halRfWriteReg(MCSM1,0x20); // CCA always (Unless currently receiving a packet)+set Idle after Reciev and Idle after Transmit data
//  The RF XT2 crystal oscillator is automatically controlled
//halRfWriteReg(MCSM0,0x18); // When going from IDLE to RX or TX
halRfWriteReg(MCSM0,0x10); // When going from IDLE to RX or TX
//halRfWriteReg(MCSM0,0x00); // manually calibrate using SCAL strobe
halRfWriteReg(FOCCFG,0x17); //Frequency Offset Compensation Configuration
halRfWriteReg(BSCFG,0x6C);	//Bit Synchronization configuration
//  The two highest gain settings cannot be used
//    halRfWriteReg(AGCCTRL2,0x02);//AGC Control2 max gain //0x43
halRfWriteReg(AGCCTRL2,0x43);//AGC Control2 max gain //
//  The two highest gain settings cannot be used
//    halRfWriteReg(AGCCTRL1,0x49);//AGC Control 1 0x40
halRfWriteReg(AGCCTRL1,0x40);//AGC Control 1 0x40
halRfWriteReg(AGCCTRL0,0x91);//AGC Control 1 0x40
//  Wake On Radio Control
halRfWriteReg(WORCTRL,0xFB);
//halRfWriteReg(WORCTRL,0x20);
//halRfWriteReg(WOREVT0,0xA0);//300ms
//halRfWriteReg(WOREVT1,0x80);

halRfWriteReg(FREND1,0x56);	//Front end RX configuration
halRfWriteReg(FREND0,0x10);	//Front end TX configuration

//Frequency Synthesizer Calibration
halRfWriteReg(FSCAL3,0xE9);
halRfWriteReg(FSCAL2,0x2A);  //Frequency Synthesizer Calibration
halRfWriteReg(FSCAL1,0x00);  //Frequency Synthesizer Calibration
halRfWriteReg(FSCAL0,0x1F);  //Frequency Synthesizer Calibration

halRfWriteReg(FSTEST,0x59);	//Frequency synthesizer calibration control
halRfWriteReg(PTEST,0x7F);	//Production test
halRfWriteReg(AGCTEST,0x3F);	//AGC test

halRfWriteReg(TEST2,0x81);   //Various Test Settings
halRfWriteReg(TEST1,0x35);   //Various Test Settings
halRfWriteReg(TEST0,0x09);   //Various Test Settings//
}


// Значения регистров MDMCFG4,MDMCFG3,DEVIATN для скоростей обмена
const char bdr_reg_val[9][3] = {
	0xF4, 0x83, 0x15, // RF_BDR_600
	0xF5, 0x83, 0x15, // RF_BDR_1200
	0xF6, 0x83, 0x15, // RF_BDR_2400
	0xC7, 0x83, 0x40, // RF_BDR_4800
	0xC8, 0x93, 0x34, // RF_BDR_9600
	0xC9, 0x83, 0x35, // RF_BDR_19200
	0xCA, 0x83, 0x35, // RF_BDR_38400
	0x7B, 0x22, 0x42, // RF_BDR_57600
	0x5C, 0x22, 0x47}; // RF_BDR_115200

void WriteRfbdr(short bdr)
{
if (bdr <= (sizeof(bdr_reg_val) / sizeof(bdr_reg_val[0])))
	{
	Strobe(RF_SIDLE);

	halRfWriteReg(MDMCFG4, bdr_reg_val[bdr][0]);
	halRfWriteReg(MDMCFG3, bdr_reg_val[bdr][1]);

	halRfWriteReg(DEVIATN, bdr_reg_val[bdr][2]);
	ReceiveOn();
	}
}



// *****************************************************************************
//Передача содержимого буфера
// *****************************************************************************
#define maxTx 64
uint8_t  rfSend(uint8_t *buf,uint8_t cnt)
{
if(cnt==0)       return 0;	//Если размр данных для передачи =0
if(cnt > maxTx)  return 0;	//или больше мах, вернем ошибку
halRfWriteReg(PKTLEN,cnt);	//Fixed Packet Length
WriteBurstReg(RF_TXFIFOWR, buf, cnt);	//Передаем данные в трансивер
//RF1AIES |= BIT9;			// 
//RF1AIFG &= ~BIT9;			// Clear pending interrupts
Strobe( RF_STX);			//Запускаем передачу в эфир
//    RF1AIE |= BIT9;  // Enable TX end-of-packet interrupt
//while(!tbit(RF1AIFG,9));		//Ждем окончания передачи
RF_transmitting = 1;
timeout=200;
while (RF_transmitting) if (timeout==0) return 0;
RF1AIFG &= ~BIT9;
//    WriteSinglePATable(rfPOWER10);
return cnt;
}



// *****************************************************************************
// @fn          TransmitCCA
// @brief       Transmitting data to RF with Clear Channel Assesment (CCA)
// @param       unsigned char *buffer  Pointer to the data for transmit
// @param       unsigned char *length  Lenght data to send
// @return      none
// *****************************************************************************
extern uint16_t timeout;
unsigned char TransmitCCA(unsigned char *buffer, unsigned char length)
{
	uint8_t state;
	if(length==0) return 0;		//Если размeр данных для передачи =0

	if ((ReadSingleReg(MARCSTATE)& 0x1F) != 0x13)	// TX
	{
		timeout=50;
		while((ReadSingleReg(MARCSTATE)& 0x1F) != 0x0D)
			if (!timeout)
				{
				RadioInit();
				return 0;
				}

		// CCA enabled: will enter TX state only if the channel is clear 
		Strobe( RF_STX );
	}
	
	state = ReadSingleReg(MARCSTATE) & 0x1F;
	if ((state == 0x13) || (state == 0x14) || (state == 0x15))	// Перешли на передачу
	{
		R_LedOn;//Красный
		
		RadioDog_tx = 50;
		
//		if (ee_modem_settings.rf_length == 0)
		if (rf_packet_len == 0)
		{
			WriteSingleReg(PKTLEN,  length+1);
			WriteSingleReg(RF_TXFIFOWR, length);
		}
		else WriteSingleReg(PKTLEN,  length);
		
		if (length < 60)
		{
			WriteBurstReg(RF_TXFIFOWR, buffer, length);
			tx_data_len = 0;
		}
		else
		{
			WriteBurstReg(RF_TXFIFOWR, buffer, 60);
			tx_data_len = length - 60;
			tx_data_index = 60;
		}
		RF_transmitting = 1;
		return 1;
	}
	else
	{
		return 0;
	}
}



//
int8_t rfCorrectFreq(uint8_t correct)
{
int8_t f;
f=ReadSingleReg(FREQEST);
f+=ReadSingleReg(FSCTRL0);
if(correct) halRfWriteReg(FSCTRL0,f);
return f;
}



// *****************************************************************************
// Установка мощьности передатчика в зависимости от уровня принимаемого сигнала
// и запрос значения RSSI
// @param       uint8_t value		RF Power correct enable
// @return      uint8_t			RSSI value
// *****************************************************************************
uint8_t rfCorrectGain(uint8_t correct)
{
uint8_t r;
r=ReadSingleReg(RSSI);
r += 128;
if(r > (128+32))
	{
	if(correct) WriteSinglePATable(rfPOWER0);
	halRfWriteReg(FIFOTHR,0x77);
	}
else if(r < (128+32))
	{
	if(correct) WriteSinglePATable(rfPOWER10);
	};
return r;
}



// *****************************************************************************
// Прием данных по радио
// @param		uint8_t *buf		Приемный буфер
// @param		uint8_t cnt		Размер буфера
// @param		uint16_t ticks		Таймаут приема (если задан 0, ведется поиск несущей)
// @return	uint8_t 			размер принятых данных/код ошибки
// *****************************************************************************
#define maxRx 25
#define TimeForCarrierSense (((1+8)*FREQXT1+1200)/2400)/2
#define TICKSTIMER  RTCPS
uint8_t rfReceive(uint8_t *buf, uint8_t cnt, uint16_t ticks)
{
uint16_t rxTimerStart;
if(cnt > maxRx) cnt = maxRx;
halRfWriteReg(PKTLEN,cnt);	//  Fixed Packet Length
halRfWriteReg(FIFOTHR,0x47);	// Max GAIN
//    halRfWriteReg(AGCCTRL2,0x02);//AGC Control2 max gain //0x43
RF1AIES &= ~BIT9;			// Rising edge of RFIFG9 SYNC
RF1AIFG &= ~BIT9;			// Clear a pending interrupt
RF1AIES &= ~BITD;			// Rising edge of RFIFG13 CS
RF1AIFG &= ~BITD;			// Clear a pending interrupt
//RF1AIE  |= BIT9;	// Enable the interrupt
Strobe( RF_SRX );			// Radio is in IDLE following a TX, so strobe SRX to enter Receive Mode
rxTimerStart=TICKSTIMER;
if(ticks < TimeForCarrierSense) ticks = TimeForCarrierSense;
//    Find Carrier Sense
while(!(RF1AIFG & BITD))
	{
	if( (TICKSTIMER-rxTimerStart) > ticks) //if timeout (несущая не появилась)
		{
		Strobe( RF_SIDLE );	//Exit RX / TX, turn off frequency synthesizer.
		return 255;		//return error - Carrier Sense not found
		};
	};
rfCorrectGain(1);//Set Gain for this RSSI
if(ticks == TimeForCarrierSense)	//Если требовалось только определить несущую
	{
	Strobe( RF_SIDLE );
	return 0;			//есть несущая (идет прием)
	};
//    Find Sync
#define COUNTER
RF1AIES |=  BITD;          // Falling edge of RFIFG13 CS
RF1AIFG &= ~BITD;          // Clear a pending interrupt
while(!(RF1AIFG & BIT9))    // wait for SYNC
	{
	if(RF1AIFG & BITD)
		{
		Strobe( RF_SIDLE );
		return 255;		//return error - Carrier Sense not found
		};
	if( (TICKSTIMER-rxTimerStart) > ticks)	//timeout
		{
		Strobe( RF_SIDLE );	//Exit RX / TX, turn off frequency synthesizer.
		return 0;
		};
	};
//  SYNC detected
RF1AIES |=  BIT9;     // Falling edge of RFIFG9 EndOfPacket
RF1AIFG &= ~BIT9;     // Clear a pending interrupt
while(!((RF1AIFG & BIT9)||(RF1AIFG & BITD))); // No CarrierSence
// EndOfRadio
Strobe( RF_SIDLE );
cnt = ReadSingleReg( RXBYTES );
ReadBurstReg(RF_RXFIFORD, buf, cnt);
//RF1AIE  &= ~BIT9;//new
RF1AIFG &= ~BIT9;
//RF1AIE  &= ~BITD;//new
RF1AIFG &= ~BITD;
//    Strobe( RF_SCAL );
return cnt;
}



bool CheckCarrierSense(void)
{
uint16_t rxTimerStart=TICKSTIMER;
RF1AIFG &=~ BITD;
//    Find Carrier Sense
while(!(RF1AIFG & BITD))
	{
	//if waiting time is long, return error - Carrier Sense not found
	if( (TICKSTIMER-rxTimerStart) > TimeForCarrierSense) return false;
	};
return true;
}


// *****************************************************************************
// Handler interrupt vector
// *****************************************************************************
//unsigned char wor_rx;
//unsigned char IsCarrierSense;
unsigned char RF_transmitting=0;
unsigned char RFRxBufferLength=0;
unsigned char RFRxBuffer[256];


// Загрузка принятых по радио данных
void rf_receive()
{
	static uint16_t rx_index=0;
	static uint16_t remains=-1;	// Остаток
	uint8_t rx_compl;
	
	// Если в режиме передачи, то не считываем
//	if (RF_transmitting) return;		//_new
	
	RFRxBufferLength = ReadSingleReg( RXBYTES ) & 0x7F;	// Без учёта флага переполнения
	
	if ((RFRxBufferLength + rx_index) >= remains ) rx_compl = 1;
	else rx_compl = 0;
	
	if (rx_compl == 0)
	{
		if (RFRxBufferLength) RFRxBufferLength--;
	}
	// Нельзя опустошать буфер до получения последнего байта!
//	if (RFRxBufferLength < (remains - rx_index))
//	{
//		if ((RFRxBufferLength > 1) && (rx_compl == 0)) RFRxBufferLength--;
//		else RFRxBufferLength = 0;
//	}
//	else RFRxBufferLength = remains - rx_index;

	if (RFRxBufferLength)
	{
//		rx_compl = 0;
		
		RadioDog_rx = 20;
		
		G_LedOn;//Зеленый
		timeout = 100;
		while (!(RF1AIFCTL1 & RFINSTRIFG))if (timeout==0) return;       // Wait for INSTRIFG
		RF1AINSTR1B = (RF_RXFIFORD | RF_REGRD);          // Send addr of first conf. reg. to be read
		// ... and the burst-register read instruction
		while(--RFRxBufferLength)
		{
			timeout = 100;
			while (!(RFDOUTIFG&RF1AIFCTL1))if (timeout==0) return;        // Wait for the Radio Core to update the RF1ADOUTB reg
			RFRxBuffer[rx_index] = RF1ADOUT1B;                 // Read DOUT from Radio Core + clears RFDOUTIFG
			if (rx_index < sizeof(RFRxBuffer)) rx_index++;
			// Also initiates auo-read for next DOUT byte
		}
		RFRxBuffer[rx_index] = RF1ADOUT0B;
		if (rx_index < sizeof(RFRxBuffer)) rx_index++;
		
		// Определяем количество ожидаемых байт
		if (remains == -1)
		{
//			if (ee_modem_settings.rf_length == 0)
			if (rf_packet_len == 0)
			{
				// В режиме переменной длины первый байт - длина посылки
				remains = RFRxBuffer[0];
				// Сдвинем данные
				rx_index--;
				memcpy( RFRxBuffer, RFRxBuffer+1, rx_index );
			}
//			else remains = ee_modem_settings.rf_length;
			else remains = rf_packet_len;
		}
		
		// Контролируем заполненость пакета
//		if (rx_index >= remains)
		if ( rx_compl )
		{
			G_LedOff;
			RFRxBufferLength = rx_index;	// Для дальнейшей обработки
			rx_index = 0;
			remains = -1;
		}
	}
	// На случай зависания
	if ((rx_index != 0) && ((RadioDog_rx == 0) || rx_compl))	//new контроль несущей
	{
		Strobe( RF_SIDLE );
		G_LedOff;
		RFRxBufferLength = rx_index;	// Для дальнейшей обработки
		rx_index = 0;
		remains = -1;
		ReceiveOn();
	}
	
//	if (rx_index > 0) rx_compl = ((RF1AIN & 0x01) == 0);//debug
}


// Продолжение передача данных по радио
void rf_transmit()
{
	uint8_t tx_buf_byts;	// Количество байтов в буфере передачи
	
	if (RF_transmitting)
	{
//	tx_buf_byts = ReadSingleReg(MARCSTATE);//debug
		tx_buf_byts = ReadSingleReg( TXBYTES );
		
		if ((tx_buf_byts & 0x80) == 0)	// Буфер недогружен
		{
			tx_buf_byts &= 0x7F;
			tx_buf_byts = 64 - tx_buf_byts;	// Свободное место
			
			if (tx_data_len)
			{
				if (tx_data_len < tx_buf_byts)
				{
					WriteBurstReg( RF_TXFIFOWR, (uint8_t*)urx_buf+tx_data_index, tx_data_len );
					tx_data_len = 0;
				}
				else
				{
					WriteBurstReg( RF_TXFIFOWR, (uint8_t*)urx_buf+tx_data_index, tx_buf_byts );
					tx_data_len -= tx_buf_byts;
					tx_data_index += tx_buf_byts;
				}
			}
			else
			{
				// Ждём окончания передачи
				if ((ReadSingleReg(MARCSTATE)& 0x1F) == 0x01)	// IDLE
				{
					RF_transmitting = 0;
					// Flush TX FIFO
					Strobe(RF_SFTX);
					R_LedOff;
					ReceiveOn();
				}
			}
		}// if ((tx_buf_byts & 0x80) == 0)
		// Антизависатель
		if (RadioDog_tx == 0)
		{
			RF_transmitting = 0;
			R_LedOff;
			RadioInit();
		}
	}// if (RF_transmitting)
	// Неожиданное включение передатчика
	else if ((ReadSingleReg(MARCSTATE)& 0x1F) == 0x13)	// TX
	{
		RF_transmitting = 0;
		R_LedOff;
		Strobe(RF_SFTX);
		ReceiveOn();
	}
}