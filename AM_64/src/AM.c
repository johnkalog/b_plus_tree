#include "functions.h"

int AM_errno = AME_OK;

void print_data(int ,int );

void AM_Init() {
		OpenIndex = malloc(AM_MAX_OPEN_FILES*sizeof(DataIndex*));     //OpenIndex pinakas 20 deiktwn se struct DataIndex
		OpenIndexScan = malloc(AM_MAXSCANS*sizeof(IndexScan*));
	 	int i;
		for( i=0;i<AM_MAX_OPEN_FILES;i++ ){
	  		OpenIndex[i] = NULL;                       //arxikoi deiktes NULL
		}
		for(i=0;i<AM_MAXSCANS;i++){
			OpenIndexScan[i] = NULL;
		}
		BF_ErrorCode bferror;
		bferror = BF_Init(LRU);                        //arxikopoihsh tou BF epipedou
		if(bferror != BF_OK){
				BF_PrintError(bferror);
        return;
    }
}

int AM_CreateIndex(char *fileName,
	               char attrType1,
	               int attrLength1,
	               char attrType2,
	               int attrLength2) {
		BF_ErrorCode bferror;
		bferror = BF_CreateFile(fileName);
		if( bferror != BF_OK ){
				BF_PrintError(bferror);
				return AME_EOF;
		}

    int filedesc;
		char msg[2]="B+";   //xarakthristiko B+ dentrou

		bferror = BF_OpenFile(fileName,&filedesc);
		if( bferror != BF_OK ){
				BF_PrintError(bferror);
				return AME_EOF;
		}

    BF_Block *block;                      //block to mplok me ta metadata
		char *data;
		BF_Block_Init(&block);
		bferror = BF_AllocateBlock(filedesc,block);
		data = BF_Block_GetData(block);
		if( bferror != BF_OK ){
				BF_PrintError(bferror);
				return AME_EOF;
		}

		memcpy(data,msg,sizeof(msg));      //eisagwgh dedomenwn sto block me ta matadata
		data += sizeof(msg);
		memcpy(data,&attrType1,sizeof(attrType1));
		data += sizeof(attrType1);
		memcpy(data,&attrLength1,sizeof(attrLength1));
		data += sizeof(attrLength1);
		memcpy(data,&attrType2,sizeof(attrType2));
		data += sizeof(attrType2);
		memcpy(data,&attrLength2,sizeof(attrLength2));
		data += sizeof(attrLength2);
		int root = -1;
    memcpy(data,&root,sizeof(int));

		BF_Block_SetDirty(block);
		bferror = BF_UnpinBlock(block);
	  if( bferror != BF_OK ){
	  		BF_PrintError(bferror);
	      return AME_EOF;
		}
		BF_CloseFile(filedesc); // kleisimo tou anoixtou arxeiou

    return AME_OK;

}


int AM_DestroyIndex(char *fileName) {


    BF_ErrorCode bf_error;
    int i;
    for ( i=0;i<AM_MAX_OPEN_FILES;i++ ) {
        if( strcmp(OpenIndex[i]->filename,fileName)==0 ) {
					if( remove(fileName) != 0 ){
							printf("Error deleting file with name %s\n",fileName);
							return AME_EOF;
					}
					else
							printf("File with name %s successfully deleted\n",fileName );
							return AME_OK;
        }
    }
  	return AME_INVALID_FILENAME;  //shmainei oti i==AM_MAX_OPEN_FILES
}


