#include<stdio.h>
#include<string.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h>
#include<sys/socket.h>
#include <netdb.h>
#include <limits.h>


#define SERVERCPORT "32513" 
#define HOST "localhost"
#define BACKLOG 10
#define MAXROW 50


float matrix[MAXROW][3];


int minDistance(float dist[], int sptSet[], int V) 
{ 
    // Initialize min value 
    //printf(" Line 24\n");
    int min = INT_MAX, min_index; 
  
    for (int v = 0; v < V; v++) 
        if (sptSet[v] == 0 && dist[v] <= min) 
            min = dist[v], min_index = v; 
    //printf("min_index:%d\n", min_index);
    return min_index; 
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
                char prop_speed[150];
                char trans_speed[150];
                char map_id[2];
                char fs[20];
				int len;
                float graph[MAXROW][3];
			}info;

        memset(info.src_index,'0', sizeof(info.src_index));
        memset(info.dest_index,'0', sizeof(info.dest_index));
        memset(info.fs,'0', sizeof(info.fs));
        memset(info.map_id, '0', sizeof(info.map_id));
        info.len = 0;
        memset(info.graph, 0.0, sizeof(info.graph));
        
        numbytes = recvfrom(socket_fd, (void*)&info, sizeof info , 0,(struct sockaddr *)&server_addr, &addr_len);
        if(numbytes == -1){
            perror("receive:from");
            exit(1);
        }
        
        
        printf("The Server C has received data for calculation:\n");
        printf("\t*Propagation Speed:%s km/s\n", info.prop_speed);
        printf("\t*Transmission Speed:%s km/s\n", info.trans_speed);
        printf("\t*map ID:%s\n", info.map_id);
        printf("\t*Source ID:%s \t Destination ID:%s\n", info.src_index, info.dest_index);


        // printf("prop_speed:%s\n", info.prop_speed);
        // printf("trans_speed:%s\n", info.trans_speed);
        // printf("source:%s\n", info.src_index);
        // printf("dest:%s\n", info.dest_index);
        // printf("map_id:%s\n", info.map_id);
        // printf("fs:%s\n", info.fs);
        // printf("len:%d\n",info.len);

        int i = 0, j=0;

        int max = 0;
        for(i = 0; i<info.len; i++){
            //if(i ==0) printf("\n%d\n", info.len);
            for(j = 0; j<2;j++){
                int num  = (int)info.graph[i][j];
                // printf("%d\t", num);
                if(max< num){
                    max = num;
                }
                
            }
    
        }

        printf("\n");
        
        
        int temp[max + 1];
        memset(temp, 0, sizeof(temp));
        for(i = 0; i<info.len; i++){
            for(j = 0; j<2;j++){
                if(temp[(int)info.graph[i][j]] != 1){
                    temp[(int)info.graph[i][j]] = 1; 
                }
                
            }
            
        }
        
        int t;
        int count = 1; 
        for(t = 0; t <= max;t++){
            if(temp[t]==1){
                temp[t]= count++;
            }
        }

        int V = count -1;
        int mapArr [V];
        memset(mapArr, 0, sizeof(mapArr));
        int z =0;

        for(t = 0; t <= max;t++){
            if(temp[t]!=0){
                //printf("t::%d\t z:%d\n",t, z);
                mapArr[z++]=t;
            }
            
        }

        float adjMatrix [V][V];
        memset(adjMatrix, 0.0, sizeof(adjMatrix));

        for(i = 0; i<info.len; i++){
                int num1  = (int)info.graph[i][0];
                int num2 = (int) info.graph[i][1];
                if(temp[num1]!=0 && temp[num1]!=0){
                    adjMatrix[temp[num1]-1][temp[num2]-1] = info.graph[i][2];
                    adjMatrix[temp[num2]-1][temp[num1]-1] = info.graph[i][2];
                }
    
        }

        // for(i= 0; i<V;i++){
        //     for(j=0; j<V;j++){
        //         printf("%.2f\t", adjMatrix[i][j]);
        //     }
        //     printf("\n");
        // }

        // find shortest path from adjacency matrix
       
        float dist[V];
        int sptSet[V];
        int parent[V];
        parent[0]=-1;

        for (int i = 0; i < V; i++) {
            dist[i] = INT_MAX, sptSet[i] = 0;
        }
        int src = atoi(info.src_index);
        dist[temp[src]-1] = 0.0;

        for(i = 0; i < V-1; i++){
            int u = minDistance(dist, sptSet, V); 
            sptSet[u]=1;
            for (int v = 0; v < V; v++) {

                if (!sptSet[v] && adjMatrix[u][v] && dist[u] != INT_MAX 
                    && dist[u] + adjMatrix[u][v] < dist[v]) {
                    dist[v] = dist[u] + adjMatrix[u][v]; 
                    parent[v] = u;
                }
            }
        }

        // for(i = 0;i<V;i++){
        //     printf("dist arr%f \t", dist[i]);
        // }
        printf("\n");

        int dest = atoi(info.dest_index);
        int x = temp[dest]-1;
        int hops = 0;
        while(parent[x]!=-1){
            hops++;
            x=parent[x];

        }
        //printf("hops::%d\n", hops);
        int path[hops+1];
        memset(path, -1, sizeof(path));
        //path[0] = dest;
        int p =0;
        x = temp[dest]-1;
        //float pathDist = 0.0;
        while(parent[x]!=-1){
            path[p++] = mapArr[x];
            x=parent[x];

        }
        //printf("outside loop apthdist:%f", pathDist);
        path[p] = mapArr[x];
        //printf("\n");
        for(i =0; i<hops+1; i++){
           // printf("%d\t", path[i]);
        }
        //printf("\n");
        int k = hops;
        i = 0;
        while(i<k){
            int temp = path[i];
            path[i]= path[k];
            path[k]= temp;
            i++;
            k--;
        }
        // for(i =0; i<hops+1; i++){
        //     printf("%d\t", path[i]);
        // }
        
        printf("The Server C has finished  the calculation:\n");
        printf("Shortest Path:");
        int total = hops+1;
        
        for(i = 0; i<total; i++){
            if(i== total-1){
                printf("%d", path[i]);
            }else{
                printf("%d -- ", path[i]);
            }
            
            if(i == total -2){
                printf("\n");
            }
        }
        printf("\n");
        printf("Shortest Distance:%.2f km\n", dist[temp[dest]-1] );
        printf("Transmission delay: %s s\n", info.trans_speed);
        printf("Propagation delay: %s s\n", info.prop_speed);

        struct Result{
				int shortest_path[3];
                int pathlen;
                float shortest_dist;
                char prop_speed[150];
                char trans_speed[150];
			}result;
        memset(result.shortest_path, '0', sizeof(result.shortest_path));
        memset(result.prop_speed, '0', sizeof(result.prop_speed));
        memset(result.trans_speed,'0', sizeof(result.trans_speed));
        
        result.pathlen = total;
        result.shortest_dist = dist[temp[dest]-1];

        memcpy(result.shortest_path, path, sizeof(path));
        strcpy(result.prop_speed, info.prop_speed);
        strcpy(result.trans_speed, info.trans_speed);
        

        if ((numbytes = sendto(socket_fd, &result, sizeof(result), 0,	// send to UDP server, the address is assigned in getaddrinfo function above
				 (struct sockaddr *)&server_addr, addr_len)) == -1) {
			perror("senderr: sendto");
			exit(1);
		}

		printf("The Server A finished sending the output to AWS\n");


    }
    





    return 0;
}