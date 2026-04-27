#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int **create(int r,int c)
{
   int i;
    int **m=(int**)malloc(r*sizeof(int*));
    for(i=0;i<r;i++)
        m[i]=(int*)malloc(c*sizeof(int));
    return m;
}

void manual(int **A,int r,int c)
{
   int i,j;
    for(i=0;i<r;i++)
        for(j=0;j<c;j++)
            scanf("%d",&A[i][j]);
}

void randomFill(int **A,int r,int c)
{
   int i,j;
    for(i=0;i<r;i++)
        for(j=0;j<c;j++)
            A[i][j]=rand()%10;
}

void serialMultiply(int **A,int **B,int **C,int m,int n,int p)
{
    clock_t start=clock();
    int i,j,k;
    for(i=0;i<m;i++)
        for(j=0;j<p;j++)
        {
            C[i][j]=0;
            for(k=0;k<n;k++)
                C[i][j]+=A[i][k]*B[k][j];
        }

    clock_t end=clock();
    printf("\nSerial Time = %.3f ms\n",
           ((double)(end-start)/CLOCKS_PER_SEC)*1000);
}

int main()
{
    int ch,m,n,p;
    srand(time(NULL));

    while(1)
    {
        printf("\n--- SERIAL MATRIX MULTIPLICATION ---");
        printf("\n1. Manual Input");
        printf("\n2. Random Input");
        printf("\n3. Exit");
        printf("\nEnter choice: ");
        scanf("%d",&ch);

        if(ch==3) break;

        printf("Enter m n p: ");
        scanf("%d %d %d",&m,&n,&p);

        int **A=create(m,n);
        int **B=create(n,p);
        int **C=create(m,p);

        if(ch==1)
        {
            printf("Enter Matrix A:\n");
            manual(A,m,n);
            printf("Enter Matrix B:\n");
            manual(B,n,p);
        }
        else
        {
            randomFill(A,m,n);
            randomFill(B,n,p);
            printf("Random matrices generated.\n");
        }

        serialMultiply(A,B,C,m,n,p);
    }
    return 0;
}
