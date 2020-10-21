#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
 
#define BUF_SIZE 1024
#define INT_SZ 4

 //display errors
void error_handling(char *message);
// analyzes the message and also compare it with the random value
int  client_msg_analysis(int msg_len, int rll_msg[], char* rator,int rnd_rand_number);
 

int main(int argc, char *argv[]) {
	
	// server and client socket
	int sver_socket, clnt_socket;
	char msg_to_clnt[BUF_SIZE];
	int result, msg__clnt;
	int recv_msg_cnt, recv_msg_len;
	int i=0;
	
	// random number changes with time
	srand( (unsigned)time( NULL ) );
	int rand_number = (rand() % (100- 1 + 1)) + 1; 
	
	//socket address	
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	
	
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	// creating the server socket
	sver_socket=socket(PF_INET, SOCK_STREAM, 0);
	if(sver_socket == -1)
		error_handling("socket() error");
	// memory allocation of server address and initializing it to zero
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	//bind the server address and port on connect
	if(bind(sver_socket, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1 )
		error_handling("bind() error"); 
	//listens for connections and sets a backlog
	if(listen(sver_socket, 5)==-1)
		error_handling("listen() error");
	
	clnt_adr_sz=sizeof(clnt_adr);  
	int clint = 1;
	// 5 loop connection 
	// iterative connection
	for(  i=0; i<5; i++) {
		int dis = 1;
		//keeps the server in a loop with the client until a client leaves
		while(1){
		msg__clnt=0;
		//accepts connection
		clnt_socket=accept(sver_socket, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		
		//write random value to client 
		//show after client guess right
		write(clnt_socket, &rand_number , sizeof(rand_number));
		if(clnt_socket==-1)
		   error_handling("accept() error!");
		else{
		if(dis==1){
		   printf("\nClient %d connected..\n",i+1);dis++;
		   printf("A random number generated:  %d \n",rand_number);
		   }}
		
		//read message sent from client to server   
		read(clnt_socket, &msg__clnt, 1);
		 
		// displays the answers from client
		printf("\nClient %d 's answers: ",i+1);
		recv_msg_len=0;
		while((msg__clnt*INT_SZ+1)>recv_msg_len){	  
			recv_msg_cnt=read(clnt_socket, &msg_to_clnt[recv_msg_len], BUF_SIZE-1);
			recv_msg_len+=recv_msg_cnt;
		}
		 
		 
		// takes the message length 
		// takes the message from client
		//and the random number for operation
		result=client_msg_analysis(msg__clnt, (int*)msg_to_clnt, msg_to_clnt, rand_number);
		
	 	// if the last index 2 is 1// then hit confirmed//
		// note the client might input correct answer more than once in the input circle
		 if(msg_to_clnt[2]=='1'||msg_to_clnt[2]=='2'||msg_to_clnt[2]=='3'){
		 	//generate random value for the next loop
			rand_number = (rand() % (100- 1 + 1)) + 1;
			dis=1;
			//write the answers to client
			write(clnt_socket, &msg_to_clnt , sizeof(msg_to_clnt));
			//break;
			char state ;
			//read the reply from client if the wanna play again or not
			read(clnt_socket, &state, sizeof(state));
			 
			if(state=='y'||state=='Y'){
			break;
			}
			//clint++;
			 
			}
			// send the message to the client even if they did not guess right
			write(clnt_socket, &msg_to_clnt , sizeof(msg_to_clnt));
		//write(clnt_socket, (char*)&result , sizeof(result));
		
		}
		
	 printf("\nClient%d left..\n ",i+1);
		
	}
	close(clnt_socket);
	
	close(sver_socket);

	return 0;
}
int client_msg_analysis(int msg_len, int rll_msg[], char* op,int rnd_rand_number  ) {
 
	int i;
	//Up: 1, Down: 1, Hit: 
	char determiner[3][5] ={"Down","Up","Hit"};
	  
	int udh[3] = {0};
	
	// client
	 
	 for(i=0; i<msg_len; i++) {
	 	printf(" %d \t",rll_msg[i]);
	 }	
	 printf("\n");
	 
		for(i=0; i<msg_len; i++) {
		     if (rnd_rand_number>rll_msg[i])
		     	udh[0]++;
		     else if (rnd_rand_number<rll_msg[i])
		     	udh[1]++;
		     else if (rnd_rand_number==rll_msg[i])
		     	udh[2]++;
		     	}
	printf("Replay : ");
	 for(i=0;i<sizeof(udh)/sizeof(udh[0]);i++)
	 	printf("%s: %d,  ",determiner[i],udh[i]);	
	 printf("\n");
 
	 for (i = 0 ; i < 3 ; ++i)
	{
   		 op[i] = udh[i] + '0';
	}
	 
	 
	 
	 return (int)udh[2];
	 
	
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

