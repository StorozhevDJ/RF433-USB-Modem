
#include "Radio\RF1A.h"

// Rf settings for CC430
/*RF_SETTINGS rfSettings = {
    0x06,  // IOCFG2        GDO2 Output Configuration
    0x0E,  // IOCFG0        GDO0 Output Configuration
    0x47,  // FIFOTHR       RX FIFO and TX FIFO Thresholds
    0xF6,  // SYNC1         Sync Word, High Byte
    0x8D,  // SYNC0         Sync Word, Low Byte
    0x3F,  // PKTLEN        Packet Length
    0x40,  // PKTCTRL1      Packet Automation Control
    0x00,  // PKTCTRL0      Packet Automation Control
    0x02,  // CHANNR        Channel Number
    0x06,  // FSCTRL1       Frequency Synthesizer Control
    0x10,  // FREQ2         Frequency Control Word, High Byte
    0xAF,  // FREQ1         Frequency Control Word, Middle Byte
    0x75,  // FREQ0         Frequency Control Word, Low Byte
    0xF6,  // MDMCFG4       Modem Configuration
    0x83,  // MDMCFG3       Modem Configuration
    0x51,  // MDMCFG1       Modem Configuration
    0x2E,  // MDMCFG0       Modem Configuration
    0x16,  // DEVIATN       Modem Deviation Setting
    0x00,  // MCSM1         Main Radio Control State Machine Configuration
    0x18,  // MCSM0         Main Radio Control State Machine Configuration
    0x16,  // FOCCFG        Frequency Offset Compensation Configuration
    0x43,  // AGCCTRL2      AGC Control
    0xFB,  // WORCTRL       Wake On Radio Control
    0xE9,  // FSCAL3        Frequency Synthesizer Calibration
    0x2A,  // FSCAL2        Frequency Synthesizer Calibration
    0x00,  // FSCAL1        Frequency Synthesizer Calibration
    0x1F,  // FSCAL0        Frequency Synthesizer Calibration
    0x81,  // TEST2         Various Test Settings
    0x35,  // TEST1         Various Test Settings
    0x09,  // TEST0         Various Test Settings
};*/

//extern RF_SETTINGS rfSettings;



const unsigned char CC110L_Cfg[] = {0x06, 0x2e, 0x0e,               // IOCFG2,1,0         0x00, 0x01, 0x02
                                    0x47,                           // FIFOTHR            0x03
                                    0xF6, 0x8D,                     // SYNC1,0            0x04, 0x05
                                    0x3f,                           // PKTLEN             0x06
                                    0x40, 0x00,                     // PKTCTRL1,0         0x07, 0x08
                                    0x00,                           // ADDR               0x09
                                    0x02,                           // CHANNR             0x0a
                                    0x06, 0x00,                     // FSCTRL1,0          0x0b, 0x0c
                                    0x10, 0xAF, 0x75,               // FREQ2,1,0          0x0d, 0x0e, 0x0f
                                    0xF6, 0x83, 0x02, 0x51, 0x2e,   // MDMCFG4,3,2,1,0    0x10, 0x11, 0x12, 0x13, 0x14
                                    0x16,                           // DEVIATN            0x15
                                    0x07, 0x00, 0x18,               // MCSM2,1,0          0x16, 0x17, 0x18
                                    0x16,                           // FOCCFG             0x19
                                    0x6C,                           // BSCFG              0x1a
                                    0x43, 0x40, 0x91,               // AGCCTRL2,1,0       0x1b, 0x1c, 0x1d
                                    0x00, 0x00,                     // Not used           0x1e, 0x1f
                                    0xfb,                           // Reserved (SmartRF) 0x20
                                    0x56, 0x10,                     // FREND1,0           0x21, 0x22
                                    0xe9, 0x2a, 0x00, 0x1f,         // FSCAL3,2,1,0       0x23, 0x24, 0x25, 0x26
                                    0x00, 0x00,                     // Not used           0x27, 0x28
                                    0x59, 0x7f, 0x3f,               // Reserved (SmartRF) 0x29, 0x2a, 0x2b
                                    0x81, 0x35, 0x09};              // TEST2,1,0          0x2c, 0x2d, 0x2e