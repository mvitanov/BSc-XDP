#ifndef C_QUEUE_H
#define C_QUEUE_H

struct QNode 
{ 
    char* pkg; 
    struct QNode *next; 
}; 

struct Queue 
{ 
    struct QNode *front, *rear; 
    int size;
}; 

struct Queue *createQueue(void);
struct QNode* newNode(char* p);
void enQueue(struct Queue *q, char* p);
char* deQueue(struct Queue *q);
void freeQueue(struct Queue *q);

#endif
