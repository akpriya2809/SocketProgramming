#include<stdio.h>
#include<string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h>


#define AWSPORT "34513"   //aws TCP port
#define HOST "localhost"

//./client A 1 3 1024
int main(int argc, char* argv[]){

    char map_id[2];
    char src_vertex_index[3];
    char dest_vertex_index[3];
    char file_size[20];
	char receiveMsg[150];
	memset(receiveMsg, '$', strlen(receiveMsg));

	strcpy(map_id,argv[1]);
    strcpy(src_vertex_index,argv[2]);
    strcpy(dest_vertex_index,argv[3]);
    strcpy(file_size,argv[4]);

    int socket_fd = 0;
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

    int rv;

    if ((rv = getaddrinfo(HOST, AWSPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
    printf("Before for loop in client. \n");

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("client: socket");
			continue;
		}

		if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_fd);
			perror("client: connect");
			continue;
		}
		break;
	}
    
if (p == NULL) {
		fprintf(stderr, "client: failed to connect. \n");
		exit(0);
		return 2;
	}

	freeaddrinfo(servinfo); 
	printf("The client is up and running. \n");

	send(socket_fd, map_id, sizeof map_id, 0);
    send(socket_fd, src_vertex_index, sizeof src_vertex_index, 0);
    send(socket_fd, dest_vertex_index, sizeof dest_vertex_index, 0);
    send(socket_fd, file_size, sizeof file_size, 0);
    
	printf("The client has sent query to AWS using TCP: start vertex %s; destination vertex %s;map %s;file size %s.\n",src_vertex_index,
	 	dest_vertex_index, map_id, file_size);
	printf("before:::%s", receiveMsg);
	int numbytes = recv(socket_fd, &receiveMsg,sizeof(receiveMsg), 0);
	if(numbytes == -1){
		perror("recv:client");
		exit(1);
	}

	printf("Message received from AWS.%s \n", receiveMsg);
	close(socket_fd);

     
    return 0;
}
