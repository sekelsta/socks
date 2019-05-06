#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <stdexcept>

#include "defines.hh"
#include "common.hh"

int main(int argc, char *argv[])
{
    if (argc != 2 && argc != 3) {
        fprintf(stderr,"usage: %s hostname\nor:    %s hostname port",
            argv[0], argv[0]);
        exit(1);
    }
    const char *port = PORT;
    if (argc == 4) {
        try {
            std::stoi(argv[2]);
        }
        catch (const std::invalid_argument& e) {
            fprintf(stderr, "Could not convert port number to integer.\n");
            exit(1);
        }
        catch (const std::out_of_range& e) {
            fprintf(stderr, "Port number is too large to convert to integer.\n");
            exit(1);
        }
        // Port is valid, so set it
        port = argv[2];
    }

    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    char buf[MAXBUFLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(argv[1], port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to create socket\n");
        return 2;
    }

    connect(sockfd, p->ai_addr, p->ai_addrlen);

    int bytes_sent = send_message(sockfd, p, REQUEST_NEW_CONNECTION);


    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];
    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }
    buf[numbytes] = '\0';

    if (strcmp(buf, ACCEPT_NEW_CONNECTION) == 0) {
        printf("Connection accepted\n");
    }
    else if (strcmp(buf, REJECT_NEW_CONNECTION) == 0) {
        printf("Connection rejected\n");
    }
    else {
        printf("Recieved unexpected reply %s\n", buf);
    }

    freeaddrinfo(servinfo);

    printf("client: sent %d bytes to %s\n", numbytes, argv[1]);
    close(sockfd);

    return 0;
}
