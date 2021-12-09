//#include <mpi.h>
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include "bmp.h"
#include <pthread.h>
#include <time.h>
using namespace std;


#define NSmooth 2000 //number of operation loops


BMPHEADER bmpHeader;                        
BMPINFO bmpInfo;
RGBTRIPLE **BMPSaveData = NULL;                                               
RGBTRIPLE **BMPData = NULL;                                                   


int readBMP( char *fileName);        //read file
int saveBMP( char *fileName);        //save file
void swap(RGBTRIPLE *a, RGBTRIPLE *b);
RGBTRIPLE **alloc_memory( int Y, int X );        //allocate memory

/*pthread initialization*/
pthread_barrier_t barrier; // the barrier synchronization object
//int total_threads_num ;
long int total_threads_num;

/*the operation each threads should do */
void * execution_for_threads(void *rank)
{
        int my_rank = (long) rank ;
        int thread_rows = bmpInfo.biHeight / total_threads_num;
        int bonus_rows = bmpInfo.biHeight % total_threads_num;
        int start_row = 0 ;
        start_row = thread_rows * my_rank;
        if (my_rank == (total_threads_num - 1) )
                thread_rows = thread_rows + bonus_rows ;
        
        //operation block
        for (int count = 0; count < NSmooth; count++)
        {
                if(my_rank == 0) swap(BMPSaveData, BMPData);
                /*critical block*/
                pthread_barrier_wait(&barrier); //make sure every thread is arrived
                for (int i = start_row; i < start_row + thread_rows ; i++){
                        for (int j = 0; j < bmpInfo.biWidth; j++)
                        {

                                int Top = i > 0 ? i - 1 : bmpInfo.biHeight - 1;
                                int Down = i < bmpInfo.biHeight - 1 ? i + 1 : 0;
                                int Left = j > 0 ? j - 1 : bmpInfo.biWidth - 1;
                                int Right = j < bmpInfo.biWidth - 1 ? j + 1 : 0;

                                BMPSaveData[i][j].rgbBlue = (double)(BMPData[i][j].rgbBlue + BMPData[Top][j].rgbBlue + BMPData[Top][Left].rgbBlue + BMPData[Top][Right].rgbBlue + BMPData[Down][j].rgbBlue + BMPData[Down][Left].rgbBlue + BMPData[Down][Right].rgbBlue + BMPData[i][Left].rgbBlue + BMPData[i][Right].rgbBlue) / 9 + 0.5;
                                BMPSaveData[i][j].rgbGreen = (double)(BMPData[i][j].rgbGreen + BMPData[Top][j].rgbGreen + BMPData[Top][Left].rgbGreen + BMPData[Top][Right].rgbGreen + BMPData[Down][j].rgbGreen + BMPData[Down][Left].rgbGreen + BMPData[Down][Right].rgbGreen + BMPData[i][Left].rgbGreen + BMPData[i][Right].rgbGreen) / 9 + 0.5;
                                BMPSaveData[i][j].rgbRed = (double)(BMPData[i][j].rgbRed + BMPData[Top][j].rgbRed + BMPData[Top][Left].rgbRed + BMPData[Top][Right].rgbRed + BMPData[Down][j].rgbRed + BMPData[Down][Left].rgbRed + BMPData[Down][Right].rgbRed + BMPData[i][Left].rgbRed + BMPData[i][Right].rgbRed) / 9 + 0.5;
                        }
                }
                pthread_barrier_wait(&barrier);//make sure to synchronize each thread's operation
        }
        return NULL;
}

int main(int argc,char *argv[])
{

        char *infileName = "input.bmp";
        char *outfileName = "output2.bmp";
        struct timespec begin, end;
        double elapsed;

        int thread ;


        if ( readBMP( infileName) )
                cout << "Read file successfully!!" << endl;
        else 
                cout << "Read file fails!!" << endl;
        
        clock_gettime(CLOCK_MONOTONIC, &begin);
        BMPData = alloc_memory( bmpInfo.biHeight, bmpInfo.biWidth);

        /*threads initialization*********************************************************************************/
        //total theads runtime get define at runtime
        total_threads_num = strtol(argv[1], NULL, 10); // conversion from string to int (in base 10)
        cout<<"total_threads_num get at runtime is :" << total_threads_num << endl;
        pthread_barrier_init(&barrier, NULL, total_threads_num);
        //dynamicly create pthread_t objects array
        pthread_t* thread_handles_ptr ;
        thread_handles_ptr = (pthread_t*)malloc(total_threads_num*sizeof(pthread_t)); //for every pthread object
        /*threads initialization*********************************************************************************/


        //fork threads
        for(thread = 0 ; thread < total_threads_num ; thread++){
                pthread_create(&thread_handles_ptr[thread] , NULL , execution_for_threads , (void*) thread);
        }
        //join after jobs of threads is done
        for(thread = 0 ; thread < total_threads_num ; thread++){
                pthread_join(thread_handles_ptr[thread] , NULL);
        }
        
        if ( saveBMP( outfileName ) )
                cout << "Save file successfully!!" << endl;
        else
                cout << "Save file fails!!" << endl;
 

        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed = end.tv_sec - begin.tv_sec;
        cout<<"total time is : "<<elapsed<<"sec"<< endl;

        //free object of pointer
        free(thread_handles_ptr);

	free(BMPData[0]);
 	free(BMPSaveData[0]);
 	free(BMPData);
 	free(BMPSaveData);


    return 0;
}

