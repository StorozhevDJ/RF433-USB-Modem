
#include <stdio.h>
#include <stdlib.h>

#include "cmd.h"
#include "Board\Board.h"
#include "Radio.h"
#include "UART.h"
#include "..\main.h"
#include "Print_scan\Parser.h"



const char cmd_esc[]	="+++";	//Go to Command mode
const char cmd_mode[]	="TM";	//Go to Data (Transparent) mode from Command mode
const char cmd_reset[]	="RST";	//Radio Reset
const char cmd_strobe[]	="STR";	//Send strobe
const char cmd_def_reg[]	="DEF";	//Set default reg
const char cmd_get_reg[]	="REG?";	//Read Reg value
const char cmd_set_reg[]	="REG=";	//Write Reg value
const char cmd_get_breg[]="BREG?";	//Read Burst Reg value
const char cmd_set_breg[]="BREG=";	//Write Burst Reg value
const char cmd_get_chan[]="CH?";	//Get chanal
const char cmd_set_chan[]="CH=";	//Set chanal
const char cmd_get_freq[]="FREQ?";	//Get frequency
const char cmd_set_freq[]="FREQ=";	//Set frequency
const char cmd_set_pat []="PAT=";	//Set PATable
const char cmd_get_rssi[]="RSSI";	//Get RSSI value
const char cmd_send_hex[]="HEX=";	//Send HEX data from string to radio
const char cmd_set_echo[]="ECHO=";	//Enable/disable Echo mode
const char cmd_set_lqi []="LQI=";	//Add RSSI to data in UART
const char cmd_ati	   []="ATI";	//Display product identification information
const char cmd_set_sn  []="WSN=";	//Write Serial Number
const char cmd_get_len[]="LEN?";	//Get length
const char cmd_set_len[]="LEN=";	//Set length
const char cmd_get_var_len[]="VRLEN?";	//Get variable length flag
const char cmd_set_var_len[]="VRLEN=";	//Set variable length flag
const char cmd_set_bdr[]="BDR=";	// Установка скорости обмена

char ansver_ok []="OK\r\n";		//Send ansver Ok
char ansver_err[]="Error\r\n";	//Send ansver Error
char ansver_no_cmd[]="Command not found\r\n";


bool echo=false;	//wM-Bus mode
bool wmb=false;
bool lqi=false;	//Add RSSI to data in UART

extern uint8_t rf_packet_len;

/*******************************************************************************
* Checking the presence of the command line
* @ret true if start command line is present
*******************************************************************************/
bool IsCmd (char * str)
{
if (str[0]!='R') return false;
if (str[1]!='M') return false;
if (str[2]!='+') return false;
return true;
}



/*******************************************************************************
* Compare two string, one from flash and one from RAM
* @ret true if comared string is equally
*******************************************************************************/
bool Cmd_compare (const char * cmd, char *buf)
{
//buf+=3;
while (*cmd!=0)
	{
	if (*cmd!=*buf) return false;
	cmd++;
	buf++;
	}
return true;
}


extern uint16_t RadioDog_rx, RadioDog_tx;

