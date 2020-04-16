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
	//printf("before:::%s", receiveMsg);
	struct Result{
				int shortest_path[3];
				int pathlen;
				float shortest_dist;
                char prop_speed[150];
                char trans_speed[150];
			}result;
	memset(result.shortest_path, 0 , sizeof(result.shortest_path));
	memset(result.prop_speed, '0', sizeof(result.prop_speed));
	memset(result.trans_speed,'0', sizeof(result.trans_speed));
	result.pathlen = 0;
	result.shortest_dist = 0.0;
	
	printf("line 90: before %d", result.shortest_path[0]);

	int numbytes = recv(socket_fd, &result, sizeof(result), 0);
	if(numbytes == -1){
		perror("recv:client");
		exit(1);
	}

	float delay = atof(result.trans_speed)+atof(result.prop_speed);
	printf("The client has received results from AWS:\n");
	printf("------------------------------------------------------\n");
	printf("Source\tDestination\tMin Length\t\t Tt \t\t Tp \t\t Delay \n");
	printf("------------------------------------------------------\n");
	printf("%s \t %s \t\t\t %.2f \t %s \t %s \t %.2f\n", src_vertex_index, dest_vertex_index, result.shortest_dist,result.trans_speed, result.prop_speed, delay);

	printf("Shortest Path:");
        int i;
        for(i = 0; i<result.pathlen; i++){
            if(i== result.pathlen-1){
                printf("%d", result.shortest_path[i]);
            }else{
                printf("%d -- ", result.shortest_path[i]);
            }
        }

	printf("\nMessage received from AWS.%d \n", result.shortest_path[0]);
	close(socket_fd);

     
    return 0;
}