int AM_OpenIndex (char *fileName) {

		BF_ErrorCode BfError;

		int fileDesc;
    BfError = BF_OpenFile(fileName,&fileDesc);
    if ( BfError != BF_OK ) {
        BF_PrintError(BfError);
        return AME_EOF;
    }

		int numBlocks;
		BfError = BF_GetBlockCounter(fileDesc,&numBlocks);
    if ( BfError != BF_OK ) {
        BF_PrintError(BfError);
        return AME_EOF;
    }

		BF_Block *block;
    BF_Block_Init(&block);
    if ( BfError != BF_OK ) {
        BF_PrintError(BfError);
        return AME_EOF;
    }

		BfError = BF_GetBlock(fileDesc,0,block);
    if ( BfError != BF_OK ) {
        BF_PrintError(BfError);
        return AME_EOF;
    }

		char* data;
		data = BF_Block_GetData(block);
		if ( strncmp(data,"B+",2) != 0) {          //lathos plhroforia den einai B+ dentro
			printf("NOT B+ but %c%c\n",data[0],data[1]);
			return AME_EOF;
		}

		int numIndex;                              //an uparxei xwros ston pinaka ton anoixtwn arxeiwn numIndex einai h thesi pou tha
		for ( numIndex=0;numIndex<AM_MAX_OPEN_FILES;numIndex++ ) {     //mpei tou arxeiou
			if( OpenIndex[numIndex] == NULL ) {
				break;
			}
		}
		if( numIndex == 20 ) {
			printf("There are 20 opened files, you can't open more\n");
			return AME_NO_FREE_SPACE_FOR_OPEN_FILE;
		}

		OpenIndex[numIndex] = malloc(sizeof(DataIndex));

    OpenIndex[numIndex]->filedesc = (int)fileDesc;     //anathesh timwn sthn domh ston pinaka twn anoiktwn arxeiwn
    OpenIndex[numIndex]->attrType1 = data[2];
    OpenIndex[numIndex]->attrLength1 = (int)data[3];
    OpenIndex[numIndex]->attrType2 = data[3+sizeof(int)];
    OpenIndex[numIndex]->attrLength2 = (int)data[3+sizeof(int)+sizeof(char)];
    strcpy(OpenIndex[numIndex]->filename,fileName);

		OpenIndex[numIndex]->numRoot = (int)data[3+sizeof(int)+sizeof(char)+sizeof(int)];

    DataIndex *x =  OpenIndex[numIndex];

		BfError = BF_UnpinBlock(block);
	  if( BfError != BF_OK ){
	  		BF_PrintError(BfError);
	      return AME_EOF;
		}
		return numIndex;
}


int AM_CloseIndex (int fileDesc) {

    int i;
    BF_ErrorCode bf_error;
		for ( i=0;i<AM_MAXSCANS;i++ ) {
			if ( OpenIndexScan[i]!=NULL ){
				if ( OpenIndexScan[i]->fD==fileDesc){   //den uparxei mporei na kleisei giati uparxei anoikth sarwsh gia auto
					return AME_CAN_NOT_CLOSE_BECAUSE_OPEN_SCAN;
				}
			}
		}
    for ( i=0;i<AM_MAX_OPEN_FILES;i++ ) {
        if( i ==fileDesc ) {
            //den einai anoixto stis sarwseis
            BF_ErrorCode bferror;
            BF_Block *block;
            char* dataBlock;
            BF_Block_Init(&block);


            bferror = BF_GetBlock(OpenIndex[i]->filedesc,0,block);


			if ( bf_error!=BF_OK ){
                BF_PrintError(bf_error);
                return AME_EOF;
            }

            dataBlock = BF_Block_GetData(block);

            dataBlock += sizeof(int)+4*sizeof(char)+sizeof(int);

            int root = OpenIndex[i]->numRoot;

            memcpy(dataBlock,&root,sizeof(int));   //enhmerwsh tou block me ta metadata giati mporei na exei allaksei h riza

            BF_Block_SetDirty(block);
            bferror = BF_UnpinBlock(block);

			if ( bf_error!=BF_OK ){
                BF_PrintError(bf_error);
                return AME_EOF;
            }

            bf_error = BF_CloseFile(OpenIndex[i]->filedesc);
            if ( bf_error!=BF_OK ){
                BF_PrintError(bf_error);
                return AME_EOF;
            }
            free(OpenIndex[i]);          //sbhsimo ths domhs
        }
    }
  return AME_OK;

}

int AM_InsertEntry(int fileDesc, void *value1, void *value2) {

    BF_ErrorCode bferror;

    int totalIndex = (BF_BLOCK_SIZE - sizeof(char) - 3*sizeof(int)) / (sizeof(int) + OpenIndex[fileDesc]->attrLength1);
    int totalData = (BF_BLOCK_SIZE - sizeof(char) - 3*sizeof(int)) / (OpenIndex[fileDesc]->attrLength1 + OpenIndex[fileDesc]->attrLength2); // fixed
				//oi aritmoi autoi upologizoun ton megisto arithmo zeygariwn kleidi,"deikth"(o deikths einai enas int) kai
				//kleidi,dedomenou(2o pedio) gia ta block eurethriou kai ta block fullou se auto to arxeio
				//sto block eurethriou afairoume apo to BF_BLOCK_SIZE enan char pou einai o tupos tou block,enan int pou einai o pateras tou
				//enan int pou einai o arithmos kleidiwn kai enan int pou einai o "deikths" tou prwtou block fullou
				//antoistoixa sto block fullou afairoume apo to BF_BLOCK_SIZE enan char pou einai o tupos tou block,enan int pou einai o
				//pateras tou enan int pou einai o arithmos kleidiwn kai enan int pou einai "deikths sto epomeno block fullou"

    int x;

    if(OpenIndex[fileDesc]->numRoot == -1) {      //an to dentro einai adeio eisagwgh rizas
        Root_Creation(fileDesc,value1,value2);
    }
    else{
						int n = OpenIndex[fileDesc]->numRoot;
						int search_position =	SearchPosition(fileDesc,n,value1);      //anazhthsh theshs gia thn
            Insert(fileDesc,search_position,value1,value2,totalData,totalIndex);    //eisagwgh tou zeugariou kleidi dedomeno
    }
		//printf("----------------------------------------------------print_data----------------------------------------------------\n");
		//int block_num = OpenIndex[fileDesc]->numRoot;
		//print_data(fileDesc,block_num);       //sunarthsh pou emfanizei ta kleidia twn block fullwn apo to prwto mexri to teleutaio
		//printf("-----------------------------------------------end_of_print_data----------------------------------------------------\n");
  return AME_OK;
}


