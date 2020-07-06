#include<err.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stddef.h>
#include <string.h> // memset
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h> // write
#include <pthread.h>
#include <stdint.h>
#include <getopt.h>
#include <sys/time.h>
#include <math.h>
#include "List.h"

pthread_mutex_t runMutex           = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  threadConditionVar = PTHREAD_COND_INITIALIZER;
pthread_cond_t  mainConditionVar   = PTHREAD_COND_INITIALIZER;

pthread_mutex_t hcMutex            = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  hcVar1             = PTHREAD_COND_INITIALIZER;
pthread_cond_t  hcVar2             = PTHREAD_COND_INITIALIZER;

pthread_mutex_t initMutex          = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  initVar            = PTHREAD_COND_INITIALIZER;

#define BUFFER_SIZE       8196
#define HC_RECV_BUFF_SIZE 255

struct threadData {

    int programStartFlag;
    List clientSockQ;
    List serverPortNumbers;
    List serverStructs;
    int globalServerPort;
    int client_to_load_balancer_port;
    int numberOfServers;
    int firstHcFlag;
    
};

struct serverData{

    int8_t flag;
    int64_t totalRequestsServiced;
    float successRate;
    int64_t portNumber;
};

/*
 * client_connect takes a port number and establishes a connection as a client.
 * connectport: port number of server to connect to
 * returns: valid socket if successful, -1 otherwise
 */
