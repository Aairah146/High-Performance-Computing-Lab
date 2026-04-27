#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h> // Required for gettimeofday

// Function to dynamically allocate a 2D matrix
int **create(int r, int c) {
    int i;
    int **m = (int **)malloc(r * sizeof(int *));
    for (i = 0; i < r; i++)
        m[i] = (int *)malloc(c * sizeof(int));
    return m;
}

// Function for manual matrix input
void manual(int **A, int r, int c) {
    int i, j;
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            scanf("%d", &A[i][j]);
}

// Function to fill matrix with random numbers (0-9)
void randomFill(int **A, int r, int c) {
    int i, j;
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            A[i][j] = rand() % 10;
}

// Function to perform multiplication and measure time using gettimeofday
void serialMultiply(int **A, int **B, int **C, int m, int n, int p) {
    struct timeval start, end;
    
    // Start measuring time
    gettimeofday(&start, NULL);

    int i, j, k;
    for (i = 0; i < m; i++) {
        for (j = 0; j < p; j++) {
            C[i][j] = 0;
            for (k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // End measuring time
    gettimeofday(&end, NULL);

    // Calculate elapsed time in milliseconds
    // tv_sec is seconds, tv_usec is microseconds
    double seconds = (double)(end.tv_sec - start.tv_sec);
    double microseconds = (double)(end.tv_usec - start.tv_usec);
    double elapsed_ms = (seconds * 1000.0) + (microseconds / 1000.0);

    printf("\nSerial Time (gettimeofday) = %.3f ms\n", elapsed_ms);
}

int main() {
    int ch, m, n, p;
    srand(time(NULL));

    while (1) {
        printf("\n--- SERIAL MATRIX MULTIPLICATION ---");
        printf("\n1. Manual Input");
        printf("\n2. Random Input");
        printf("\n3. Exit");
        printf("\nEnter choice: ");
        if (scanf("%d", &ch) != 1) break;

        if (ch == 3) break;

        printf("Enter m n p (dimensions): ");
        scanf("%d %d %d", &m, &n, &p);

        int **A = create(m, n);
        int **B = create(n, p);
        int **C = create(m, p);

        if (ch == 1) {
            printf("Enter Matrix A (%dx%d):\n", m, n);
            manual(A, m, n);
            printf("Enter Matrix B (%dx%d):\n", n, p);
            manual(B, n, p);
        } else {
            randomFill(A, m, n);
            randomFill(B, n, p);
            printf("Random matrices generated.\n");
        }

        serialMultiply(A, B, C, m, n, p);

        // Optional: Free allocated memory to prevent leaks
        for (int i = 0; i < m; i++) free(A[i]);
        for (int i = 0; i < n; i++) free(B[i]);
        for (int i = 0; i < m; i++) free(C[i]);
        free(A); free(B); free(C);
    }
    return 0;
}

