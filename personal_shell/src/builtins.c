#include <string.h>
#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>
#include "commands.h"
#include <sys/socket.h>
#include <netinet/in.h>   
#include <arpa/inet.h> 
#include <netdb.h>
#include "other_helpers.h"
#include <assert.h>

int sigint_received = 0;


// ====== Command execution =====

/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd) {
    ssize_t cmd_num = 0;
    while (cmd_num < BUILTINS_COUNT &&
           strncmp(BUILTINS[cmd_num], cmd, MAX_STR_LEN) != 0) {
        cmd_num += 1;
    }
    return BUILTINS_FN[cmd_num];
}


char *variable(char **tokens, int index){
    if((strncmp(tokens[index], "$", 1)) == 0){
        char *var = tokens[index] + 1;
        // char *var = strtok(tokens[index], "$");
        char *output = (check_variable(var));
        if (output != NULL){
            return output;
        }
        else{
           return tokens[index];
        }
    }
    else{
        return tokens[index];
    }
}


// ===== Builtins =====

/* Prereq: tokens is a NULL terminated sequence of strings.
 * Return 0 on success and -1 on error ... but there are no errors on echo.
 */

ssize_t bn_echo(char **tokens) {
    ssize_t index = 1;

    if (tokens[index] != NULL) {
        display_message(variable(tokens, index));
        index += 1;
        // TODO:
        // Implement the echo command
    }
    while (tokens[index] != NULL) {
        if((strncmp(tokens[index], "$", 1)) == 0){
             char *var = tokens[index] + 1;
            // char *var = strtok(tokens[index], "$");
            char *output = check_variable(var);
            if (output != NULL){
                display_message(" ");
                display_message(output);
            }
            else{
                display_message(" ");
                display_message(tokens[index]);
            }
        }
        else{
            display_message(" ");
            display_message(tokens[index]);
            // TODO:
            // Implement the echo command
        }
        index += 1;
    }
    display_message("\n");

    return 0;
}

ssize_t bn_ls(char **tokens){
    DIR *d;
    ssize_t index = 1;
    char file[64];
    char substring[64];
    int depth;
    int rec_check = 0;
    int f_check = 0;
    int depth_check = 0;
    if (tokens[1] == NULL){
        return bn_ls_helper(".", 1, NULL);
    }
    
    while (tokens[index] != NULL) {
        if (((strcmp(tokens[index], "--rec")) == 0)){
            rec_check = rec_check + 1;
            if (tokens[index + 1] != NULL){
                strcpy(file, variable(tokens, (index + 1)));
            }
        }
        if (((strcmp(tokens[index], "--d")) == 0)){
            if (tokens[index + 1] != NULL){
                depth_check = depth_check + 1;
                depth = atoi(tokens[index + 1]);
            }
        }
        if (((strcmp(tokens[index], "--f")) == 0)){
            if (tokens[index + 1] != NULL){
                f_check = f_check + 1;
                strcpy(substring, tokens[index + 1]);
            }
        }
         index = index + 1;
    }
    if (depth_check == 1 && rec_check == 1){
        d = opendir(file);
        if (d != NULL){
            closedir(d);
            if (f_check == 1 && substring != NULL){
                return bn_ls_helper(file, depth, substring);
            }
            return bn_ls_helper(file, depth, NULL);
        }
        else{
            display_error("ERROR: Invalid path ", file);
            return -1;
        }
        closedir(d);      
    }
    else if (tokens[4] == NULL && f_check == 1){
        char file3[100];
        strcpy(file3, variable(tokens, 1));
        d = opendir(file3);
        if (d != NULL){
            closedir(d);
            return bn_ls_helper(file3, 1, substring);
        }
        else{
            display_error("ERROR: Invalid path ", file3);
            return -1;
        }
        closedir(d);      
    }
    else if (tokens[2] == NULL){
        char file2[100];
        strcpy(file2, variable(tokens, 1));
        d = opendir(file2);
        if (d != NULL){
            closedir(d);
            return bn_ls_helper(file2, 1, NULL);
        }
        else{
            display_error("ERROR: Invalid path ", file2);
            return -1;
        }
        closedir(d);      
    }
    else {
        display_message("ERROR: Invalid path ");
        return -1;
    }
    return 0;
}


