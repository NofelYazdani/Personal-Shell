#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include <unistd.h>


/* Type for builtin handling functions
 * Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
typedef ssize_t (*bn_ptr)(char **);
ssize_t bn_echo(char **tokens);
char *variable(char **tokens, int index);
ssize_t bn_ls(char **tokens);
ssize_t bn_ls_helper(char *filename, int depth, char* f);
ssize_t bn_cd(char **tokens);
ssize_t bn_cat(char **tokens);
ssize_t bn_wc(char **tokens);
ssize_t bn_ps(char **tokens);
ssize_t bn_kill(char **tokens);
ssize_t bn_send(char **tokens);
ssize_t bn_start_server(char **tokens);
ssize_t bn_close_server();
ssize_t bn_start_client(char **tokens);




/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd);


/* BUILTINS and BUILTINS_FN are parallel arrays of length BUILTINS_COUNT
 */
static const char * const BUILTINS[] = {"echo", "ls", "cd", "cat", "wc", "ps", "kill", "start-server", "send", "start-client", "close-server"};
static const bn_ptr BUILTINS_FN[] = {bn_echo, bn_ls, bn_cd, bn_cat, bn_wc, bn_ps, bn_kill, bn_start_server, bn_send, bn_start_client, bn_close_server, NULL};    // Extra null element for 'non-builtin'
static const size_t BUILTINS_COUNT = sizeof(BUILTINS) / sizeof(char *);

#endif
