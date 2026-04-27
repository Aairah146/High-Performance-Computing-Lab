#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h> // Required for gettimeofday

int **create(int r, int c) {
    int **m = (int **)malloc(r * sizeof(int *));
    int i;
    for (i = 0; i < r; i++)
        m[i] = (int *)malloc(c * sizeof(int));
    return m;
}

void manual(int **A, int r, int c) {
   int i,j;
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            scanf("%d", &A[i][j]);
}

void randomFill(int **A, int r, int c) {
   int i,j;
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            A[i][j] = rand() % 10;
}

void parallelMultiply(int **A, int **B, int m, int n, int p) {
    // Shared memory for Result Matrix C
    int shmid1 = shmget(IPC_PRIVATE, sizeof(int) * m * p, IPC_CREAT | 0666);
    int (*C)[p] = shmat(shmid1, NULL, 0);

    // Shared memory for tracking time of each row-process
    int shmid2 = shmget(IPC_PRIVATE, sizeof(double) * m, IPC_CREAT | 0666);
    double *T = shmat(shmid2, NULL, 0);
    int i,j,k;
    for (i = 0; i < m; i++) {
        if (fork() == 0) {
            struct timeval start, end;
            gettimeofday(&start, NULL); // Start precision timer

            for (j = 0; j < p; j++) {
                C[i][j] = 0;
                for (k = 0; k < n; k++)
                    C[i][j] += A[i][k] * B[k][j];
            }

            gettimeofday(&end, NULL); // End precision timer
            
            // Calculate elapsed time in ms
            T[i] = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
            
            shmdt(C);
            shmdt(T);
            exit(0);
        }
    }

    for (i = 0; i < m; i++) wait(NULL);

    // Finding the maximum time taken by a single process (bottleneck)
    double max_time = T[0];
    for (i = 1; i < m; i++)
        if (T[i] > max_time) max_time = T[i];

    printf("\nParallel Completion Time (gettimeofday) = %.3f ms\n", max_time);

    shmdt(C);
    shmdt(T);
    shmctl(shmid1, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);
}

int main() {
    int ch, m, n, p;
    srand(time(NULL));

    while (1) {
        printf("\n--- PARALLEL MATRIX MULTIPLICATION ---");
        printf("\n1. Manual Input");
        printf("\n2. Random Input");
        printf("\n3. Exit");
        printf("\nEnter choice: ");
        if (scanf("%d", &ch) != 1 || ch == 3) break;

        printf("Enter m n p: ");
        scanf("%d %d %d", &m, &n, &p);

        int **A = create(m, n);
        int **B = create(n, p);

        if (ch == 1) {
            printf("Enter Matrix A:\n");
            manual(A, m, n);
            printf("Enter Matrix B:\n");
            manual(B, n, p);
        } else {
            randomFill(A, m, n);
            randomFill(B, n, p);
            printf("Random matrices generated.\n");
        }

        parallelMultiply(A, B, m, n, p);
	int i;
        // Cleanup local pointers
        for(i=0; i<m; i++) free(A[i]);
        for(i=0; i<n; i++) free(B[i]);
        free(A); free(B);
    }
    return 0;
}

