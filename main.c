#include <stdio.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include "sbuf.h"
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define BACKLOG 20
#define NTHREADS 20
#define SBUFSIZE 20
int main(int argc, char** argv)
{
    signal(SIGPIPE, SIG_IGN);
    int lsfd;
    pthread_t tid;
    struct addrinfo hints, *result, *rp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* TCP socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    //getaddrinfo
    int status;
    if((status = getaddrinfo(NULL, argv[1], &hints, &result)) != 0){
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    //socket and bind
    for(rp = result; rp != NULL; rp = rp->ai_next){
        if((lsfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1){
            perror("server socket error");
            continue;
        }
        if(bind(lsfd, rp->ai_addr, rp->ai_addrlen) == -1){
            close(lsfd);
            perror("server bind error");
            continue;
        }
        break; //we make it through socket/bind, we can leave the list
    }
    freeaddrinfo(result);

    //listen
    if(listen(lsfd, BACKLOG) == -1){
        perror("server listen error");
        exit(1);
    }

    //pool up, bois
    sbuf_init(&sbuf, SBUFSIZE);
    slog_init(&slog, SBUFSIZE);
    scache_init(&scache, MAX_CACHE_SIZE);
    for(int i = 0; i < NTHREADS; i++){
        Pthread_create(&tid, NULL, thread, NULL);
    }

    struct sockaddr_storage client_addr;
    int cfd;
    socklen_t addr_size;
    //new threaded loop
    while(1) {
        addr_size = sizeof(client_addr);
        cfd = accept(lsfd, (struct sockaddr *)&client_addr, &addr_size);
        if(cfd == -1) {
            perror("client-proxy accept:");
        }
        sbuf_insert(&sbuf, cfd);
    }
}