int AM_OpenIndexScan(int fileDesc, int op, void *value) {

	int scanDesc;                              //an uparxei xwros ston pinaka ton anoixtwn arxeiwn numIndex einai h thesi pou tha
	for ( scanDesc=0; scanDesc<AM_MAXSCANS; scanDesc++ ) {     //mpei tou arxeiou
		if( OpenIndexScan[scanDesc] == NULL ) {
			break;
		}
	}
	if( scanDesc == 20 ) {
		printf("There are 20 opened scan files, you can't open more\n");
		return AME_MAXSCANS_ARRAY_FULL;     //o pinakas twn sarwsewn einai gematos
	}


	BF_ErrorCode bferror;
	BF_Block *block;
	char* dataBlock;
	BF_Block_Init(&block);
	bferror = BF_GetBlock(OpenIndex[fileDesc]->filedesc,1,block);	            //kanonika xreiazetai sth thesi tou 1 na mpei to
	if( bferror != BF_OK ){																//OpenIndex[fileDesc]->numRoot
		BF_PrintError(bferror);
		return AME_EOF;
	}

	dataBlock = BF_Block_GetData(block);

	char type = dataBlock[0];
	int block_num;
	if(type!='I' && type!='D')
			return AME_EOF;

	while( type == 'I' ){
		dataBlock += sizeof(char) + 2*sizeof(int);
		block_num = (int)dataBlock[0];
		BF_UnpinBlock(block);
		BF_GetBlock(OpenIndex[fileDesc]->filedesc,block_num,block);
		dataBlock = BF_Block_GetData(block);
		type = dataBlock[0];
	}
	dataBlock += sizeof(char) + sizeof(int);
	int numIndex = (int)dataBlock[0];
	dataBlock += sizeof(int);
	int comp,i=0,flag=0;		/// flag is -1 for failed scan, 0 for continue to scan and 1 for scan completed

	while( i <= numIndex){
			if(i==numIndex){
					dataBlock = BF_Block_GetData(block);
					dataBlock += BF_BLOCK_SIZE - sizeof(int);
					block_num = (int)dataBlock[0];
					if( block_num == -1){
							printf("Searched all the records and didnt find any with op=%d \n",op );
							BF_UnpinBlock(block);
							return AME_NONE_ENTRY_FOR_SCAN;
					}
					else{
							BF_UnpinBlock(block);
							BF_GetBlock(OpenIndex[fileDesc]->filedesc,block_num,block);
							dataBlock = BF_Block_GetData(block);
							dataBlock += sizeof(char) + sizeof(int);
							numIndex = (int)dataBlock[0];
							dataBlock += sizeof(int);
							i=0;
					}
			}
			else{
					comp = compare_attr1(dataBlock,value,fileDesc);
					switch ( op ) {
						case 1:
								if(comp == 1){		//FAILED
										flag = -1;	//-1
								}
								else if(comp==0){	//SUCCESS
										flag = 1;
								}
								else{
										flag = 0;	//CONTINUE
								}
								break;
						case 2:
								if(comp!=0){	//SUCCESS
										flag = 1;
								}
								else{	//CONTINUE
										flag = 0;
								}
								break;
						case 3:
								if(comp==1 || comp==0){	//FAILED
										flag = -1;
								}
								else{	//SUCCESS
										flag = 1;
								}
								break;
						case 4:
								if(comp==1){ //FAILED
										flag = -1;
								}
								else{	//SUCCESS
										flag = 1;
								}
								break;
						case 5:
								if(comp==1){	//SUCCESS
										flag = 1;
								}
								else{	//CONTINUE
										flag = 0;
								}
								break;
						case 6:
								if(comp==1 || comp==0){	//SUCCESS
										flag = 1;
								}
								else{	//CONTINUE
										flag = 0;
								}
								break;
						default: break;
					}

					if(flag==-1){
							printf("No file could be opened because no record was found with op=%d !!!\n",op );
							BF_UnpinBlock(block);
							return AME_NONE_ENTRY_FOR_SCAN;
					}
					else if(flag==0){
							dataBlock += OpenIndex[fileDesc]->attrLength1 + OpenIndex[fileDesc]->attrLength2;
							i++;
					}
					else{
							OpenIndexScan[scanDesc] = malloc(sizeof(IndexScan));
							OpenIndexScan[scanDesc]->filedesc = OpenIndex[fileDesc]->filedesc;
							OpenIndexScan[scanDesc]->op = op;
							OpenIndexScan[scanDesc]->blocknum = block_num;
							OpenIndexScan[scanDesc]->indexpos = i;
							OpenIndexScan[scanDesc]->fD = fileDesc;

							return scanDesc;
					}
			}
	}
  return AME_OK;
}


