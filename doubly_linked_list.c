#include "doubly_linked_list.h"
#include <stdlib.h>
#include <stdio.h>

// Crea una nuova lista doppiamente collegata
DoublyLinkedList* createList() {
    DoublyLinkedList *list = (DoublyLinkedList *)malloc(sizeof(DoublyLinkedList));
    if (!list) return NULL;
    list->head = NULL;
    list->tail = NULL;
    list->cache = NULL;
    pthread_mutex_init(&list->lock, NULL);
    return list;
}

// Distrugge la lista e libera la memoria
void destroyList(DoublyLinkedList *list) {
    pthread_mutex_lock(&list->lock);
    Node *current = list->head;
    while (current) {
        Node *next = current->next;
        free(current);
        current = next;
    }
    pthread_mutex_unlock(&list->lock);
    pthread_mutex_destroy(&list->lock);
    free(list);
}

// Crea un nuovo nodo
Node* createNode(ElementType type, ElementValue value) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) return NULL;
    node->type = type;
    node->value = value;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

// Inserisce un nodo alla fine della lista
Node* insert(DoublyLinkedList *list, ElementType type, ElementValue value) {
    pthread_mutex_lock(&list->lock);
    Node *node = createNode(type, value);
    if (!node) {
        pthread_mutex_unlock(&list->lock);
        return NULL;
    }

    if (!list->head) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }
    list->cache = node;
    pthread_mutex_unlock(&list->lock);
    return node;
}

// Inserisce un nodo in una posizione specifica
Node* insertAt(DoublyLinkedList *list, ElementType type, ElementValue value, int position) {
    pthread_mutex_lock(&list->lock);
    Node *node = createNode(type, value);
    if (!node) {
        pthread_mutex_unlock(&list->lock);
        return NULL;
    }

    if (position == 0) {
        if (!list->head) {
            list->head = node;
            list->tail = node;
        } else {
            node->next = list->head;
            list->head->prev = node;
            list->head = node;
        }
    } else {
        Node *current = list->head;
        int index = 0;
        while (current && index < position) {
            current = current->next;
            index++;
        }

        if (current) {
            node->next = current;
            node->prev = current->prev;
            if (current->prev) {
                current->prev->next = node;
            }
            current->prev = node;
            if (current == list->head) {
                list->head = node;
            }
        } else {
            if (list->tail) {
                list->tail->next = node;
                node->prev = list->tail;
                list->tail = node;
            } else {
                list->head = node;
                list->tail = node;
            }
        }
    }

    list->cache = node;
    pthread_mutex_unlock(&list->lock);
    return node;
}

// Rimuove l'ultimo nodo dalla lista
Node* removeNode(DoublyLinkedList *list) {
    pthread_mutex_lock(&list->lock);
    if (!list->tail) {
        pthread_mutex_unlock(&list->lock);
        return NULL;
    }

    Node *node = list->tail;
    if (list->tail->prev) {
        list->tail = list->tail->prev;
        list->tail->next = NULL;
    } else {
        list->head = NULL;
        list->tail = NULL;
    }

    if (list->cache == node) {
        list->cache = NULL;
    }

    pthread_mutex_unlock(&list->lock);
    free(node);
    return list->tail;
}

// Rimuove un nodo in una posizione specifica
Node* removeAt(DoublyLinkedList *list, int position) {
    pthread_mutex_lock(&list->lock);
    if (!list->head) {
        pthread_mutex_unlock(&list->lock);
        return NULL;
    }

    Node *current = list->head;
    int index = 0;
    while (current && index < position) {
        current = current->next;
        index++;
    }

    if (current) {
        if (current->prev) {
            current->prev->next = current->next;
        } else {
            list->head = current->next;
        }

        if (current->next) {
            current->next->prev = current->prev;
        } else {
            list->tail = current->prev;
        }

        if (list->cache == current) {
            list->cache = NULL;
        }

        Node *nextNode = current->next;
        free(current);
        pthread_mutex_unlock(&list->lock);
        return nextNode;
    }

    pthread_mutex_unlock(&list->lock);
    return NULL;
}

// Ottiene un nodo in una posizione specifica
Node* getAt(DoublyLinkedList *list, int position) {
    pthread_mutex_lock(&list->lock);
    Node *current = list->head;
    int index = 0;
    while (current && index < position) {
        current = current->next;
        index++;
    }
    pthread_mutex_unlock(&list->lock);
    return current;
}

// Struttura per gli argomenti del thread di map
typedef struct {
        Node *node;
    void (*function)(ElementValue *);
} MapArgs;

// Funzione eseguita dal thread per l'operazione map
void* mapThread(void *arg) {
    MapArgs *args = (MapArgs *)arg;
    Node *current = args->node;
    while (current) {
        args->function(&current->value);
        current = current->next;
    }
    return NULL;
}

// Applica una funzione a ogni elemento della lista utilizzando thread
void map(DoublyLinkedList *list, void (*function)(ElementValue *)) {
    pthread_t thread;
    MapArgs args = { list->head, function };
    pthread_create(&thread, NULL, mapThread, &args);
    pthread_join(thread, NULL);
}

// Struttura per gli argomenti del thread di reduce
typedef struct {
    Node *node;
    ElementValue (*function)(ElementValue, ElementValue);
    ElementValue result;
} ReduceArgs;

// Funzione eseguita dal thread per l'operazione reduce
void* reduceThread(void *arg) {
    ReduceArgs *args = (ReduceArgs *)arg;
    Node *current = args->node;
    while (current) {
        args->result = args->function(args->result, current->value);
        current = current->next;
    }
    return NULL;
}

// Applica una funzione di riduzione a tutti gli elementi della lista utilizzando thread
ElementValue reduce(DoublyLinkedList *list, ElementValue (*function)(ElementValue, ElementValue), ElementValue initial) {
    pthread_t thread;
    ReduceArgs args = { list->head, function, initial };
    pthread_create(&thread, NULL, reduceThread, &args);
    pthread_join(thread, NULL);
    return args.result;
}
