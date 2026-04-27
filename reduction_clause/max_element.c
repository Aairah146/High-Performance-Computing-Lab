#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
int main(){
	int n,maxi=0,i;
	printf("Enter the size of the collection: ");
	scanf("%d",&n);
	int *arr=(int *)malloc(sizeof(int)*n);
	printf("Enter the elements in the collection: \n");
	for(i=0;i<n;i++){
		scanf("%d",&arr[i]);
	}
	#pragma omp parallel for reduction(max:maxi)
	for(i=0;i<n;i++){
		if (arr[i]>maxi){
			maxi=arr[i];
		}
	}
	printf("The maximum element in the collection: %d\n",maxi);
	free(arr);
	return 0;
}
