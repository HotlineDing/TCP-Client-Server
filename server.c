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
#include <arpa/inet.h>

int sendall(int, char*, int );
void decode(char*, char*);
void upper(char*, char*);
void storetofile(char*);
void readfromfile(char*);
void receiveall(int,int,char*,char*);

int main(int argc, char* argv[]){
    int socket_des;
    struct sockaddr_in local_addr;
   
    if(argc < 2){
        fprintf(stderr,"Usage: ./server [port]\n");
        exit(1);
    }

    // Creating the socket
    socket_des = socket(AF_INET,SOCK_STREAM,0);
    if (socket_des == -1){
        perror("Could not connect to socket!");              
    }
   
    // Setting up the local address
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(atoi(argv[1]));
   
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
            char* client_ipaddress = malloc(1024);
            new_socket = accept(socket_des, (struct sockaddr *)&remote_addr, &addrlen);
            if(new_socket == -1){
                perror("Could not accept the connection!");
            }else{strcpy(client_ipaddress,inet_ntoa(remote_addr.sin_addr));
                send(new_socket,"Server is ready...",18,0);
                printf("server: got connection from client %s \n", client_ipaddress);
            }
           
            // Child process to accept multiple connections
            if(!fork()){
                close(socket_des);
                while(1){
                    // Receive data
                    int bufsize = 4024;
                    char *buffer = malloc(bufsize);
        		    char *received = malloc(bufsize);
                    int num = 0;

                    //Receives the command
		            do{
        		        if((num = recv(new_socket,buffer,bufsize,0)) == -1){ perror("Could not recv");}
		        	    strcat(received,buffer);
        		    }while(strpbrk(received,".\n") == NULL);
		            strcpy(buffer,strtok(received,"."));
                
                    printf("%s sends %s\n",client_ipaddress,buffer);

                    //List of if/else to determin if it is a valid command.
		            if(strcmp(buffer,"GET\n") == 0){
                        char * get = malloc(1024);
			            sendall(new_socket,"200 OK\n",8);
		                readfromfile(get);
                        send(new_socket,get,strlen(get),0);
                        free(get);
		            }else if(strcmp(buffer,"STORE\n") == 0){
			            sendall(new_socket,"200 OK\n",8);
			            char* store = malloc(8080);
                        recv(new_socket,buffer,strlen(buffer),0);
                        receiveall(new_socket,atoi(buffer),buffer,store);
			            storetofile(store);
                        sendall(new_socket,"200 OK\n",8);
                        free(store);
                    }else if(strcmp(buffer,"TRANSLATE\n") == 0){
			            sendall(new_socket,"200 OK\n",8);
			            char* trans = malloc(8080);
			            char* sending = malloc(8080);
			            recv(new_socket,buffer,strlen(buffer),0);
                        receiveall(new_socket,atoi(buffer),buffer,trans);
			            upper(sending,trans);
			            send(new_socket,sending,strlen(sending),0);
                        free(trans);
                        free(sending);
		            }else if(strcmp(buffer,"EXIT\n") == 0){
		                send(new_socket,"200 OK\n",8,0);
                        shutdown(new_socket,2);
                        close(new_socket);
                        exit(0);
                    }else{
			            sendall(new_socket,"400 Command not valid.\n",24);
		            }
                }
            }
            free(client_ipaddress);
            close(new_socket);
        }
    }
}
void receiveall(int socket,int packetsize,char* buffer,char* rval){
	int num = 0;
    int total = 0;
	do{
	    if((num = recv(socket,buffer,strlen(buffer),0)) == -1){
		perror("Could not recv");
	    }
        total += num;
        if(packetsize > total)
	        strncat(rval,buffer,num);
        else
            strncat(rval,buffer,(num - (total -packetsize)));
	}while(strlen(rval) < packetsize);
    return;
}
void upper(char* rval,char* buffer){
    int i = 0;
    for(i = 0; i < strlen(buffer); i++){
        buffer[i] = toupper(buffer[i]);
    }
    strcpy(rval,buffer);
    return;
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
void storetofile(char* message){
	FILE *file;
	file = fopen("store.txt","w");
	if(!file) perror("can't open file!");
	fwrite(message,sizeof(char*),strlen(message),file);
	fclose(file);
	return;
}
void readfromfile(char*rval){
	FILE *file;
	long lSize;
	char* buffer;
	file = fopen("store.txt","r");
	
    if( !file ) perror("blah.txt"),exit(1);

    fseek( file , 0L , SEEK_END);
    lSize = ftell( file );
    rewind( file );

    buffer = calloc( 1, lSize+1 );
    if( !buffer ) fclose(file),fputs("memory alloc fails",stderr),exit(1);

    if( 1!=fread( buffer , lSize, 1 , file) )
        fclose(file),free(buffer),fputs("entire read fails",stderr),exit(1);

    strcpy(rval,buffer);
    fclose(file);
    free(buffer);
	return;
}
