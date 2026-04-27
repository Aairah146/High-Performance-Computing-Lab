#include<stdio.h>
#include<string.h>
#include<mpi.h>

const int MAX_STRING=100;
int main(){
   char msg[100];
   int comm_size;
   int rank;
   MPI_Init(Null,Null);
   MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
   MPI_COMM_rank(MPI_COMM_WORLD,&my_rank);
   int i;
   if(my_rank!=0){
      sprintf(msg,"Hello from process %d",rank);
      MPI_Send(msg,strlen(msg)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
   }
   else{
      printf("Helllo from process %D",rank);
      for(i=1;i<comm_sz;i++){
	 MPI_Recv(msg,100,MPI_CHAR,i,0,MPI_COMM_WORLD,NPI_STATUS_IGNORE);
	 printf("%s\n",msg);
      }
   }
   MPI_Finalise();
}

