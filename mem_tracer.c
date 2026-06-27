/**
 * Description: This module stores command lines in a dynamically allocated array 
                and traces the memory usage.
 * Author names: Adelyn Tam, Sanaa Stanezai
 * Author emails: Adelyn.tam@sjsu.edu, Sanaa.stanezai@sjsu.edu 
 * Last modified date:04/20/2025
 * Creation date: 04/20/2025
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_LENGTH 1024
#define INITIAL_SIZE 10

//below is helper code from prof
struct TRACE_NODE_STRUCT {
    char* functionid; // ptr to function identifier (a function name)
    struct TRACE_NODE_STRUCT* next; // ptr to next frama
};
typedef struct TRACE_NODE_STRUCT TRACE_NODE;
static TRACE_NODE* TRACE_TOP = NULL; // ptr to the top of the stack

void PUSH_TRACE(char* p) // push p on the stack
{
    TRACE_NODE* tnode;
    static char glob[]="global";
    if (TRACE_TOP==NULL) {
        // initialize the stack with "global" identifier
        TRACE_TOP=(TRACE_NODE*) malloc(sizeof(TRACE_NODE));
        // no recovery needed if allocation failed, this is only
        // used in debugging, not in production
    if (TRACE_TOP==NULL) {
        printf("PUSH_TRACE: memory allocation error\n");
        exit(1);
    }
    TRACE_TOP->functionid = glob;
    TRACE_TOP->next=NULL;
    }
    tnode = (TRACE_NODE*) malloc(sizeof(TRACE_NODE));
    if (tnode==NULL) {
    printf("PUSH_TRACE: memory allocation error\n");
    exit(1);
    }
    tnode->functionid=p;
    tnode->next = TRACE_TOP; // insert fnode as the first in the list
    TRACE_TOP=tnode; // point TRACE_TOP to the first node
}

void POP_TRACE() // remove the top of the stack
{
    TRACE_NODE* tnode;
    tnode = TRACE_TOP;
    TRACE_TOP = tnode->next;
    free(tnode);
}

char* PRINT_TRACE() {
    int depth = 50; //A max of 50 levels in the stack will be combined in a string for printing out.
    int i, length, j;
    TRACE_NODE* tnode;
    static char buf[100];
    if (TRACE_TOP==NULL) { // stack not initialized yet, so we are
        strcpy(buf,"global"); // still in the `global' area
        return buf;
    }
    sprintf(buf,"%s",TRACE_TOP->functionid);
    length = strlen(buf); // length of the string so far
    for(i=1, tnode=TRACE_TOP->next;
        tnode!=NULL && i < depth;
            i++,tnode=tnode->next) {
    j = strlen(tnode->functionid); // length of what we want to add
    if (length+j+1 < 100) { // total length is ok
        sprintf(buf+length,":%s",tnode->functionid);
        length += j+1;}
    else // it would be too long
        break;}
    return buf;
} 

int add_column(int** array,int rows,int columns) {
    PUSH_TRACE("add_column");
    int i;
    for(i=0; i<rows; i++) {
        array[i]=(int*) realloc(array[i],sizeof(int)*(columns+1));
        array[i][columns]=10*i+columns;
    }//for
    POP_TRACE();
    return (columns+1);
}

void make_extend_array() {
    PUSH_TRACE("make_extend_array");
    int i, j;
    int **array;
    int ROW = 4;
    int COL = 3;
    //make array
    array = (int**) malloc(sizeof(int*)*4); // 4 rows
    for(i=0; i<ROW; i++) {
        array[i]=(int*) malloc(sizeof(int)*3); // 3 columns
        for(j=0; j<COL; j++)
            array[i][j]=10*i+j;
    }
    for(i=0; i<ROW; i++)
        for(j=0; j<COL; j++)
            printf("array[%d][%d]=%d\n",i,j,array[i][j]);
    int NEWCOL = add_column(array,ROW,COL);
    // now display the array again
    for(i=0; i<ROW; i++)
        for(j=0; j<NEWCOL; j++)
            printf("array[%d][%d]=%d\n",i,j,array[i][j]);
    //now deallocate it
    for(i=0; i<ROW; i++)
        free((void*)array[i]);
    free((void*)array);
    POP_TRACE();
    return;
}

typedef struct Node {
    char*line;
    int index;
    struct Node *next;
} Node;

//recursive print linked list function
void printNodes(Node *node) {
    if (node==NULL) {
        return;
    }
    printf("Line %d: %s\n", node->index,node->line);
    printNodes(node->next);
}

//create a node function
Node* createNode(const char *line, int index){
    Node *new_node = malloc(sizeof(Node));
    if(!new_node){
        perror("malloc failure");
        exit(EXIT_FAILURE);
    }
    new_node->line = malloc(strlen(line)+1);
    if(!new_node->line) {
        perror("malloc failure");
    }
    strcpy(new_node->line, line);
    new_node->index = index;
    new_node->next = NULL;
    return new_node;
}

//free linked list function
void freeLinkedlist(Node *head){
    while(head){
        Node *temp = head;
        head = head->next;
        free(temp->line);
        free(temp);
    }
}

//overwrite malloc function with printf statement
void* MALLOC(int t, char* file, int line){
    void*p = malloc(t);
    printf("File %s, line %d, function %s allocated new memory segment at address %p with size %d\n", 
        file, line, PRINT_TRACE(),p,t);
    return p;
}

//overwrite realloc function with printf statement
void* REALLOC(void* p, int t, char* file, int line){
    void* new_p = realloc(p, t);
    printf("File %s, line %d, function %s reallocated memory from address %p to new address %p with size %d\n", 
        file, line, PRINT_TRACE(), p, new_p, t);
    return new_p;
}


//overwrite free with new printf statement
void FREE(void* p, char* file, int line){
    printf("File %s, line %d, function %s freed memory segment at address %p\n", 
        file, line, PRINT_TRACE(), p);
    free(p); //  actually frees memory
}

#define realloc(a,b) REALLOC(a,b,__FILE__,__LINE__)
#define malloc(a) MALLOC(a,__FILE__,__LINE__)
#define free(a) FREE(a,__FILE__,__LINE__)

int main() {
    //open file memtrace.out for writing
    int fd = open("memtrace.out", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd <0) {
        perror("Failure to open memtrace.out");
        exit(1);
    }
    //redirect stdout to memtrace.out
    if(dup2(fd, STDOUT_FILENO) < 0) {
        perror("Failure to redirect stdout");
        exit(1);
    }
    PUSH_TRACE("main");
    make_extend_array();

    int capacity = INITIAL_SIZE;
    int command_count = 0;
    char **commands = malloc(capacity * sizeof(char*)); //freed at end
    if (!commands) {
        perror("malloc failure");
        exit(EXIT_FAILURE);
    }
    char buffer[MAX_LENGTH];
    Node *head = NULL;
    Node *tail = NULL;
    // Read commands from stdin
    while (fgets(buffer, MAX_LENGTH, stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        //expand array if reached capacity
        if(command_count == capacity) {
            capacity *= 2;
            char **temp = realloc(commands, capacity*sizeof(char*));
            if (!temp) {
                perror("realloc failure");
                exit(EXIT_FAILURE);
            }
            commands = temp;
        }
        //allocate space for command string
        commands[command_count]= malloc((strlen(buffer)+1)*sizeof(char));
        if(!commands[command_count]) {
            perror("malloc failure");
            exit(EXIT_FAILURE);
        }
        strcpy(commands[command_count],buffer);
        //store in linked list
        Node *new_node = createNode(buffer,command_count+1);
        if(tail) {
            tail->next = new_node;
        }
        else {
            head = new_node;
        }
        tail = new_node;
        command_count++;
    }
    //printing linked list
    printf("Linked list nodes: \n");
    printNodes(head);
    // free memory
    for (int i = 0; i < command_count; i++) {
        free(commands[i]);
    }
    free(commands); 
    freeLinkedlist(head); 
    POP_TRACE();
    return 0;
}
