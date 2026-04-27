#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
int main(){
	int target,n,i,count=0;
	printf("Enter the size of array: ");
	scanf("%d",&n);
	printf("Enter the target value: ");
	scanf("%d",&target);
	int *arr=(int*)malloc(sizeof(int)*n);
	printf("Enter the values of array: ");
	for(i=0;i<n;i++)
		scanf("%d",&arr[i]);
#pragma omp parallel for reduction (+:count)
	for(i=0;i<n;i++)
		if(arr[i]<target)
			count+=1;
	printf("The total number of values less than target: %d\n",count);
}
