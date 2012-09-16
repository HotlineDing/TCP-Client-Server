#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int sendall(int sockfd, char *buf, int len);

int main(char * args){
    int socket_des;
    struct sockaddr_in local_addr;
    
    // Creating the socket
    socket_des = socket(AF_INET,SOCK_STREAM,0);
    if (socket_des == -1){
        perror("Could not connect to socket!");               
    }
    
    // Setting up the local address
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(29117);
    
    // Binding the local address to the socket
    int bind_error;
    bind_error = bind(socket_des,(struct sockaddr *)&local_addr,sizeof(local_addr));
    if(bind_error == -1){
        perror("Could not bind address to port!");  
        return 0;                         
    }
    
    while(1){
    
        // Listen for connections
        if(listen(socket_des,3) == 0){
            // Accepting new connections
            int addrlen;
            int new_socket;
            struct sockaddr_in remote_addr;
            new_socket = accept(socket_des, (struct sockaddr *)&remote_addr, &addrlen);
            if(new_socket == -1){
                perror("Could not accept the connection!");
            }else{//inet_ntoa(dest.sin_addr) 
                send(new_socket, "200 OK\n\rServer is ready...\n\r",32,0);
                printf("server:got connection from client %s \n\r", inet_ntoa(remote_addr.sin_addr));
                }
            
            while(1){
                // Receive data
                int bufsize = 1024;
                char *buffer = malloc(bufsize);
                int num;
                if((num = recv(new_socket,buffer,bufsize,0)) == -1){
                    perror("Could not recv");
                }else{
                    buffer[num] = '\0';
                    printf("%s",buffer);
                }
                sendall(new_socket,buffer,strlen(buffer));
            }
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

