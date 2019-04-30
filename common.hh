#ifndef COMMON_HH
#define COMON_HH

inline int send_message(int sockfd, addrinfo *p, const char *message) {
    int numbytes;
    if ((numbytes = sendto(sockfd, message, strlen(message), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("client: sendto");
        exit(1);
    }
    return numbytes;
}

#endif
