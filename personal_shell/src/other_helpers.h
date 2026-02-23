#ifndef _OTHER_HELPERSH
#define _OTHER_HELPERSH

#ifndef BUF_SIZE
    #define BUF_SIZE MAX_PROTO_MSG+1 
#endif

#ifndef MAX_PROTO_MSG
    #define MAX_PROTO_MSG 10+1+128+2
#endif


typedef struct s_sock {
    int sock_fd;
    char buf[BUF_SIZE];
    int inbuf;
} s_sock;

typedef struct l_sock {
    struct sockaddr_in *addr;
    int sock_fd;
} l_sock;


typedef struct c_sock {
    int sock_fd;
    int state;
    char buf[BUF_SIZE];
    int inbuf;
    struct c_sock *next;
} c_sock;

int setup_server_socket(struct l_sock *s, char **tokens);
int accept_connection(int fd, struct c_sock **clients);
int remove_client(struct c_sock **curr, struct c_sock **clients);


extern s_sock s;
extern l_sock l;
extern c_sock *c;

#endif