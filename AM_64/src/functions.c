#include "functions.h"


int compare_attr1(void* x, void* y,int filedesc){                      //Epistrefei 0 if x=y,  1 if x>y,   -1 if x<y.
        char attr1 = OpenIndex[filedesc]->attrType1;									 //h sugkrish ginetai mono metaksh twn kleidiwn
        if (attr1 == 'c'){																						 //gia kathe enan apo tous kathe tupous tou pretou pediou
            int value = strcmp((char*)x,(char*)y);									   //ginetai to katallhlo cast
            if ( value > 0 )
                    return 1;
            else if( value < 0 )
                    return -1;
            else
                    return 0;
        }
        else if( attr1 == 'i'){
                if( *((int*)x) > *((int*)y) )
                        return 1;
                else if (*((int*)x) < *((int*)y))
                        return -1;
                else
                        return 0;
        }
        else{
                if( *((float*)x) > *((float*)y) )
                        return 1;
                else if (*((float*)x) < *((float*)y))
                        return -1;
                else
                        return 0;

        }
}

void change_key(void** key,void* to_copy,int val,int filedesc){			  //h metablhth key pairnei thn timh ths metablhths to _copy
		char attr;																												//me to katallhlo cast analoga me ton tupo tou prwtou h tou
		int size;																													//deuterou pediou an h metablhth val einai 1 h 0 antoistoixa
		if( val == 1){																										//prin thn anathesh timhs desmeyetai o katallhlos xwros?free
			attr = OpenIndex[filedesc]->attrType1;
			size = OpenIndex[filedesc]->attrLength1;
		}
		else{
			attr = OpenIndex[filedesc]->attrType2;
			size = OpenIndex[filedesc]->attrLength2;
		}
    if (attr == 'c'){
        *key = (char*)malloc(size);
				memcpy(*key,to_copy,size);
    }
    else if (attr == 'i'){
        *key  = (int*)malloc(size);
    		memcpy(*key,to_copy,size);
    }
    else{
        *key  = (float*)malloc(size);
    		memcpy(*key,to_copy,size);
    }
}

void InsertWithSorting(int fileDesc,char *dataData,void *value1,void *value2) {//eisagwgou zeugous value1,value2 sto block fullou
																																								//pou deixnei to dataData
    int my_keys = (int)dataData[0];
    int new_keys = my_keys + 1;
    memcpy(dataData,&new_keys,sizeof(int));                                //enhmerrwsh newn kleidiwn tou blockData
    dataData += sizeof(int);
    int i=0;
    while(i<=my_keys) {            //h eisagwgh ginetai prin thn prwth timh tmp opou value1<tmp.Etsi ta kleidia sto block paramenoun
        if(i==my_keys) {				   //taksinomhmena se auksousa seira
            break;
        }
        void *tmp = dataData;
        int x = compare_attr1(value1,tmp,fileDesc);

        if(x==-1) {
            memmove((&dataData[0]+OpenIndex[fileDesc]->attrLength1+OpenIndex[fileDesc]->attrLength2),dataData,(my_keys-i)*(OpenIndex[fileDesc]->attrLength1+OpenIndex[fileDesc]->attrLength2));
            break;         //metakinhsh twn epomenwn twn megaluterwn kleidiwn tou value1 toso owste na xwresei kai to valuew1 mazi me
        }                  //to valu2 sth swsth thesh

        dataData += (OpenIndex[fileDesc]->attrLength1+OpenIndex[fileDesc]->attrLength2);//value1>=tmp kai ara metakinhsh tou dataData
        i+=1;                                                                           //prin to epomeno kleidi
    }

    memcpy(dataData,value1,OpenIndex[fileDesc]->attrLength1);
    dataData += OpenIndex[fileDesc]->attrLength1;
    memcpy(dataData,value2,OpenIndex[fileDesc]->attrLength2);
}


