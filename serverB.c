#include<stdio.h>
#include<string.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h>
#include<sys/socket.h>
#include <netdb.h>

#define SERVERBPORT "31513"
#define HOST "localhost"
#define BACKLOG 10
#define MAXROW 50

float matrix[MAXROW][3];
int len = 0;


int compareLine(char* str, char* in) {
    int i = 0;
    int result = 0;
    if(str[0] == in[0]) {
        result = 1;
    }

    return result;
}

void readValues(FILE *fp){
    
    char line[150];
    int i = 0;
    
    while(1){
        if(fgets(line, 150, fp)){
            char *token_line;
            const char delimiter[2] = "\n";

            token_line = strtok(line, delimiter);

            if(strlen(token_line)==1) {
                break;
            }
            else {
                float token_one, token_two, token_three;
                const char delimiterAgain[2] = " ";

                token_one = atof(strtok(line, delimiterAgain));

                token_two = atof(strtok(NULL, delimiterAgain));

                token_three = atof(strtok(NULL, delimiterAgain));
                matrix[i][0] = token_one;
                matrix[i][1] = token_two;
                matrix[i][2] = token_three;
                i++;
                len = i;

            }
        }


    }
   
    
}

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

    if ((rv = getaddrinfo(HOST, SERVERBPORT, &hints, &servinfo)) != 0) { // --beej tutorials
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 0;
	}
    for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) // --beej tutorials
				== -1) {
			perror("serverB: socket");
			continue;
		}
		if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_fd);
			perror("serverB: bind");
			continue;
		}
		break;
	}

    if (p == NULL) {
		fprintf(stderr, "serverB: failed to bind socket\n");
		return 0;
	}

    freeaddrinfo(servinfo);
    
	printf( "The Server B is up and running using UDP on port %s.\n", SERVERBPORT);
    

    while(1){
        addr_len = sizeof server_addr;
        char val1[15];
        memset(val1, '0', sizeof(val1));
        char val2[15];
        memset(val2, '0', sizeof(val2));
        char msg[2];
        memset(msg, 'N', sizeof(msg));

        
        char map_id[2];
        memset(map_id, '0', strlen(map_id));

        int numbytes = recvfrom(socket_fd, map_id, sizeof map_id , 0,(struct sockaddr *)&server_addr, &addr_len);
        if(numbytes == -1){
            perror("receive:from");
            exit(1);
        }
        printf("The Server B has received input for finding graph of map %s\n", map_id);
        
        //search map id from Map2.txt
        char buf[255];
        memset(buf, '0', strlen(buf));
        FILE *fp;
        fp = fopen("map2.txt", "r");
        int i = 0;
        
        while (1) {
            if (fgets(buf,150, fp) == NULL) 
                break; 
           

            if(compareLine(buf, map_id)) {
                msg[0] = 'B';
               
                fgets(val1, 15, fp);

                fgets(val2, 150, fp);
                
                readValues(fp);
                break;
            } 
        } 
        
        fclose(fp);
        // --beej tutorials
        if ((numbytes = sendto(socket_fd, &msg, sizeof(msg), 0,	
            (struct sockaddr *)&server_addr, addr_len)) == -1) {
			perror("senderr: sendto");
			exit(1);
		}
        if ((numbytes = sendto(socket_fd, &len, sizeof(len), 0,	
            (struct sockaddr *)&server_addr, addr_len)) == -1) {
			perror("senderr: sendto");
			exit(1);
		}
        
        if ((numbytes = sendto(socket_fd, &val1, sizeof(val1), 0,	
            (struct sockaddr *)&server_addr, addr_len)) == -1) {
			perror("senderr: sendto");
			exit(1);
		}
        if ((numbytes = sendto(socket_fd, &val2, sizeof(val2), 0,	
            (struct sockaddr *)&server_addr, addr_len)) == -1) {
			perror("senderr: sendto");
			exit(1);
		}
        
        if ((numbytes = sendto(socket_fd, &matrix, sizeof(matrix), 0,	
            (struct sockaddr *)&server_addr, addr_len)) == -1) {
			perror("senderr: sendto");
			exit(1);
		}

        
        if(msg[0] == 'N'){
            printf("The Server B does not have required graph id %s\n", map_id);
            printf("The Server B has sent \"Graph not found\" to AWS\n");
        }else{
            printf("The Server B sent Graph to AWS.\n");
        }
    
       
        
    }
    return 0;
}