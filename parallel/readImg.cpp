#include <iostream>
#include <unistd.h>
#include <fstream>
#include <chrono> 
#include <ctime>
#include <ratio>
#define MAXLEN 10000
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

void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer)
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

void smoothingfilter(int rows, int cols)
{
  int count = 1;
  int extra = cols % 4;
  int sum = 0;
  int pixs = 0;
  int x,y ;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
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
}

void sepia(int rows, int cols)
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

void meanfilter(int rows, int cols)
{
  int count = 1;
  int extra = cols % 4;
  int red_sum = 0 , green_sum = 0 , blue_sum = 0 , red_mean , green_mean , blue_mean;
  for (int i = 0 ; i < rows ; i ++)
    for(int j = 0 ; j < cols ; j++)
    {
      red_sum += reds[i][j];
      green_sum += greens[i][j];
      blue_sum += blues[i][j];
    }

  red_mean = int(red_sum / rows / cols);
  green_mean = int(green_sum / rows / cols);
  blue_mean = int(blue_sum / rows / cols);


  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
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
void xfilter(int rows, int cols)
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
          if (i == j || i+j == rows) 
          {
            reds[i][j] =  255 ;
          }
          
          break;
        case 1:
          if (i == j || i+j == rows) 
          {
            greens[i][j] =  255 ;
          }
          
          break;
        case 2:
          if (i == j || i+j == rows) 
          {
            blues[i][j] =  255 ;
          }
          
          break;
        }
        count++;
      }
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
  smoothingfilter(rows , cols);
  writeOutBmp24(fileBuffer , "ut1.bmp" , bufferSize);
  sepia(rows , cols);
  writeOutBmp24(fileBuffer , "ut2.bmp" , bufferSize);
  meanfilter(rows , cols);
  writeOutBmp24(fileBuffer , "ut3.bmp" , bufferSize);
  xfilter(rows , cols);
  writeOutBmp24(fileBuffer , "ut4.bmp" , bufferSize);
  
  auto end = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> diff = end - start;
   std::cout <<  diff.count() << endl;
  return 0;
}