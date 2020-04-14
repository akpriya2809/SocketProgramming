#include<stdio.h>
#include<string.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h>
#include<sys/socket.h>
#include <netdb.h>


#define SERVERCPORT "32513" 
#define HOST "localhost"
#define BACKLOG 10
#define MAXROW 50

int main(){
    int socket_fd;
    int numbytes;
    struct addrinfo hints;
	struct addrinfo *servinfo, *p;
    struct sockaddr_storage server_addr;
    socklen_t addr_len;
    int rv;

    memset(&hints, 0, sizeof hints); 
    hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM; 
	hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(HOST, SERVERCPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 0;
	}
    for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("serverC: socket");
			continue;
		}
		if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_fd);
			perror("serverC: bind");
			continue;
		}
		break;
	}

    if (p == NULL) {
		fprintf(stderr, "serverC: failed to bind socket\n");
		return 0;
	}

    freeaddrinfo(servinfo);
    
	printf( "The Server C is up and running using UDP on port %s.\n", SERVERCPORT);
    
    while(1){
        addr_len = sizeof server_addr;
        struct Info{
				char src_index[3];
				char dest_index[3];
				char fs[20];
				int len;
			}info;
        memset(info.src_index,'0', sizeof(info.src_index));
        memset(info.dest_index,'0', sizeof(info.dest_index));
        memset(info.fs,'0', sizeof(info.fs));
        info.len = 0;
        //printf("%s",info.src_index);
        int numbytes = recvfrom(socket_fd, (void*)&info, sizeof info , 0,(struct sockaddr *)&server_addr, &addr_len);
        if(numbytes == -1){
            perror("receive:from");
            exit(1);
        }
        printf("The Server A has received input for finding graph of map %s . \n", info.src_index);
    }





    return 0;
}