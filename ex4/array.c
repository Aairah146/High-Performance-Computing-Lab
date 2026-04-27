#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>
int main(){
	int rank,size;
	int *array=NULL;
	int *sub_array=NULL;
	int n=10;
	int search_num;
	int local_count=0,total_count=0;
	int element_per_proc;
	int i;
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	element_per_proc=n/size;
	if(rank==0){
		printf("Enter number to find frequency:");
		scanf("%d",&search_num);
		array=(int*)malloc(n*sizeof(int));
		srand(time(NULL));
		printf("Array:\n");
	
		for(i=0;i<n;i++){
			array[i]=rand()%10;
			printf("%d ",array[i]);
		}
		printf("\n");
	}
	MPI_Bcast(&search_num,1,MPI_INT,0,MPI_COMM_WORLD);
	sub_array=(int*)malloc(element_per_proc*sizeof(int));
	MPI_Scatter(array,element_per_proc,MPI_INT,sub_array,element_per_proc,MPI_INT,0,MPI_COMM_WORLD);
	for(i=0;i<element_per_proc;i++){
		if(sub_array[i]==search_num){
			local_count++;
		}
	}
	MPI_Reduce(&local_count,&total_count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
	if(rank==0){
		printf("\nFrequency of %d=%d\n",search_num,total_count);
	}
	free(sub_array);
	if(rank==0){
		free(array);
	}
	MPI_Finalize();
	return 0;
}
