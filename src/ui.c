#include <stdio.h>
#include <string.h>

#include "ui.h"


void printMinterm(Minterm *minterm){
    for(int i = minterm->size-1; i >= 0; i--){
        printf("%d", minterm->bits[i]);
    }
    printf("\n");
}

void printGroup(Group *group){
    printf("GROUP %d:\n", group->set_bits);
    for(int i = 0; i < group->size; i++){
        printMinterm(group->minterms[i]);
    }
    printf("\n");
}

void printTable(Table *table){
    printf("TABLE:\n");
    for(int i = 0; i < table->size; i++){
        printGroup(table->groups[i]);
        printf("\n");
    }
    printf("\n");
}
