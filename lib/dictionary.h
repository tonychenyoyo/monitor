#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_


   								
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


typedef struct _dictionary_ {
	int				n ;		/** Number of entries in dictionary */
	int				size ;	/** Storage size */
	char 		**	val ;	/** List of string values */
	char 		**  key ;	/** List of string keys */
	unsigned	 *	hash ;	/** List of hash values for keys */
} dictionary ;


//计算KEY的哈希值
unsigned dictionary_hash(char * key);
//创建一个字典
dictionary * dictionary_new(int size);

void dictionary_del(dictionary * vd);

//获取键值对
char * dictionary_get(dictionary * d, char * key, char * def);

//设置键值对
int dictionary_set(dictionary * vd, char * key, char * val);

void dictionary_unset(dictionary * d, char * key);

//导出字典里面的键值对到文件或是标准输出
void dictionary_dump(dictionary * d, FILE * out);

#endif
