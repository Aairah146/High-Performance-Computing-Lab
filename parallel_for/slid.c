#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
int main(){
	int i,j,n,s;
	printf("Enter the size of array: ");
	scanf("%d",&n);
	printf("Enter the size of sliding window: ");
	scanf("%d",&s);
	int t=n-s+1;
	int *arr=(int*)malloc(sizeof(int)*n);
	int *sums=(int*)malloc(sizeof(int)*t);
	printf("The generated array: ");
	for(i=0;i<n;i++){
		arr[i]=rand()%100;
		printf("%d ",arr[i]);
	}
	printf("\n");
#pragma omp parallel for
	for(i=0;i<t;i++){
		int locsum=0;
		for(j=i;j<i+s;j++)
			locsum+=arr[j];
		sums[i]=locsum;
	}
	printf("Window sum: ");
	for (i=0;i<t;i++)
		printf("%d ",sums[i]);
	printf("\n");
}
