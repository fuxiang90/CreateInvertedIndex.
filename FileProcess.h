#ifndef OS_LINUX
#pragma once
#endif

#include "HashSearch.h"

#ifdef OS_LINUX
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

#else
/************************************************************************/
/* function: 得到目录下所有文件名
/* parameter:
/* author:
/* time:
/************************************************************************/

int GetFileName(char filename[][FILENAME_MAX_LEN])
{
    _finddata_t file;
    long handle;
    int filenum = 0;
    if ((handle = _findfirst("12\\ky_res.*", &file)) == -1)
    {
        printf("File Not Found\n");
    }
    else
    {
        do
        {
            strcpy_s(filename[filenum++], file.name);
        }
        while (!_findnext(handle, &file));
    }
    _findclose(handle);
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
    char *abspath;
    char dirpath[] = {"12\\"};
    abspath = (char *)malloc(ABSPATH_MAX_LEN);
    strcpy_s(abspath, ABSPATH_MAX_LEN, dirpath);
    strcat_s(abspath, FILENAME_MAX_LEN, filename[index]);

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

#endif
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
