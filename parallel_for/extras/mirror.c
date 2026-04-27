#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
int main(){
	int i,j,n;
	printf("Enter the size of matrix: ");
	scanf("%d",&n);
	int **mat=(int**)malloc(sizeof(int*)*n);
	printf("The generated matrix:\n");
	for (i=0;i<n;i++){
		mat[i]=(int*)malloc(sizeof(int)*n);
		for(j=0;j<n;j++){
			mat[i][j]=rand()%100;
			printf("%d ",mat[i][j]);
		}
		printf("\n");
	}
	int t=n/2;
#pragma omp parallel for
	for(i=0;i<n;i++){
		for(j=0;j<t;j++){
			int temp=mat[i][j];
			mat[i][j]=mat[i][n-1-j];
			mat[i][n-1-j]=temp;
		}
	}
	printf("The mirror form:\n");
	for(i=0;i<n;i++){
		for(j=0;j<n;j++)
			printf("%d ",mat[i][j]);
		printf("\n");
	}
	free(mat);
}
