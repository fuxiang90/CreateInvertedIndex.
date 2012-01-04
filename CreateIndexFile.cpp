/*
*如要在linux 下编译 请加上这句#define OS_LINUX
*如果在windows 下 请注释掉
*/
#define OS_LINUX

#ifndef OS_LINUX
#pragma once

#else

    #define strcpy_s strcpy
    #define strncpy_s strncpy

#endif

#include "CreateIndexFile.h"
#include "FileProcess.h"
#include "HashSearch.h"
#include "StringProcess.h"

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
	if (*(move + 5) == '#') {
		flag = true;
	}

	if (move) {
		len = move - front;
		strncpy(items[count], front, len);
	}
	items[count][len] = '\0';
	count++;

	if (flag) {
		move = move + 10;
	} else {
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
	char date[9];
	doc_list infolist = (doc_list) malloc(sizeof(DOCNODE));
	strcpy_s(infolist->id, items[0]);
	infolist->classOne = atoi(items[1]);
	strcpy_s(infolist->classTwo, items[2]);
	infolist->classThree = atoi(items[3]);
	strncpy_s(date, items[4], 8);
	date[8] = '\0';
	strncpy_s(infolist->time, date, 9);
	strcpy_s(infolist->md5, items[5]);
	infolist->weight = atoi(items[6]);
	return infolist;
}

int main()
{
	key_list keynode;
	char *pbuf, *move;
	int filenum = GetFileName(filename);
	FILE *frp;
	pbuf = (char *)malloc(BUF_MAX_LEN);
	memset(pbuf, 0, BUF_MAX_LEN);

	HASHVALUE *hashvalue = (HASHVALUE *)malloc(sizeof(HASHVALUE));

	FILE *fwp = OpenWriteFile("index.txt");
	if (fwp == NULL) {
		return 0;
	}

	PrepareCryptTable();

	int wordnum = 0;
	for (int i = 0; i < filenum; i++)
	{
		frp = OpenReadFile(i, filename);
		if (frp == NULL) {
			break;
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

			for (int i = 7; i < count; i++) {
				// 将关键字对应的文档内容加入文档结点链表中
				InitHashValue(items[i], hashvalue);
				if (keynode = SearchByString(items[i], hashvalue))
				{
					doc_list infonode = SaveItems();
					doc_list p = keynode->next;
					// 根据时间由早到晚排序
					if (strcmp(infonode->time, p->time) > 0)
					{
						//考虑infonode插入keynode后的情况
						infonode->next = p;
						keynode->next = infonode;
					} else {
						//考虑其他情况
						doc_list pre = p;
						p = p->next;
						while (p)
						{
							if (strcmp(infonode->time, p->time) < 0)
							{
								p = p->next;
								pre = pre->next;
							}
							else
							{
								break;
							}
						}
						infonode->next = p;
						pre->next = infonode;
					}
					keynode->count++;
				}
				else
					// 如果关键字第一次出现，则将其加入hash表
				{
					int pos = InsertString(items[i], hashvalue);
					keynode = key_array[pos];
					doc_list infolist = SaveItems();
					infolist->next = NULL;
					keynode->next = infolist;
					if (pos != -1) {
						strcpy_s(words[wordnum++], items[i]);
					}
				}
			}
		}
	}

	// 通过快排对关键字进行排序
	qsort(words, WORD_MAX_NUM, WORD_MAX_LEN, strcoll);

	// 遍历关键字数组，将关键字及其对应的文档内容写入文件中
	int rownum = 1;
	for (int i = 0; i < WORD_MAX_NUM; i++) {
		InitHashValue(words[i], hashvalue);
		keynode = SearchByString(words[i], hashvalue);
		if (keynode != NULL) {
			doc_list infolist = keynode->next;
			char date[9];
			int *count = new int[WORD_MAX_NUM];
			memset(count, 0, WORD_MAX_NUM);
			strcpy_s(date, infolist->time);
			int num = 0;
			// 得到单个日期的文档数目
			for (int j = 0; j < keynode->count; j++)
			{
				if (strcmp(date, infolist->time) == 0) {
					count[num]++;
				} else {
					count[++num]++;
				}
				strcpy_s(date, infolist->time);
				infolist = infolist->next;
			}
			fprintf(fwp, "%s %d %d\n", words[i], num + 1, rownum);
			WriteFile(keynode, num, fwp, count);
			rownum++;
		}
	}

	free(pbuf);
	free(hashvalue);
	fclose(frp);
	fclose(fwp);
	#ifndef OS_LINUX
	system("pause");
	#endif
	return 0;
}
