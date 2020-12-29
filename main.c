// /*
//  * main.c
//  */
 #include <stdio.h>
 #include <math.h>
 #include "custombmplib.h"

 const char iFileName[] = "lena2.bmp";
 const char oFileName[] = "result10.bmp";
 const int output_size[2] = {512, 512};	//output HEIGHT & WIDTH

 extern void median_filter(BYTE* psrc, BYTE* pdst, int height, int width, int N);

 int main(void) {
 	BITMAPFILEHEADER ibmfh, obmfh;	//inBitmapFileHead, outBitmapFileHead
     BITMAPINFOHEADER ibmih, obmih;	//inBitmapInfoHead, outBitmapInfoHead
     FILE* pifile = fopen(iFileName, "rb");	//pointer input file
     FILE* pofile = fopen(oFileName, "wb");	//pointer output file

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
 	obmih.biHeight = output_size[0];
 	obmih.biWidth = output_size[1];
 	int oWidthByte = WIDTHBYTES(obmih.biWidth*obmih.biBitCount);
 	fwrite(&obmih, 1, sizeof(BITMAPINFOHEADER), pofile);
	
 	//save palette
 	size_t Offbit_File_Info = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
 	BYTE *pPale = (BYTE *)malloc(ibmfh.bfOffBits-Offbit_File_Info);	//pointer palette
 	fread(pPale, 1, ibmfh.bfOffBits-Offbit_File_Info, pifile);
 	fwrite(pPale, 1, ibmfh.bfOffBits-Offbit_File_Info, pofile);

 	//read color data
 	unsigned int iWidthByte = WIDTHBYTES(ibmih.biWidth * ibmih.biBitCount);
 	BYTE *piColorData = (BYTE *)malloc(ibmih.biSizeImage);
 	fread(piColorData, 1, ibmih.biSizeImage, pifile);

 	//bilinear interpolation resize
 	BYTE *poColorData = (BYTE *)malloc(obmih.biSizeImage);
 	median_filter(piColorData,poColorData,obmih.biHeight,obmih.biWidth,5);
 	fwrite(poColorData, 1, obmih.biSizeImage, pofile);
 	printf("finish.\n");
 	fclose(pifile);
    fclose(pofile);
 	return 0;
 }


