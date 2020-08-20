
#include <string>
#include <stdarg.h>

#include "Parser.h"



Parser parser;



/*******************************************************************************
* str - ������ ��� ������ ������� ���������								*
* ch  - ����������� ����������										*
* num - ����� �������� ���������										*
* return - �������� �� ������ ������, �� �������� ���������� ������� ��������	*
* ���� �������� �� ������, ������������ -1
*******************************************************************************/
int Parser::FindPos(const char *str, char ch, char num)
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
int Parser::GetParam(const char *str, char ch, char num, char &val)
{
int pos;
pos=FindPos(str, ch, num);
if (pos==-1) return 0;
if (str[pos]!=ch) val=str[pos]; else val=0;
return val;
}



/*******************************************************************************
* str - ������ ��� ������ ���������									*
* ch  - ����������� ����������										*
* num - ����� �������� ���������										*
* val - ������ �� ������ ��� �������� �������� ��������� (����� int)			*
* return - �������� �� ������ ������, �� �������� ���������� ������� ��������	*
* ���� �������� �� ������, ������������ 0, ���� ������ �� ������ ������		*
*******************************************************************************/
int Parser::GetParam(const char *str, char ch, char num, int &val)
{
char intstr[11];
char ptr;
int pos;
pos=FindPos(str, ch, num);
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
val=atoi(intstr);
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
int Parser::GetParam(const char *str, char ch, char num, long &val)
{
char intstr[11];
char ptr;
int pos;
pos=FindPos(str, ch, num);
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
val=atoi(intstr);
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
int Parser::GetParam(const char *str, char ch, char num, short &val)
{
char intstr[11];
char ptr;
int pos;
pos=FindPos(str, ch, num);
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
val=atoi(intstr);
return ptr;
}



/*******************************************************************************
* str - ������ ��� ������ ���������									*
* ch  - ����������� ����������										*
* num - ����� �������� ���������										*
* val - ������ �� ������ ��� �������� �������� ��������� (������)			*
* return - �������� �� ������ ������, �� �������� ���������� ������� ��������	*
* ���� �������� �� ������, ������������ 0, ���� ������ �� ������ ������		*
*******************************************************************************/
int Parser::GetParam(const char *str, char ch, char num, char *val)
{
char ptr;
int pos;
pos=FindPos(str, ch, num);
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







void Parser::write(uint8_t b)
{

}




/*******************************************************************************
* ��������������� ����� ������
* str - ������������ ������
* format - ������ �������
* ��������� ��� ������:
* - %s - ��������� ������
* - %i, %ni - ��������� ����� (int), n - ���������� ������ ����� �������
* - %� - ��������� ������ (char)
*******************************************************************************/
void Parser::BufPrintf(char *str, const char *format ...)
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
			itoa(p, &str[pos_str], (format[pos_format]&0x0f));
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