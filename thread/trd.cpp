#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <pthread.h>
#include <chrono> 
#include <ctime>
#include <ratio>
#define MAXLEN 10000
#define NUM_THREADS 10
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

#pragma pack(1)
#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER
{
  WORD bfType;
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
struct thread_data {
   int thread_id;
   int start;
   int end;
   int rows;
   int cols;
};
int rows;
int cols;
unsigned char reds[MAXLEN][MAXLEN];
unsigned char greens[MAXLEN][MAXLEN];
unsigned char blues[MAXLEN][MAXLEN];
bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize)
{
  std::ifstream file(fileName);

  if (file)
  {
    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    return 1;
  }
  else
  {
    cout << "File" << fileName << " doesn't exist!" << endl;
    return 0;
  }
}

void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer )
{
  int count = 1;
  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          reds[i][j] =  fileReadBuffer[end - count] ;
          
          break;
        case 1:
          greens[i][j] =  fileReadBuffer[end - count] ;
          
          break;
        case 2:
          blues[i][j] =  fileReadBuffer[end - count] ;
          
          break;
        }
        count++;
      }
  }
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize)
{
  std::ofstream write(nameOfFileToCreate);
  if (!write)
  {
    cout << "Failed to write " << nameOfFileToCreate << endl;
    return;
  }
  int count = 1;
  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          fileBuffer[bufferSize - count] = reds[i][j];
          
          break;
        case 1:
          fileBuffer[bufferSize - count] = greens[i][j];
          
          break;
        case 2:
          fileBuffer[bufferSize - count] = blues[i][j];
          
          break;
        }
        count++;
      }
  }
  write.write(fileBuffer, bufferSize);
}

void *smoothingfilter(void *threadarg)
{
  struct thread_data *my_data;
  my_data = (struct thread_data *) threadarg;
//  cout << my_data->start <<"  " << my_data->end << endl;
  int count = 1;
  int extra = my_data->cols % 4;
  int sum = 0;
  int pixs = 0;
  int x,y ;
  for (int i = my_data->start; i < my_data->end; i++)
  {
    count += extra;
    for (int j = my_data->cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          //reds[i][j] =  fileReadBuffer[end - count] ;
          x = i;
          y = j;
          for(int k1 = x - 1 ; k1 < x + 2 ; k1++)
          {
            for(int z = y - 1 ; z < y + 2 ; z++)
            {
              if (k1 >= 0 and k1 < rows and z>=0 and z < cols)
              {
                pixs ++;
                sum += reds[k1][z];
              }
            }
          }
          reds[i][j] = int(sum / pixs);
          
          break;
        case 1:
          //greens[i][j] =  fileReadBuffer[end - count] ;
          x = i;
          y = j;
          for(int k1 = x - 1 ; k1 < x + 2 ; k1++)
          {
            for(int z = y - 1 ; z < y + 2 ; z++)
            {
              if (k1 >= 0 and k1 < rows and z>=0 and z < cols)
              {
                pixs ++;
                sum += greens[k1][z];
              }
            }
          }
          greens[i][j] = int(sum / pixs);

          break;
        case 2:
          //blues[i][j] =  fileReadBuffer[end - count] ;
          x = i;
          y = j;
          for(int k1 = x - 1 ; k1 < x + 2 ; k1++)
          {
            for(int z = y - 1 ; z < y + 2 ; z++)
            {
              if (k1 >= 0 and k1 < rows and z>=0 and z < cols)
              {
                pixs ++;
                sum += blues[k1][z];
              }
            }
          }
          blues[i][j] = int(sum / pixs);
          
          break;
        }
        count++;
        sum = 0;
        pixs = 0;
      }
  }
  pthread_exit(NULL);
}

