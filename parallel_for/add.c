#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    int i,j,N;
    int *A,*B,*C;

    printf("Enter size of array:");
    scanf("%d",&N);
    A = (int*) malloc(N * sizeof(int));
    B = (int*) malloc(N * sizeof(int));
    C = (int*) malloc(N * sizeof(int));

    for(i = 0; i < N; i++){
	    A[i]=rand()%5;
	    B[i]=rand()%5;
    }
    printf("Input:\nA: ");
    for(i=0;i<N;i++){
	    printf("%d ",A[i]);
    }
    printf("\nB: ");
    for(i=0;i<N;i++){
	    printf("%d ", B[i]);
    }
    #pragma omp parallel for 
    for(i = 0; i < N; i++) {
	    C[i] = A[i] + B[i];
    }
    printf("\nOutput:\nC: ");    
    for(i = 0; i < N; i++) {
	    printf("%d ",C[i]);
    }
    printf("\n");
    free(A);

    return 0;
}
