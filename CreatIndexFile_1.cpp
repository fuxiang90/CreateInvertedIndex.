#include <stdio.h>
//#include <direct.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <locale.h>
//#include <io.h>

#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#define BUF_MAX_LEN 1024
#define FILE_MAX_NUM 1000
#define FILENAME_MAX_LEN 64
#define ABSPATH_MAX_LEN 128
#define WORD_MAX_LEN 64
#define WORD_MAX_NUM 20000
#define ITEM_MAX_NUM 32
#define TABLE_SIZE  200000

//#pragma once
//#include "CreateIndexFile.h"

unsigned long cryptTable[0x100];
int	HashATable[TABLE_SIZE];
int	HashBTable[TABLE_SIZE];
//int HashValue[5];

typedef struct table
{
    char bExists;
};
struct table HashTable[TABLE_SIZE] = {0};

typedef struct key_node
{
    char *pkey;		// 关键词实体
    int count;      // 关键词出现次数
    int pos;        // 关键词在hash表中位置
    struct doc_node *next;  // 指向文档结点
} KEYNODE, *key_list;

key_list key_array[TABLE_SIZE] = {0};

typedef struct doc_node
{
    char id[WORD_MAX_LEN];
    int classOne;
    char classTwo[WORD_MAX_LEN];
    int classThree;
    char time[WORD_MAX_LEN];
    char md5[WORD_MAX_LEN];
    int weight;
    struct doc_node *next;
} DOCNODE, *doc_list;

typedef struct hash_value
{
    unsigned int nHash;
    unsigned int nHashA;
    unsigned int nHashB;
    unsigned int nHashStart;
    unsigned int nHashPos;
} HASHVALUE;

