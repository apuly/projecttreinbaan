#ifndef  _LINKED_LIST_H_
#define _LINKED_LIST_H_

/*includes*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*structures*/
struct sensorupdate{
    int16_t sensor;
    int16_t state;
};

struct listItem{
    struct sensorupdate sensor;
    struct listItem* next;
};

/*function prototypes*/
/*create the first item in the list*/
struct listItem* createItem(struct sensorupdate sensorvalue);

/*add an item to the end of the list*/
struct listItem* addItem(struct listItem* head, struct sensorupdate sensorvalue);

/*remove the entire list*/
struct listItem* clearlist(struct listItem* head);

/*get the item at index*/
struct sensorupdate getItem(struct listItem* head, int index);

/*count the number of entries in the list*/
int countItems(struct listItem* head);

#endif
