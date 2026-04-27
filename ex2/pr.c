#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>


int get_Det(int **mat, int n){
    double det = 1;
    int i, j, k;
    for(i=0;i<n;i++){
        for(j=i+1;j<n;j++){
            double factor = (double)mat[j][i]/mat[i][i];
            for(k=i;k<n;k++)
                mat[j][k] -= factor*mat[i][k];
        }
        det *= mat[i][i];
    }
    return (int)det;
}

int main(){
   int m1, n1, m2, n2;
   int i, j, k;
   int sum, sid;
   double *max;
   sid=shmget(2063,4*sizeof(double),IPC_CREAT|0777);
   max=(double*)shmat(sid,0,0);
   printf("\nEnter matrixA size [m,n] : ");
   scanf("%d %d", &m1, &n1);
   printf("Enter matrixB size [m,n] : ");
   scanf("%d %d", &m2, &n2);

   if(!(m1 == m2 && n1==n2)){
       printf("Matrix size invalid for addition and subtraction!");
       return 0;
   }
   if(!(m2 == n1)){
       printf("Matrix size invalid for multiplication!");
       return 0;
   }
   if(!(m1 == n1 && m2 == n2)){
       printf("Matrix size is invalid for determinant!");
       return 0;
   }

   int **A = (int **)malloc(m1*sizeof(int*));
   int **B = (int **)malloc(m2*sizeof(int*));
   int **C = (int **)malloc(m1*sizeof(int*));

   for(i=0;i<m1;i++)
      A[i] = (int *)malloc(n1*sizeof(int));
   for(i=0;i<m2;i++)
      B[i] = (int *)malloc(n2*sizeof(int));
   for(i=0;i<m1;i++)
      C[i] = (int *)malloc(n2*sizeof(int));

   for(i=0;i<m1;i++)
      for(j=0;j<n1;j++)
        A[i][j]=rand()%5;

   for(i=0;i<m2;i++)
      for(j=0;j<n2;j++)
        B[i][j]=rand()%5;

   int pid1=fork();
   if(pid1==0){ //child
        clock_t start=clock();
        for(i=0;i<m1;i++){
            for(j=0;j<n1;j++){
                C[i][j]=A[i][j]+B[i][j];
            }
        }
        clock_t end=clock();
        double time = ((double)(end-start)*1000.0/CLOCKS_PER_SEC);
        max[0]=time;
        exit(0);
    }
    int pid2=fork();
    if(pid2==0){ //child
        clock_t start=clock();
        for(i=0;i<m1;i++){
            for(j=0;j<n1;j++){
                C[i][j]=A[i][j]-B[i][j];
            }
        }
        clock_t end=clock();
        double time = ((double)(end-start)*1000.0/CLOCKS_PER_SEC);
        max[1]=time;
        exit(0);
    }
    int pid3=fork();
    if(pid3==0){ //child
        clock_t start=clock();
        for(i=0;i<m1;i++){
            for(j=0;j<m2;j++){
                sum=0;
                for(k=0;k<n1;k++){
                    sum+=A[i][k]*B[k][j];
                }
                C[i][j]=sum;
            }
        }
        clock_t end=clock();
        double time = ((double)(end-start)*1000.0/CLOCKS_PER_SEC);
        max[2]=time;
        exit(0);
    }
    int pid4=fork();
    if(pid4==0){ //child
        clock_t start=clock();
        clock_t end=clock();
        double time = ((double)(end-start)*1000.0/CLOCKS_PER_SEC);
        max[3]=time;
        exit(0);
    }
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

   double max_t=max[0];
   for(i=1;i<4;i++){
      if(max_t<max[i])
         max_t=max[i];
   }
   printf("\nTime taken : %f\n\n", max_t);
   shmdt(max);
   shmctl(sid,IPC_RMID,0);
}
