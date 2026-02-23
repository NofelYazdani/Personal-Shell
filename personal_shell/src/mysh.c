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

    
int main(int argc, char* argv[]) {    
    char *prompt = "mysh$ "; // TODO Step 1, Uncomment this.    
    char input_buf[MAX_STR_LEN + 1];  
    input_buf[MAX_STR_LEN] = '\0';  
    char *token_arr[MAX_STR_LEN] = {NULL};  
    char *pipe_arr[MAX_STR_LEN] = {NULL};  
    size_t token_count;  
    char input_buf2[MAX_STR_LEN + 1];
    while (1) {
   
        void handler(int code) {
           display_message("\n");
    }
        struct sigaction newact;
        newact.sa_handler = handler;
        newact.sa_flags = 0;
        sigemptyset(&newact.sa_mask);
        sigaction(SIGINT, &newact, NULL);    
  
        // Prompt and input tokenization    
    
        // TODO Step 2:   
        // Display the prompt via the display_message function.  
        display_message(prompt);  
        int ret = get_input(input_buf);  
        strcpy(input_buf2, input_buf);  
        token_count = tokenize_input(input_buf, token_arr); 

        for (int i = 0; i < token_count; i++){
            char *temp;
            temp = variable(token_arr, i);
            token_arr[i] = temp;
        }         
        // Clean exit    
        if (ret != -1 && ret != 1 && (token_count == 0 || (strcmp("exit", token_arr[0]) == 0 ))) {       
            break;        
        }    
        int k = 0;  
        if (strstr(input_buf2, "|") != NULL){  
            token_count = tokenize_pipe(input_buf2, pipe_arr);  
            k = 1;  
        }       
        // Command execution
        check_pid();
        if (token_count >= 1) {   
            if (k == 1){  
                pipe_ex(pipe_arr, token_count); 
            } 
            else if (strcmp(token_arr[token_count - 1], "&") == 0){
                bg(token_arr, token_count);
            }
            else{  
                bn_ptr builtin_fn = check_builtin(token_arr[0]);    
                if (builtin_fn != NULL) {    
                    ssize_t err = builtin_fn(token_arr);    
                    if (err == - 1) {    
                        display_error("ERROR: Builtin failed: ", token_arr[0]);    
                    }    
                }
                else if (strchr(token_arr[0], '=') == NULL){
                    pid_t pid = fork();
                    if (pid == 0){
                        int err = execvp(token_arr[0], token_arr);
                        if (err == -1){
                            display_error("ERROR: Unrecognized command: ", token_arr[0]);
                        }
                        exit(1);
                    }
                    else{
                        wait(NULL);
                    }

                }   
                else {  
                    if(strchr(token_arr[0], '=') && token_count == 1){
                        char *strcopy = malloc(sizeof(char)* 64);    
                        strcpy(strcopy, token_arr[0]);   
                        char *right = strchr(strcopy, '=');    
                        right++;    
                        char *temp = strtok(strcopy, "=");
                        if(exist(temp) == 1){    
                            replace(temp, right);
                        }
                        else{    
                            add_node(temp);    
                            add_node(right);
                        }  
                    }    
                    else{    
                        display_error("ERROR: Unrecognized command: ", token_arr[0]);    
                    }    
                }    
            }  
  
        }  
      
    }  
    return 0;    
}  