void *sepia(void *threadarg)
{
  struct thread_data *my_data;
  my_data = (struct thread_data *) threadarg;
  int count = 1;
  int extra = my_data->cols % 4;
  for (int i = my_data->start; i < my_data->end; i++)
  {
    count += extra;
    for (int j = my_data->cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          if (((reds[i][j] * 0.393) + (greens[i][j] * 0.769) + (blues[i][j] * 0.189) )> 255)
            reds[i][j] = 255;
          else 
            reds[i][j] = (reds[i][j] * 0.393) + (greens[i][j] * 0.769) + (blues[i][j] * 0.189);
          break;
        case 1:
          if(((reds[i][j] * 0.349) + (greens[i][j] * 0.686) + (blues[i][j] * 0.168) )> 255)
            greens[i][j] = 255;
          else
            greens[i][j] = (reds[i][j] * 0.349) + (greens[i][j] * 0.686) + (blues[i][j] * 0.168);
          break;
        case 2:
          if( ((reds[i][j] * 0.272) + (greens[i][j] * 0.534) + (blues[i][j] * 0.131) )> 255)
            blues[i][j] = 255;
          else
            blues[i][j] = (reds[i][j] * 0.272) + (greens[i][j] * 0.534) + (blues[i][j] * 0.131);
          break;
        }
        count++;
      }
  }
}

