
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h>
#include "c_queue.h"

struct QNode* newNode(char* p) 
{ 
    struct QNode *temp = (struct QNode*)malloc(sizeof(struct QNode)); 
    temp->pkg = p;
    temp->next = NULL; 
    return temp;  
} 

struct Queue *createQueue(void) 
{ 
    struct Queue *q = (struct Queue*)malloc(sizeof(struct Queue)); 
    q->front = q->rear = NULL; 
    q->size = 0;
    return q; 
} 
  
void enQueue(struct Queue *q, char* p) 
{ 
    struct QNode *temp = newNode(p); 

    if (q->rear == NULL) 
    { 
       q->front = q->rear = temp; 
       q->size = 1;
       return; 
    } 
  
    // Add the new node at the end of queue and change rear 
    q->rear->next = temp; 
    q->rear = q->rear->next; 
    q->size += 1;
} 

char* deQueue(struct Queue *q) 
{ 
    if (q->front == NULL) {
        q->size = 0;
        return NULL; 
    }
  
    // Store previous front and move front one node ahead 
    struct QNode *temp = q->front; 
    q->front = q->front->next; 
  
    if (q->front == NULL) {
        q->size = 0;
        q->rear = NULL; 
    }

    char* elem = temp->pkg;
    free(temp);
    q->size -= 1;

    return elem; 
} 

void freeQueue(struct Queue *q)
{
    free(q);
}