/************************************************************************/
/* function:
/* parameter:
/* author:
/* time:
/************************************************************************/
void PrepareCryptTable()
{
    unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;

    for(index1 = 0; index1 <0x100; index1++)
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
unsigned long HashString(const char *lpszkeyName, unsigned long dwHashType)
{
    unsigned char *key  = (unsigned char *)lpszkeyName;
    unsigned long seed1 = 0x7FED7FED;
    unsigned long seed2 = 0xEEEEEEEE;
    int ch;

    while(*key != 0)
    {
        ch = *key++;
        seed1 = cryptTable[(dwHashType<<8) + ch] ^ (seed1 + seed2);
        seed2 = ch + seed1 + seed2 + (seed2<<5) + 3;
    }
    return seed1;
}

/************************************************************************/
/* function:
/* parameter:
/* author:
/* time:
/************************************************************************/
void InitHashValue(const char *str, HASHVALUE *hashvalue)
{
    const int HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;
    hashvalue->nHash = HashString(str, HASH_OFFSET);
    hashvalue->nHashA = HashString(str, HASH_A);
    hashvalue->nHashB = HashString(str, HASH_B);
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
        if (HashATable[nHashPos] == nHashA && HashBTable[nHashPos] == nHashB)
        {
            break;
        }
        else
        {
            nHashPos = (nHashPos + 1) % TABLE_SIZE;
        }

        if (nHashPos == nHashStart)
        {
            break;
        }
    }

    if (key_array[nHashPos] && strlen(key_array[nHashPos]->pkey))
    {
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
        if(key_array[nHashPos] && strlen(key_array[nHashPos]->pkey))
        {
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
        if(key_array[nHashPos] == NULL)
        {
            printf("10000 EMS ERROR !!!!\n");
            return 0;
        }

        key_array[nHashPos]->pkey = (char *)malloc(len+1);
        if(key_array[nHashPos]->pkey == NULL)
        {
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

//#pragma once
//#include "HashSearch.h"
/************************************************************************/
/* function: 得到目录下所有文件名
/* parameter:
/* author:
/* time:

/* 这个函数 应该不处理子目录 付翔 */
/************************************************************************/
int GetFileName(char filename[][FILENAME_MAX_LEN])
{
    int filenum = 0;
    char *dir = "./12";

    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if( (dp = opendir(dir))  == NULL )
    {
        fprintf(stderr, "cannot open directory: %s\n", dir);
        return -1;
    }

    chdir(dir);
    while( (entry = readdir(dp)) != NULL)
    {
        if( S_ISDIR(statbuf.st_mode) )
        {
//            if( strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0 )
//                continue;
        }
        else
        {
            printf("%s\n",  entry->d_name);
            strcpy(filename[filenum++], entry->d_name);

        }
    }

    closedir(dp);
//	_finddata_t file;
//	long handle;
//	int filenum = 0;
//	if ((handle = _findfirst("12\\ky_res.*", &file)) == -1) {
//		printf("File Not Found\n");
//	} else {
//		do {
//			strcpy(filename[filenum++], file.name);
//		} while (!_findnext(handle, &file));
//	}
//	_findclose(handle);
    return filenum;
}

/************************************************************************/
/* function: 以读方式打开文件，如果成功返回文件指针
/* parameter:
/* author:
/* time:
/************************************************************************/
FILE* OpenReadFile(int index, char filename[][FILENAME_MAX_LEN])
{
    char *abspath = (char *)malloc(ABSPATH_MAX_LEN);
    char *dir = "./12";

    chdir(dir); //进入某个目录
//    char *abspath;
//    char dirpath[] = {"12\\"};
//    abspath = (char *)malloc(ABSPATH_MAX_LEN);
//    //strcpy(abspath, ABSPATH_MAX_LEN, dirpath);
//    strncpy(abspath, dirpath,ABSPATH_MAX_LEN);
//    strncat(abspath, filename[index], FILENAME_MAX_LEN);
    strncat(abspath, filename[index], FILENAME_MAX_LEN);
    FILE *fp = fopen (abspath, "r");
    if (fp == NULL)
    {
        printf("open read file error!\n");
        return NULL;
    }
    else
    {
        return fp;
    }
}

/************************************************************************/
/* function: 以写方式打开文件，如果成功返回文件指针
/* parameter:
/* author:
/* time:
/************************************************************************/
FILE* OpenWriteFile(const char *in_file_path)
{
    if (in_file_path == NULL)
    {
        printf("output file path error!\n");
        return NULL;
    }

    FILE *fp = fopen(in_file_path, "w+");
    if (fp == NULL)
    {
        printf("open write file error!\n");
    }
    return fp;
}

/************************************************************************/
/* function: 将索引信息写入文档
/* parameter:
/* author:
/* time:
/************************************************************************/
void WriteFile(key_list keylist, int num, FILE *fwp, int *count)
{
    doc_list infolist = keylist->next;
    for (int i = 0; i <= num; i++)
    {
        fprintf(fwp, "%s %d %s %d %s %d %s %d#####", infolist->time, count[i], infolist->id, infolist->classOne,
                infolist->classTwo, infolist->classThree, infolist->md5, infolist->weight);
        infolist = infolist->next;
        for (int k = 0; k < count[i] - 1; k++)
        {
            fprintf(fwp, "%s %d %s %d %s %d#####", infolist->id, infolist->classOne,
                    infolist->classTwo, infolist->classThree, infolist->md5, infolist->weight);
            infolist = infolist->next;
        }
        fprintf(fwp, "\n", NULL);
    }
}

/************************************************************************/
/* function: 处理空白字符和空白行
/* parameter:
/* author:
/* time:
/************************************************************************/
int GetRealString(char *pbuf)
{
    int len = strlen(pbuf) - 1;
    while (len > 0 && (pbuf[len] == (char)0x0d || pbuf[len] == (char)0x0a || pbuf[len] == ' ' || pbuf[len] == '\t'))
    {
        len--;
    }

    if (len < 0)
    {
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
        if (*c_s1++ == '\0')
        {
            return 0;
        }
    }

    return *c_s1 - *--c_s2;
}

//#pragma once
//#include "CreateIndexFile.h"
//#include "FileProcess.h"
//#include "HashSearch.h"
//#include "StringProcess.h"

static char filename[FILE_MAX_NUM][FILENAME_MAX_LEN] = {0};
static char words[WORD_MAX_NUM][WORD_MAX_LEN] = {0};
static char items[ITEM_MAX_NUM][WORD_MAX_LEN] = {0};

/************************************************************************/
/* function: 从行缓冲中得到各项信息，将其写入items数组
/* parameter:
/* author:
/* time:
/************************************************************************/
void GetItems(char *&move, int &count, int &wordnum)
{
    char *front = move;
    bool flag = false;
    int len;
    move = strstr(move, "#####");
    if (*(move + 5) == '#')
    {
        flag = true;
    }

    if (move)
    {
        len = move - front;
        strncpy(items[count], front, len);
    }
    items[count][len] = '\0';
    count++;

    if (flag)
    {
        move = move + 10;
    }
    else
    {
        move = move + 5;
    }
}

/************************************************************************/
/* function: 保存关键字相应的文档内容
/* parameter:
/* author:
/* time:
/************************************************************************/
doc_list SaveItems()
{
    doc_list infolist = (doc_list) malloc(sizeof(DOCNODE));
    strcpy(infolist->id, items[0]);
    infolist->classOne = atoi(items[1]);
    strcpy(infolist->classTwo, items[2]);
    infolist->classThree = atoi(items[3]);
    strcpy(infolist->time, items[4]);
    strcpy(infolist->md5, items[5]);
    infolist->weight = atoi(items[6]);
    return infolist;
}

/************************************************************************/
/* function: 对换相邻节点的内容
/* parameter:
/* author:
/* time:
/************************************************************************/
void SwapDocNode(doc_list p)
{
    doc_list temp = (doc_list) malloc(sizeof(DOCNODE));
    strcpy(temp->id, p->id);
    temp->classOne	= p->classOne;
    strcpy(temp->classTwo, p->classTwo);
    temp->classThree= p->classThree;
    strcpy(temp->time, p->time);
    strcpy(temp->md5, p->md5);
    temp->weight	= p->weight;

    strcpy(p->id, p->next->id);
    p->classOne     = p->next->classOne;
    strcpy(p->classTwo, p->next->classTwo);
    p->classThree   = p->next->classThree;
    strcpy(p->time, p->next->time);
    strcpy(p->md5, p->next->md5);
    p->weight       = p->next->weight;


    strcpy(p->next->id, temp->id);
    p->next->classOne  = temp->classOne;
    strcpy(p->next->classTwo, temp->classTwo);
    p->next->classThree= temp->classThree;
    strcpy(p->next->time, temp->time);
    strcpy(p->next->md5, temp->md5);
    p->next->weight    = temp->weight;
}

/************************************************************************/
/* function: 对链表进行冒泡排序
/* parameter:
/* author:
/* time:
/************************************************************************/
void ListSort(key_list keylist)
{
    doc_list p = keylist->next;
    doc_list final = NULL;
    while (true)
    {
        bool isfinish = true;
        while (p->next != final)
        {
            if (strcmp(p->time, p->next->time) < 0)
            {
                SwapDocNode(p);
                isfinish = false;
            }
            p = p->next;
        }
        final = p;
        p = keylist->next;
        if (isfinish || p->next == final)
        {
            break;
        }
    }
}

int main()
{
    key_list keylist;
    char *pbuf, *move;
    int filenum = GetFileName(filename);
    FILE *frp;
    pbuf = (char *)malloc(BUF_MAX_LEN);
    memset(pbuf, 0, BUF_MAX_LEN);
    HASHVALUE *hashvalue = (HASHVALUE *) malloc(sizeof(HASHVALUE));

    FILE *fwp = OpenWriteFile("index.txt");
    if (fwp == NULL)
    {
        return 0;
    }

    PrepareCryptTable();

    int wordnum = 0;
    for (int i = 0; i < filenum; i++)
    {
        frp = OpenReadFile(i, filename);
        if (frp == NULL)
        {
            system("pause");
            return 0;
        }

        // 每次读取一行处理
        while (fgets(pbuf, BUF_MAX_LEN, frp))
        {
            int count = 0;
            move = pbuf;
            if (GetRealString(pbuf) <= 1)
                continue;

            while (move != NULL)
            {
                // 找到第一个非'#'的字符
                while (*move == '#')
                    move++;

                if (!strcmp(move, ""))
                    break;

                GetItems(move, count, wordnum);
            }

            for (int i = 7; i < count; i++)
            {
                // 将关键字对应的文档内容加入文档结点链表中
                // 如果关键字第一次出现，则将其加入hash表
                InitHashValue(items[i], hashvalue);
                if (keylist = SearchByString(items[i], hashvalue))
                {
                    doc_list infolist = SaveItems();
                    infolist->next = keylist->next;
                    keylist->count++;
                    keylist->next = infolist;
                }
                else
                {
                    int pos = InsertString(items[i], hashvalue);
                    keylist = key_array[pos];
                    doc_list infolist = SaveItems();
                    infolist->next = NULL;
                    keylist->next = infolist;
                    if (pos != -1)
                    {
                        strcpy(words[wordnum++], items[i]);
                    }
                }
            }
        }
    }

    // 通过快排对关键字进行排序
    qsort(words, WORD_MAX_NUM, WORD_MAX_LEN, strcoll);

    // 遍历关键字数组，将关键字及其对应的文档内容写入文件中
    int rownum = 1;
    for (int i = 0; i < WORD_MAX_NUM; i++)
    {
        InitHashValue(words[i], hashvalue);
        keylist = SearchByString(words[i], hashvalue);
        if (keylist != NULL)
        {
            doc_list infolist = keylist->next;

            char date[9];
            // 截取年月日
            for (int j = 0; j < keylist->count; j++)
            {
                strncpy(date, infolist->time, 8);
                date[8] = '\0';
                strncpy(infolist->time, date, 9);
                infolist = infolist->next;
            }

            // 对链表根据时间进行排序
            ListSort(keylist);

            infolist = keylist->next;
            int *count = new int[WORD_MAX_NUM];
            memset(count, 0, WORD_MAX_NUM);
            strcpy(date, infolist->time);
            int num = 0;
            // 得到单个日期的文档数目
            for (int j = 0; j < keylist->count; j++)
            {
                if (strcmp(date, infolist->time) == 0)
                {
                    count[num]++;
                }
                else
                {
                    count[++num]++;
                }
                strcpy(date, infolist->time);
                infolist = infolist->next;
            }
            fprintf(fwp, "%s %d %d\n", words[i], num + 1, rownum);
            WriteFile(keylist, num, fwp, count);
            rownum++;
        }
    }

    free(pbuf);
    free(hashvalue);
    fclose(frp);
    fclose(fwp);
    //system("pause");
    return 0;
}