void *AM_FindNextEntry(int scanDesc) {
	if(scanDesc<0){
		printf("NO SCAN FILE OPEN\n");
		return NULL;
	}

	void* value;
	void* temp_value;
	int block_num = OpenIndexScan[scanDesc]->blocknum;
	int pos = OpenIndexScan[scanDesc]->indexpos;
	int op = OpenIndexScan[scanDesc]->op;
	int fD = OpenIndexScan[scanDesc]->fD;

	BF_ErrorCode bferror;
	BF_Block *block;
	char* dataBlock;
	BF_Block_Init(&block);
	BF_GetBlock(OpenIndexScan[scanDesc]->filedesc,block_num,block);
	dataBlock = BF_Block_GetData(block);
	dataBlock += sizeof(char) + sizeof(int);
	int numIndex = (int)dataBlock[0];
	dataBlock += sizeof(int);
	dataBlock += pos*(OpenIndex[fD]->attrLength1 + OpenIndex[fD]->attrLength2);
	change_key(&temp_value,dataBlock,1,fD);

	int i=pos,comp,flag=0;
	while( i <= numIndex){
			if(i==numIndex){
					dataBlock = BF_Block_GetData(block);
					dataBlock += BF_BLOCK_SIZE - sizeof(int);
					block_num = (int)dataBlock[0];
					if( block_num == -1){
							printf("There arent any values left \n" );
							BF_UnpinBlock(block);
							free(temp_value);
							AM_errno = AME_EOF;
							return NULL;
					}
					else{
							BF_UnpinBlock(block);
							BF_GetBlock(OpenIndexScan[scanDesc]->filedesc,block_num,block);
							dataBlock = BF_Block_GetData(block);
							dataBlock += sizeof(char) + sizeof(int);
							numIndex = (int)dataBlock[0];
							dataBlock += sizeof(int);
							i=0;
					}
			}
			else{
					comp = compare_attr1(dataBlock,temp_value,fD);
					switch ( op ) {
						case 1:
								if(comp == 1){		//FAILED
										flag = -1;	//-1
								}
								else if(comp==0){	//SUCCESS
										flag = 1;
								}
								else{
										flag = 0;	//CONTINUE
								}
								break;
						case 2:
								if(comp!=0){	//SUCCESS
										flag = 1;
								}
								else{	//CONTINUE
										flag = 0;
								}
								break;
						case 3:
								if(comp==1 || comp==0){	//FAILED
										flag = -1;
								}
								else{	//SUCCESS
										flag = 1;
								}
								break;
						case 4:
								if(comp==1){ //FAILED
										flag = -1;
								}
								else{	//SUCCESS
										flag = 1;
								}
								break;
						case 5:
								if(comp==1){	//SUCCESS
										flag = 1;
								}
								else{	//CONTINUE
										flag = 0;
								}
								break;
						case 6:
								if(comp==1 || comp==0){	//SUCCESS
										flag = 1;
								}
								else{	//CONTINUE
										flag = 0;
								}
								break;
						default: break;
					}
					if(flag==-1){
							printf("No more records to find !!!\n" );
							BF_UnpinBlock(block);
							free(temp_value);
							AM_errno = AME_EOF;
							return NULL;
					}
					else if(flag==0){
							dataBlock += OpenIndex[fD]->attrLength1 + OpenIndex[fD]->attrLength2;
							i++;
					}
					else{
							dataBlock += OpenIndex[fD]->attrLength1;
							change_key(&value,dataBlock,2,fD);
							OpenIndexScan[scanDesc]->blocknum = block_num;
							OpenIndexScan[scanDesc]->indexpos = i;
							free(temp_value);
							BF_UnpinBlock(block);
							return value;
					}
			}
	}

	return NULL;
}