void InsertForIndexWithSorting(int fileDesc,char *dataData,void *value1,void *value2) { //paromoia me thn InsertWithSorting me thn
    int my_keys = (int)dataData[0];                      //monh diafora oti thn thesh tou OpenIndex[fileDesc]->attrLength2 pairnei
    int new_keys = my_keys + 1;                          //to sizeof(int) afou to value2 einai "deikths"
    memcpy(dataData,&new_keys,sizeof(int));
    dataData += sizeof(int);
    dataData += sizeof(int);
    int i=0;
    while(i<=my_keys) {
        if(i==my_keys) {
            break;
        }
        void *tmp = dataData;
        int x = compare_attr1(value1,tmp,fileDesc);

        if(x==-1) {
            memmove((&dataData[0]+OpenIndex[fileDesc]->attrLength1+sizeof(int)),dataData,(my_keys-i)*(OpenIndex[fileDesc]->attrLength1+sizeof(int)));
            break;
        }

        dataData += (OpenIndex[fileDesc]->attrLength1 + sizeof(int));
        i+=1;
    }

    memcpy(dataData,value1,OpenIndex[fileDesc]->attrLength1);
    dataData += OpenIndex[fileDesc]->attrLength1;
    memcpy(dataData,value2,sizeof(int));

		void *tmp = dataData;
		int x = compare_attr1(value1,tmp,fileDesc);
}



void Root_Creation(int fileDesc,void *value1,void *value2)             //dhmiourgeitai ena block eurethriou me enan deikth
{                                                                      //kai ena block dedomenwn me to kleidi kai to data tou
        BF_ErrorCode bferror;
        int x;
        ////////////////////////////block eurethriou/////////////////////////////////////
        BF_Block *blockRoot;
        BF_Block_Init(&blockRoot);                                       //arxikopoihsh block apo to epipedo bf

        bferror = BF_AllocateBlock(OpenIndex[fileDesc]->filedesc,blockRoot);       //desmeysh xwrou

        char *dataIndex;
        dataIndex = BF_Block_GetData(blockRoot);                      //dataIndex deikths sthn arxh toy block

        dataIndex[0] = 'I';
        dataIndex +=sizeof(char);

        x = -1;                                                 //afou h riza den exei patera ws pateras orizetai h timh -1
        memcpy(dataIndex,&x,sizeof(int));
        dataIndex +=sizeof(int);

	      x =0;                                     //0 arithmos kleidiwn dhladh enas deikths
        memcpy(dataIndex,&x,sizeof(int));
        dataIndex += sizeof(int);

        x=2;    																				//giati kseroume oti sth synexeia tha dhmiourghthei ena block fullou
        memcpy(dataIndex,&x,sizeof(int));


        OpenIndex[fileDesc]->numRoot = 1;

        BF_Block_SetDirty(blockRoot);
        bferror = BF_UnpinBlock(blockRoot);
        ////////////////////////////block fulllou/////////////////////////////////////
        BF_Block *blockData;                      //block to mplok me ta metadata
        BF_Block_Init(&blockData);

        bferror = BF_AllocateBlock(OpenIndex[fileDesc]->filedesc,blockData);

        char *dataData;
        dataData = BF_Block_GetData(blockData);

        dataData[0] = 'D';
        dataData +=sizeof(char);

	      x = 1; 																								//o anagnwristikos arithmos block tou patera
        memcpy(dataData,&x,sizeof(int));
        dataData +=sizeof(int);

        x = 1;
        memcpy(dataData,&x,sizeof(int));
        dataData +=sizeof(int);
        memcpy(dataData,value1,OpenIndex[fileDesc]->attrLength1);
        dataData += OpenIndex[fileDesc]->attrLength1;
        memcpy(dataData,value2,OpenIndex[fileDesc]->attrLength2);

				dataData = dataData = BF_Block_GetData(blockData);
				dataData += BF_BLOCK_SIZE -sizeof(int);
				x=-1;                                    //epeidh uparxei ena block dedomenwn to epomeno apo auto orizetai me timh -1
				memcpy(dataData,&x,sizeof(int));

        BF_Block_SetDirty(blockData);
        bferror = BF_UnpinBlock(blockData);
}