/*******************************************************************************
* Finding, parsing and executing command
*******************************************************************************/
void Cmd_execute (char *cmd)
{
TSmodem_settings settings;
TSmodem_settings *settings_ee = (TSmodem_settings *)&ee_modem_settings;
settings=*settings_ee;

//Go to Data (transparent) mode from Command mode
if (Cmd_compare(cmd_mode, cmd))
	{
	modem_mode=transparent;
	UART_SET_DTR;
	R_LedOff;
	G_LedOff;
	USART_SetMode(BIN);
	USART_SendString(ansver_ok);
	}

//Reset radio
else if (Cmd_compare(cmd_reset, &cmd[0]))
	{
	RadioInit();
	USART_SendString(ansver_ok);
	return;
	}

//Send Strobe
else if (Cmd_compare(cmd_strobe, cmd))
	{
	short val;
	if (Parser_GetParamShort(&cmd[4], RX_END_STR_CHAR, 0, &val))
		{
		Strobe(val);
		USART_SendString(ansver_ok);
		if (val == 53) {RF_transmitting = 1; RadioDog_tx = 0xffff;}
		else if (val == 52) RF_transmitting = 0;
		}
	return;
	}

//Set chanal
else if (Cmd_compare(cmd_set_chan, cmd))
	{
	short chn;
	if (Parser_GetParamShort(&cmd[sizeof(cmd_set_chan)-1], ' ', 0, &chn))
		{
		Strobe(RF_SIDLE);
		halRfWriteReg(CHANNR, chn);
		ReceiveOn();
		USART_SendBuf(ansver_ok, 4);
		}
	return;
	}

//Get chanal
else if (Cmd_compare(cmd_get_chan, cmd))
	{
	char ansver[16];
	Parser_BufPrintf(ansver, "+Channel:%i\r\n", ReadSingleReg(CHANNR));
	USART_SendString(ansver);
	return;
	}

//Set PATable
else if (Cmd_compare(cmd_set_pat, cmd))
	{
	short pat_val;
	if (Parser_GetParamShort(&cmd[4], RX_END_STR_CHAR, 0, &pat_val))
		{
		WriteSinglePATable(pat_val);
		USART_SendString(ansver_ok);
		}
	return;
	}

//Get RSSI value
else if (Cmd_compare(cmd_get_rssi, cmd))
	{
	uint8_t rssi_dec; 
	int16_t rssi_dBm; 
	uint8_t rssi_offset = 72;
	rssi_dec = ReadSingleReg(RSSI); 
	if(rssi_dec >= 128) 
	rssi_dBm = (int16_t)((int16_t)( rssi_dec - 256) / 2) - rssi_offset; 
	else 
	rssi_dBm = (rssi_dec / 2) - rssi_offset;
	
	char ansver[16];
	Parser_BufPrintf(ansver, "+RSSI:%idBm\r\n", rssi_dBm);
	USART_SendString(ansver);
	return;
	}

//Read Reg value
else if (Cmd_compare(cmd_get_reg, cmd))
	{
	short reg_adr;
	if (Parser_GetParamShort(&cmd[4], RX_END_STR_CHAR, 0, &reg_adr))
		{
		char ansver[16];
		Parser_BufPrintf(ansver, "+REG:%i\r\n", ReadSingleReg(reg_adr));
		USART_SendString(ansver);
		}
	else USART_SendString(ansver_err);
	return;
	}

//Write Reg value (REG=12,05\r)
else if (Cmd_compare(cmd_set_reg, cmd))
	{
	short reg_adr, reg_val;
	short reg_adr_len, reg_val_len;
	reg_adr_len=Parser_GetParamShort(&cmd[4], ',', 0, &reg_adr);
	reg_val_len=Parser_GetParamShort(&cmd[4+reg_adr_len+1], RX_END_STR_CHAR, 0, &reg_val);
	if (reg_adr_len&&reg_val_len)
		{
		WriteSingleReg(reg_adr, reg_val);
		USART_SendString(ansver_ok);
		}
	else USART_SendString(ansver_err);
	return;
	}

//Send Hex string data to radio
else if (Cmd_compare(cmd_send_hex, cmd))
	{
	short val;
	uint8_t data_buf_tx[32];
	for (uint8_t n=0; n<32; n++)
		{
		//Если найдено число с номером n (разделенное символом ' ')
		if (Parser_GetParamHex(&cmd[4], ' ', n, &val)) data_buf_tx[n]=val;
		else
			{
			TransmitCCA(data_buf_tx, n);
			break;
			}
		}
	return;
	}

//Set ECHO mode
else if (Cmd_compare(cmd_set_echo, cmd))
	{
	short val;
	if (Parser_GetParamShort(&cmd[5], RX_END_STR_CHAR, 0, &val))
		{
		echo = (bool)val;
		USART_SendString(ansver_ok);
		}
	return;
	}

//Set RSSI
else if (Cmd_compare(cmd_set_lqi, cmd))
	{
	short val;
	if (Parser_GetParamShort(&cmd[4], RX_END_STR_CHAR, 0, &val))
		{
		lqi = (bool)val;
		USART_SendString(ansver_ok);
		}
	return;
	}

//Get Manufacturer Identification
else if (Cmd_compare(cmd_ati, cmd))
	{
	char buf[64];
	Parser_BufPrintf(buf, "Manufacturer: SET\r\nSerial: %8H\r\nVer.: 07\r\nType: 36\r\n", settings_ee->modem_serial);
	USART_SendString(buf);
	return;
	}

//Write to flash FREQ value
else if (Cmd_compare(cmd_set_freq, cmd))
	{
	short val;
	for (uint8_t n=0; n<3; n++)
		{
		if (Parser_GetParamHex(&cmd[5], ' ', n, &val)) settings.rf_freq[2-n]=val;
		}
	halRfWriteReg(FREQ2, settings.rf_freq[2]);   //Frequency Control Word, Low Byte
	halRfWriteReg(FREQ1, settings.rf_freq[1]);   //Frequency Control Word, Middle Byte
	halRfWriteReg(FREQ0, settings.rf_freq[0]);   // Carrier frequency = 433.82
	eeWrite((uint8_t *)&ee_modem_settings, &settings, sizeof(TSmodem_settings));
	USART_SendString(ansver_ok);
	return;
	}

//Write to flash serial number value
else if (Cmd_compare(cmd_set_sn, cmd))
	{
	long val;
	if (Parser_GetParamLong(&cmd[4], ' ', 0, &val)) settings.modem_serial=val;
	eeWrite((uint8_t *)&ee_modem_settings, &settings, sizeof(TSmodem_settings));
	USART_SendString(ansver_ok);
	return;
	}

//Read rf_length
else if (Cmd_compare(cmd_get_len, cmd))
	{
	char ansver[16];
	Parser_BufPrintf(ansver, "+Length:%i\r\n", settings.rf_length);
	USART_SendString(ansver);
	return;
	}

//Read var_rf_length flag
else if (Cmd_compare(cmd_get_var_len, cmd))
	{
	char ansver[16];
	Parser_BufPrintf(ansver, "+VarLength:%i\r\n", settings.var_length);
	USART_SendString(ansver);
	return;
	}

//Write to flash rf_length
else if (Cmd_compare(cmd_set_len, cmd))
	{
	short val;
//	if (Parser_GetParamShort(&cmd[sizeof(cmd_set_len)-1], ' ', 0, &val)) settings.rf_length=val;
	if (Parser_GetParamShort(&cmd[sizeof(cmd_set_len)-1], ' ', 0, &val)) rf_packet_len=val;
	if (val == 0) settings.var_length = 1;
	else 		  settings.var_length = 0;
//	eeWrite((uint8_t *)&ee_modem_settings, &settings, sizeof(TSmodem_settings));
	ReceiveOn();
	USART_SendString(ansver_ok);
	return;
	}

//Write to flash var_rf_length flag
else if (Cmd_compare(cmd_set_var_len, cmd))
	{
	short val;
	if (Parser_GetParamShort(&cmd[sizeof(cmd_set_var_len)-1], ' ', 0, &val))
	{
		if ((val == 1)||(val == 0)) settings.var_length = val;
	}
	eeWrite((uint8_t *)&ee_modem_settings, &settings, sizeof(TSmodem_settings));
	USART_SendString(ansver_ok);
	return;
	}

// Set BDR
else if (Cmd_compare(cmd_set_bdr, cmd))
	{
	short bdr;
	if (Parser_GetParamShort(&cmd[sizeof(cmd_set_bdr)-1], ' ', 0, &bdr))
		{
		WriteRfbdr(bdr);
		USART_SendString(ansver_ok);
		}
	else
		USART_SendString(ansver_err);
	}

else USART_SendString(ansver_no_cmd);
}