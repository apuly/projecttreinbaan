#include "linklist.h"

int main(int argc, char* argv[]){
    /*make a temporary structure to store items in*/
    struct sensorupdate tmpsens;
    /*reserve a pointer for the list*/
    struct listItem* list;
    /*variable for storing the item count*/
    int count, i;

    /* make sure the head is initialized as Null*/
    list = NULL;
    count = 0;

    printf("filling list with 5 items\n");

    /*make put 5 items in the list*/
    for(i = 0; i < 5; i++){
        tmpsens.sensor = i;
        tmpsens.state = 0;
        list = addItem(list, tmpsens);
    }

    /* count how manny items there are in the list */
    count = countItems(list);
    printf("items in list: %d\n", count);

    /*then print the list*/
    for(i = 0; i < 5; i++){
        tmpsens = getItem(list, i);
        printf("sensor: %d\n", tmpsens.sensor);
    }

    /*lastly clear the list*/
    printf("clearing list...\n");
    list = clearlist(list);

    /* count how manny items there are in the list */
    count = countItems(list);
    printf("items in list: %d\n", count);

    if(NULL == list){
        printf("the list is now empty\n");
    }

    return 0;
}
