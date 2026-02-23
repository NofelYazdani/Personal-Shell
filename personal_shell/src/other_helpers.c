#include <stdio.h>    
#include <stdlib.h>    
#include <string.h>    
#include <sys/types.h>    
#include <unistd.h>    
#include "builtins.h"    
#include "io_helpers.h"    
#include "variables.h"    
#include <sys/wait.h>  
#include "commands.h"
#include <sys/socket.h>
#include <netinet/in.h>   
#include <arpa/inet.h> 
#include <netdb.h>
#include <errno.h>
#include <assert.h>
#include "other_helpers.h"

s_sock s;
l_sock l;
c_sock *c = NULL;

int setup_server_socket(struct l_sock *s, char **tokens) {
    if(!(s->addr = malloc(sizeof(struct sockaddr_in)))) {
        return -1;
    }
    s->addr->sin_family = AF_INET;
    s->addr->sin_port = atoi(tokens[1]);
    memset(&(s->addr->sin_zero), 0, 8);
    s->addr->sin_addr.s_addr = INADDR_ANY;
    s->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s->sock_fd < 0) {
        return -1;
    }
    int on = 1;
    int status = setsockopt(s->sock_fd, SOL_SOCKET, SO_REUSEADDR,
        (const char *) &on, sizeof(on));
    if (status < 0) {
        display_error("socc", " ");
        return -1;
    }
    if (bind(s->sock_fd, (struct sockaddr *)s->addr, sizeof(*(s->addr))) < 0) {
        close(s->sock_fd);
        return -1;
    }
    if (listen(s->sock_fd, 5) < 0) {
        close(s->sock_fd);
        return -1;
    }
    return 0;
}


int accept_connection(int fd, struct c_sock **clients) {
    struct sockaddr_in peer;
    unsigned int peer_len = sizeof(peer);
    peer.sin_family = AF_INET;

    int num_clients = 0;
    struct c_sock *curr = *clients;
    while (curr != NULL && num_clients < 14 && curr->next != NULL) {
        curr = curr->next;
        num_clients++;
    }

    int client_fd = accept(fd, (struct sockaddr *)&peer, &peer_len);
    
    if (client_fd < 0) {
        close(fd);
        return -1;
    }
    if (num_clients == 13) {
        close(client_fd);
        return -1;
    }
    struct c_sock *newclient = malloc(sizeof(struct c_sock));
    newclient->sock_fd = client_fd;
    newclient->inbuf = newclient->state = 0;
    newclient->next = NULL;
    memset(newclient->buf, 0, BUF_SIZE);
    if (*clients == NULL) {
        *clients = newclient;
    }
    else {
        curr->next = newclient;
    }
    return client_fd;
}



int remove_client(struct c_sock **curr, struct c_sock **clients) {
    struct c_sock *node, *node2;
    node = NULL;
    node2 = *clients;
    if (node2 == NULL){
        return 1;
    }
    while (node2 != NULL) {
        if (node != NULL) {
            if (*curr == node2) {
                node->next = node2->next;
                *curr = node2->next;    
                close(node2->sock_fd);
                free(node2);
                return 0;
            }
        }
        else {
            if (*curr == node2) {
                *clients = node2->next;
                *curr = node2->next;
                close(node2->sock_fd);
                free(node2);
                return 0;
            }
        }
        node = node2;
        node2 = node2->next;
    }
    return 1;
}