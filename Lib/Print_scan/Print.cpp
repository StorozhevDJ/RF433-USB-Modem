

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>


#include "Print.hpp"




// Public Methods //////////////////////////////////////////////////////////////
Print& Print::print(uint8_t b)
{
write(b);
return *this;
}

Print& Print::print(char c)
{
print((byte) c);
return *this;
}

Print& Print::print(const char c[])
{
while (*c) print(*c++);
return *this;
}

Print& Print::print(int n)
{
print((long) n);
return *this;
}

Print& Print::print(unsigned int n)
{
print((unsigned long) n);
return *this;
}

Print& Print::print(long n)
{
if (n < 0)
	{
	print('-');
	n = -n;
	}
printNumber(n, 10);
return *this;
}

Print& Print::print(unsigned long n)
{
printNumber(n, 10);
return *this;
}

Print& Print::print(long n, int base)
{
if (base == 0) print((char) n);
else if (base == 10) print(n);
else printNumber(n, base);
return *this;
}

Print& Print::print(double n)
{
printFloat(n, 2);
return *this;
}

Print& Print::println(void)
{
print('\r');
print('\n');  
return *this;
}

Print& Print::println(char c)
{
print(c);
println();  
return *this;
}

Print& Print::println(const char c[])
{
print(c);
println();
return *this;
}

Print& Print::println(uint8_t b)
{
print(b);
println();
return *this;
}

Print& Print::println(int n)
{
print(n);
println();
return *this;
}

Print& Print::println(unsigned int n)
{
print(n);
println();
return *this;
}

Print& Print::println(long n)
{
print(n);
println();  
return *this;
}

Print& Print::println(unsigned long n)
{
print(n);
println();  
return *this;
}

Print& Print::println(long n, int base)
{
print(n, base);
println();
return *this;
}

Print& Print::println(double n)
{
print(n);
println();
return *this;
}

// Private Methods /////////////////////////////////////////////////////////////

void Print::printNumber(unsigned long n, uint8_t base)
{
  unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
  unsigned long i = 0;

  if (n == 0) {
    print('0');
    return;
  } 

  while (n > 0) {
    buf[i++] = n % base;
    n /= base;
  }

  for (; i > 0; i--)
    print( (char) (buf[i - 1] < 10 ? '0' + buf[i - 1] : 'A' + buf[i - 1] - 10) );
}

void Print::printFloat(double number, uint8_t digits) 
{ 
  // Handle negative numbers
  if (number < 0.0)
  {
     print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    print(toPrint);
    remainder -= toPrint; 
  } 
}





/* reverse:  �������������� ������ s �� ����� */
void reverse(char s[])
{
int i, j;
char c;

for (i = 0, j = strlen(s)-1; i<j; i++, j--)
	{
	c = s[i];
	s[i] = s[j];
	s[j] = c;
	}
}



/* itoa:  ������������ n � ������� � s */
void itoa(int n, char s[])
{
int i, sign;

// ���������� ����
if ((sign = n) < 0) n = -n;	// ������ n ������������� ������
i = 0;

// ���������� ����� � �������� �������
do	
	{
	s[i++] = n % 10 + '0';	// ����� ��������� �����
	} while ((n /= 10) > 0);	// �������
if (sign < 0) s[i++] = '-';
s[i] = '\0';
reverse(s);
}



/* itoa:  ������������ n � ������� � s */
void itoa(int n, char s[], int l)
{
int i=l, sign;

// ���������� ����
if ((sign = n) < 0) n = -n;	// ������ n ������������� ������
i = 0;

// ���������� ����� � �������� �������
do
	{
	s[i++] = n % 10 + '0';	// ����� ��������� �����
	if (i==l) s[i++]='.';
	(n /= 10) > 0;	// �������
	} while (n||(l>=i-1));
if (sign < 0) s[i++] = '-';
s[i] = '\0';
reverse(s);
}



