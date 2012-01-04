/************************************************************************/
/* function: 处理空白字符和空白行
/* parameter:
/* author:
/* time:
/************************************************************************/
int GetRealString(char *pbuf)
{
	int len = strlen(pbuf) - 1;
	while (len > 0 && (pbuf[len] == (char)0x0d || pbuf[len] == (char)0x0a || pbuf[len] == ' ' || pbuf[len] == '\t')) {
		len--;
	}

	if (len < 0) {
		*pbuf = '\0';
		return len;
	}
	pbuf[len+1] = '\0';
	return len + 1;
}

/************************************************************************/
/* function: 重新修改strcoll字符串比较函数
/* parameter:
/* author:
/* time:
/************************************************************************/
int strcoll(const void *s1, const void *s2) 
{
	char *c_s1 = (char *)s1;
	char *c_s2 = (char *)s2;
	while (*c_s1 == *c_s2++)
	{
		if (*c_s1++ == '\0') {
			return 0;
		}
	}

	return *c_s1 - *--c_s2;
}