void break_Index(int fileDesc,char *dataIndex, int totalIndex,void *value1,void* value2){  //paromoia logika me thn break_it
	int father_pointer = (int)dataIndex[0];                                    //mono pou to value2 einai enas int
	dataIndex +=sizeof(int);

	int my_keys = totalIndex;

	BF_ErrorCode bferror;

	BF_Block *newBlock;
	BF_Block_Init(&newBlock);
	bferror = BF_AllocateBlock(OpenIndex[fileDesc]->filedesc,newBlock);         //dhmiourgia neou block eurethriou
	char *newdataData;
	newdataData = BF_Block_GetData(newBlock);
	newdataData[0] = 'I';
	newdataData +=sizeof(char);

	memcpy(newdataData,&father_pointer,sizeof(int));
	newdataData += sizeof(int);
	dataIndex += 2*sizeof(int);
	void* last_key1;
	int last_int;
	if ( totalIndex%2==0 ){
		dataIndex += (totalIndex/2-1)*(OpenIndex[fileDesc]->attrLength1 + sizeof(int));
	}
	else{
		dataIndex += (totalIndex/2)*(OpenIndex[fileDesc]->attrLength1 + sizeof(int) );
	}
	change_key(&last_key1,(void*)dataIndex,1,fileDesc);
	memcpy(&last_int,dataIndex+OpenIndex[fileDesc]->attrLength1,sizeof(int));

	if ( totalIndex%2==0 ){
		dataIndex -= (totalIndex/2-1)*(OpenIndex[fileDesc]->attrLength1 + sizeof(int));
	}
	else{
		dataIndex -= (totalIndex/2)*(OpenIndex[fileDesc]->attrLength1 + sizeof(int) );
	}
	int value = compare_attr1(value1,last_key1,fileDesc);
	int keys_old,keys_new;
	if( value == 1 || value == 0){
			if ( totalIndex%2==0 ){
				keys_old = totalIndex/2 ;
			}
			else{
				keys_old = totalIndex/2 + 1;
			}        // kleidia block pou eixame
			keys_new = totalIndex - keys_old;

			memcpy(newdataData,&keys_new,sizeof(int));
			newdataData += 2*sizeof(int);

			dataIndex -= 2*sizeof(int);
			memcpy(dataIndex,&keys_old,sizeof(int));
			dataIndex += 2*sizeof(int) + keys_old * (OpenIndex[fileDesc]->attrLength1 + sizeof(int) );
			memmove(newdataData, dataIndex, keys_new*(OpenIndex[fileDesc]->attrLength1 + sizeof(int) ));
			newdataData -= 2*sizeof(int);
			dataIndex -= 2*sizeof(int) + keys_old * (OpenIndex[fileDesc]->attrLength1 + sizeof(int) );
			InsertForIndexWithSorting(fileDesc,&newdataData[0],value1,value2);
	}
	else{
					if ( totalIndex%2==0 ){
						keys_old = totalIndex/2 ;
					}
					else{
						keys_old = totalIndex/2 + 1;
					}
					keys_new = totalIndex - keys_old;

					memcpy(newdataData,&keys_new,sizeof(int));
					newdataData += 2*sizeof(int);

					dataIndex -= 2*sizeof(int);
					memcpy(dataIndex,&keys_old,sizeof(int));
					dataIndex += 2*sizeof(int) + (keys_old-1) * (OpenIndex[fileDesc]->attrLength1 + sizeof(int) );

					memmove(newdataData, dataIndex, keys_new*(OpenIndex[fileDesc]->attrLength1 + sizeof(int) ));
					newdataData -= 2*sizeof(int);
					dataIndex -= 2*sizeof(int) + (keys_old-1) * (OpenIndex[fileDesc]->attrLength1 + sizeof(int) );
					InsertForIndexWithSorting(fileDesc,&dataIndex[0],value1,value2);
	}

	int numBlocks;
	bferror = BF_GetBlockCounter(fileDesc,&numBlocks);
	numBlocks -= 1;

	//////////////////////////////////
  BF_Block *blockFather;
	BF_Block_Init(&blockFather);
	void *new_value1;
	newdataData += 2*sizeof(int);
	change_key(&new_value1,(void*)newdataData,1,fileDesc);   //new_value1 exei to prwto kleidi apo to neo block
	newdataData -= sizeof(int);
	memmove(newdataData,newdataData+sizeof(int)+OpenIndex[fileDesc]->attrLength1,keys_new*(OpenIndex[fileDesc]->attrLength1 + sizeof(int)));
   //h metakinhsh auth ginetai etsi wste meta ton arithmo twn kleidiwn sto neo block na uparxei deikths kai ta dedomena pou akolouthoun
	if ( father_pointer==-1 ){      //spasimo rizas
        dataIndex -= sizeof(int);
        numBlocks++;
        memcpy(dataIndex,&numBlocks,sizeof(int));   //pateras tou paliou block o arithmos block tou patera pu ftiaxnoume sthn sunexeia
        numBlocks--;
				BF_AllocateBlock(OpenIndex[fileDesc]->filedesc,blockFather);
				char *dataFather;
				dataFather = BF_Block_GetData(blockFather);   //edw blockFather einai h nea riza

				dataFather[0] = 'I';
				dataFather +=sizeof(char);

				int x = -1;   //enhmerwsh patera rizas
				memcpy(dataIndex,&x,sizeof(int));
				dataIndex +=sizeof(int);

				x =1; //arithmos kleidiwn rizas
				memcpy(dataFather,&x,sizeof(int)); // not used
				dataFather += sizeof(int);

				x = OpenIndex[fileDesc]->numRoot;   //o deksios deikths apo to monadiko kleidi ths rizas "deixnei" sto dataIndex
				memcpy(dataFather,&x,sizeof(int));
				dataFather += sizeof(int);
				InsertForIndexWithSorting(fileDesc,dataFather,new_value1,&numBlocks);  //eisagwgh kleidiou-deikths
																								//o deksios deikths apo to monadiko kleidi ths rizas "deixnei" sto newdataData
				newdataData -= sizeof(int);
				numBlocks ++;
				memcpy(newdataData,&numBlocks,sizeof(int));
				OpenIndex[fileDesc]->numRoot = numBlocks-1;    //enherwsh rizas ston pinaka twn anoiktwn arxeiwn
	}
	else{
		bferror = BF_GetBlock(OpenIndex[fileDesc]->filedesc,father_pointer,blockFather);

		char *dataFather;
		dataFather = BF_Block_GetData(blockFather);
		dataFather += sizeof(char);

		dataFather += sizeof(int);

		int root_keys = (int)dataFather[0];

		if(root_keys == totalIndex) {
					dataFather -= sizeof(int);
					break_Index(fileDesc,dataFather,totalIndex,new_value1,&numBlocks);
		}else {
					InsertForIndexWithSorting(fileDesc,&dataFather[0],new_value1,&numBlocks);

   }
 }

 free(last_key1);

	BF_Block_SetDirty(blockFather);
	bferror = BF_UnpinBlock(blockFather);

	 BF_Block_SetDirty(newBlock);
	 bferror = BF_UnpinBlock(newBlock);

}