int client_connect(uint16_t connectport) {
    int connfd;
    struct sockaddr_in servaddr;
    

    connfd=socket(AF_INET,SOCK_STREAM,0);
    if (connfd < 0)
        return -1;

    
    memset(&servaddr, 0, sizeof servaddr);

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(connectport);

    /* For this assignment the IP address can be fixed */
    inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));

    if(connect(connfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
        return -1;
    
    return connfd;
}

/*
 * bridge_connections send up to 100 bytes from fromfd to tofd
 * fromfd, tofd: valid sockets
 * returns: number of bytes sent, 0 if connection closed, -1 on error
 */
int bridge_connections(int fromfd, int tofd) {
    uint8_t recvline[BUFFER_SIZE + 1];
    recvline[BUFFER_SIZE] = '\0';
    int n = recv(fromfd, recvline, BUFFER_SIZE, 0);
    if (n < 0) {
        printf("connection error receiving\n");
        return -1;
    } else if (n == 0) {
        printf("receiving connection ended\n");
        return 0;
    }
    recvline[n] = '\0';
    //printf("%s", recvline);
    //sleep(1);
    n = send(tofd, recvline, n, 0);
    if (n < 0) {
        printf("connection error sending\n");
        return -1;
    } else if (n == 0) {
        printf("sending connection ended\n");
        return 0;
    }
    return n;
}

/*
 * bridge_loop forwards all messages between both sockets until the connection
 * is interrupted. It also prints a message if both channels are idle.
 * sockfd1, sockfd2: valid sockets
 */
void bridge_loop(int sockfd1, int sockfd2) {
    fd_set set;
    struct timeval timeout;

    int fromfd, tofd;
    while(1) {
        // set for select usage must be initialized before each select call
        // set manages which file descriptors are being watched
        FD_ZERO (&set);
        FD_SET (sockfd1, &set);
        FD_SET (sockfd2, &set);

        // same for timeout
        // max time waiting, 5 seconds, 0 microseconds
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        // select return the number of file descriptors ready for reading in set
        switch (select(FD_SETSIZE, &set, NULL, NULL, &timeout)) {
            case -1:
                printf("error during select, exiting\n");
                dprintf(sockfd1, "%s", "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n");
                if(sockfd1 >= 0){
                    close(sockfd1);
                }
                if(sockfd2 >= 0){
                    close(sockfd2);
                }
                return;
            case 0:
                printf("both channels are idle, waiting again\n"); //Is this where I should throw the 500?
                dprintf(sockfd1, "%s", "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n");
                if(sockfd1 >= 0){
                    close(sockfd1);
                }
                if(sockfd2 >= 0){
                    close(sockfd2);
                }
                
                //continue;
                return;
                
            default:
                if (FD_ISSET(sockfd1, &set)) {//if client_sockd has data to send, send from client to server
                    fromfd = sockfd1;
                    tofd = sockfd2;
                } else if (FD_ISSET(sockfd2, &set)) { //if server_sockd has data to send, send from server to client
                    fromfd = sockfd2;
                    tofd = sockfd1;
                } else {
                    printf("this should be unreachable\n");
                    return;
                }
        }
        if (bridge_connections(fromfd, tofd) <= 0){

            dprintf(fromfd, "%s", "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n");
            return;
        }
    }
}
void serverPortSetter(struct threadData* data){
    printf("%s\n", "Hello from inside serverPortSetter");
    struct serverData *parms;
    int len       = length(data->serverStructs);
    int count     = 0;
    int64_t min   = 0;
    float sucRate = 0;
    
    moveFront(data->serverStructs);
    parms = (struct serverData*)(get(data->serverStructs));
    
    while( (parms->flag == 1) && (count < (len - 1)) ){ //moving through the list of serverStructs until we find the first one that isn't marked as down.
        moveNext(data->serverStructs);
        parms = (struct serverData*)(get(data->serverStructs));
        count++;
    }
    if(parms->flag == 0){ //setting the min, sucRate, and globalServerPort to the fields of the first serverStruct we encounter that isn't down.

        min                    = parms->totalRequestsServiced;
        sucRate                = parms->successRate;
        data->globalServerPort = parms->portNumber;
    }
    
    moveNext(data->serverStructs);
    count++;

    //printf("%s%ld\n", "Here's the portnum: ", parms->portNumber);
    
    while( count < len ){

        printf("%s\n", "Shit");
        
        parms = (struct serverData*)(get(data->serverStructs));

        if(parms->flag == 1){
            if( count < (len - 1) ){

                moveNext(data->serverStructs);//skipping if the server has been determined to be unresponsive or broken
                parms = (struct serverData*)(get(data->serverStructs));
                count++;
                continue;
            }
            else{
                break;
            }
        }

        if(parms->totalRequestsServiced < min){

            min                    = parms->totalRequestsServiced;
            sucRate                = parms->successRate;
            data->globalServerPort = parms->portNumber;
        }
        else if(parms->totalRequestsServiced == min && parms->successRate > sucRate){

            min                    = parms->totalRequestsServiced;
            sucRate                = parms->successRate;
            data->globalServerPort = parms->portNumber;
        }

        moveNext(data->serverStructs);
        count++;
        printf("%s%ld\n", "Here's the portnum: ", parms->portNumber);
    }
    printf("%s%d\n", "Here's data->globalServerPort from bottom of serverPortSetter: ", data->globalServerPort);
}

void lbWorker(int client_sockd, struct threadData* data) {
    printf("%s\n", "hello from lbWorker");
    int connfd,acceptfd;
    uint16_t connectport;
    int count = 1;
    struct serverData *parms;
    acceptfd = client_sockd;

    printf("%s\n", "Hello from just above first mutex lock in lbWorker");
    pthread_mutex_lock(&hcMutex);
    connectport = data->globalServerPort;
    pthread_mutex_unlock(&hcMutex);
    printf("%s\n", "Hello from just below first mutex lock inlbWorker");
    
    // Remember to validate return values
    // You can fail tests for not validating
    
    if( (data->numberOfServers == 1) && (connfd = client_connect(connectport)) < 0){

        printf("%s\n", "HEllo from first if statement in lbWorker");

        dprintf(acceptfd, "%s", "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n");
        close(acceptfd);
        return;

    }
    
    while( ((connfd = client_connect(connectport)) < 0) && (count < (data->numberOfServers - 1)) ){
        printf("%s\n", "Hello from while loop in lbWorker");
        pthread_mutex_lock(&hcMutex);
        moveFront(data->serverStructs);
        parms = (struct serverData*)(get(data->serverStructs));

        while(parms->portNumber != data->globalServerPort){
            
            moveNext(data->serverStructs);
            parms = (struct serverData*)(get(data->serverStructs));
        }

        parms->flag = 1;
        //serverPortSetter(data);
        pthread_cond_signal(&hcVar1);
        pthread_cond_wait(&hcVar2, &hcMutex);
        connectport = data->globalServerPort;
        pthread_mutex_unlock(&hcMutex);

        count++;

    }

    printf("%s%d\n","Here's the globalServerPort num from lbWorker: ", data->globalServerPort);
    
    if(connfd < 0){

        dprintf(acceptfd, "%s", "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n");
        close(acceptfd);
        //right here we need to set the flag of the serverStruct with the port number of the currently being used server to 1.
        return;
    }
    if(acceptfd < 0){
        warn("%d%s\n", client_sockd, "acceptfd from lbWorker thread was bad.");
        return;
    }
    // This is a sample on how to bridge connections.
    // Modify as needed.
    printf("%s\n", "Hello from bridge_loop call");
    bridge_loop(acceptfd, connfd);
    if(acceptfd >= 0){
        close(acceptfd);
    }
    if(connfd >= 0){
        close(connfd);
    }
}

void * hcHelper(void *arg){
    printf("%s\n", "Hello from hcHelper");

    int writeReturn         = 0;
    int readReturn          = 0;
    int offSet              = 0;
    int8_t sscanfReturn     = 0;
    int64_t endOfHcResponse = 0;
    char contentLength[5];
    char conLenHeader[20];
    char failTotal[8];
    char total[8];
    char httpVersion[10];
    char statusCode[5];
    char ok[3];
    int ft                  = 0;
    int t                   = 0;
    char hcRequest[]        = "GET /healthcheck HTTP/1.1\r\n\r\n";

    int fd;
    char buffer[HC_RECV_BUFF_SIZE];
    memset(&buffer, 0, sizeof(buffer));
    printf("%s\n", "HOOBLAH");

    struct serverData *parms;
    parms = (struct serverData*)arg;
    fd = client_connect(parms->portNumber);
    if(fd < 0){

        warn("%d%s%ld%s\n", fd, " Attempt to open fd from hcHelper thread ", pthread_self(), " failed");

        parms->flag = 1;
        return NULL;
    }

    fd_set set;
    struct timeval timeout;
    printf("%s\n", "BOOBLAH");
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    FD_ZERO (&set);
    FD_SET (fd, &set);

    /*if(fd < 0){

        warn("%d%s%ld%s\n", fd, " Attempt to open fd from hcHelper thread ", pthread_self(), " failed");

        parms->flag = 1;
        return NULL;
    }*/

    writeReturn = send(fd, hcRequest, strlen(hcRequest), 0);
    if(writeReturn < 0){

        warn("%d%s%ld%s\n", fd, " Write attempt from hcHelper thread ", pthread_self(), " failed");
        parms->flag = 1;
        return NULL;
    }

    switch (select(FD_SETSIZE, &set, NULL, NULL, &timeout)) {
        case -1:
            printf("error during select, returning.\n");
            close(fd);
            parms->flag = 1;
            return NULL;
        case 0:
            printf("Server was unresponsive.  Marking as down and returning.\n");
            parms->flag = 1;
            close(fd);
            return NULL;
            
        default:
            if (FD_ISSET(fd, &set)) {
                break;
            }
    }        

    
    while((readReturn = recv(fd, &buffer[offSet], (HC_RECV_BUFF_SIZE-offSet), 0)) > 0){
        offSet += readReturn;
    }
    endOfHcResponse = (ssize_t)strstr((const char *)buffer, "\r\n\r\n");

    if(readReturn < 0){
        warn("%d%s%ld%s\n", fd, " Read attempt from hcHelper thread ", pthread_self(), " failed");
        close(fd);
        parms->flag = 1;
        return NULL;
    }

    if(!endOfHcResponse && readReturn == 0){
        warn("%d%s%ld%s\n", fd, " Health check response header from hcHelper thread ", pthread_self(), " is malformed.");
        close(fd); 
        parms->flag = 1;
        return NULL;
    }

    //If we've made it this far, we have the healthcheck response in a buffer, and we can parse it.
    if((sscanfReturn = sscanf(buffer, "%s %[0-9] %s\r\n%s %[0-9]\r\n\r\n%[0-9]\n%[0-9]", httpVersion, statusCode, ok, conLenHeader, contentLength, failTotal, total)) != 7){
        printf("%s\n", "Hello from inside sscanf block");
        close(fd);
        parms->flag = 1;
        return NULL;
    }

    if(sscanfReturn == 7){

        if(strcmp("HTTP/1.1", httpVersion) != 0){
            parms->flag = 1;
            close(fd);   
            return NULL;
        }
        if(strcmp("200", statusCode) != 0){
            parms->flag = 1;
            close(fd);
            return NULL;
        }
        if(strcmp("OK", ok) != 0){
            parms->flag = 1;
            close(fd);
            return NULL;
        }
        if(strcmp("Content-Length:", conLenHeader) != 0){
            parms->flag = 1;
            close(fd);
            return NULL;
        }
    }
    
    ft                           = atoi(failTotal);
    t                            = atoi(total); 
    parms->totalRequestsServiced = t;
    if(t == 0 && ft == 0){
        parms->successRate = 0;
    }
    if(t != 0 && ft == 0){
        parms->successRate = INFINITY;
    }
    if(t > 0 && ft > 0){
        parms->successRate = (float)t/(float)ft;
    }    
    parms->flag            = 0;
    close(fd);
    //printf("%s%ld%s%ld\n", "The totalRequestsServiced for port number ", parms->portNumber, " is ", parms->totalRequestsServiced);
    //printf("%s%ld%s%f\n", "The successRate for portNumber ", parms->portNumber, " is ", parms->successRate);

    return NULL;
}

void * healthCheck(void *arg){

    printf("%s\n", "HELLO FROM HEALTHCHECK!!");
    struct threadData *parms;
    parms = (struct threadData*)arg;
    int len = length(parms->serverStructs);
    

    pthread_t hcThreadPool[len];

    while(1){

        printf("%s\n", "Hello from healthCheck while loop");

        struct timespec ts;
        struct timeval now;
        

        memset(&ts, 0, sizeof(ts));
        gettimeofday(&now, NULL);

        if(parms->programStartFlag == 1){

            ts.tv_sec = (now.tv_sec) + 2;
        }

        if(parms->programStartFlag == 0){
            ts.tv_sec = (now.tv_sec) + 0;
            parms->programStartFlag = 1;
        }

        pthread_mutex_lock(&hcMutex);
        pthread_cond_timedwait(&hcVar1, &hcMutex, &ts);
    


        struct serverData *parms2;

        moveFront(parms->serverStructs);

        for(int x = 0; x < len; x++){
            parms2 = (struct serverData*)(get(parms->serverStructs));
            pthread_create(&hcThreadPool[x], NULL, hcHelper, parms2);
            if(len == 1){
                break;
            }
            moveNext(parms->serverStructs);
        }

        for(int x = 0; x < len; x++){
            pthread_join(hcThreadPool[x], NULL);
        }

        serverPortSetter(parms);

        printf("%s\n", "Hello from below serverPortSetter call healthCheckHelper");
        pthread_mutex_unlock(&hcMutex);
        pthread_cond_signal(&hcVar2);

    }

}

void * threadRunner(void *arg){

    printf("%s%ld\n", "Active thread id: ", pthread_self());

    struct threadData *parms;
    parms = (struct threadData*)arg;
    int client_sockd;
    while(1){

        pthread_mutex_lock(&runMutex);

        while(length(parms->clientSockQ) == 0){
            pthread_cond_wait(&threadConditionVar, &runMutex);
        }
        client_sockd = (intptr_t)back(parms->clientSockQ);
        //printf("%s%d\n", "Here's client_sockd from threadRunner: ", client_sockd);
        deleteBack(parms->clientSockQ);

        pthread_mutex_unlock(&runMutex);
        pthread_cond_signal(&mainConditionVar);

        lbWorker(client_sockd, parms);

    }

    return NULL;
}


int main(int argc, char** argv) {

    if (argc < 2) {

        fprintf(stderr, "Minimum requirements for running server: port number.\n");
        return -1;
    }

    int option                       = 0;
    int requestThreshold             = 5;
    int numberOfWorkers              = 4;
    int requestsReceived             = 0;
    int remainder                    = 0;
    intptr_t temp                    = 0;
    struct threadData dat;
    dat.serverPortNumbers            = newList();
    dat.serverStructs                = newList();
    dat.clientSockQ                  = newList();
    dat.client_to_load_balancer_port = atoi(argv[1]);
    dat.programStartFlag             = 0;
    dat.firstHcFlag                  = 0;

    //handling command line args w/getOpt

    while((option = getopt(argc, argv, "R:N:")) != -1){
        switch(option){
            case 'R':
                for(uint8_t x = 0; x < strlen(optarg); x++){
                    if(!isdigit(optarg[x])){
                        fprintf(stderr, "Error. Request threshold must be an integer. Exiting.\n.");
                        exit(1);
                    }
                }
                
                requestThreshold = atoi(optarg);
                break;
            case 'N':
                for(uint8_t x = 0; x < strlen(optarg); x++){
                    if(!isdigit(optarg[x])){
                        fprintf(stderr, "Error. Argument must be an integer.  Exiting.\n");
                        exit(1);
                    }
                }
                
                numberOfWorkers = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Error.  Server port number must be a positive integer.  Exiting.");
                exit(1);    
        }
    }

    remainder = argc - optind;
    if(remainder == 0){
        printf("%s\n", "Error.  Loadbalancer and server port numbers must be specified.  Exiting.");
        exit(1);
    }
    dat.numberOfServers = (remainder - 1);
    if(dat.numberOfServers == 0){

        printf("%s\n", "Error.  At least one server port number is required.  Exiting.");
        exit(1);

    }

    for(int x = 1; x < remainder; x++){//appending the port numbers to dat.serverPortNumbers;

        for(uint8_t y = 0; y < strlen(argv[optind + x]); y++){
            if(!isdigit(argv[optind + x][y])){
                fprintf(stderr, "Error.  Server port number must be integer.  Exiting.\n");
                exit(1);
            }
        }
        temp = atoi(argv[optind + x]);

        append(dat.serverPortNumbers, (void *)temp);
    }
    int len = length(dat.serverPortNumbers);
    struct serverData serverStructArray[len]; //creating an array of serverData structs (one for each port number)
    moveFront(dat.serverPortNumbers);

    for(int x = 0; x < len; x++){

        serverStructArray[x].portNumber            = (intptr_t)get(dat.serverPortNumbers);//populating the structs
        serverStructArray[x].flag                  = 0;
        serverStructArray[x].successRate           = 0;
        serverStructArray[x].totalRequestsServiced = 0;

        append(dat.serverStructs, (void *)&serverStructArray[x]); //appending the structs to the List "serverStructs" so that they can live in the threadData struct.
        if(len == 1){
            break;
        }
        moveNext(dat.serverPortNumbers); 
    }

    for(uint8_t x = 0; x < strlen(argv[optind]); x++){ //checking client-to-lb port number for errors.

        if(!isdigit(argv[optind][x])){

            fprintf(stderr, "Error.  Load balancer port number must be a positive integer.  Exiting.\n");
            exit(1);
        }
    }

    
    dat.globalServerPort = (intptr_t)front(dat.serverPortNumbers);//Initializing the port number so we pass the fucking baby tests.
    
    pthread_t threadPool[numberOfWorkers]; //creating what amounts to an array of thread variables.
    pthread_t healthCheckThread;

    
    pthread_create(&healthCheckThread, NULL, healthCheck, &dat);
    
    for(int x = 0; x < numberOfWorkers; x++){ //initializing those thread variables (creating them).

        pthread_create(&threadPool[x], NULL, threadRunner, &dat);
    }
    

    char* port = argv[optind];
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t addrlen = sizeof(server_addr);

    int server_sockd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sockd < 0) {
        perror("socket");
    }

    int enable = 1;

    /*
        This allows you to avoid: 'Bind: Address Already in Use' error
    */
    int ret = setsockopt(server_sockd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    /*
        Bind server address to socket that is open
    */
    ret = bind(server_sockd, (struct sockaddr *) &server_addr, addrlen);

    /*
        Listen for incoming connections
    */
    ret = listen(server_sockd, SOMAXCONN);
        
     // 5 should be enough, if not use SOMAXCONN

    if (ret < 0) {
        return EXIT_FAILURE;
    }

    struct sockaddr client_addr;
    socklen_t client_addrlen = sizeof(client_addr);
    intptr_t client_sockd;

    dat.numberOfServers = (remainder - 1);

    printf("%s%d\n", "Here's dat.numberOfServers from main(): ", dat.numberOfServers );
    
    while(1){

        printf("[+] Load balancer is waiting...\n");
    
        client_sockd = accept(server_sockd, &client_addr, &client_addrlen);

        if(client_sockd < 0){

            warn("%ld%s\n", client_sockd, "Error.  Client socket invalid.");
        }

        else{

            requestsReceived++;

            if(requestsReceived == requestThreshold){
                pthread_mutex_lock(&hcMutex);
                pthread_cond_signal(&hcVar1);
                pthread_cond_wait(&hcVar2, &hcMutex);
                requestsReceived = 0;
                pthread_mutex_unlock(&hcMutex);
            }

            pthread_mutex_lock(&runMutex);
            while(length(dat.clientSockQ) > numberOfWorkers){//the number 4 should be changed to the number of threads (number of servers)
                pthread_cond_wait(&mainConditionVar, &runMutex);
            }
            prepend(dat.clientSockQ, (void *)client_sockd);
            pthread_mutex_unlock(&runMutex); 
            pthread_cond_signal(&threadConditionVar); 
        }

    }

    return 0;
}