ssize_t bn_ls_helper(char *filename, int depth, char *f){
    DIR *d;
    struct dirent *r;
    d = opendir(filename);
    if (d == NULL){
        return -1;
    }
    if (depth <= 0){
        closedir(d);
        return 0;
    }
    while ((r = readdir(d)) != NULL){
        if (f != NULL){
            if(strstr(r->d_name,f)){
                display_message(r->d_name);
                display_message("\n");
            }
            if(r->d_type == DT_DIR && strcmp(r->d_name, ".") != 0 && strcmp(r->d_name, "..") != 0){
                char newdir[300] = {};
                strcat(newdir, filename);
                strcat(newdir, "/");
                strcat(newdir, r->d_name);
                bn_ls_helper(newdir, depth - 1, f);
                }
        }
        else{
            display_message(r->d_name);
            display_message("\n");
            if(r->d_type == DT_DIR && strcmp(r->d_name, ".") != 0 && strcmp(r->d_name, "..") != 0){
                char newdir[300] = {};
                strcat(newdir, filename);
                strcat(newdir, "/");
                strcat(newdir, r->d_name);
                bn_ls_helper(newdir, depth - 1, f);
            }
        }

    }
    closedir(d);
    return 0;

    }

ssize_t bn_cd(char **tokens){
    if (tokens[1] == NULL){
        int change = chdir("~");
        if (change == -1){
            display_error("ERROR: Invalid path", " ");
            return -1;
            }
    return 0;

    }
    if ((strcmp(tokens[1], "..")) != 0 && (strcmp(tokens[1], ".") != 0 )){
        char final[70] = {};
        for (int i = 2; i < 102; i++) { 
            if (*((*(tokens+1))+i) == '.'){
                if(i == 2){
                    strcat(final, "../..");
                }
                else{
                    strcat(final, "/..");
                }
            }
            else{
                if(*((*(tokens+1))+i) != '\0'){
                    strcat(final, "breaknow!!!!");
                    break;
                }
                else{
                    break;
                }
            }
        }
        if (strstr(tokens[1], "..")){
            int change = chdir(final);
            if (change == -1){
                display_error("ERROR: Invalid path ", tokens[1]);
                return -1;
            }
    return 0;
        }
    }  
    int change = chdir(tokens[1]);
    if (change == -1){
        display_error("ERROR: Invalid path ", tokens[1]);
        return -1;
    }
    return 0;
}


ssize_t bn_cat(char **tokens){
    char buffer[4096];
    if (tokens[1] == NULL){
         while(fgets(buffer, 4097, stdin) != NULL){
            display_message(buffer);
        }
    }
    else{
        FILE *fileopen;
        fileopen = fopen(tokens[1], "r");
        if (fileopen == NULL){
            display_error("ERROR: Cannot open file ", tokens[1]);
            return -1;
        }
        while(fgets(buffer, 4097, fileopen) != NULL){
            display_message(buffer);
        }
        int error = fclose(fileopen);
        if (error != 0) {
            display_message("\n");
            display_error("ERROR: Cannot close file ", tokens[1]);
            return -1;
        }
    }

    return 0;
}