void break_it(int fileDesc,char *dataData, int totalData,int totalIndex,void *value1,void *value2) {
//----------------katallhlo spasimo tou block fullou gia thn eisagwgh twn value1,value2---------------------------
            int father_pointer = (int)dataData[0];      //pateras tou fullou
            dataData +=sizeof(int);                  //dataData einai prin ton arithmo twn kleidiwn

            int my_keys = totalData;            //afou to block sto deixnei to dataData einai gemato

            BF_ErrorCode bferror;

            BF_Block *newBlock;
            BF_Block_Init(&newBlock);
            bferror = BF_AllocateBlock(OpenIndex[fileDesc]->filedesc,newBlock);    //dhmiourgia neou block fullou
            char *newdataData;
            newdataData = BF_Block_GetData(newBlock);
            newdataData[0] = 'D';
            newdataData +=sizeof(char);

            memcpy(newdataData,&father_pointer,sizeof(int));						//apothhkeush patera
		        newdataData += sizeof(int);								//newdataData einai meta ton patera

			      dataData += sizeof(int);					       //dataData einai prin to prwto kleidi
            void* last_key1;                         //tou klidi me to opoio tha sygkrithei to neoeisaxthento kleidi
            void* last_key2;                         //to data tou.Den einai aparaithta to last key apo to palio block apla
																										 //etsi exei oristei to onoma tou
						if ( totalData%2==0 ){ //an exoume artio n plhthos kleidiwn tote to n/2 kleidi tha xrhsimeusei sth syukrish me value1
							dataData += (totalData/2-1)*(OpenIndex[fileDesc]->attrLength1 + OpenIndex[fileDesc]->attrLength2 );
						}
						else{                  //an peritto plhthos tote to mesaio xrhsimeuei gia thn sugkrish
							dataData += (totalData/2)*(OpenIndex[fileDesc]->attrLength1 + OpenIndex[fileDesc]->attrLength2 );
						}
            change_key(&last_key1,(void*)dataData,1,fileDesc); //apothhkeush kleidiou sugkrishs kai dedomenou tou se metablhtes
            change_key(&last_key2,(void*)(dataData + OpenIndex[fileDesc]->attrLength1),2,fileDesc);
						if ( totalData%2==0 ){
							dataData -= (sizeof(int)+totalData/2-1)*(OpenIndex[fileDesc]->attrLength1 + OpenIndex[fileDesc]->attrLength2 );
						}
						else{
							dataData -= sizeof(int)+(totalData/2)*(OpenIndex[fileDesc]->attrLength1 + OpenIndex[fileDesc]->attrLength2 );
						}                            //dataData einai prin ton arithmo twn kleidiwn
            int value = compare_attr1(value1,last_key1,fileDesc);
            int keys_old,keys_new;
            if( value == 1 || value == 0){
								if ( totalData%2==0 ){
									keys_old = totalData/2 ;                       //ta palia kleidia menoun idia
								}
								else{
									keys_old = totalData/2 + 1;         //ta palia kleidia auksanontai kata 1 kai ta 2 block exoun idio ariumo kleidiwn
								}
                keys_new = totalData - keys_old;    //kleidia tou kainouriou
                memcpy(newdataData,&keys_new,sizeof(int));   //enhmerwsh arithmou kleidiwn sto block pou spaei
                newdataData += sizeof(int);                  //newdataData einai prin to prwto kleidi

                memcpy(dataData,&keys_old,sizeof(int));      //apothhkeush aritmou kleidiwn sto neo block
                dataData += sizeof(int) + keys_old * (OpenIndex[fileDesc]->attrLength1 + OpenIndex[fileDesc]->attrLength2 );
																					//dataData einai meta to teleutaio kleidi pou tha meinei sto palio block
                memmove(newdataData, dataData, keys_new*(OpenIndex[fileDesc]->attrLength1 + OpenIndex[fileDesc]->attrLength2 ));
																								//metafora upollhpwn kleidiwn sto neo block
                newdataData -= sizeof(int);     //newdataData einai prin ton patera
                dataData -= sizeof(int) + keys_old * (OpenIndex[fileDesc]->attrLength1 + OpenIndex[fileDesc]->attrLength2 );
																										//dataData einai prin ton patera
                InsertWithSorting(fileDesc,&newdataData[0],value1,value2); //eisagwgh value1,value2 sto neo block(newdataData)
            }
            else{
										if ( totalData%2==0 ){
											keys_old = totalData/2 ;
										}
										else{
											keys_old = totalData/2 + 1;
										}
                    keys_new = totalData - keys_old;

                    memcpy(newdataData,&keys_new,sizeof(int));
                    newdataData += sizeof(int);

                    memcpy(dataData,&keys_old,sizeof(int));
                    dataData += sizeof(int) + (keys_old-1)* (OpenIndex[fileDesc]->attrLength1 + OpenIndex[fileDesc]->attrLength2 );
                    memmove(newdataData, dataData, keys_new*(OpenIndex[fileDesc]->attrLength1 + OpenIndex[fileDesc]->attrLength2 ));
										newdataData -= sizeof(int);
                    dataData -= sizeof(int) + (keys_old-1) * (OpenIndex[fileDesc]->attrLength1 + OpenIndex[fileDesc]->attrLength2 );
                    InsertWithSorting(fileDesc,&dataData[0],value1,value2);   //eisagwgh value1,value2 sto palio block(ataData)

            }

            int numBlocks;
            bferror = BF_GetBlockCounter(fileDesc,&numBlocks);
						numBlocks -= 1;     //o anagnwrisistikos arithmos toy neou block pou dhmiourghthhke(newdataData).Einai numBlocks -= 1;
																//giati h arithmhsh twn block arxizei apo to 0
						dataData -= sizeof(char) + sizeof(int);   //dataData sthn arxh tou block
						dataData += BF_BLOCK_SIZE - sizeof(int);  //dataData sizeof(int) prin to telos tou block gia enhmerwsh "deikth"
						int next_of_dataData;                     //epomenou block
						memcpy(&next_of_dataData,dataData,sizeof(int)); //next_of_dataData o epomenos tou paliou block(dataData)
						memcpy(dataData,&numBlocks,sizeof(int));        //o neos epomenos tou paliou
						newdataData -= (sizeof(char) + sizeof(int));    //newdataData sthn arxh tou block
						newdataData += (BF_BLOCK_SIZE - sizeof(int));
						memcpy(newdataData,&next_of_dataData,sizeof(int));  //o epomenos tou kainourgiou tha einai o
																																//palios epomenos(next_of_dataData) tou paliou
						newdataData -= (BF_BLOCK_SIZE - sizeof(int));
						newdataData += (sizeof(char)+sizeof(int));          //newdataData meta ton patera
            //////////////////////
            BF_Block *blockFather;                      //pairnoume to block tou patera
            BF_Block_Init(&blockFather);

            void *new_value1 = newdataData + sizeof(int);     //new_value1 meta ton arithmo twn kleidiwn

            bferror = BF_GetBlock(OpenIndex[fileDesc]->filedesc,father_pointer,blockFather);

            char *dataIndex;
            dataIndex = BF_Block_GetData(blockFather);     //dataIndex sthn arxh tou block tou blockFather
            dataIndex += sizeof(char);                    //dataIndex prin ton patera

            int father = (int) dataIndex[0];        //father o pateras tou patera tou(dataData kai tou newdataData)
            dataIndex += sizeof(int);               //dataIndex meta ton patera

            int root_keys = (int)dataIndex[0]; //root_keys arithmos kleidiwn patera

            if(root_keys == totalIndex) {    //an to blockFather einai gemato
									dataIndex -= sizeof(int);
									break_Index(fileDesc,dataIndex,totalIndex,new_value1,&numBlocks);    //spasimo blockFather(blok eurethriou)
            }else {                        //uparxei xwros
                InsertForIndexWithSorting(fileDesc,&dataIndex[0],new_value1,&numBlocks);    //eisagetai sto blockFather to zeugari
            }                                //kleidi-deikths new_value1-&numblocks.Epilegetai ws kleii to mikrotero
																						 //apo to neo block(newdataData)

						free(last_key1);
						free(last_key2);
            BF_Block_SetDirty(blockFather);
            bferror = BF_UnpinBlock(blockFather);

             BF_Block_SetDirty(newBlock);
             bferror = BF_UnpinBlock(newBlock);
}

