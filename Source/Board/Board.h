
#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "cc430x513x.h"
#include "Utils\Utils.h"

#define LowerPower_0_ENTER   __bis_SR_register(LPM0_bits | GIE)
#define LowerPower_0_EXIT    __bic_SR_register_on_exit(LPM0_bits)
#define LowerPower_3_ENTER   __bis_SR_register(LPM3_bits | GIE)
#define LowerPower_3_EXIT    __bic_SR_register_on_exit(LPM3_bits)

#define F_OSC 1000000UL		//Частота кварца в герцах

#define FREQXT1			32768
#define BAUD				57600//115200
#ifdef MODEM
#define FMULTY				2
#else
#define FMULTY				1
#endif
#define FSYS				(FREQXT1*32*FMULTY)
#define FGERKON			16
#define FREQXT2			26000000
#define MAX_PACKET_LEN		30
#define timeForRadioSleep(x)	(16*60*x-16*3)	//Время (x мин-3сек) в течении которого радио в спящем режиме
#define RadioInterval(x)		(16*x)		//Максимальное время (x сек), в течении которого радио включено



#define R_LedInit sbit(P3DIR,5)
#define R_LedOn   sbit(P3OUT,5)
#define R_LedOff  cbit(P3OUT,5)
#define R_LedFlp  fbit(P3OUT,5)
#define G_LedInit sbit(P3DIR,4)
#define G_LedOn   sbit(P3OUT,4)
#define G_LedOff  cbit(P3OUT,4)
#define G_LedFlp  fbit(P3OUT,4)


#ifndef __cplusplus
typedef enum {false = 0, true} bool;
#endif
enum {SUCCESS = 0, FAILURE = -1};



#pragma pack(push, 1)
typedef struct
	{
	uint32_t modem_serial;
	uint8_t rf_freq[3];
	uint8_t rf_length;			// Длина сообщений
	uint8_t var_length	: 1;	// Флаг переменной длины сообщений
	} TSmodem_settings;
#pragma pack(pop)


extern const TSmodem_settings ee_modem_settings;


// Hardware Function
void StopWdt(void);
void RunWdt(void);
void InitPMM (void);
void InitClock(void);
void StartXT2(void);
void StopXT2(void);
void WaitXT2(void);
void StartXT1(void);
void StopXT1(void);
void InitLed(void);

void InitTimer0(void);
//void InitAdc(void);
void InitUart(void);
void InitSleep(void);
void InitRTC(void);
void SetupRTC(uint8_t *TimeBuf);
void DelayCPUx10(uint16_t t);
void DelayXT1(uint16_t ticks);
void PrintInfo(void);

//
void LedBlinking(void);
uint16_t  CRC16(uint8_t *buf,uint8_t length);
// EEPROM function
bool eeWrite(void *Flash_ptr, void *eData, uint8_t cnt);
void eeErase(uint8_t *Flash_ptr);
void unlockJTAG(void);
void lockJTAG(void);

void UnusedPORT_Init (void);


#endif