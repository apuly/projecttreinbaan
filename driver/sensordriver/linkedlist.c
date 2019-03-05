#include "linkedlist.h"

/*create the first item in the list*/
struct listItem* createItem(struct sensorupdate sensorvalue){
    /*create a temporary item pointer*/
    struct listItem* tmp;
    /*malloc some memory to store the list item*/
    tmp = (struct listItem*)malloc(sizeof(struct listItem));
    /*set the values*/
    tmp->sensor = sensorvalue;
    tmp->next = NULL;
    /*return the pointer to the item*/
    return tmp;
}

/*add an item to the end of the list*/
struct listItem* addItem(struct listItem * head, struct sensorupdate sensorvalue){
    /*make some pointer for a new item and one as the index*/
    struct listItem* tmp;
    struct listItem* p;
    /*create a new item in the list*/
    tmp = createItem(sensorvalue);
    /*if the list is empty*/
    if(NULL == head){
        /*then use the new item as the head*/
        head = tmp;
    }else{
        /*else set the head to p*/
        p = head;
        /*then look through the list intill we find the end of the list*/
        while(NULL != p->next){
            p = p->next;
        }
        /*add the new item to the end of the list*/
        p->next = tmp; 
    }
    /*return head*/
    fflush(stdout);
    return head;
}

/*remove the entire list*/
struct listItem* clearlist(struct listItem* head){
    /*make two pointers to point at the current and next item in the list*/
    struct listItem* p;
    struct listItem* q;
    /*set the head to p*/
    p = head;
    /*as long as the head isn't empty*/
    while(NULL != p){
        /*set q to the next item in the list*/
        q = p->next;
        /*free P*/
        free(p);
        /*then set p to q*/
        p = q;
    }
    /*lastly set the head to NULL*/
    head = NULL;
    /*then return the head of the empty list*/
    return head;
}

/*get the item at index*/
struct sensorupdate getItem(struct listItem* head, int index){
    /* make a pointer to keep track of where we are in the list */
    struct listItem* p;
    int i;
    /* set p to the start of the list */
    p = head;
    /* walk through the list looking for the index */
    for(i = 0; i != index; i++){
        p = p->next;
    }
    /* when we find the item we are looking for, return the item */
    return p->sensor;
}

/*count the number of entries in the list*/
int countItems(struct listItem* head){
    /* make a pointer to keep track of where we are in the list */
    struct listItem* p;
    int count;
    
    /*set these variables to their starting values */
    count = 0;
    p = head;

    /*walk through the list*/
    while(NULL != p){
        /*set p to the next item in the list*/
        p = p->next;
        /*count the item we visited*/
        count++;
    }
    return count;
}
