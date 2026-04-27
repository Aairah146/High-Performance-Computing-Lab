#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    int i,N,CONST_VAL;
    int *A;

    printf("Enter no of elements:");
    scanf("%d",&N);
    A = (int*) malloc(N * sizeof(int)); 
    for(i = 0; i < N; i++) {
        A[i] = rand()%10;
    }
    printf("Enter const value to be added: ");
    scanf("%d",&CONST_VAL);

    printf("\nInput taken : ");
    for(i = 0;i < N;i++){
            printf("%d ",A[i]);
    }    
    #pragma omp parallel for
    for(i = 0; i < N; i++) {
        A[i] = A[i] + CONST_VAL;
    }    
    printf("\nOutput given: ");
    for(i = 0; i < N; i++) {
        printf("%d ", A[i]);
    }

    printf("\n");
    free(A);

    return 0;
}