int AM_CloseIndexScan(int scanDesc) {
	int i;
	for(i=0; i<AM_MAXSCANS; i++){
		if(i==scanDesc){
			if ( OpenIndexScan[scanDesc]==NULL ){
				printf("Can't close because it's not open\n");
				return AME_TRIED_TO_CLOSE_NONEXISTENT_SCAN; //den einai anoikth h sarwsh pou zhththhke na sbhsei
			}
			else{
				free(OpenIndexScan[i]);   //apodesmeush domhs
				return AME_OK;
			}
		}
	}
	return AME_INVALID_SCANDESC;
}

void AM_PrintError(char *errString) {
	printf("%s\n", errString);    //ektupwsh lathous
}

void AM_Close() {
    int i;
		for ( i=0;i<AM_MAXSCANS;i++ ){
        if ( OpenIndexScan[i]!=NULL ){
            if ( AM_CloseIndexScan(i)<0 ){
                break;
            }
        }
    }
    for ( i=0;i<AM_MAX_OPEN_FILES;i++ ){
        if ( OpenIndex[i]!=NULL ){
            if ( AM_CloseIndex(i)<0 ){
                break;
            }
        }
    }
    BF_Close();
}



//------------------------------------------------------------------------------------------
void print_data(int fileDesc,int block_num){
	BF_Block *blockTmp;
	BF_Block_Init(&blockTmp);
	BF_ErrorCode bferror;
	bferror = BF_GetBlock(OpenIndex[fileDesc]->filedesc,block_num,blockTmp);
	char *dataTmp;
	dataTmp = BF_Block_GetData(blockTmp);
	char identity = dataTmp[0];
	dataTmp += sizeof(char) + 2*sizeof(int);

	int next=(int)dataTmp[0];

	while ( identity=='I'){
		bferror = BF_GetBlock(OpenIndex[fileDesc]->filedesc,next,blockTmp);
		dataTmp = BF_Block_GetData(blockTmp);
		identity = dataTmp[0];
		if ( identity=='I'){
			dataTmp += sizeof(char) + 2*sizeof(int);
		}
		next=(int)dataTmp[0];
	}
	//eimai sto prwto block fullou
	dataTmp += BF_BLOCK_SIZE - sizeof(int);
	memcpy(&next,dataTmp,sizeof(int));
	dataTmp -= BF_BLOCK_SIZE - sizeof(int);
	while ( next!=-1 ){
        printf("identity iiiiiiiiiiiiiiiiiiiiii %c    ",dataTmp[0]);
		dataTmp += sizeof(char) + sizeof(int);
		int my_keys = (int)dataTmp[0];
		dataTmp += sizeof(int);
		char attr1 = OpenIndex[fileDesc]->attrType1;
		int i;
		for ( i=0; i<my_keys; i++ ){
			void *x = (void*)dataTmp;
			if (attr1 == 'c'){
					printf("string      print in   %s\n",(char*)x);
			}
			else if( attr1 == 'i'){
							printf("int    print in %d\n",*((int*)x));
			}
			else{
							printf("float    print in %f\n",*(float*)x);

			}
            dataTmp += OpenIndex[fileDesc]->attrLength1;

			dataTmp += OpenIndex[fileDesc]->attrLength2;
		}
		bferror = BF_GetBlock(OpenIndex[fileDesc]->filedesc,next,blockTmp);
		dataTmp = BF_Block_GetData(blockTmp);
		dataTmp += BF_BLOCK_SIZE - sizeof(int);
		memcpy(&next,dataTmp,sizeof(int));
		dataTmp -= BF_BLOCK_SIZE - sizeof(int);
	}
	dataTmp += sizeof(char) + sizeof(int);
	int my_keys = (int)dataTmp[0];
	dataTmp += sizeof(int);
	char attr1 = OpenIndex[fileDesc]->attrType1;
	int i;
	for ( i=0; i<my_keys; i++ ){
		void *x = (void*)dataTmp;
		if (attr1 == 'c'){
				printf("string      print in   %s\n",(char*)x);
		}
		else if( attr1 == 'i'){
						printf("int    print in %d\n",*((int*)x));
		}
		else{
						printf("float    print in %f\n",*(float*)x);

		}
        dataTmp += OpenIndex[fileDesc]->attrLength1;

		dataTmp += OpenIndex[fileDesc]->attrLength2;
	}

	bferror = BF_UnpinBlock(blockTmp);
}
//----------------------------------------------------------------------------------------
