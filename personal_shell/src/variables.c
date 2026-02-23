#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <string.h>  
#include "builtins.h"  
#include "io_helpers.h"  
#include "variables.h"  
  
Node *head;  
  
void add_node(char *user_input) {  
    Node *new_node = malloc(sizeof(Node));  
    new_node->value = user_input;  
    new_node->next = NULL;  
    if (head == NULL){  
        head = new_node;  
    }  
    else{  
        Node *curr = head;  
        while (curr->next != NULL) {  
            curr = curr->next;  
        }  
        curr->next = new_node;  
    }  
}  
  
  
char* check_variable(char *var){  
    Node *curr = head;  
    while (curr != NULL) {  
        if ((strncmp(curr->value, var, MAX_STR_LEN)) == 0){  
            curr = curr->next;  
            return curr->value;  
        }  
        else{  
            curr = curr->next;  
            curr = curr->next;  
        }  
    }  
    return NULL;  
  
}  
  
int exist(char *var){  
    Node *curr = head;  
    while (curr != NULL) {  
        if ((strncmp(curr->value, var, MAX_STR_LEN)) == 0){  
            return 1;  
        }  
        else{  
            curr = curr->next;  
            curr = curr->next;  
        }  
    }  
    return 0;  
}  
  
void replace(char *var, char *id){ 
    Node *newNode = malloc(sizeof(Node));  
    newNode->value = var;
    Node *newNode2 = malloc(sizeof(Node));  
    newNode2->value = id;
    newNode->next = newNode2;
    newNode2->next = head;
    head = newNode;

} 
