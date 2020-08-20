

#ifndef Print_h
#define Print_h


#define byte	u8



class Print
{
public:
	virtual Print& print(uint8_t b);
	virtual Print& print(const char c[]);
	virtual Print& print(char c);
	virtual Print& print(int n);
	virtual Print& print(unsigned int n);
	virtual Print& print(long n);
	virtual Print& print(unsigned long n);
	virtual Print& print(long n, int base);
	virtual Print& print(double n);
	
	virtual Print& println(void);
	virtual Print& println(char c);
	virtual Print& println(const char c[]);
	virtual Print& println(uint8_t b);
	virtual Print& println(int n);
	virtual Print& println(unsigned int n);
	virtual Print& println(long n);
	virtual Print& println(unsigned long n);
	virtual Print& println(long n, int base);
	virtual Print& println(double n);
	
	virtual void write(uint8_t) = 0;
	//virtual size_t write(uint8_t) = 0;
	//virtual void write(const char *str);
	//virtual void write(const uint8_t *buffer, size_t size);
	
private:
	void printNumber(unsigned long n, uint8_t base);
	void printFloat(double number, uint8_t digits);
};


void reverse(char s[]);
void itoa(int n, char s[]);
void itoa(int n, char s[], int l);


#endif