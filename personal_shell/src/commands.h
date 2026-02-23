#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <unistd.h>

typedef struct pid_node {
    char *pid_value;
    struct pid_node *next_id;
}pid_node;

// struct client_sock {
//     int sock_fd;
//     int state;
//     char *username;
//     char buf[64];
//     int inbuf;
//     struct client_sock *next;
// };

// struct listen_sock {
//     struct sockaddr_in *addr;
//     int sock_fd;
// };

// struct server_sock {
//     int sock_fd;
//     char buf[64];
//     int inbuf;
// };


int pipe_ex(char **pipe_arr, int token_count);

void add_pid(char *user_input);

void check_pid();

void bg(char **token_arr, int token_count);


// void setup_server_socket(struct listen_sock *s, int port_number);

// int accept_connection(int fd, struct client_sock **clients);

// int read_from_client(struct client_sock *curr);

int read_from_socket(int sock_fd, char *buf, int *inbuf);

int get_message(char **dst, char *src, int *inbuf);

// void clean_exit(struct listen_sock s, struct client_sock *clients, int exit_status);

// int remove_client(struct client_sock **curr, struct client_sock **clients);

int find_network_newline(const char *buf, int inbuf);

// int write_buf_to_client(struct client_sock *c, char *buf, int len);

int write_to_socket(int sock_fd, char *buf, int len);

extern pid_node *front;

#endif