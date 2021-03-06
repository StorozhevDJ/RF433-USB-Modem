
/* ------------------------------------------------------------------------------------------------
 *                                          Defines
 * ------------------------------------------------------------------------------------------------
 */

/********************
 * Variable definition
 */
typedef struct S_RF_SETTINGS {
    unsigned char iocfg2;     // GDO2 Output Configuration
    unsigned char iocfg1;     // GDO1 Output Configuration
    unsigned char iocfg0;     // GDO0 Output Configuration
    unsigned char fifothr;    // RX FIFO and TX FIFO Thresholds
    unsigned char sync1;      // Sync Word, High Byte
    unsigned char sync0;      // Sync Word, Low Byte
    unsigned char pktlen;     // Packet Length
    unsigned char pktctrl1;   // Packet Automation Control
    unsigned char pktctrl0;   // Packet Automation Control
    unsigned char channr;     // Channel Number
    unsigned char fsctrl1;    // Frequency Synthesizer Control
    unsigned char fsctrl0;    // Frequency Synthesizer Control
    unsigned char freq2;      // Frequency Control Word, High Byte
    unsigned char freq1;      // Frequency Control Word, Middle Byte
    unsigned char freq0;      // Frequency Control Word, Low Byte
    unsigned char mdmcfg4;    // Modem Configuration
    unsigned char mdmcfg3;    // Modem Configuration
    unsigned char mdmcfg2;    // Modem Configuration
    unsigned char mdmcfg1;    // Modem Configuration
    unsigned char mdmcfg0;    // Modem Configuration
    unsigned char deviatn;    // Modem Deviation Setting
    unsigned char mcsm2;      // Main Radio Control State Machine Configuration
    unsigned char mcsm1;      // Main Radio Control State Machine Configuration
    unsigned char mcsm0;      // Main Radio Control State Machine Configuration
    unsigned char foccfg;     // Frequency Offset Compensation Configuration
    unsigned char bscfg;      // Bit Synchronization Configuration
    unsigned char agcctrl2;   // AGC Control
    unsigned char agcctrl1;   // AGC Control
    unsigned char agcctrl0;   // AGC Control
    unsigned char worctrl;    // Wake On Radio Control
    unsigned char frend1;     // Front End RX Configuration
    unsigned char frend0;     // Front End TX Configuration
    unsigned char fscal3;     // Frequency Synthesizer Calibration
    unsigned char fscal2;     // Frequency Synthesizer Calibration
    unsigned char fscal1;     // Frequency Synthesizer Calibration
    unsigned char fscal0;     // Frequency Synthesizer Calibration
    unsigned char test2;      // Various Test Settings
    unsigned char test1;      // Various Test Settings
    unsigned char test0;      // Various Test Settings
} RF_SETTINGS;

void ResetRadioCore (void);
unsigned char Strobe(unsigned char strobe);

void WriteRfSettings(RF_SETTINGS *pRfSettings);

void WriteSingleReg(unsigned char addr, unsigned char value);
void WriteBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count);
unsigned char ReadSingleReg(unsigned char addr);
void ReadBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count);
void WriteSinglePATable(unsigned char value);
void WriteBurstPATable(unsigned char *buffer, unsigned char count); 
