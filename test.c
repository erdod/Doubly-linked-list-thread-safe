#include "doubly_linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// Funzione di esempio per map: incrementa il valore dell'elemento
void increment(ElementValue *value) {
    if (value->ldouble != 0.0) {
        value->ldouble += 1.0;
    }
}

// Funzione di esempio per reduce: somma due valori
ElementValue sum(ElementValue a, ElementValue b) {
    ElementValue result;
    result.ldouble = a.ldouble + b.ldouble;
    return result;
}

// Funzione per stampare la lista corrente
void stampaLista(DoublyLinkedList *list) {
    printf("Lista corrente:\n");
    Node *current = list->head;
    while (current) {
        printf("%.2Lf ", current->value.ldouble);
        current = current->next;
    }
    printf("\n");
}

void popolaLista(DoublyLinkedList *list, ElementType type) {
    char input[100];
    printf("Inserisci elemento in lista e premi invio(premi 'e' per passare al menù di scelta della funzione da applicare alla lista):\n");
    while (1) {
        printf("Inserisci un valore: ");
        scanf("%s", input);
        if (strcmp(input, "e") == 0) {
            break;
        }
        ElementValue value;
        if (type == TYPE_LDOUBLE) {
            sscanf(input, "%Lf", &value.ldouble);
        } else {
            // Tronca la stringa se troppo lunga e converte in unsigned char
            input[8] = '\0';
            memcpy(value.uchar, input, 8);
        }
        insert(list, type, value);
        stampaLista(list);
    }
}

void testInsertAtBeginning(ElementType type, DoublyLinkedList *list) {
    printf("Running testInsertAtBeginning...\n");
    ElementValue value;
    value.ldouble = 1.0;
    insert(list, type, value);
    stampaLista(list);
    printf("testInsertAtBeginning passed.\n");
}

void testRemove(DoublyLinkedList *list) {
    printf("Running testRemove...\n");
    removeNode(list);
    stampaLista(list);
    printf("testRemove passed.\n");
}

void testGetAt(DoublyLinkedList *list) {
    printf("Running testGetAt...\n");
    int index;
    printf("Inserisci l'indice dell'elemento da ottenere: ");
    scanf("%d", &index);
    Node *node = getAt(list, index);
    if (node) {
        printf("Elemento ottenuto: ");
        if (node->type == TYPE_UCHAR) {
            printf("%hhu\n", node->value.uchar);
        } else {
            printf("%Lf\n", node->value.ldouble);
        }
    } else {
        printf("Indice non valido.\n");
    }
    printf("testGetAt passed.\n");
}

void testMap(DoublyLinkedList *list) {
    printf("Running testMap...\n");
    map(list, increment);
    stampaLista(list);
    printf("testMap passed.\n");
}

void testReduce(DoublyLinkedList *list) {
    printf("Running testReduce...\n");
    ElementValue value1, value2, initial;
    value1.ldouble = 1.0;
    value2.ldouble = 2.0;
    initial.ldouble = 0.0;
    printf("Aggiungendo i valori alla lista...\n");
    insert(list, TYPE_LDOUBLE, value1);
    stampaLista(list);
    insert(list, TYPE_LDOUBLE, value2);
    stampaLista(list);
    printf("Eseguendo la riduzione...\n");
    ElementValue result = reduce(list, sum, initial);
    printf("Il risultato della riduzione è %.2Lf\n", result.ldouble);
    printf("La lista dopo la riduzione è:\n");
    stampaLista(list);
    assert(result.ldouble == 3.0);
    printf("testReduce passed.\n");
}

void eseguiFunzione(DoublyLinkedList *list, int choice) {
    switch (choice) {
        case 1:
            testInsertAtBeginning(TYPE_LDOUBLE, list);
            break;
        case 2:
            testRemove(list);
            break;
        case 3:
            testGetAt(list);
            break;
        case 4:
            testMap(list);
            break;
        case 5:
            testReduce(list);
            break;
        default:
            printf("Scelta non valida.\n");
    }
}

int main() {
    ElementType type;
    printf("Seleziona il tipo di valore da inserire (0 per unsigned char[8], 1 per long double): ");
    int typeChoice;
    scanf("%d", &typeChoice);
    if (typeChoice == 0) {
        type = TYPE_UCHAR;
    } else {
        type = TYPE_LDOUBLE;
    }

    DoublyLinkedList *list = createList();

    // Popola la lista con gli elementi inseriti dall'utente
    popolaLista(list, type);

    int choice;
    do {
        printf("\nScegli una delle seguenti funzioni:\n");
        printf("1. Test insertAtBeginning\n");
        printf("2. Test removeNode\n");
        printf("3. Test getAt\n");
        printf("4. Test map\n");
        printf("5. Test reduce\n");
        printf("0. Esci dal programma\n");
        printf("Inserisci il numero corrispondente alla funzione da eseguire: ");
        scanf("%d", &choice);

        switch (choice) {
            case 0:
                printf("Uscita dal programma.\n");
                break;
            case 1:
                testInsertAtBeginning(type, list);
                break;
            case 2:
                testRemove(list);
                break;
            case 3:
                testGetAt(list);
                break;
            case 4:
                testMap(list);
                break;
            case 5:
                testReduce(list);
                break;
            default:
                printf("Scelta non valida.\n");
        }
    } while (choice != 0);

    destroyList(list);
    return 0;
}
