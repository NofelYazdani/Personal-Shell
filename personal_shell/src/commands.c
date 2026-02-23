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
#include "other_helpers.h"


pid_node *front;  


int pipe_ex(char **pipe_arr, int token_count){
    int fd[2 * (token_count -1)];
    int pid;
    for (int i = 0; i < (token_count -1); i++){
        pipe(fd + i*2);
    }
    int index = 0;
    while(index < token_count){
        char *commands[MAX_STR_LEN] = {NULL};
        tokenize_input(pipe_arr[index], commands);
        pid = fork();

        if (pid == 0){
            if (index  > 0){
                dup2(fd[(index-1)*2], STDIN_FILENO);

            }
            if (index != (token_count - 1)){
                dup2(fd[index*2 + 1], STDOUT_FILENO);
            }

            for (int i = 0; i < 2 * (token_count -1); i++){
                close(fd[i]);
            }
            bn_ptr pipe_fn = check_builtin(commands[0]);
                if (pipe_fn != NULL) {
                    ssize_t err = pipe_fn(commands);  
                    if (err == - 1) {  
                        display_error("ERROR: Builtin failed: ", commands[0]);  
                        }
                exit(1); 
                }
                else if (strchr(commands[0], '=') == NULL){
                    pid_t pid = fork();
                    if (pid == 0){
                        int err = execvp(commands[0], commands);
                        if (err == -1){
                            display_error("ERROR: Unrecognized command: ", commands[0]);
                        }
                    }
                    else{
                        wait(NULL);
                    }

                }   
            else{
                if(strchr(commands[0], '=') && token_count == 1){  
                char *strcopy = malloc(sizeof(char)* 64);  
                strcpy(strcopy, commands[0]); 
                char *right = strchr(strcopy, '=');  
                right++;  
                char *temp = strtok(strcopy, "=");  
                if(exist(temp) == 1){  

                    replace(temp ,right);  
                }  
                else{  
                    add_node(temp);  
                    add_node(right);  
                }
            }  
            else{  
                display_error("ERROR: Unrecognized command: ", commands[0]);  

            }
            }
        }
        index = index + 1;
    }
    for (int i = 0; i < 2*(token_count -1); i++){
        close(fd[i]);
    }
    for (int i = 0; i < token_count; i++){
        wait(NULL);
    }
    return 0;

}

void add_pid(char *user_input) {  
    pid_node *new_node = malloc(sizeof(pid_node));  
    new_node->pid_value = user_input;  
    new_node->next_id = NULL;  
    if (front == NULL){  
        front = new_node;  
    }  
    else{  
        pid_node *curr = front;  
        while (curr->next_id != NULL) {  
            curr = curr->next_id;  
        }  
        curr->next_id = new_node;  
    }  
}

void check_pid(){
    pid_node *curr = front;  
    while (curr != NULL) {
        int new_pid = atoi(curr->pid_value);
        if (waitpid(new_pid, NULL, WNOHANG) > 0){
            display_message(curr->next_id->pid_value);
            display_message("Done ");
            char *final[MAX_STR_LEN] = {NULL};
            tokenize_input(curr->pid_value, final);
            display_message(final[1]);
            display_message(" ");
            display_message(final[2]);
            display_message("\n");
        }
        curr = curr->next_id;
        curr = curr->next_id;
    }
}  

int length_node(){
    int count = 1; 
    pid_node *curr = front;
    while (curr != NULL){
        int new_pid = atoi(curr->pid_value);
        if (waitpid(new_pid, NULL, WNOHANG) == 0){
            count = count + 1;
        }
        curr = curr->next_id;
        curr = curr->next_id;
    }
    return count;
}

void bg(char **token_arr, int token_count){
    token_arr[token_count - 1] = NULL;
    int count = length_node();
    pid_t pid = fork();
    char *parentid = malloc(sizeof(char)* 64);
    char num[20];
    char new_num[30] = "\0";
    char id[30];
    char procces_number[20];
    sprintf(procces_number, "%d", count);
    if (pid == 0){
        strcat(new_num, "[");
        strcat(new_num, procces_number);
        strcat(new_num, "] ");
        display_message(new_num);
        sprintf(id, "%d", getpid());
        display_message(id);
        display_message("\n");
        bn_ptr builtin_fn = check_builtin(token_arr[0]);  
        if (builtin_fn != NULL) {    
            ssize_t err = builtin_fn(token_arr);    
            if (err == - 1) {    
                display_error("ERROR: Builtin failed: ", token_arr[0]);
            }
            exit(1);
        }
        else{
            execvp(token_arr[0], token_arr);
            if (waitpid(getpid(), NULL, WNOHANG) > 0){
                display_message("DONE1");
        }
            exit(1);
        }
    }
    else{
        char copy_id[64] = "\0";
        char ps_id[64] = "\0";
        char *final_id = malloc(sizeof(char) * 64);
        if (waitpid(pid, NULL, WNOHANG) > 0){
            display_message("DONE1");
        }
        else if (waitpid(pid, NULL, WNOHANG) == 0){
            strcat(ps_id, "[");
            strcat(ps_id, procces_number);
            strcat(ps_id, "]+  ");
                sprintf(num, "%d", pid);
                strcat(copy_id, num);
                strcat(copy_id, " ");
                strcat(copy_id, token_arr[0]);
                strcat(copy_id, " ");
                strcat(copy_id, token_arr[1]);
                strcpy(parentid, copy_id);
                strcpy(final_id, ps_id);
                add_pid(parentid);
                add_pid(final_id);
                }
        else if (waitpid(pid, NULL, WNOHANG) == -1){
            display_message("ERROR");
        }
    }
}

int find_network_newline(const char *buf, int inbuf) {
    for (int x = 0; x < inbuf - 1; x++) {
        if(buf[x] == '\r' && buf[x + 1] == '\n') {
            return x + 2;
        }
    }
    return -1;
}

int read_from_socket(int sock_fd, char *buf, int *inbuf) {
    int size = read(sock_fd, buf + *inbuf, BUF_SIZE - *inbuf);
    if(size == 0) {
        return 1;
    }
    else if(size < 0) {
        return -1;
    }
    *inbuf = *inbuf + size;
    char const newline = '\n';
    int i = -1;
    while(i <= (*inbuf - 1)){
        if(buf[i] == '\r'){
            if (buf[i + 1] == newline){
                return 0;
            }
        }
        i = i + 1;
    }
    if (*inbuf == BUF_SIZE) {
        return -1;
    }

    return 2;
}

int get_message(char **dst, char *src, int *inbuf) {
    int size = find_network_newline(src, *inbuf);
    if(size == -1){
        return 1;
    }
    *dst = malloc(BUF_SIZE);
    if (*dst == NULL) {
        return 1;
    }
    memcpy(*dst, src, size - 2);
    (*dst)[size -2] = '\0';
    memmove(src, src + size, BUF_SIZE - size);
    *inbuf -= size;
    return 0;
}

int write_to_socket(int sock_fd, char *buf, int len) {
  int s = write(sock_fd, buf, len);
  if (s == 0) {
      return 2;
  }
  else if (s == -1) {
      return 1;
  }
  return 0;
}

