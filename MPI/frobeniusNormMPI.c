#include <stdio.h>
#include <sodium.h>
#include <math.h>
#include "mpi.h"

#define N 1000
#define M 1000
#define UPPER_BOUND 100
#define MASTER 0
#define MY_MPI_TAG 1

/*

Frobenius implementation in OpenMPI using blocking send/receive.
*/
int main(int argc, char**argv){

  int mypid;
  int numprocs;
  int frobArr [N][M];
  int local_row;
  int* arrSizes;
  int* barrage;
  int i,j;
  unsigned long long int local_sum,global_sum,limit;
  MPI_Request *req;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&mypid);
  if(mypid==MASTER){
    //Initialization.
    for(i=0;i<N;i++){
      for(j=0;j<M;j++){
        frobArr[i][j]=randombytes_uniform(UPPER_BOUND)+1;
      }
    }

    int divider,remainder;
    int distributor=N;
    int splitter=0;

   
    //Allocation.
    arrSizes=malloc(numprocs * sizeof(int));
    barrage=malloc(numprocs * sizeof(int));

    for(i=0;i<numprocs;i++){
      arrSizes[i]=0;
    }
    //Local array sizes to distribute rows almost equally (Work sharing).
    while(distributor!=0){
      arrSizes[(splitter%(numprocs-1))+1]+=1;
      distributor--;
      splitter++;
    }

    barrage[0]=0;
    //Setting row intervals will be given to each processor.
    for(i=1;i<numprocs;i++){
      barrage[i]=arrSizes[i]+barrage[i-1];
    }
    global_sum=0;
    
    for(i=0;i<N;i++){

      for(j=0;j<numprocs-1;j++){
        //Assign appropriate row to an appropriate processor.
        if(i>=barrage[j] && i<barrage[j+1]){
          limit=j+1;
          break;
        }

      }
      //Sends rows of matrix to all slave processors.
      MPI_Send(frobArr[i],M,MPI_INT,limit,MY_MPI_TAG,MPI_COMM_WORLD);
    }
  }
//Scatter all local array sizes from master to slaves.
MPI_Scatter(arrSizes,1,MPI_INT,&local_row,1,MPI_INT,MASTER,MPI_COMM_WORLD);

  if(mypid!=MASTER){
    local_sum=0;
    if(local_row!=0){
      int local_arr[local_row][M];
      for(i=0;i<local_row;i++){
        //Receive element.
        MPI_Recv(local_arr[i],M,MPI_INT,MASTER,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      }
      //Local calculation.
      for(i=0;i<local_row;i++){
        for(j=0;j<M;j++){
          local_sum+=local_arr[i][j]*local_arr[i][j];
        }
      }
    
   }
    
  }
  //Reduce result.
  MPI_Reduce(&local_sum,&global_sum,1,MPI_INT,MPI_SUM,MASTER,MPI_COMM_WORLD);
    if(mypid==MASTER){
    printf("%.2f ",sqrt(global_sum));
    }
  MPI_Finalize();

  return 0;
}