void *meanfilter(void *threadarg)
{
  struct thread_data *my_data;
  my_data = (struct thread_data *) threadarg;
  int count = 1;
  int extra = my_data->cols % 4;
  int red_sum = 0 , green_sum = 0 , blue_sum = 0 , red_mean , green_mean , blue_mean;
  for (int i = 0 ; i < my_data->rows ; i ++)
    for(int j = 0 ; j < cols ; j++)
    {
      red_sum += reds[i][j];
      green_sum += greens[i][j];
      blue_sum += blues[i][j];
    }

  red_mean = int(red_sum / my_data->rows / my_data->cols);
  green_mean = int(green_sum / my_data->rows / my_data->cols);
  blue_mean = int(blue_sum / my_data->rows / my_data->cols);


  for (int i = my_data->start; i < my_data->end; i++)
  {
    count += extra;
    for (int j = my_data->cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          reds[i][j] =  (reds[i][j] * 0.4) + (red_mean * 0.6);
          if (reds[i][j] > 255)
            reds[i][j] = 255;
          break;
        case 1:
          greens[i][j] =  (greens[i][j] * 0.4) + (green_mean * 0.6);
          if (greens[i][j] > 255)
            greens[i][j] = 255;
          break;
        case 2:
          blues[i][j] =  (blues[i][j] * 0.4) + (blue_mean * 0.6);
          if (blues[i][j] > 255)
            blues[i][j] = 255;
          break;
        }
        count++;
      }
  }
}
void *xfilter(void *threadarg)
{
  struct thread_data *my_data;
  my_data = (struct thread_data *) threadarg;
  int count = 1;
  int extra = my_data->cols % 4;
  for (int i = my_data->start; i < my_data->end; i++)
  {
    count += extra;
    for (int j = my_data->cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          if (i == j || i+j == my_data->rows) 
          {
            reds[i][j] =  255 ;
          }
          
          break;
        case 1:
          if (i == j || i+j == my_data->rows) 
          {
            greens[i][j] =  255 ;
          }
          
          break;
        case 2:
          if (i == j || i+j == my_data->rows) 
          {
            blues[i][j] =  255 ;
          }
          
          break;
        }
        count++;
      }
  }
}
void trd_smooth(int rows , int cols)
{
   int gaps_x = int(rows/NUM_THREADS);
   int rc;
   int i;
   pthread_t threads[NUM_THREADS];
   pthread_attr_t attr;
   void *status;
   struct thread_data td[NUM_THREADS];

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   for( i = 0; i < NUM_THREADS; i++ ) {
    //  cout << "main() : creating thread, " << i << endl;
      td[i].thread_id = i;
      td[i].start = i * gaps_x;
      td[i].end = (i+1) * gaps_x;
      td[i].rows = rows;
      td[i].cols = cols;
      rc = pthread_create(&threads[i], &attr, smoothingfilter, (void *)&td[i] );
      if (rc) {
      //   cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }

   // free attribute and wait for the other threads
   pthread_attr_destroy(&attr);
   for( i = 0; i < NUM_THREADS; i++ ) {
      rc = pthread_join(threads[i], &status);
      if (rc) {
      //   cout << "Error:unable to join," << rc << endl;
         exit(-1);
      }
      //cout << "Main: completed thread id :" << i ;
      //cout << "  exiting with status :" << status << endl;
   }

}
void trd_sepia(int rows , int cols)
{
   int gaps_x = int(rows/NUM_THREADS);
   int rc;
   int i;
   pthread_t threads[NUM_THREADS];
   pthread_attr_t attr;
   void *status;
   struct thread_data td[NUM_THREADS];

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   for( i = 0; i < NUM_THREADS; i++ ) {
      //cout << "main() : creating thread, " << i << endl;
      td[i].thread_id = i;
      td[i].start = i * gaps_x;
      td[i].end = (i+1) * gaps_x;
      td[i].rows = rows;
      td[i].cols = cols;
      rc = pthread_create(&threads[i], &attr, sepia, (void *)&td[i] );
      if (rc) {
      //   cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }

   // free attribute and wait for the other threads
   pthread_attr_destroy(&attr);
   for( i = 0; i < NUM_THREADS; i++ ) {
      rc = pthread_join(threads[i], &status);
      if (rc) {
      //   cout << "Error:unable to join," << rc << endl;
         exit(-1);
      }
      //cout << "Main: completed thread id :" << i ;
      //cout << "  exiting with status :" << status << endl;
   }

}

void trd_mean_filter(int rows , int cols)
{
   int gaps_x = int(rows/NUM_THREADS);
   int rc;
   int i;
   pthread_t threads[NUM_THREADS];
   pthread_attr_t attr;
   void *status;
   struct thread_data td[NUM_THREADS];

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   for( i = 0; i < NUM_THREADS; i++ ) {
      //cout << "main() : creating thread, " << i << endl;
      td[i].thread_id = i;
      td[i].start = i * gaps_x;
      td[i].end = (i+1) * gaps_x;
      td[i].rows = rows;
      td[i].cols = cols;
      rc = pthread_create(&threads[i], &attr, meanfilter, (void *)&td[i] );
      if (rc) {
      //   cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }

   // free attribute and wait for the other threads
   pthread_attr_destroy(&attr);
   for( i = 0; i < NUM_THREADS; i++ ) {
      rc = pthread_join(threads[i], &status);
      if (rc) {
         cout << "Error:unable to join," << rc << endl;
         exit(-1);
      }
      //cout << "Main: completed thread id :" << i ;
      //cout << "  exiting with status :" << status << endl;
   }

}

void trd_xfilter(int rows , int cols)
{
   int gaps_x = int(rows/NUM_THREADS);
   int rc;
   int i;
   pthread_t threads[NUM_THREADS];
   pthread_attr_t attr;
   void *status;
   struct thread_data td[NUM_THREADS];

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   for( i = 0; i < NUM_THREADS; i++ ) {
      //cout << "main() : creating thread, " << i << endl;
      td[i].thread_id = i;
      td[i].start = i * gaps_x;
      td[i].end = (i+1) * gaps_x;
      td[i].rows = rows;
      td[i].cols = cols;
      rc = pthread_create(&threads[i], &attr, xfilter, (void *)&td[i] );
      if (rc) {
      //   cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }

   // free attribute and wait for the other threads
   pthread_attr_destroy(&attr);
   for( i = 0; i < NUM_THREADS; i++ ) {
      rc = pthread_join(threads[i], &status);
      if (rc) {
        // cout << "Error:unable to join," << rc << endl;
         exit(-1);
      }
      //cout << "Main: completed thread id :" << i ;
      //cout << "  exiting with status :" << status << endl;
   }

}
int main(int argc, char *argv[])
{
  

  char *fileBuffer;
  int bufferSize;
  char *fileName = argv[1];
  if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize))
  {
    cout << "File read error" << endl;
    return 1;
  }
  for (int i = 0; i < MAXLEN; i++)
    for (int j = 0; j < MAXLEN; j++)
    {
      reds[i][j] = -1;
      greens[i][j] = -1;
      blues[i][j] = -1;
    }
    
  
  getPixlesFromBMP24(bufferSize , rows , cols , fileBuffer);

  auto start = std::chrono::high_resolution_clock::now();
  trd_smooth(rows , cols);
  writeOutBmp24(fileBuffer , "ut1.bmp" , bufferSize);
  trd_sepia(rows , cols);
  writeOutBmp24(fileBuffer , "ut2.bmp" , bufferSize);
  trd_mean_filter(rows , cols);
  writeOutBmp24(fileBuffer , "ut3.bmp" , bufferSize);
  trd_xfilter(rows , cols);
  writeOutBmp24(fileBuffer , "ut4.bmp" , bufferSize);

  auto end = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> diff = end - start;
   std::cout <<  diff.count() << endl;
  return 0;
}