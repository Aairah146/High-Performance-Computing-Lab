#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h> 

int get_Det(int **mat, int n) {
    double det = 1;
    int i, j, k;
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (mat[i][i] == 0) continue; 
            double factor = (double)mat[j][i] / mat[i][i];
            for (k = i; k < n; k++)
                mat[j][k] -= factor * mat[i][k];
        }
        det *= mat[i][i];
    }
    return (int)det;
}

int main() {
    int m1, n1, m2, n2;
    int i, j, k;
    int sum, sid;
    double *max_times;
    struct timeval start, end;
    sid = shmget(2063, 4 * sizeof(double), IPC_CREAT | 0777);
    max_times = (double *)shmat(sid, 0, 0);

    printf("\nEnter matrixA size [m,n] : ");
    scanf("%d %d", &m1, &n1);
    printf("Enter matrixB size [m,n] : ");
    scanf("%d %d", &m2, &n2);

    if (!(m1 == m2 && n1 == n2)) {
        printf("Matrix size invalid for addition and subtraction!\n");
        return 0;
    }
    if (!(n1 == m2)) {
        printf("Matrix size invalid for multiplication!\n");
        return 0;
    }
    int **A = (int **)malloc(m1 * sizeof(int *));
    int **B = (int **)malloc(m2 * sizeof(int *));
    int **C = (int **)malloc(m1 * sizeof(int *));
    for (i = 0; i < m1; i++) A[i] = (int *)malloc(n1 * sizeof(int));
    for (i = 0; i < m2; i++) B[i] = (int *)malloc(n2 * sizeof(int));
    for (i = 0; i < m1; i++) C[i] = (int *)malloc(n2 * sizeof(int));
    srand(time(NULL));
    for (i = 0; i < m1; i++) for (j = 0; j < n1; j++) A[i][j] = rand() % 5;
    for (i = 0; i < m2; i++) for (j = 0; j < n2; j++) B[i][j] = rand() % 5;

    
    if (fork() == 0) {
        gettimeofday(&start, NULL);
        for (i = 0; i < m1; i++)
            for (j = 0; j < n1; j++)
                C[i][j] = A[i][j] + B[i][j];
        gettimeofday(&end, NULL);
        max_times[0] = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
        exit(0);
    }

    
    if (fork() == 0) {
        gettimeofday(&start, NULL);
        for (i = 0; i < m1; i++)
            for (j = 0; j < n1; j++)
                C[i][j] = A[i][j] - B[i][j];
        gettimeofday(&end, NULL);
        max_times[1] = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
        exit(0);
    }

    
    if (fork() == 0) {
        gettimeofday(&start, NULL);
        for (i = 0; i < m1; i++) {
            for (j = 0; j < n2; j++) {
                sum = 0;
                for (k = 0; k < n1; k++) sum += A[i][k] * B[k][j];
                C[i][j] = sum;
            }
        }
        gettimeofday(&end, NULL);
        max_times[2] = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
        exit(0);
    }

    
    if (fork() == 0) {
        gettimeofday(&start, NULL);
        if (m1 == n1) {
            get_Det(A, m1);
        }
        gettimeofday(&end, NULL);
        max_times[3] = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
        exit(0);
    }

    
    for (i = 0; i < 4; i++) wait(NULL);

   
    double max_t = max_times[0];
    for (i = 1; i < 4; i++) {
        if (max_times[i] > max_t) max_t = max_times[i];
    }

    printf("\nParallel Execution Time (gettimeofday) : %.3f ms\n\n", max_t);

    shmdt(max_times);
    shmctl(sid, IPC_RMID, 0);
    return 0;
}

