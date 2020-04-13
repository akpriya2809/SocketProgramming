#include<stdio.h>
#include<string.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h>
#include<sys/socket.h>
#include <netdb.h>


#define TCPPORT "34513"   //TCP port
#define UDPPORT "33513"
#define HOST "localhost"
#define BACKLOG 10
#define PORTA "30513"
#define PORTB "31513"
#define PORTC "32513"
#define MAXROW 50

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void sendData(char map_id[], char src_vertex_index[], char dest_vertex_index[], char file_size[]){
	printf("Hello %s \n", map_id);
	printf("Hello %s \n", src_vertex_index);
	printf("Hello %s \n", dest_vertex_index);
	//printf("Hello %s \n", file_size);
	int my_sock;
	struct addrinfo hints, *servinfo, *p;
    int rv;
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
	//printf("Hello %s \n", file_size);
	if ((rv = getaddrinfo(HOST, UDPPORT, &hints, &servinfo))!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return;
	}
	//printf("Hellosasas %s \n", file_size);
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((my_sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}
		break;
	}
	//printf("Hellortrt %s \n", file_size);
	
}



int main(int argc, char* argv[]){
    int socket_fd, child_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage client_addr;
    char dest[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rv;
    if ((rv = getaddrinfo(HOST, TCPPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
    for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("server: socket");
			continue;
		}
		if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_fd);
			perror("aws server: bind");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "aws server: failed to bind. \n");
		exit(0);
		return 2;
	}

	freeaddrinfo(servinfo);
    if (listen(socket_fd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	//UDP socket initialization

	int udp_sockfd;
	struct addrinfo udp_hints, *udp_servinfo, *udp_p;
	int udp_rv;
	struct sockaddr_storage udp_their_addr;
	float udp_buf[2];
	socklen_t udp_addr_len;
	char udp_s[INET6_ADDRSTRLEN];

	memset(&udp_hints, 0, sizeof udp_hints);
	udp_hints.ai_family = AF_UNSPEC; 	// set to AF_INET to force IPv4
	udp_hints.ai_socktype = SOCK_DGRAM;
	udp_hints.ai_flags = AI_PASSIVE;	 // use my IP

	if ((udp_rv = getaddrinfo(NULL, UDPPORT, &udp_hints, &udp_servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(udp_rv));
		return 1;
	}

	for(udp_p = udp_servinfo; udp_p != NULL; udp_p = udp_p->ai_next) {
		if ((udp_sockfd = socket(udp_p->ai_family, udp_p->ai_socktype,
				udp_p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(udp_sockfd, udp_p->ai_addr, udp_p->ai_addrlen) == -1) {
			close(udp_sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (udp_p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(udp_servinfo);

	struct addrinfo udp_A_hints, *udp_A_servinfo, *udp_A_p;
	int udp_A_rv;

	memset(&udp_A_hints, 0, sizeof udp_A_hints);
	udp_A_hints.ai_family = AF_UNSPEC;
	udp_A_hints.ai_socktype = SOCK_DGRAM;

	if ((udp_A_rv = getaddrinfo("127.0.0.1", PORTA, &udp_A_hints, &udp_A_servinfo)) != 0) {	// server port is defined, the hostname should be 127.0.0.1
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(udp_A_rv));
		return 1;
	}

	udp_A_p = udp_A_servinfo; 

	struct addrinfo udp_B_hints, *udp_B_servinfo, *udp_B_p;
	int udp_B_rv;

	memset(&udp_B_hints, 0, sizeof udp_B_hints);
	udp_B_hints.ai_family = AF_UNSPEC;
	udp_B_hints.ai_socktype = SOCK_DGRAM;

	if ((udp_B_rv = getaddrinfo("127.0.0.1", PORTB, &udp_B_hints, &udp_B_servinfo)) != 0) {	// server port is defined, the hostname should be 127.0.0.1
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(udp_B_rv));
		return 1;
	}

	udp_B_p = udp_B_servinfo; 

	struct addrinfo udp_C_hints, *udp_C_servinfo, *udp_C_p;
	int udp_C_rv;

	memset(&udp_C_hints, 0, sizeof udp_C_hints);
	udp_C_hints.ai_family = AF_UNSPEC;
	udp_C_hints.ai_socktype = SOCK_DGRAM;

	if ((udp_C_rv = getaddrinfo("127.0.0.1", PORTC, &udp_C_hints, &udp_C_servinfo)) != 0) {	// server port is defined, the hostname should be 127.0.0.1
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(udp_C_rv));
		return 1;
	}

	udp_C_p = udp_C_servinfo;



    printf("The AWS is up and running.\n");

    while(1){
		
        socklen_t addr_size = sizeof client_addr;
        child_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &addr_size);
        if (child_fd == -1) {
			perror("accept");			
			exit(1);
		}

        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr), dest, sizeof dest);

		struct sockaddr_in addrPeer;
		memset(&addrPeer, 0, sizeof(addrPeer));
		int len = sizeof(addrPeer);
		int peerRv = getpeername(child_fd, (struct sockaddr *) &addrPeer, (socklen_t *) &len);
		if(peerRv == 1){
			perror("getpeername");
			//exit(1);
		}
		int client_port = addrPeer.sin_port;

		char map_id[2];
		memset(map_id, '0', strlen(map_id));

		char src_vertex_index[2];
		memset(src_vertex_index, '0', strlen(src_vertex_index));

    	char dest_vertex_index[2];
		memset(dest_vertex_index, '0', strlen(dest_vertex_index));

    	char file_size[20];
		memset(file_size, '0', strlen(file_size));

		recv(child_fd, map_id, sizeof map_id, 0);	
		recv(child_fd, src_vertex_index, sizeof src_vertex_index, 0);	
		recv(child_fd, dest_vertex_index, sizeof dest_vertex_index, 0);	
		recv(child_fd, file_size, sizeof file_size, 0);	
		printf("The AWS has received mapID %s , start vertex %s, destination vertex %s and file size %s from the client using TCP over port %d. \n", map_id, src_vertex_index, dest_vertex_index, file_size, client_port);
		
		//send map is to server A
		int numbytes;
		if ((numbytes = sendto(udp_sockfd, &map_id, sizeof map_id, 0,	// send to UDP server, the address is assigned in getaddrinfo function above
				 udp_A_p->ai_addr, udp_A_p->ai_addrlen)) == -1) {
				perror("talker: sendto");
				exit(1);
			}
		printf("The AWS sent <%s> to Backend-Server A\n", map_id);

		char matrix[MAXROW][3][150];

		char val1[150];
		memset(val1, '0', strlen(val1));

		char val2[150];
		memset(val2, '0', strlen(val2));

		char msg[2];
		memset(msg, 'N', strlen(msg));

		udp_addr_len = sizeof udp_their_addr;
		if ((numbytes = recvfrom(udp_sockfd, &msg, sizeof(val1) , 0,			//wait for the incoming packets
			(struct sockaddr *)&udp_their_addr, &udp_addr_len)) == -1) {
				perror("recvfrom");
				exit(1);
		}
		
		if ((numbytes = recvfrom(udp_sockfd, &val1, sizeof(val1) , 0,			//wait for the incoming packets
			(struct sockaddr *)&udp_their_addr, &udp_addr_len)) == -1) {
				perror("recvfrom");
				exit(1);
		}
		if ((numbytes = recvfrom(udp_sockfd, &val2, sizeof(val2) , 0,			//wait for the incoming packets
			(struct sockaddr *)&udp_their_addr, &udp_addr_len)) == -1) {
				perror("recvfrom");
				exit(1);
		}
		if ((numbytes = recvfrom(udp_sockfd, &matrix, sizeof(matrix) , 0,			//wait for the incoming packets
			(struct sockaddr *)&udp_their_addr, &udp_addr_len)) == -1) {
				perror("recvfrom");
				exit(1);
		}
		if(val1[0] == '0'){
			printf("Not found from server A \n");
			//send to server B
			if ((numbytes = sendto(udp_sockfd, &map_id, sizeof map_id, 0,	// send to UDP server, the address is assigned in getaddrinfo function above
				 udp_B_p->ai_addr, udp_B_p->ai_addrlen)) == -1) {
				perror("talker: sendto");
				exit(1);
			}
			printf("The AWS sent <%s> to Backend-Server B\n", map_id);
			
			if ((numbytes = recvfrom(udp_sockfd, &msg, sizeof(val1) , 0,			//wait for the incoming packets
			(struct sockaddr *)&udp_their_addr, &udp_addr_len)) == -1) {
				perror("recvfrom");
				exit(1);
			}
			if ((numbytes = recvfrom(udp_sockfd, &val1, sizeof(val1) , 0,			//wait for the incoming packets
			(struct sockaddr *)&udp_their_addr, &udp_addr_len)) == -1) {
				perror("recvfrom");
				exit(1);
			}
			if ((numbytes = recvfrom(udp_sockfd, &val2, sizeof(val2) , 0,			//wait for the incoming packets
				(struct sockaddr *)&udp_their_addr, &udp_addr_len)) == -1) {
					perror("recvfrom");
					exit(1);
			}
			if ((numbytes = recvfrom(udp_sockfd, &matrix, sizeof(matrix) , 0,			//wait for the incoming packets
				(struct sockaddr *)&udp_their_addr, &udp_addr_len)) == -1) {
					perror("recvfrom");
					exit(1);
			}
			if(val1[0] == '0'){
				printf("Not found from server B As well \n");
			}else{
				printf("Received from ServerB %s \n", val1);
			}
		}else{
			printf("Received from ServerA %s \n", val1);
		}

		
		printf("End %s\n",msg);
		


    }


    return 0;
}