int readBMP(char *fileName)
{
	//�إ߿�J�ɮת���	
        ifstream bmpFile( fileName, ios::in | ios::binary );
 
        //�ɮ׵L�k�}��
        if ( !bmpFile ){
                cout << "It can't open file!!" << endl;
                return 0;
        }
 
        //Ū��BMP���ɪ����Y���
    	bmpFile.read( ( char* ) &bmpHeader, sizeof( BMPHEADER ) );
 
        //�P�M�O�_��BMP����
        if( bmpHeader.bfType != 0x4d42 ){
                cout << "This file is not .BMP!!" << endl ;
                return 0;
        }
 
        //Ū��BMP����T
        bmpFile.read( ( char* ) &bmpInfo, sizeof( BMPINFO ) );
        
        //�P�_�줸�`�׬O�_��24 bits
        if ( bmpInfo.biBitCount != 24 ){
                cout << "The file is not 24 bits!!" << endl;
                return 0;
        }

        //�ץ��Ϥ����e�׬�4������
        while( bmpInfo.biWidth % 4 != 0 )
        	bmpInfo.biWidth++;

        //�ʺA���t�O����
        BMPSaveData = alloc_memory( bmpInfo.biHeight, bmpInfo.biWidth);
        
        //Ū���������
    	//for(int i = 0; i < bmpInfo.biHeight; i++)
        //	bmpFile.read( (char* )BMPSaveData[i], bmpInfo.biWidth*sizeof(RGBTRIPLE));
	    bmpFile.read( (char* )BMPSaveData[0], bmpInfo.biWidth*sizeof(RGBTRIPLE)*bmpInfo.biHeight);
	
        //�����ɮ�
        bmpFile.close();
 
        return 1;
 
}
/*********************************************************/
/* �x�s����                                              */
/*********************************************************/
int saveBMP( char *fileName)
{
 	//�P�M�O�_��BMP����
        if( bmpHeader.bfType != 0x4d42 ){
                cout << "This file is not .BMP!!" << endl ;
                return 0;
        }
        
 	//�إ߿�X�ɮת���
        ofstream newFile( fileName,  ios:: out | ios::binary );
 
        //�ɮ׵L�k�إ�
        if ( !newFile ){
                cout << "The File can't create!!" << endl;
                return 0;
        }
 	
        //�g�JBMP���ɪ����Y���
        newFile.write( ( char* )&bmpHeader, sizeof( BMPHEADER ) );

	//�g�JBMP����T
        newFile.write( ( char* )&bmpInfo, sizeof( BMPINFO ) );

        //�g�J�������
        //for( int i = 0; i < bmpInfo.biHeight; i++ )
        //        newFile.write( ( char* )BMPSaveData[i], bmpInfo.biWidth*sizeof(RGBTRIPLE) );
        newFile.write( ( char* )BMPSaveData[0], bmpInfo.biWidth*sizeof(RGBTRIPLE)*bmpInfo.biHeight );

        //�g�J�ɮ�
        newFile.close();
 
        return 1;
 
}


/*********************************************************/
/* ���t�O����G�^�Ǭ�Y*X���x�}                           */
/*********************************************************/
RGBTRIPLE **alloc_memory(int Y, int X )
{        
	//�إߪ��׬�Y�����а}�C
        RGBTRIPLE **temp = new RGBTRIPLE *[ Y ];
	    RGBTRIPLE *temp2 = new RGBTRIPLE [ Y * X ];
        memset( temp, 0, sizeof( RGBTRIPLE ) * Y);
        memset( temp2, 0, sizeof( RGBTRIPLE ) * Y * X );

	//��C�ӫ��а}�C�̪����Ыŧi�@�Ӫ��׬�X���}�C 
        for( int i = 0; i < Y; i++){
                temp[ i ] = &temp2[i*X];
        }
 
        return temp;
 
}
/*********************************************************/
/* �洫�G�ӫ���                                          */
/*********************************************************/
void swap(RGBTRIPLE *a, RGBTRIPLE *b)
{
	RGBTRIPLE *temp;
	temp = a;
	a = b;
	b = temp;
}

