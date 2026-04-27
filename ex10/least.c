#include<stdio.h>
#include<omp.h>
int main(){
   int n;
   printf("Enter the number of elements in the array:");
   scanf("%d",&n);
   int *arr=(int *)malloc(n*sizeof(n));
   printf("Enter the array elements:");
   int i;
   for(i=0;i<n;i++){
      scanf("%d",&arr[i]);
   }
   int t;
   printf("Enter the target value:");
   scanf("%d",&t);
   printf("The array elements are:");
   for(i=0;i<n;i++){
      printf("%d ",arr[i]);
   }
   int count=0;
#pragma omp parallel for reduction(+:count)
   for(i=0;i<n;i++){
      if(arr[i]<t){
	 count++;
      }
   }
   printf("\nThe number of values less than the %d is %d.\n",t,count);
   free(arr);
   return 0;
}
