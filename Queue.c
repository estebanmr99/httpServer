#include "Queue.h"
#include <stdlib.h>

Node* head = NULL;
Node* tail = NULL;

void enqueue(int *client){
    Node *node = malloc(sizeof(Node));
    node->client_socket = client;
    node->next = NULL;

    if(tail==NULL)

        head = node;

    else
        tail->next = node;
    
    tail = node;
}

int *dequeue(){
    if(head == NULL)
        return NULL;
    else{
        Node *temp = head;
        int *result = head->client_socket;
        head = head->next;
        if(head == NULL)
            tail = NULL;
        free(temp);
        return result;
    }
}