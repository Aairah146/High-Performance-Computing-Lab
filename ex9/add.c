#include<stdio.h>
#include<omp.h>
#define SIZE 1000000
int main(){
   int A[SIZE],B[SIZE],C[SIZE];
   thread_count=strl
   int i;
   for(i=0;i<SIZE;i++){
      A[i]=i;
      B[i]=i*2;
   }
#pragma omp parallel for
   for(i=0;i<SIZE;i++){
      C[i]=A[i]+B[i];
   }
   printf("First 10 elements of result\n");
   for(i=0;i<10;i++){
       printf("%d",C[i]);
   }
   return 0;
}