int SearchPosition(int fileDesc,int numblock,void *value1){
		int i=0;
    int value,numIndex;
		char type;
		BF_ErrorCode bferror;
		char *dataBlock;
		BF_Block *block;
		BF_Block_Init(&block);

		bferror = BF_GetBlock(OpenIndex[fileDesc]->filedesc,numblock,block);
		dataBlock = BF_Block_GetData(block);

		type = dataBlock[0];
		dataBlock += sizeof(char);
		dataBlock += sizeof(int);
		numIndex = (int)dataBlock[0];
		dataBlock += sizeof(int);


		if( numIndex == 0){
			numblock = (int)dataBlock[0];
			bferror = BF_UnpinBlock(block);
			return numblock;
		}

        if(numIndex == 1) {
            dataBlock += sizeof(int);
            void *tmp = dataBlock;
            value = compare_attr1(value1,tmp,fileDesc);
            dataBlock += OpenIndex[fileDesc]->attrLength1;
            numblock = dataBlock[0];
            dataBlock -= sizeof(int);
            dataBlock -= OpenIndex[fileDesc]->attrLength1;

        }

		while( type == 'I' && i<=numIndex){

				if( i == numIndex){

						numblock = (int)dataBlock[0];
						bferror = BF_UnpinBlock(block);
						bferror = BF_GetBlock(OpenIndex[fileDesc]->filedesc,numblock,block);
						dataBlock = BF_Block_GetData(block);

						type = dataBlock[0];
						if( type == 'D')	break;
						dataBlock += sizeof(char);
						dataBlock += sizeof(int);
						numIndex = (int)dataBlock[0];
						dataBlock += sizeof(int);
						i=0;
				}
				else{
                		dataBlock += sizeof(int);
                        void *x;
						value = compare_attr1(value1,dataBlock,fileDesc);
						if( value==-1 || value==0 ){
								dataBlock -= sizeof(int);
								numblock = (int)dataBlock[0];
								bferror = BF_UnpinBlock(block);
								bferror = BF_GetBlock(OpenIndex[fileDesc]->filedesc,numblock,block);
								dataBlock = BF_Block_GetData(block);

								type = dataBlock[0];
								if( type == 'D')	break;
								dataBlock += sizeof(char);
								dataBlock += sizeof(int);
								numIndex = (int)dataBlock[0];
								dataBlock += sizeof(int);
								i=0;
						}
						else{
								dataBlock += OpenIndex[fileDesc]->attrLength1;
								i++;
						}
				}

		}

		if( type == 'D'){
				bferror = BF_UnpinBlock(block);
				return numblock;
		}
		else{
				printf("ERROR no Data Block found!!!\n");
				return -1;
		}
}

