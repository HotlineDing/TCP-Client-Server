/**
*David Rivera
*Sept. 25th 2012
*CNT4704
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

int sendall(int , char *, int);
int receiveall(int);

int main(int argc, char* argv[]){
    int socket_desc;

    if(argc < 3){
        fprintf(stderr,"Usage: ./client [hostname] [port]\n");
        exit(1);
    }
   
    // Creates a socket
    socket_desc = socket(AF_INET,SOCK_STREAM,0);
    if(socket_desc == -1)
        perror("Could not create socket!");
       
    // Bind the socket to a port
    struct sockaddr_in local_address;

    local_address.sin_family = AF_INET;
    local_address.sin_addr.s_addr = INADDR_ANY;
    local_address.sin_port = 0;
   
    bind(socket_desc,(struct sockaddr *)&local_address,sizeof(local_address));
   
    // Getting the remote address
    char *HOST_ADDRESS = argv[1];
    struct hostent *hp;
    hp = gethostbyname(HOST_ADDRESS);
   
    struct sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(atoi(argv[2]));
    remote_addr.sin_addr = *((struct in_addr*)hp->h_addr);
    memset(&(remote_addr.sin_zero),'\0',8);
   
    // Connects to the server
    if(connect(socket_desc,(struct sockaddr *)&remote_addr,sizeof(remote_addr))){
        puts("error connecting...");
        return 0;                       
    }else{receiveall(socket_desc);printf("\n\r");}
   
    // Sending a message to the remote server
    while(1){
	    char *message = malloc(8080);
	    char *line = malloc(1024);
	    char *command = malloc(1024);
	    int boolean;
	    gets(line);
	    strcpy(command,line);
	    strcat(command,"\n.\n");
	    sendall(socket_desc,command,strlen(command));

	    if(strcmp(line,"GET") == 0){
            char * buffer = malloc(1024);
            recv(socket_desc,buffer,1024,0);
            printf("%s",buffer);
	        receiveall(socket_desc);
            free(buffer);
            //receiveall(socket_desc);
	    }else if(strcmp(line,"STORE") == 0){
		    char * buffer = malloc(1024);
            recv(socket_desc,buffer,1024,0);
		    printf("%s",buffer);

            do{
                gets(line);
                strcat(message,strcat(line,"\n\r"));
            }while(strcmp(line,".\n\r") != 0);
            char * number = malloc(255);
            snprintf(number,10,"%d",(int)strlen(message));
            send(socket_desc,number,strlen(number),0);
            send(socket_desc,message,strlen(message),0);
            recv(socket_desc,buffer,1024,0);
            printf("%s",buffer);
            free(number);
            free(buffer);

	    }else if(strcmp(line,"TRANSLATE") == 0){
		    char * buffer = malloc(1024);
		    recv(socket_desc,buffer,1024,0);
		    printf("%s",buffer);
	   	    do{
			    gets(line);
			    strcat(message,strcat(line,"\n\r"));
		    }while(strcmp(line,".\n\r") != 0);
		    char * number = malloc(255);
		    snprintf(number,10,"%d",(int)strlen(message));
	   	    send(socket_desc,number,strlen(number),0);
	   	    send(socket_desc,message,strlen(message),0);
		    char* trans = malloc(1024);
	   	    recv(socket_desc,trans,1024,0);
	   	    printf("%s",trans);
            free(number);
            free(trans);
            free(buffer);
	    }else if(strcmp(line,"EXIT") == 0){
            receiveall(socket_desc);
            exit(0);
            return 0;
	    }else{
            char* buffer = malloc(1024);
            recv(socket_desc,buffer,1024,0);
            printf("%s",buffer);
            free(buffer);
        }
    }       
}
int sendall(int sockfd, char *buf, int len){
    int total = 0; // how many bytes we've sent
    int bytesleft = len; // how many we have left to send
    int n;
    while(total < len) {
        n = send(sockfd, buf+total, bytesleft, 0);
    if (n == -1) { break; }
        total += n; bytesleft -= n;
    }
    len = total; // return number actually sent here
    return n==-1?-1:0; // return -1 on failure, 0 on success
}
int receiveall(int new_socket){
    
    int bufsize = 1024;
    char *buffer = malloc(bufsize);
    char *received = malloc(bufsize);
    int num;
    do{
    if((num = recv(new_socket,buffer,bufsize,0)) == -1){
        perror("Could not recv");
    }else{
        buffer[num] = '\0';
        printf("%s",buffer);
	    strcpy(received,buffer);
    }
}while(strpbrk(buffer,".") == NULL);
if(strcmp(received,"200 OK") == 0) return 1;
else return 0;
}
