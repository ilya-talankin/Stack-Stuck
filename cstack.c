#include "cstack.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNUSED(VAR) (void)(VAR)

typedef struct node
{
    const struct node* prev;
    unsigned int size;
    char* data;
    unsigned int node_number;
} node_t;

typedef struct top_node_table {
    unsigned short int reserved;
    struct node* p_top_node;
} stack_t;

stack_t** handlers_table; 
int stacks_count = 0;

// Functions:

hstack_t stack_new()
{
    hstack_t handler = -1;
    // search for empty space
    for (int i = 0; i < stacks_count; i++) {
        if (handlers_table[i] -> reserved == 0) {
            handlers_table[i] -> reserved = 1;
            handlers_table[i] -> p_top_node = NULL;
            return i;
        }
    }
    // array extension
    handlers_table = realloc(handlers_table, (stacks_count + 1) * sizeof(handlers_table));
    if (handlers_table == NULL) { //checking for correct memory allocation
        return -1;
    }
    ++ stacks_count;
    handler = stacks_count - 1;
    handlers_table[handler] = malloc(sizeof(stack_t*));
    handlers_table[handler] -> reserved = 1;
    handlers_table[handler] -> p_top_node = NULL;
    return handler;
}

void stack_free(const hstack_t hstack) {
    if (stack_valid_handler(hstack) != 0) {
        return;
    }
    if (handlers_table[hstack] -> p_top_node == NULL) {
        handlers_table[hstack] -> reserved = 0;
        return;
    } //добавить дефрагментацию!!!
    while (handlers_table[hstack] -> p_top_node != NULL) {
        const struct node* prev_node = handlers_table[hstack] -> p_top_node -> prev;
        //НЕ РАБОТАЕТ ОСВОБОЖДЕНИЕ ПАМЯТИ!!!!!!!!!!!!!!
        //free(handlers_table[hstack] -> p_top_node -> data); 
        //free(handlers_table[hstack] -> p_top_node);
        handlers_table[hstack] -> p_top_node = prev_node;
    }
    handlers_table[hstack] -> reserved = 0;
}   

int stack_valid_handler(const hstack_t hstack){
    if ((hstack >= 0) && (hstack < stacks_count) && (handlers_table[hstack] -> reserved != 0)){
        return 0;
    } else return 1;
}

unsigned int stack_size(const hstack_t hstack){   
    if (stack_valid_handler(hstack) == 0){
        if (handlers_table[hstack] -> p_top_node != NULL){
            unsigned int size = handlers_table[hstack] -> p_top_node -> node_number;
            return size;
        }
        else return 0;
    }
}

//НУЖНО СДЕЛАТЬ ЧИТАБЕЛЬНЕЕ!!!

void stack_push(const hstack_t hstack, const void* data_in, const unsigned int size) {     
    if ((stack_valid_handler(hstack) == 0)&&(data_in != NULL)){
        struct node* p_prev = handlers_table[hstack] -> p_top_node; //saving pointer to prev element
        int new_node_number = 1;
        if (p_prev != NULL) {
            new_node_number = p_prev -> node_number + 1; //increment number of node
        }
        handlers_table[hstack] -> p_top_node = (node_t*) malloc(sizeof(node_t*));
        if (handlers_table[hstack] -> p_top_node == NULL) { //checking for correct memory allocation
            return;
        }
        handlers_table[hstack] -> p_top_node -> prev = p_prev;
        handlers_table[hstack] -> p_top_node -> node_number = new_node_number;
        handlers_table[hstack] -> p_top_node -> data = (char*) malloc (size);
        if (handlers_table[hstack] -> p_top_node -> data == NULL) { //checking for correct memory allocation
            return;
        }
        handlers_table[hstack] -> p_top_node -> size = size;
        char *data_out = handlers_table[hstack] -> p_top_node -> data;
        memcpy(data_out, data_in, size);
    }
}

unsigned int stack_pop(const hstack_t hstack, void* data_out, const unsigned int size)
{
    if ((stack_valid_handler(hstack) == 0)
        &&(data_out != NULL)
        &&(handlers_table[hstack] -> p_top_node != NULL)) {

        int size_to_copy = (handlers_table[hstack] -> p_top_node -> size > size) ? size : handlers_table[hstack] -> p_top_node -> size;
        char *data_in = handlers_table[hstack] -> p_top_node -> data;
        memcpy(data_out, data_in, size_to_copy);
        const struct node* prev_node = handlers_table[hstack] -> p_top_node -> prev;
        //НЕ РАБОТАЕТ ОСВОБОЖДЕНИЕ ПАМЯТИ!!!!!!!!!!!!!!
        //free(handlers_table[hstack] -> p_top_node -> data); 
        //free(handlers_table[hstack] -> p_top_node);
        handlers_table[hstack] -> p_top_node = prev_node;
        return size_to_copy;
    }
}

int main()
{   
    while(1){
        char input;
        printf("\nChoose function:\nn - stack_new\nf - stack free\nv - stack_valid_handler\ns - stack_size\nu - stack_push\no - stack_pop\nq - quit\nYour char: ");
        scanf("%c", &input);
        if (input == 'n'){
            printf("%d\n", stack_new());
        }
        if (input == 'v'){
            printf("Enter the handler: ");
            int h;
            scanf("%d", &h);
            if (!stack_valid_handler(h)) {
                printf("Handler is valid\n");
            } else printf("Handler is NOT valid\n");
        }
        if (input == 'u'){
            printf("Enter the handler: ");
            int h;
            scanf("%d", &h);
            int x;
            printf("Enter the int to push: ");
            scanf("%d", &x);
            stack_push(h, &x, sizeof(x));
        }
        if (input == 'o') {
            printf("Enter the handler: ");
            int h;
            scanf("%d", &h);
            int y;
            stack_pop(h, &y, 4);
            printf("Popped: %d\n", y);
        }
        if (input == 'f'){
            printf("Enter the handler: ");
            int h;
            scanf("%d", &h);
            stack_free(h);
        }
        if (input == 's'){
            printf("Enter the handler: ");
            int h;
            scanf("%d", &h);
            printf("%d\n", stack_size(h));
        }
        
        if (input == 'q') {
            return 0;
        }

    }
    return 0;
}