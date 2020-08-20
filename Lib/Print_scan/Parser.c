/*******************************************************************************
* Ver 15.02.04
*******************************************************************************/


#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "Parser.h"


void reverse(char s[]);

/* itoa:  ������������ n � ������� � s */
void itoaf(int n, char s[], int l)
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

void ltoa(long int n,char *str)
{
unsigned long i;
unsigned char j,p;
i=1000000000L;
p=0;
if (n<0)
	{
	n=-n;
	*str++='-';
	};
do
	{
	j=(unsigned char) (n/i);
	if (j || p || (i==1))
		{
		*str++=j+'0';
		p=1;
		}
	n%=i;
	i/=10L;
	}
while (i!=0);
*str=0;
}



void ltoa_null(long n, char s[], int l)
{
long sign;
// ���������� ����
if ((sign = n) < 0) n = -n;	// ������ n ������������� ������

for (uint8_t i=0; i<l; i++)
	{
	s[i] = n%10+'0';
	n/=10;
	};
if (sign<0)
	{
	s[l] = '-';
	s[l+1] = '\0';
	}
else s[l] = '\0';
reverse(s);
}

/*******************************************************************************
* str - ������ ��� ������ ������� ���������								*
* ch  - ����������� ����������										*
* num - ����� �������� ���������										*
* return - �������� �� ������ ������, �� �������� ���������� ������� ��������	*
* ���� �������� �� ������, ������������ -1
*******************************************************************************/
int Parser_FindPos(const char *str, char ch, char num)
{
int pos;
pos=0;
while (*str)
	{
	if (num==0) return pos;
	if (*str==ch) num--;
	pos++;
	str++;
	}
return -1;
}



/*******************************************************************************
* str - ������ ��� ������ ���������									*
* ch  - ����������� ����������										*
* num - ����� �������� ���������										*
* val - ������ �� ������ ��� �������� �������� ��������� (�������)			*
* return - �������� �� ������ ������, �� �������� ���������� ������� ��������	*
* ���� �������� �� ������, ������������ 0								*
*******************************************************************************/
int Parser_GetParamChar(const char *str, char ch, char num, char *val)
{
int pos;
pos=Parser_FindPos(str, ch, num);
if (pos==-1) return 0;
if (str[pos]!=ch) *val=str[pos]; else val=0;
return *val;
}



/*******************************************************************************
* str - ������ ��� ������ ���������									*
* ch  - ����������� ����������										*
* num - ����� �������� ���������										*
* val - ������ �� ������ ��� �������� �������� ��������� (����� int)			*
* return - �������� �� ������ ������, �� �������� ���������� ������� ��������	*
* ���� �������� �� ������, ������������ 0, ���� ������ �� ������ ������		*
*******************************************************************************/
int Parser_GetParamInt(const char *str, char ch, char num, int *val)
{
char intstr[11];
char ptr;
int pos;
pos=Parser_FindPos(str, ch, num);
if (pos==-1) return 0;
if (str[pos]==ch)
	{
	val=0;
	return 0;
	}

for (ptr=0; ptr<11;)
	{
	if ((str[pos]==ch)||(str[pos]==0)) break;
	if (((str[pos]>0x39)||(str[pos]<0x30))&&
	    ((str[pos]!='-')&&(str[pos]!=' ')&&(str[pos]!='.'))) return 0;
	if ((str[pos]!=' ')&&(str[pos]!='.')) intstr[ptr++]=str[pos];
	pos++;
	}
intstr[ptr]=0;
*val=atoi(intstr);
return ptr;
}



/*******************************************************************************
* str - ������ ��� ������ ���������									*
* ch  - ����������� ����������										*
* num - ����� �������� ���������										*
* val - ������ �� ������ ��� �������� �������� ��������� (����� long)			*
* return - �������� �� ������ ������, �� �������� ���������� ������� ��������	*
* ���� �������� �� ������, ������������ 0, ���� ������ �� ������ ������		*
*******************************************************************************/
int Parser_GetParamLong(const char *str, char ch, char num, long *val)
{
char intstr[11];
char ptr;
int pos;
pos=Parser_FindPos(str, ch, num);
if (pos==-1) return 0;
if (str[pos]==ch)
	{
	val=0;
	return 0;
	}

for (ptr=0; ptr<11;)
	{
	if ((str[pos]==ch)||(str[pos]==0)) break;
	if (((str[pos]>0x39)||(str[pos]<0x30))&&
	    ((str[pos]!='-')&&(str[pos]!=' ')&&(str[pos]!='.'))) return 0;
	if ((str[pos]!=' ')&&(str[pos]!='.')) intstr[ptr++]=str[pos];
	pos++;
	}
intstr[ptr]=0;
*val=atol(intstr);
return ptr;
}