ssize_t bn_wc(char **tokens){
    FILE *fileopen;
    char buffer;
    char buffer2[4096];
    // char buffer3[4096];
    int words = 0;
    int lines = 0;
    int characters = 0;
    char temp;
    if (tokens[1] == NULL){
        while((buffer = fgetc(stdin)) != EOF){
            characters = characters + 1;
            if (buffer == '\n'){
                lines = lines + 1;
            }
            if (buffer == ' ' || buffer == '\t' || buffer == '\n' || buffer == '\0'){
                if (temp != '\0' && temp != '\n' && temp != ' ' && temp != '\t'){
                    words = words + 1;
                }
            }
            temp = buffer;
        }
        if (characters == lines){
            words = 0;
        }
        char str_word[200];
        sprintf(str_word, "word count %d", words);
        display_message(str_word);
        display_message("\n");
        char str_char[200];
        sprintf(str_char, "character count %d", characters);
        display_message(str_char);
        display_message("\n");
        char str_lines[200];
        sprintf(str_lines, "newline count %d", lines);
        display_message(str_lines);
        display_message("\n");
    }
    else{
        fileopen = fopen(tokens[1], "r");
        if (fileopen == NULL){
            display_error("ERROR: Cannot open file ", tokens[1]);
            return -1;
        }
            for (buffer = getc(fileopen); buffer != EOF; buffer = getc(fileopen)){
                if (buffer == '\n'){ 
                    lines = lines + 1;
                }
            }
        // while(fgets(buffer, 4097, fileopen) != NULL){
        //     lines = lines + 1;
        // }
        fclose(fileopen);
        fileopen = fopen(tokens[1], "r");
        while (fscanf(fileopen, " %s", buffer2) != EOF){
            words = words + 1;
        }
        fclose(fileopen);
        fileopen = fopen(tokens[1], "rb");
        fseek(fileopen, 0, SEEK_END);
        characters = ftell(fileopen);
        char str_word[200];
        sprintf(str_word, "word count %d", words);
        display_message(str_word);
        display_message("\n");
        char str_char[200];
        sprintf(str_char, "character count %d", characters);
        display_message(str_char);
        display_message("\n");
        char str_lines[200];
        sprintf(str_lines, "newline count %d", lines);
        display_message(str_lines);
        display_message("\n");
        int error = fclose(fileopen);
        if (error != 0) {
            display_error("ERROR: Cannot close file ", tokens[1]);
            return -1;
        }
    }
    return 0;

}


ssize_t bn_ps(char **tokens){
    pid_node *curr = front;  
    while (curr != NULL) {
        int new_pid = atoi(curr->pid_value);
        if (waitpid(new_pid, NULL, WNOHANG) == 0){
            display_message(curr->pid_value);
            display_message("\n");
            }
        curr = curr->next_id;
        curr = curr->next_id;
        }
    return 0;
}


ssize_t bn_kill(char **tokens){
    char pid[64];
    strcpy(pid, variable(tokens, 1));
    int new_pid = atoi(pid);
    if (tokens[2] == NULL){
        int err = kill(new_pid, SIGKILL);
        if (err == -1){
            display_error("ERROR: The process does not exist ", tokens[1]);
            return -1;
        }
    }
    else{
        char var_signal[64];
        strcpy(var_signal, variable(tokens, 2));
        int signal = atoi(var_signal);
        if (signal != 0 && signal < 32){
            int err = kill(new_pid, signal);
            if (err == -1){
                display_error("ERROR: The process does not exist ", tokens[1]);
                return -1;
            }
        }
        else{
            display_error("ERROR: Invalid signal specified ", tokens[2]);
            return -1 ;
        }
    }
    return 0;
}

ssize_t bn_start_server(char **tokens){
    if (tokens[1] == NULL){
        display_error("ERROR: No port provided", "");
        return -1;
    }
    setbuf(stdout, NULL);
    int sockett;
    sockett = setup_server_socket(&l, tokens);
    if (sockett == -1) {
        display_error("ERROR: Could not start server", "");
        return -1;
    }
    int file_d = l.sock_fd;
    fd_set a_file, l_file;
    FD_ZERO(&a_file);
    FD_SET(l.sock_fd, &a_file);
    pid_t pid = fork();
    if (pid == 0) {
        while (1) {
            l_file = a_file;
            select(file_d + 1, &l_file, NULL, NULL, NULL);
            if (FD_ISSET(l.sock_fd, &l_file)) {
                int client_fd = accept_connection(l.sock_fd, &c);
            if (client_fd < 0) {
                continue;
            }
            if (client_fd > file_d) {
                file_d = client_fd;
            }
            FD_SET(client_fd, &a_file);
            }
            struct c_sock *curr = c;
            while (curr != NULL) {
            if (!FD_ISSET(curr->sock_fd, &l_file)) {
                curr = curr->next;
                continue;
            }
            int client_closed = read_from_socket(curr->sock_fd, curr->buf, &(curr->inbuf));
            if (client_closed == -1) {
                client_closed = 1;
            }
            char buffer[BUF_SIZE];
            char *word;
            while (client_closed == 0 && !get_message(&word, curr->buf, &(curr->inbuf))) {
                buffer[0] = '\0';
                strncat(buffer, word, 128);
                display_message("\n");
                display_message(buffer);
                display_message("\n");
                free(word);
            }
            
            if (client_closed == 1) {
                FD_CLR(curr->sock_fd, &a_file);
                close(curr->sock_fd);
                assert(remove_client(&curr, &c) == 0);
            }
            else {
                curr = curr->next;
            }
            }
        }
    }
    return 1;


}

