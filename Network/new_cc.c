#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>

#define RLT_SIZE 4
#define INT_SZ  4
#define BUF_SIZE 1024

//for displaying errors
void error_handling(char *message);
//displays the head message
void head(char * s1,char* s2);

int main(int argc, char *argv[]) {
	 
	 // for percent count of client trial
	int count_number_of_operations =0;
	char answer='y';
	int sock;
	int checkT = 1;
	//random value from server
	int randvalue= 0;
	
	//continues loop
	while(1){
	
	char clnt_msg[BUF_SIZE];  //message from client
	char result[BUF_SIZE] ;   //result received from server after calculations
	//counts the number of input of client
	int clnt_input_cnt, i;
	
	
	struct sockaddr_in serv_adr;
	if(argc!=3){
	          printf("Usage : %s <IP> <port>\n", argv[0]);
	          exit(1);
	}
	
	//creating a tcp connection
	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");
		
	// memory allocation of server address and initializing it to zero
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	// connecting to server //else section prints the header on the first run
	if(connect(sock, (struct sockaddr*)&serv_adr, 	sizeof(serv_adr))==-1) 
		error_handling("connect() error!");
	else if(checkT==1)	
		head(argv[1],argv[2]);
		checkT++;
	 
	//read random value from server
	read(sock, &randvalue, sizeof(randvalue)); 
	
	// stdin for number of answers to input
	fputs("# of your answers?> ", stdout);
	scanf("%d", &clnt_input_cnt);
	// cast the count as first index of the message to be sent
	clnt_msg[0]=(char)clnt_input_cnt;
	
	//loop the count and stdin the values
	// from 1,5,9 index because the size is 4bytes for each integer
	
	for(i=0; i<clnt_input_cnt; i++) {
		printf("Answer %d > ", i+1);
		scanf("%d", (int*)&clnt_msg[i*INT_SZ+1]);	
		
		count_number_of_operations++;   //increment for the final division
	}

	 
	//write message to server
	write(sock, clnt_msg, clnt_input_cnt*INT_SZ+2);
	 
	 // read message from server after the values received has been checked
	 // message of length 3 is received from server.
	read(sock, &result, RLT_SIZE);	
			 
	//Up: 1, Down: 1, Hit: 
	char determiner[3][5] ={"Down","Up","Hit"};
	printf("\n[Replay from Server]\n"); 
	// prints the values received in order of down,up and hit	
	for(i=0;i< 3;i++)
	 	printf("%s: %c,  ",determiner[i],result[i]);	
	 printf("\n");
	 
	 // if the last index 2 is 1// then hit confirmed//
	 // note the client might input correct answer more than once in the input circle
	 if(result[2]=='1'||result[2]=='2'||result[2]=='3'){
	 printf("\nYou’ve got the right answer!!\nThe random value was %d\n",randvalue);
	 printf("Your score is %d/%d=%d.\n\n",100,count_number_of_operations,(int)(100/count_number_of_operations));
	 count_number_of_operations = 0;
	 // buffer holds for next input
	 fgetc(stdin);
	 fputs("Try again? (y/n) ", stdout);  //determines if they wanna play again or not
	 scanf("%c", &answer);
	 if(answer=='y'||answer=='Y'){
	 	//head(argv[1],argv[2]);
	 	write(sock, &answer, sizeof(answer));
	 	checkT = 1;
	 	continue;}
	 else{
	 	break;}
	 }
	 printf("\n");
	 //
	

 	 };
 	 
 	 //closing the socket created above
	close(sock);

	return 0;
}

void head(char * s,char * s2){
		puts("Welcome!\nThis is Guess Number Game Server.");
		printf("(%s : %s)\n",s,s2);
		printf("\n");
}
	
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

