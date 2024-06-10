#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <pthread.h>

// Tipo dell'elemento
typedef enum {
    TYPE_UCHAR,
    TYPE_LDOUBLE
} ElementType;

// Valore dell'elemento, può essere unsigned char[8] o long double
typedef union {
    unsigned char uchar[8];
    long double ldouble;
} ElementValue;

// Nodo della lista doppiamente collegata
typedef struct Node {
    ElementType type;
    ElementValue value;
    struct Node *prev;
    struct Node *next;
} Node;

// Struttura della lista doppiamente collegata
typedef struct {
    Node *head; // Puntatore alla testa della lista
    Node *tail; // Puntatore alla coda della lista
    Node *cache; // Puntatore per il caching dell'ultimo elemento utilizzato
    pthread_mutex_t lock; // Mutex per la thread-safety
} DoublyLinkedList;

DoublyLinkedList* createList();
void destroyList(DoublyLinkedList *list);
Node* insert(DoublyLinkedList *list, ElementType type, ElementValue value);
Node* insertAt(DoublyLinkedList *list, ElementType type, ElementValue value, int position);
Node* removeNode(DoublyLinkedList *list); // Rinominata in removeNode
Node* removeAt(DoublyLinkedList *list, int position);
Node* getAt(DoublyLinkedList *list, int position);
void map(DoublyLinkedList *list, void (*function)(ElementValue *));
ElementValue reduce(DoublyLinkedList *list, ElementValue (*function)(ElementValue, ElementValue), ElementValue initial);

#endif // DOUBLY_LINKED_LIST_H
