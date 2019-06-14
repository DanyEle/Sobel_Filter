//============================================================================
// Name        : main.c
// Author      : Daniele Gadler
// Version     :
// Description : Sobel operator in native C
// Credits to  : Pedro Melgueira
//============================================================================


#include <stdio.h>
#include <stdlib.h>
#include "file_operations.h"
#include "image_operations.h"
#include "math.h"
#include "string.h"

typedef unsigned char byte;

#define STRING_BUFFER_SIZE 1024



int main( int argc, char** argv )
{

	//###########1. STEP - LOAD THE IMAGE, ITS HEIGHT, WIDTH AND CONVERT IT TO RGB FORMAT#########ÀÀ
	//NB: Only support square pictures
	char * fileInputName = "imgs_in/lena.png";
	char * spaceDiv = " ";
	char * fileInputRGB = "imgs_in/lena.rgb";

	char *pngStrings[4];
	pngStrings[0] = "convert ";
	pngStrings[1] = fileInputName;
	pngStrings[2] = spaceDiv;
	pngStrings[3] = fileInputRGB;

	char * strPngToRGB = arrayStringsToString(pngStrings, 4, STRING_BUFFER_SIZE);

	printf("Loading input image [%s] \n", fileInputName);

	//actually execute the conversion from PNG to RGB, as that format is required for the program
	int status_conversion = system(strPngToRGB);

	if(status_conversion != 0)
	{
		printf("Conversion of input PNG image to RGB was not successful. Program aborting.");
		return -1;
	}
	printf("Converted input image to RGB [%s] \n", fileInputRGB);

	//get the height and width of the input image
	int width = 0;
	int height = 0;

	getImageSize(fileInputName, &width, &height);

	printf("Size of the loaded image : width=%d height=%d \n", width, height);

	//Three dimensions because the input image is in colored format(R,G,B)
	int rgb_size = width * height * 3;
	printf("Total amount of pixels in RGB input image is [%d] \n", rgb_size);
	//Used as a buffer for all pixels of the image
	byte * rgbImage;

	//Load up the input image in RGB format into one single flattened array (rgbImage)
	readFile(fileInputRGB, &rgbImage, rgb_size);

	//#########2. STEP - CONVERT IMAGE TO GRAY-SCALE #################À
	byte * grayImage;
	int gray_size  = rgbToGray(rgbImage, &grayImage, rgb_size);
	char * file_gray = "imgs_out/img_gray.gray";

	writeFile(file_gray, grayImage, gray_size);
	printf("Total amount of pixels in gray-scale image is [%d] \n", gray_size);

	char * file_png_gray = "imgs_out/img_gray.png";

	pngStrings[0] = "convert -size 512x512 -depth 8 ";
	pngStrings[1] = file_gray;
	pngStrings[2] = spaceDiv;
	pngStrings[3] = file_png_gray;

	char * strGrayToPNG = arrayStringsToString(pngStrings, 4, STRING_BUFFER_SIZE);
	system(strGrayToPNG);

	printf("Converted gray image to PNG [%s] \n ", file_png_gray);

	//######################3. Step - Compute vertical and horizontal gradient ##########
    byte * sobel_h_res;
    byte * sobel_v_res;

    //kernel for the horizontal axis
    int sobel_h[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};

    itConv(grayImage, gray_size, width, sobel_h, &sobel_h_res);
    //output the horizontal axis-gradient to a file
    char * file_out_h_grad = "imgs_out/sobel_horiz_grad.gray";

    //Output the horizontal axis' gradient calculation
    writeFile(file_out_h_grad, sobel_h_res, gray_size);

    printf("Output horizontal gradient to [%s] \n", file_out_h_grad);

    char * fileHorGradPNG = "imgs_out/sobel_horiz_grad.png";

    printf("Converted horizontal gradient: ");
	printf("[%s] \n", fileHorGradPNG);

    //Convert the output file to PNG

    pngStrings[0] = "convert -size 512x512 -depth 8 ";
    pngStrings[1] = file_out_h_grad;
    pngStrings[2] = spaceDiv;
    pngStrings[3] = fileHorGradPNG;

    char * strGradToPNG = arrayStringsToString(pngStrings, 4, STRING_BUFFER_SIZE);

	system(strGradToPNG);

    //kernel for the vertical axis
    int sobel_v[] = {1, 2, 1, 0, 0, 0, -1, -2, -1};

    itConv(grayImage, gray_size, width, sobel_v, &sobel_v_res);

    char * file_out_v_grad = "imgs_out/sobel_vert_grad.gray";

    //Output the vertical axis' gradient calculated
    writeFile(file_out_v_grad, sobel_v_res, gray_size);

    printf("Output vertical gradient to [%s] \n", file_out_v_grad);
    char * fileVerGradPNG = "imgs_out/sobel_vert_grad.png";

    pngStrings[0] = "convert -size 512x512 -depth 8 ";
    pngStrings[1] = file_out_v_grad;
    pngStrings[2] = spaceDiv;
    pngStrings[3] = fileVerGradPNG;

    strGradToPNG = arrayStringsToString(pngStrings, 4, STRING_BUFFER_SIZE);
	system(strGradToPNG);

	//#############4. Step - Compute the countour by putting together the vertical and horizontal gradients##

	byte * countour_img;

    contour(sobel_h_res, sobel_v_res, gray_size, &countour_img);

    char * file_sobel_out = "imgs_out/sobel_countour.gray";
    writeFile(file_sobel_out, countour_img, gray_size);
    printf("Output countour to [%s] \n", file_sobel_out);
    char * file_sobel_png = "imgs_out/sobel_countour.png";

    pngStrings[0] = "convert -size 512x512 -depth 8 ";
    pngStrings[1] = file_sobel_out;
    pngStrings[2] = spaceDiv;
    pngStrings[3] = file_sobel_png;

    char * strSobelToPNG = arrayStringsToString(pngStrings, 4, STRING_BUFFER_SIZE);
   	system(strSobelToPNG);

    printf("Converted countour to [%s]", strSobelToPNG);

/*
	int gray_size = sobelFilter(rgbImage, &grayImage, &sobel_h_res, &sobel_v_res, &contour_img, width, height);

	// Write gray image
	char * file_gray = "imgs_out/file_gray.gray";
	writeFile(file_gray, grayImage, gray_size);

	char * file_out_h = "imgs_out/file_h.gray";
	char * file_out_v = "imgs_out/file_v.gray";

	// Write image after each sobel operator
	writeFile(file_out_h, sobel_h_res, gray_size);
	writeFile(file_out_v, sobel_v_res, gray_size);

	char * file_out = "imgs_out/sobel_out.gray";

	// Write sobel img to a file
	writeFile(file_out, contour_img, gray_size);

	return 0;

	*/


}

