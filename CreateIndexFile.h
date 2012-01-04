#include <stdio.h>
//#include <direct.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <locale.h>
//#include <io.h>



#ifdef OS_LINUX
    #include <unistd.h>
    #include <dirent.h>
    #include <sys/stat.h>
#else
    #include <io.h>
    #include <direct.h>
#endif


/*
#define BUF_MAX_LEN 1024
#define FILE_MAX_NUM 1000
#define FILENAME_MAX_LEN 64
#define ABSPATH_MAX_LEN 128
#define WORD_MAX_LEN 64
#define WORD_MAX_NUM 20000
#define ITEM_MAX_NUM 32
#define TABLE_SIZE  200000*/

#define BUF_MAX_LEN 1024
#define FILE_MAX_NUM 1000
#define FILENAME_MAX_LEN 64
#define ABSPATH_MAX_LEN 128
#define WORD_MAX_LEN 64
#define WORD_MAX_NUM 10000
#define ITEM_MAX_NUM 32
#define TABLE_SIZE  200000

