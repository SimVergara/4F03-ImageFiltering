#include <stdlib.h>
#include <string.h>
#include "a1.h"

#include <mpi.h>

int main(int argc, char** argv)
{
  int myRank, p;
  MPI_Status status;  

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  
  //read image
  RGB *image;
  RGB *result;
  int width, height, max;  
  int n;

  if(myRank == 0){
    n = atoi(argv[3]);
    image = readPPM(argv[1], &width, &height, &max);
    result = (RGB*)malloc(sizeof(RGB)*width*height);
  }
  
  //distribute our file parameters
  int buffer[4];
  if(myRank==0){
    buffer[0] = n;
    buffer[1] = width;
    buffer[2] = height;
    buffer[3] = max;
  }

  MPI_Bcast(&buffer, 4, MPI_INTEGER, 0, MPI_COMM_WORLD);

  if(myRank!=0){
    n = buffer[0];
    width = buffer[1];
    height = buffer[2];
    max = buffer[3];
  }
  
  //figure out how many items to send to each processor
  int *rows = (int*)malloc(sizeof(int)*p);
  int i; 
  //take care of even division
  for(i = 0; i < p; i++)
    rows[i] = height/p;
  //take care of the remainder
  int bound = height%p;
  for(i = 0; i < bound; i++)
    rows[i]++;
  
  //figure out the border
  int border = (n-1)/2;

  //send image portions to each node
  if(myRank ==0){
    int currentRow = 0;
    for(i = 1; i< p; i++){
      currentRow+=rows[i-1];
      MPI_Send(image+(currentRow*width)-(border*width), 
	       sizeof(RGB)*(width*rows[i]+2*border*width), 
	       MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
  }
  //we are a client and need to get data
  else{
    //allocate enough memory for the image transfer
    image = (RGB*)malloc(sizeof(RGB)*(width*rows[myRank]+2*border*width));
    result = (RGB*)malloc(sizeof(RGB)*(width*rows[myRank]+2*border*width));
    MPI_Recv(image, sizeof(RGB)*(width*rows[myRank]+2*border*width), 
	     MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
  }
  
  //process in parallel
  //modify row count to account for border
  if ( p > 1 )
    {
      rows[0] += border;
      rows[p-1] +=border;
    }
  for(i = 1; i < p-1;i++)
    rows[i]+=2*border;

  //perform either average or median filtering
  if (strcmp(argv[4], "M") == 0)
    medianFilter(width, rows[myRank], image, n, result);
  else
    meanFilter(width, rows[myRank], image, n, result);
  
  //return row count correctly
  if ( p > 1 )
    {
      rows[0] -= border;
      rows[p-1] -=border;
    }
  
  for(i = 1; i < p-1;i++)
    rows[i]-=2*border;
  
  //gather image
  //await data from nodes
  if(myRank == 0){
    int currentRow = 0;
    for(i = 1; i< p; i++){
      currentRow+=rows[i-1];
      MPI_Recv(result+(currentRow*width), sizeof(RGB)*width*rows[i], 
	       MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
    }
  }
  //send data to root
  else
    MPI_Send(result+width*border, sizeof(RGB)*width*rows[myRank], 
	     MPI_CHAR, 0, 0, MPI_COMM_WORLD);

  free(rows);
  free(image);
  
  //root writes image
  if(myRank == 0){
    writePPM(argv[2], width, height, max, result);
  }
  
  free(result);
  MPI_Finalize();

  return(0);
}
