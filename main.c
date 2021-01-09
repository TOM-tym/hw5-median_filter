// /*
//  * main.c
//  */

 #include <stdio.h>
 #include <math.h>
 #include <time.h>
 #include "custombmplib.h"
 const char iFileName[] = "../imgs/lena2.bmp";
 const char oFileName[] = "../imgs/result_lena_3.bmp";
 const int filter_size = 3;
 extern void median_filter(BYTE* psrc, BYTE* pdst, int height, int width, int N);

 clock_t clock();

 int main(void) {
     TSCL = 1;
     BITMAPFILEHEADER ibmfh, obmfh;    //inBitmapFileHead, outBitmapFileHead
     BITMAPINFOHEADER ibmih, obmih;    //inBitmapInfoHead, outBitmapInfoHead
     FILE* pifile = fopen(iFileName, "rb");    //pointer input file
     FILE* pofile = fopen(oFileName, "wb");    //pointer output file

     //judge file format
     WORD fileType;
     fread(&fileType, 1, sizeof(WORD), pifile);
     if (fileType != 0x4d42)
     {
         printf("This file is not a .bmp file!");
         return -1;
     }

     //read input file head
     rewind(pifile);
     fread(&ibmfh, sizeof(BITMAPFILEHEADER), 1, pifile);
     fread(&ibmih, sizeof(BITMAPINFOHEADER), 1, pifile);

     //write output file head
     obmfh = ibmfh;
     obmih = ibmih;
     int oWidthByte = WIDTHBYTES(obmih.biWidth*obmih.biBitCount);
     obmih.biSizeImage = oWidthByte * obmih.biHeight;
     obmfh.bfSize = obmfh.bfOffBits + obmih.biSizeImage;
     fwrite(&obmfh, 1, sizeof(BITMAPFILEHEADER), pofile);
     fwrite(&obmih, 1, sizeof(BITMAPINFOHEADER), pofile);
    
     //save palette
     size_t Offbit_File_Info = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
     BYTE *pPale = (BYTE *)malloc(ibmfh.bfOffBits-Offbit_File_Info);    //pointer palette
     fread(pPale, 1, ibmfh.bfOffBits-Offbit_File_Info, pifile);
     fwrite(pPale, 1, ibmfh.bfOffBits-Offbit_File_Info, pofile);

     //read color data
     unsigned int iWidthByte = WIDTHBYTES(ibmih.biWidth * ibmih.biBitCount);
     BYTE *piColorData = (BYTE *)malloc(ibmih.biSizeImage);
     fread(piColorData, 1, ibmih.biSizeImage, pifile);

     //bilinear interpolation resize
     BYTE *poColorData = (BYTE *)malloc(obmih.biSizeImage);
     clock_t start_t = clock();
     median_filter(piColorData,poColorData,obmih.biHeight,obmih.biWidth,filter_size);
     clock_t end_t = clock();
     printf("Filtering took %d cycles\n",end_t - start_t);
     fwrite(poColorData, 1, obmih.biSizeImage, pofile);
     printf("finish.\n");
     fclose(pifile);
     fclose(pofile);
     return 0;
 }

clock_t clock() {
    unsigned int low = TSCL;
    unsigned int high = TSCH;
    if (high) return (clock_t) -1;
    return low;
}