ssize_t bn_send(char **tokens){
    s_sock s;
    s.inbuf = 0;
    s.sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s.sock_fd < 0) {
        return -1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = atoi(tokens[1]);
    if(tokens[2] != NULL){
      if (inet_pton(AF_INET, tokens[2], &server.sin_addr) < 1) {
        close(s.sock_fd);
        return 1;
      }
    }
    else{
      display_error("ERROR: No port provided", "");
      }
    int con = connect(s.sock_fd, (struct sockaddr *)&server, sizeof(server));
    if (con == -1) {
        close(s.sock_fd);
        return -1;
    }
    char buffer[64+2];
    buffer[0] = '\0';
    int i = 3;
    while (tokens[i] != NULL) {
        strncat(buffer, tokens[i], strlen(tokens[i]));
        strncat(buffer, " ", 2);
        i++;
    }
    buffer[64] = '\r';
    buffer[64+1] = '\n';
    write_to_socket(s.sock_fd, buffer, MAX_STR_LEN+2);
    close(s.sock_fd);
    return 1;
}


ssize_t bn_start_client(char **tokens) {
    if (tokens[1] == NULL){
        display_error("ERROR: No port provided", "");
        display_error("ERROR: No hostname provided", "");
        return -1;
            }
    if (tokens[2] == NULL){
        display_error("ERROR: No hostname provided", "");
        return -1;
        }
    s.inbuf = 0;    
    s.sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s.sock_fd < 0) {
        return -1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = atoi(tokens[1]);
    if(tokens[2] != NULL){
      if (inet_pton(AF_INET, tokens[2], &server.sin_addr) < 1) {
        close(s.sock_fd);
        return -1;
      }
    }
    else{
      display_error("ERROR: No port provided", "");
      }
    int con = connect(s.sock_fd, (struct sockaddr *)&server, sizeof(server));
    if (con == -1) {
        close(s.sock_fd);
        return -1;
    }
    int counter;
    char temp[BUF_SIZE];
    fd_set file_des;
    while(1) {
        FD_ZERO(&file_des);
        FD_SET(STDIN_FILENO, &file_des);
        FD_SET(s.sock_fd, &file_des);
        if(STDIN_FILENO > s.sock_fd){
          counter = STDIN_FILENO;
        } 
        else {
         counter = s.sock_fd;
        }
        int soc = select(counter + 1, &file_des, NULL, NULL, NULL);
        if (soc == -1) {
            close(s.sock_fd);
            return -1;
        }
        if (FD_ISSET(STDIN_FILENO, &file_des)) {   
            do {
            while (fgets(temp, 126, stdin) != NULL) {
                int length = strlen(temp);
                const char newline = '\n';
                const char r = '\r';
                // temp[strlen(temp)] = newline;
                // temp[strlen(temp) - 1] = r;
                temp[length- 1] = r;
                temp[length] = newline;
                write_to_socket(s.sock_fd, temp, length + 1);
                temp[0] = '\0';
            }
            }
            while (sigint_received != 1);
            break;
        }      
    }

    close(s.sock_fd);
    return 1;
}

ssize_t bn_close_server() {
    close(l.sock_fd);
    free(l.addr);
    return 1;

}










