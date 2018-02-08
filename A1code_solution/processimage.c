#include <stdlib.h>
#include "a1.h"

void meanFilter(int width, int height, RGB *image, int n, RGB *result)
{
  int b = (n-1)/2;
  
  // Set inner image pixels
  int i, j;
  RGB *curr = result;
  for (i=b; i < height-b; i++)
    {
      curr = result + i*width + b;
      for(j=b; j < width-b; j++)
  	{
  	  // Find appropriate bounding box for neighbouring pixels (n x n)
  	  int top = i-b;
  	  int left = j-b;
  	  int bottom = i+b;
  	  int right = j+b;
	  
  	  int size = n*n;
  	  pixelMean(image, width, height, curr, top, bottom, left, right, size);
	  
  	  curr++;
  	}
    }
  // Set image border (special cases for sliding window)
  // Top b rows
  meanFilterRange(width, height, image, result, b, 0, b-1, 0, width-1);
  // Bottom b rows
  meanFilterRange(width, height, image, result, b, height-b, height-1, 0, width-1);
  // Left b columns
  meanFilterRange(width, height, image, result, b, b, height-b-1, 0, b);
  // Right b columns
  meanFilterRange(width, height, image, result, b, b, height-b-1, width-b-1, width-1);
}

void medianFilter(int width, int height, RGB *image, int n, RGB *result)
{
  // Border width for neighbours window
  int b = (n-1)/2;
  
  // RGB arrays for median calculation
  int *rs, *gs, *bs;
  rs = malloc(sizeof(int) * n * n);
  gs = malloc(sizeof(int) * n * n);
  bs = malloc(sizeof(int) * n * n);
  
  // Set inner image pixels
  int i, j;
  RGB *curr = result;
  for (i=b; i < height-b; i++)
    {
      curr = result + i*width + b;
      for(j=b; j < width-b; j++)
	{
	  // Find appropriate bounding box for neighbouring pixels (n x n)
	  int top = i-b;
	  int left = j-b;
	  int bottom = i+b;
	  int right = j+b;
	  
	  int size = n*n;
	  
	  pixelMedian(image, width, height, curr, top, bottom, left, right, size, rs, gs, bs);
	  curr++;
	}
    }
  
  // Set image border (special cases for sliding window)
  // Top b rows
  medianFilterRange(width, height, image, result, b, 0, b-1, 0, width-1, rs, gs, bs);
  // Bottom b rows
  medianFilterRange(width, height, image, result, b, height-b, height-1, 0, width-1, rs, gs, bs);
  // Left b columns
  medianFilterRange(width, height, image, result, b, b, height-b-1, 0, b, rs, gs, bs);
  // Right b columns
  medianFilterRange(width, height, image, result, b, b, height-b-1, width-b-1, width-1, rs, gs, bs);
  
  free(rs);
  free(gs);
  free(bs);
}

inline int max(int a, int b)
{
  return a > b ? a : b;
}

inline int min(int a, int b)
{
  return a < b ? a : b;
}

int compare (const void * a, const void * b)
{
  return (*(int*)a - *(int*)b);
}

void pixelMean(RGB *image, int width, int height, 
	       RGB *curr, int top, int bottom, int left, int right, int size)
{
  // Iterate over all neighbouring pixels, including itself,
  // accumulating r, g and b values indpendently
  int s, t;
  int r = 0, g = 0, b = 0;
  RGB *p; // current neighbour
  for (s = top; s <= bottom; s++)
    {
      p = image + s*width + left; // beginning of window row
      for (t = left; t <= right; t++)
	{
	  r += p->r;
	  g += p->g;
	  b += p->b;
	  p++;
	}
    }
  
  // Divide r, g and b values by number of neighbouring pixels
  // and set in result
  curr->r = r / size;
  curr->g = g / size;
  curr->b = b / size;
}

void pixelMedian(RGB *image, int width, int height, 
		 RGB *curr, int top, int bottom, int left, int right, int size, int *rs, int *gs, int *bs)
{
  // Iterate over all neighbouring pixels, including itself,
  // sorting r, g and b values into independent arrays
  int s, t;
  int count = 0;
  RGB *p = image; // current neighbour
  for (s = top; s <= bottom; s++)
    {
      p = image + s*width + left; // beginning of window row
      for (t = left; t <= right; t++)
	{
	  rs[count] = p->r;
	  gs[count] = p->g;
	  bs[count] = p->b;
	  count++;
	  p++;
	}
    }
  
  // Take median r, g and b values of neighbouring pixels
  // and set in result
  qsort(rs, count, sizeof(int), compare);
  qsort(gs, count, sizeof(int), compare);
  qsort(bs, count, sizeof(int), compare);
  
  
  curr->r = rs[count/2];
  curr->g = gs[count/2];
  curr->b = bs[count/2];
}

void meanFilterRange(int width, int height, RGB *image, RGB *result, int bor, int top, int bottom, int left, int right) {
  int i, j;
  RGB *curr = result;
  for (i = top; i <= bottom; i++)
    {
      curr = result + i*width + left;
      for (j = left; j <= right; j++)
        {
	  // Find appropriate bounding box for neighbouring pixels (maximum n x n)
	  int t = max(i-bor, 0);
	  int l = max(j-bor, 0);
	  int b = min(i+bor, height-1);
	  int r = min(j+bor, width-1);
	  
	  int size = (r-l+1) * (b-t+1);

	  pixelMean(image, width, height, curr, t, b, l, r, size);
	  
	  curr++;
	}
    }
}

void medianFilterRange(int width, int height, RGB *image, RGB *result, int bor, int top, int bottom, int left, int right, int *rs, int *gs, int *bs) {
  int i, j;
  RGB *curr = result;
  for (i = top; i <= bottom; i++)
    {
      curr = result + i*width + left;
      for (j = left; j <= right; j++)
        {
	  // Find appropriate bounding box for neighbouring pixels (maximum n x n)
	  int t = max(i-bor, 0);
	  int l = max(j-bor, 0);
	  int b = min(i+bor, height-1);
	  int r = min(j+bor, width-1);
	  
	  int size = (r-l+1) * (b-t+1);

	  pixelMedian(image, width, height, curr, t, b, l, r, size, rs, gs, bs);
	  
	  curr++;
	}
    }
}
