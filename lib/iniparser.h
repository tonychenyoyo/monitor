#ifndef _INIPARSER_H_
#define _INIPARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

#define iniparser_getstr(d, k)  iniparser_getstring(d, k, NULL)
#define iniparser_setstr        iniparser_setstring


int iniparser_getnsec(dictionary * d);

char * iniparser_getsecname(dictionary * d, int n);

void iniparser_dump_ini(dictionary * d, FILE * f);

void iniparser_dump(dictionary * d, FILE * f);

char * iniparser_getstring(dictionary * d, const char * key, char * def);

int iniparser_getint(dictionary * d, const char * key, int notfound);

double iniparser_getdouble(dictionary * d, char * key, double notfound);

int iniparser_getboolean(dictionary * d, const char * key, int notfound);

int iniparser_setstring(dictionary * ini, char * entry, char * val);

void iniparser_unset(dictionary * ini, char * entry);

int iniparser_find_entry(dictionary * ini, char * entry) ;

dictionary * iniparser_load(const char * ininame);

void iniparser_freedict(dictionary * d);

#endif
