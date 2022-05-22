#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <getopt.h>

#include "tcp_client.h"

#define SA struct sockaddr
int VERBOSE;

int verbose(const char * restrict format, ...) {
    if (!VERBOSE) {
        return 0;
    }

    va_list args;
    va_start(args, format);
    int ret = vprintf(format, args);
    va_end(args);

    return ret;
}

void call_server(int sockfd, char *input, int i) {
    write(sockfd, input, i);
}

void tcp_client(char *ip, int port, char *cmd) {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        verbose("Socket Creation Failed...\n"); // TODO print standard failure
        exit(0);
    }
    verbose("Socket Creation Successful...\n");
    bzero(&servaddr, sizeof(servaddr));
    
    //  Assign IP, Port...
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);

    // Connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        verbose("Connection with the server failed...\n"); // TODO Standard failure
        exit(0);
    }

    verbose("Connected to the sever!\n");
    verbose("IP:    %s \n",    ip);
    verbose("PORT:  %d \n",  port);
    verbose("CMD:   %s \n",   cmd);

    call_server(sockfd, cmd, strlen(cmd));

    close(sockfd);
}

/*
 *  The following code for the lookup_host function was taken from:
 *  
 *  https://gist.github.com/jirihnidek/bf7a2363e480491da72301b228b35d5d
 *
 *  Title:
 *  Example getaddrinfo() program
 *
 *  Author(s):
 *  jirihnidek (Original Author)
 *  n1s3 (Author of This Program)
 *
 */

char *lookup_host(const char *host) {
    struct addrinfo hints, *res, *result;
    int errcode;
    char *addrstr;
    void *ptr;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family     = PF_UNSPEC;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_flags     |= AI_CANONNAME;

    errcode = getaddrinfo(host, NULL, &hints, &result);

    if (errcode != 0) {
        perror("getaddrinfo");
        return "error";
    }

    res = result;

    verbose ("Host: %s\n", host);
    while (res) {
        addrstr = (char *) malloc(res->ai_addrlen);
        inet_ntop (res->ai_family, res->ai_addr->sa_data, addrstr, res->ai_addrlen);

        switch (res->ai_family) {
            case AF_INET:
                ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
                break;
            case AF_INET6:
                ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
                break;
            }
        inet_ntop (res->ai_family, ptr, addrstr, res->ai_addrlen);
        verbose("IPv%d address: %s (%s)\n", res->ai_family == PF_INET6 ? 6 : 4, addrstr, res->ai_canonname);
        res = res->ai_next;
    }
  
    freeaddrinfo(result);
    
    return addrstr;
}

int main(int argc, char **argv) {
    char *cmd   = "00"; // Default cmd is toggle
    char *ip    = IP_ADDR;
    int port    = PORT;

    int c;
    while(1) {
        static struct option long_options[] = {
            {"version", no_argument,        0,  'V'},
            {"verbose", no_argument,        0,  'v'},
            {"help",    no_argument,        0,  'h'},
            {"address", required_argument,  0,  'i'},
            {"port",    required_argument,  0,  'p'},
            {"command", required_argument,  0,  'c'},  
            {0, 0, 0, 0,}
        };
         
        int option_index = 0;
        c = getopt_long(argc, argv, "Vvhi:p:c:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        //TODO Handle command line arguments
        switch(c) {
            case 'V':
                printf("Client Version %s\n", PROGRAM_VERSION); //TODO follow standard version output
                break;
            case 'v':
                VERBOSE = 1;
                break;
            case 'h':
                printf("Print help related info here.. TODO\n"); //TODO follow standard help output
                exit(0);
                break;
            case 'i':
                ip = (char *) malloc(strlen(optarg));
                strcpy(ip, optarg);
                break;
            case 'p':
                port = atoi(optarg); 
                if (port == 0) { abort(); } //TODO invalid port handling
                break;
            case 'c':
                cmd = (char *) malloc(strlen(optarg));
                strcpy(cmd, optarg);
                break;
            default:
                abort();
        }
    }
    
    tcp_client(lookup_host(ip), port, cmd);
}
