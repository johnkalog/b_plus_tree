#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "AM.h"
#include "bf.h"
#include "defn.h"

#define AM_MAX_OPEN_FILES 20
#define AM_MAXSCANS 20

typedef struct DataIndex {
		int filedesc;
		char attrType1;
		int attrLength1;
    char attrType2;
    int attrLength2;
    char filename[20];
    int numRoot;
} DataIndex;

struct DataIndex **OpenIndex;

typedef struct IndexScan{
		int filedesc;
		int op;
		int blocknum;
		int indexpos;
		int fD;

}	IndexScan;

struct IndexScan **OpenIndexScan;


struct IndexScan **OpenIndexScan;

int compare_attr1(void* , void* ,int );

void change_key(void** ,void* ,int ,int );

void InsertWithSorting(int ,char *,void *,void *);

void InsertForIndexWithSorting(int ,char *,void *,void *);

void Root_Creation(int ,void *,void *);

void break_Index(int ,char *, int ,void *,void* );

void break_it(int ,char *, int ,int ,void *,void *);

int SearchPosition(int ,int ,void *);

void Insert(int ,int , void *, void *, int ,int );


