
#include <kernel.h>


int k_strlen(const char* str)
{
	int l = 0;
	while (*str++ != '\0') ++l;
	return (l);
}

void* k_memcpy(void* dst, const void* src, int len)
{
	char* cdst = (char*) dst;
	char* csrc = (char*) src;
	while (len > 0) {
		*cdst++ = *csrc++;
		len--;
	}
	return (dst);
}

int k_memcmp(const void* b1, const void* b2, int len)
{
	unsigned char* c1 = (unsigned char*) b1;
	unsigned char* c2 = (unsigned char*) b2;
	while (len > 0) {
		int d = *c1++ - *c2++;
		if (d != 0) return (d);
		len--;
	}

	return (0);
}

int k_strcmp(const char* str1, const char* str2)
{		
	while(*str1 == *str2)
	{	
		if(*str1 == '\0' || *str2 == '\0'){
			break;
		}
		str1++;
		str2++;
	}	
	if(*str1 == '\0' && *str2 == '\0'){
		return (1);
	}
	else{
		return (0);
	}
}

int atoi(const char* str)
{
	int num = 0;
	BOOL flag = FALSE;
	while (*str != '\0')
	{
		if (*str >= '0' && *str <= '9')
		{
			num = num * 10 + *str - '0';
			flag = TRUE;
		}
		else if (!flag && (*str == ' ' || *str == '\t'))
		{
			continue;
		}
		else
		{
			break;
		}
		str++;
	}
	return num;
}

BOOL is_num(const char *str)
{
	if (*str >= '0' && *str <= '9')
		return TRUE;
	else 
		return FALSE;
}