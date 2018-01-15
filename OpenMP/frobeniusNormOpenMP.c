#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <sodium.h>

#define N 5
#define M 6
#define UPPER_BOUND 20
#define CHUNK_SIZE 100
#define NUM_THREADS 3
#define SCHEDULER_INDEX 0
int i,j;

int main(int argc, char**argv){
  
  int localn=N;
  int localm=M;
  //Scheduler array to change scheduler easily..
  omp_sched_t schedulers[3]={omp_sched_static,omp_sched_dynamic,omp_sched_guided};
  unsigned long long int sum=0;
  int frobArr[N][M];
 
  for(i=0;i<N;i++){
    for(j=0;j<M;j++){
      //
    frobArr[i][j]=randombytes_uniform(UPPER_BOUND)+1;
    }
  }
  //Set current scheduler.
  omp_set_schedule(schedulers[SCHEDULER_INDEX],CHUNK_SIZE);
  #pragma omp parallel shared(frobArr,localn,localm) num_threads(NUM_THREADS) private(i,j) reduction(+: sum)
  {
    #pragma omp for 

      for (i = 0; i < localn; i++){
      for(j=0;j<localm;j++){
        sum+=frobArr[i][j]*frobArr[i][j];
      }
      }
  }
  printf("%.2f\n",sqrt(sum));
  return 0;
}
