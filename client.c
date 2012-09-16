#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netdb.h>

int sendall(int , char *, int);
void receiveall(int);
int main(){
    //if(strlen(args) < 1) printf("Usage: client [hostname]\n\r"); return 0;
    int socket_desc;
    
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
    char *HOST_ADDRESS = "localhost";
    struct hostent *hp;
    hp = gethostbyname(HOST_ADDRESS);
    
    struct sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(29117);
    remote_addr.sin_addr = *((struct in_addr*)hp->h_addr);
    memset(&(remote_addr.sin_zero),'\0',8);
    
    // Connects to the server
    if(connect(socket_desc,(struct sockaddr *)&remote_addr,sizeof(remote_addr))){
        puts("error connecting...");
        return 0;                        
    }else{receiveall(socket_desc);printf("\n\r");}
    
    // Sending a message to the remote server
    while(1){
        char *message;
        fgets(message,sizeof(message),stdin);
	printf("%s\n\r",message);
        sendall(socket_desc,message,strlen(message));
        receiveall(socket_desc);
    }
    //char *message = "I am sending this from the client!\n\r"
    //send(socket_des,message,strlen(message),0);
                              
        
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
void receiveall(int new_socket){
    int bufsize = 1024;
    char *buffer = malloc(bufsize);
    int num;
    if((num = recv(new_socket,buffer,bufsize,0)) == -1){
        perror("Could not recv");
    }else{
        buffer[num] = '\0';
        printf("%s",buffer);
    }
}

