#ifndef USART_H
#define USART_H

#define RX_TIMEOUT_VAL	2		//timeout for setting flag ending receive
#define RX_END_STR_CHAR	'\r'		//symbol ending string in ASCII mode

#define RX_BUF_SIZE		255		//Size of received buffer
#define TX_BUF_SIZE		64		//Size of transmitted buffer

#define UART_IS_CTS		(P1IN&BIT4)
#define UART_SET_RTS	P1OUT|=BIT3
#define UART_RESET_RTS	P1OUT&=~BIT3

#define UART_IS_DSR		(P2IN&BIT0)
#define UART_SET_DTR	P1OUT|=BIT7
#define UART_RESET_DTR	P1OUT&=~BIT7

//#define NON_BLOCKING_UART_RX


typedef enum
	{
	BIN=0,
	ASCII
	}Emode_data;

typedef struct
	{
	struct
		{
		char buf[RX_BUF_SIZE];	//Buffer for received bytes
		unsigned char cnt;		//Counter bytes of buffer
		unsigned char ptr;		//Pointer to receive byte (available value from 0 to 63)
		unsigned char timeout;	//Timeout receive
		unsigned char complete:1;//Receive complete
		unsigned char overflow:1;//buffer overflow
		}rx;
	struct
		{
		char buf[TX_BUF_SIZE];	//Buffer for transmited bytes
		unsigned char cnt;		//Counter bytes for transmit
		unsigned char ptr;		//Pointer to transmit byte (available value from 0 to 63)
		unsigned char complete:1;//Transmit complete
		}tx;
	Emode_data mode;
	}volatile TSusart;


//extern TSusart usart;

//public function
void USART_Init(void (*rx_funct_ptr)(char *buf, char cnt));
void USART_SendChar(char data);
char USART_SendString(void * str);
void USART_SendBuf(char * buf, unsigned char len);
void USART_Timer10ms(void);
void USART_SetMode(Emode_data m);

//private function
void USART_RxBufEnqueue(char byte);

#endif //USART_H