void Insert(int fileDesc,int search_position, void *value1, void *value2, int totalData,int totalIndex) {
//-------------------eisagei to kleidi value1 mazi me thn timh value2 sto dentro---------------------------
        BF_Block *blockData;
        BF_Block_Init(&blockData);

        BF_ErrorCode bferror;
        bferror = BF_GetBlock(OpenIndex[fileDesc]->filedesc,search_position,blockData);  // sto blockData uparxei to block
        if( bferror != BF_OK ){                                            //sto opoio tha ginei h eisagwgh
                BF_PrintError(bferror);
        }

        char *dataData;
        dataData = BF_Block_GetData(blockData);
        dataData += sizeof(char);
        dataData += sizeof(int);  //dataData meta ton patera
        int my_keys=(int)dataData[0];

        if ( my_keys==totalData ){  //an den uparxei xwros se auto to block dhmiourgeitai
            												//neo block fullou me tis katallhles allages twn periexomenwn tou paliou blockData
            dataData -=sizeof(int); //kai tou neou block
            break_it(fileDesc,&dataData[0],totalData,totalIndex,value1,value2); //se auto to shmeio to dataData deixnei sto
																																								//blockData prin thn timh tou patera tou
        }
        else{        //uparxei xwros kai eisagwgh sto blockData
            InsertWithSorting(fileDesc,&dataData[0],value1,value2);
        }
        BF_Block_SetDirty(blockData);
        bferror = BF_UnpinBlock(blockData);
        if( bferror != BF_OK ){
            BF_PrintError(bferror);
        }

}