/*******************************************************************************
* str - ������ ��� ������ ���������									*
* ch  - ����������� ����������										*
* num - ����� �������� ���������										*
* val - ������ �� ������ ��� �������� �������� ��������� (����� long)			*
* return - �������� �� ������ ������, �� �������� ���������� ������� ��������	*
* ���� �������� �� ������, ������������ 0, ���� ������ �� ������ ������		*
*******************************************************************************/
int Parser_GetParamShort(const char *str, char ch, char num, short *val)
{
char intstr[11];
char ptr;
int pos;
pos=Parser_FindPos(str, ch, num);
if (pos==-1) return 0;
if (str[pos]==ch)
	{
	val=0;
	return 0;
	}

for (ptr=0; ptr<11;)
	{
	if ((str[pos]==ch)||(str[pos]==0)) break;
	if (((str[pos]>0x39)||(str[pos]<0x30))&&
	    ((str[pos]!='-')&&(str[pos]!=' ')&&(str[pos]!='.'))) return 0;
	if ((str[pos]!=' ')&&(str[pos]!='.')) intstr[ptr++]=str[pos];
	pos++;
	}
intstr[ptr]=0;
*val=atoi(intstr);
return ptr;
}



/*******************************************************************************
* str - ������ ��� ������ ���������									*
* ch  - ����������� ����������										*
* num - ����� �������� ���������										*
* val - ������ �� ������ ��� �������� �������� ��������� (����� char)			*
* return - �������� �� ������ ������, �� �������� ���������� ������� ��������	*
* ���� �������� �� ������, ������������ 0, ���� ������ �� ������ ������		*
*******************************************************************************/
int Parser_GetParamHex(const char *str, char ch, char num, short *val)
{
char ptr;
int pos;
char ret_val=0;
pos=Parser_FindPos(str, ch, num);
if (pos==-1) return 0;
if (str[pos]==ch)
	{
	val=0;
	return 0;
	}

for (ptr=0; ptr<11; ptr++)
	{
	if ((str[pos]==ch)||(str[pos]==0)) break;
	/*if (((str[pos]>'9')||(str[pos]<'0'))&&	//���� ��� �� �����
	    ((str[pos]>'F')||(str[pos]<'A'))&&	//� �� ������ A...F
	    ((str[pos]>'f')||(str[pos]<'a'))&&	//� �� ������ a...f
	    (str[pos]!=' ')) return 0;		//� �� ������, �� ������*/
	if ((str[pos]>='0')&&(str[pos]<='9')) ret_val=(ret_val<<4)|(str[pos]-'0');	//���� ��� �����
	else if ((str[pos]>='A')&&(str[pos]<='F')) ret_val=(ret_val<<4)|(str[pos]-'A'+10);	//��� ������ A...F
	else if ((str[pos]>='a')&&(str[pos]<='f')) ret_val=(ret_val<<4)|(str[pos]-'a'+10);	//��� ������ a...f
	else if (str[pos]==' ') break;
	else return 0;
	pos++;
	}
*val=ret_val;
return pos;
}



/*******************************************************************************
* str - ������ ��� ������ ���������									*
* ch  - ����������� ����������										*
* num - ����� �������� ���������										*
* val - ������ �� ������ ��� �������� �������� ��������� (������)			*
* return - �������� �� ������ ������, �� �������� ���������� ������� ��������	*
* ���� �������� �� ������, ������������ 0, ���� ������ �� ������ ������		*
*******************************************************************************/
int Parser_GetParam(const char *str, char ch, char num, char *val)
{
char ptr;
int pos;
pos=Parser_FindPos(str, ch, num);
if ((pos==-1)||(str[pos]==ch))
	{
	val[0]=0;
	return 0;
	}

int max_str_len;
max_str_len=strlen(str)-pos;
for (ptr=0; ptr<max_str_len; ptr++)
	{
	if ( (str[pos]== ch)||(str[pos] == 0)) break;
	val[ptr]=str[pos++];
	}
val[ptr]=0;

return ptr;
}







