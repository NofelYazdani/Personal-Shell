#ifndef __VARIABLES_H__
#define __VARIABLES_H__

#include <sys/types.h>


typedef struct node {
    char *value;
    struct node *next;
}Node;

char *check_variable(char *var);

void add_node(char *str);


// void delete_node(char *var, char *name);

int exist(char *var);

void replace(char *temp, char *id);

int length_node();

extern Node *head;

#endif