#pragma once
#include "CreateIndexFile.h"

unsigned long cryptTable[0x500];
int	HashATable[TABLE_SIZE];
int	HashBTable[TABLE_SIZE];
int HashValue[5] = {0};

typedef struct table { 
	char bExists;
};
struct table HashTable[TABLE_SIZE] = {0};

typedef struct key_node {
	char *pkey;		// 关键词实体
	int count;      // 关键词出现次数
	int pos;        // 关键词在hash表中位置
	struct doc_node *next;  // 指向文档结点
}KEYNODE, *key_list;

key_list key_array[TABLE_SIZE];

typedef struct doc_node {
	char id[WORD_MAX_LEN];
	int classOne;
	char classTwo[WORD_MAX_LEN];
	int classThree;
	char time[WORD_MAX_LEN];
	char md5[WORD_MAX_LEN];
	int weight;
	struct doc_node *next;
}DOCNODE, *doc_list;

typedef struct hash_value {
	unsigned int nHash;
	unsigned int nHashA;
	unsigned int nHashB;
	unsigned int nHashStart;
	unsigned int nHashPos;
}HASHVALUE;

/************************************************************************/
/* function: 
/* parameter:
/* author:
/* time:
/************************************************************************/
void PrepareCryptTable()
{
	unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;

	for( index1 = 0; index1 <0x100; index1++ )
	{
		for( index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
		{
			unsigned long temp1, temp2;
			seed = (seed * 125 + 3) % 0x2AAAAB;
			temp1 = (seed & 0xFFFF)<<0x10;
			seed = (seed * 125 + 3) % 0x2AAAAB;
			temp2 = (seed & 0xFFFF);
			cryptTable[index2] = ( temp1 | temp2 );
		}
	}
}

/************************************************************************/
/* function: 
/* parameter:
/* author:
/* time:
/************************************************************************/
unsigned long HashString(const char *lpszkeyName, unsigned long dwHashType )
{
	unsigned char *key  = (unsigned char *)lpszkeyName;
	unsigned long seed1 = 0x7FED7FED;
	unsigned long seed2 = 0xEEEEEEEE;
	int ch;

	while( *key != 0 )
	{
		ch = *key++;
		seed1 = cryptTable[(dwHashType<<8) + ch] ^ (seed1 + seed2);
		seed2 = ch + seed1 + seed2 + (seed2<<5) + 3;
	}
	return seed1;
}

void InitHashValue(const char *string_in, HASHVALUE *hashvalue) 
{
	const int HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;
	hashvalue->nHash = HashString(string_in, HASH_OFFSET);
	hashvalue->nHashA = HashString(string_in, HASH_A);
	hashvalue->nHashB = HashString(string_in, HASH_B);
	hashvalue->nHashStart = hashvalue->nHash % TABLE_SIZE;
	hashvalue->nHashPos = hashvalue->nHashStart;
}

/************************************************************************/
/* function: 按关键字查询，如果成功返回hash表中索引位置
/* parameter:
/* author:
/* time:
/************************************************************************/
key_list SearchByString(const char *string_in, HASHVALUE *hashvalue)
{
	unsigned int nHash = hashvalue->nHash;
	unsigned int nHashA = hashvalue->nHashA;
	unsigned int nHashB = hashvalue->nHashB;
	unsigned int nHashStart = hashvalue->nHashStart;
	unsigned int nHashPos = hashvalue->nHashPos;

	while (HashTable[nHashPos].bExists) 
	{
		if (HashATable[nHashPos] == (int) nHashA && HashBTable[nHashPos] == (int) nHashB) {
			break;
		} else {
			nHashPos = (nHashPos + 1) % TABLE_SIZE;
		}

		if (nHashPos == nHashStart) {
			break;
		}
	}

	if( key_array[nHashPos] && strlen(key_array[nHashPos]->pkey)) {
		return key_array[nHashPos];
	} 

	return NULL;
}

/************************************************************************/
/* function: 按索引查询，如果成功返回关键字
/* parameter:
/* author:
/* time:
/************************************************************************/
key_list SearchByIndex(unsigned int nIndex)
{
	unsigned int nHashPos = nIndex;
	if (nIndex < TABLE_SIZE)
	{		
		if(key_array[nHashPos] && strlen(key_array[nHashPos]->pkey)) {
			return key_array[nHashPos];
		}
	}

	return NULL;
}

/************************************************************************/
/* function: 插入关键字，如果成功返回hash值
/* parameter:
/* author:
/* time:
/************************************************************************/
int InsertString(const char *str, HASHVALUE *hashvalue)
{
	unsigned int nHash = hashvalue->nHash;
	unsigned int nHashA = hashvalue->nHashA;
	unsigned int nHashB = hashvalue->nHashB;
	unsigned int nHashStart = hashvalue->nHashStart;
	unsigned int nHashPos = hashvalue->nHashPos;

	while (HashTable[nHashPos].bExists)
	{
		nHashPos = (nHashPos + 1) % TABLE_SIZE;

		if (nHashPos == nHashStart)
			break;
	}

	int len = strlen(str);
	if (!HashTable[nHashPos].bExists && (len < WORD_MAX_LEN))
	{ 
		HashATable[nHashPos] = nHashA;
		HashBTable[nHashPos] = nHashB;

		key_array[nHashPos] = (KEYNODE *) malloc (sizeof(KEYNODE) * 1);
		if(key_array[nHashPos] == NULL) {
			printf("10000 EMS ERROR !!!!\n");
			return 0;
		}

		key_array[nHashPos]->pkey = (char *)malloc(len+1);
		if(key_array[nHashPos]->pkey == NULL) {
			printf("10000 EMS ERROR !!!!\n");
			return 0;
		}

		memset(key_array[nHashPos]->pkey, 0, len+1);
		strncpy(key_array[nHashPos]->pkey, str, len);
		*((key_array[nHashPos]->pkey)+len) = 0;
		key_array[nHashPos]->pos = nHashPos;
		key_array[nHashPos]->count = 1;
		key_array[nHashPos]->next = NULL;
		HashTable[nHashPos].bExists = 1;
		return nHashPos;
	}

	if(HashTable[nHashPos].bExists)
		printf("30000 in the hash table %s !!!\n", str);
	else
		printf("90000 strkey error !!!\n");
	return -1;
}