void Parser_write(uint8_t b)
{

}




/*******************************************************************************
* ��������������� ����� ������
* str - ������������ ������
* format - ������ �������
* ��������� ��� ������:
* - %s - ��������� ������
* - %i, %ni - ��������� ����� (int), n - ���������� ������ ����� �������
* - %l - ��������� ����� (long)
* - %nL - ��������� ����� (long) c n ����������� ������
* - %h, %nh, %H, %nH,  - ��������� ����� � HEX, n - ���������� ����
* - %� - ��������� ������ (char)
*******************************************************************************/
void Parser_BufPrintf(char *str, const char *format, ...)
{
va_list list;

int pos_str=0;
int pos_format=0;
int n=0;

va_start(list, format);

while(1)
	{
	if (format[pos_format]==0) break;
	if (format[pos_format]!='%') str[pos_str++]=format[pos_format++];
	else
		{
		pos_format++;
		if (format[pos_format]=='%') str[pos_str++]=format[pos_format++];
		//���� � ������� ������� ������
		else if (format[pos_format]=='s')
			{
			char *pp;
			pp = va_arg(list, char *);
			for (n=0; n<strlen(pp); n++) if (pp[n]) str[pos_str++]=pp[n];
			pos_format++;
			}
		//����� �����
		else if (format[pos_format]=='i')
			{
			int p;
			p = va_arg(list, int);
			itoa(p, &str[pos_str]);
			pos_str=strlen(str);
			pos_format++;
			}
		//����� �������� �����
		else if ((format[pos_format]>=0x30)&&(format[pos_format]<=0x39)&&(format[pos_format+1]=='i'))
			{
			int p;
			p = va_arg(list, int);
			itoaf(p, &str[pos_str], (format[pos_format]&0x0f));
			pos_str=strlen(str);
			pos_format+=2;
			}
		//����� ����� LONG
		else if (format[pos_format]=='l')
			{
			long p;
			p = va_arg(list, long);
			ltoa(p, &str[pos_str]);
			pos_str=strlen(str);
			pos_format++;
			}
		//����� ����� LONG � ����������� ������
		else if ((format[pos_format]>=0x30)&&(format[pos_format]<=0x39)&&(format[pos_format+1]=='L'))
			{
			long p;
			p = va_arg(list, long);
			ltoa_null(p, &str[pos_str], (format[pos_format]&0x0f));
			pos_str=strlen(str);
			pos_format+=2;
			}
		//����� ����� � HEX
		else if ((format[pos_format]=='h')||(format[pos_format]=='H'))
			{
			char p;
			char tmpstr[9];
			p = va_arg(list, char);
			for (uint8_t i=0; i<8; i++)
				{
				if ((p&0x0F)>=10) tmpstr[i]=(p&0x0F)-10+((format[pos_format]=='h')?'a':'A');
				else tmpstr[i]=(p&0x0F)+'0';
				p/=0x10;
				if (p==0) break;
				}
			reverse(tmpstr);
			strcpy(&str[pos_str], tmpstr);
			pos_str=strlen(str);
			pos_format++;
			}
		//����� ����� � HEX � ����������� ������
		else if ((format[pos_format]>=0x30)&&(format[pos_format]<=0x39)&&((format[pos_format+1]=='h')||(format[pos_format+1]=='H')))
			{
			long p;
			char tmpstr[9]={0};
			char num_col=format[pos_format]&0x0f;
			if (num_col<=2) p = va_arg(list, char);
			else if (num_col<=4) p = va_arg(list, short);
			else if (num_col<=8) p = va_arg(list, long);
			//else p = va_arg(list, long long);
			else num_col=0;
			for (uint8_t i=0; i<num_col; i++)
				{
				if ((p&0x0F)>=10) tmpstr[i]=(p&0x0F)-10+((format[pos_format+1]=='h')?'a':'A');
				else tmpstr[i]=(p&0x0F)+'0';
				p/=0x10;
				}
			reverse(tmpstr);
			strcpy(&str[pos_str], tmpstr);
			pos_str=strlen(str);
			pos_format+=2;
			}
		//����� �������
		else if (format[pos_format]=='c')
			{
			char p;
			p = va_arg(list, char);
			str[pos_str++]=p;
			pos_format++;
			}
		}
	}
str[pos_str]=0;

va_end(list); 
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
void itoa_dot(int n, char s[], int l)
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
