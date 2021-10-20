#ifndef Queue_h
#define Queue_h

typedef struct Node
{
    struct Node *next;

    int *client_socket;

}Node;

void enqueue(int *client);
int *dequeue();

#endif
