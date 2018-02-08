#ifndef INCLUDED_A1_H
#define INCLUDED_A1_H

typedef struct {
  unsigned char r,g,b;
} RGB;

RGB * readPPM(char* file, int* width, int* height, int* max);
void writePPM(char* file, int width, int height, int max, const RGB *image);

void meanFilter(int width, int height, RGB *image, int n, RGB *result);
void medianFilter(int width, int height, RGB *image, int n, RGB *result);

void pixelMean(RGB *image, int width, int height, RGB *curr, int top, int bottom, int left, int right, int size);
void pixelMedian(RGB *image, int width, int height, RGB *curr, int top, int bottom, int left, int right, int size, int *rs, int *gs, int *bs);
void meanFilterRange(int width, int height, RGB *image, RGB *result, int b, int top, int bottom, int left, int right);
void medianFilterRange(int width, int height, RGB *image, RGB *result, int b, int top, int bottom, int left, int right, int *rs, int *gs, int *bs);
		 
#endif
