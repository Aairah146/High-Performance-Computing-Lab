#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>


int get_Det(int **mat, int n) {
    int det = 0;
    int i, j, x;
    if (n == 1) {
        return mat[0][0];
    }
    if (n == 2) {
        return (mat[0][0] * mat[1][1]) - (mat[0][1] * mat[1][0]);
    }
    int **temp = (int **)malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        temp[i] = (int *)malloc(n * sizeof(int));
    }

    for (x = 0; x < n; x++) {
        int subi = 0;
        for (i = 1; i < n; i++) {
            int subj = 0;
            for (j = 0; j < n; j++) {
                if (j == x) continue;
                temp[subi][subj] = mat[i][j];
                subj++;
            }
            subi++;
        }
        int sign = (x % 2 == 0) ? 1 : -1;
        det += sign * mat[0][x] * get_Det(temp, n - 1);
    }
    return det;
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

   printf("\nEnter matrixA:\n");
   for(i=0;i<m1;i++)
      for(j=0;j<n1;j++)
         scanf("%d", &A[i][j]);

   printf("\nEnter matrixB:\n");
   for(i=0;i<m2;i++)
      for(j=0;j<n2;j++)
         scanf("%d", &B[i][j]);

   int pid1=fork();
   if(pid1==0){ //child
        clock_t start=clock();
        printf("\nAddition of Matrix A and B:\n");
        for(i=0;i<m1;i++){
            for(j=0;j<n1;j++){
                C[i][j]=A[i][j]+B[i][j];
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
        clock_t end=clock();
        double time = ((double)(end-start)*1000.0/CLOCKS_PER_SEC);
        max[0]=time;
        exit(0);
    }
    int pid2=fork();
    if(pid2==0){ //child
        clock_t start=clock();
        printf("\nSubtraction of Matrix A and B:\n");
        for(i=0;i<m1;i++){
            for(j=0;j<n1;j++){
                C[i][j]=A[i][j]-B[i][j];
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
        clock_t end=clock();
        double time = ((double)(end-start)*1000.0/CLOCKS_PER_SEC);
        max[1]=time;
        exit(0);
    }
    int pid3=fork();
    if(pid3==0){ //child
        clock_t start=clock();
        printf("\nMultiplication of Matrix A and B:\n");
        for(i=0;i<m1;i++){
            for(j=0;j<m2;j++){
                sum=0;
                for(k=0;k<n1;k++){
                    sum+=A[i][k]*B[k][j];
                }
                C[i][j]=sum;
                printf("%d ",C[i][j]);
            }
            printf("\n");
        }
        clock_t end=clock();
        double time = ((double)(end-start)*1000.0/CLOCKS_PER_SEC);
        max[2]=time;
        exit(0);
    }
    int pid4=fork();
    if(pid4==0){ //child
        clock_t start=clock();
        printf("\nDeterminant of matrixA: %d", get_Det(A,m1));
        printf("\nDeterminant of matrixB: %d", get_Det(B,m2));
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
