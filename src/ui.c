#include <stdio.h>
#include <string.h>

#include "ui.h"

/*
 * ncurses:
 *
 * struct windows{
 *  global window;
 *  tab1;
 *  tab2;
 *  tab3;
 * }
 *
 * must be passed to all ui functions
 * make init functions
 * main() will call all ui functions, no need
 * to pass windows to algorithms
 */


void printMinterm(Minterm *minterm){
    if(!minterm) return;

    if(minterm->size == 0){
        printf("0");
    }

    int leading_spaces = 25 - minterm->size;
    //printf("%0*d", leading_spaces, 0);

    for(int i = minterm->size-1; i >= 0; i--){
        if(minterm->bits[i] == BIT_X){
            printf("-");
        }else{
            printf("%d", minterm->bits[i]);
        }
    }
}

void printGroup(Group *group){
    if(!group) return;

    printf("*-%d BITS GROUP:\n| size %d\n", group->set_bits, group->size);
    for(int i = 0; i < group->size; i++){
        printf("| ");
        printMinterm(group->minterms[i]);
        printf("\n");
    }
    printf("*--\n");
}

void printTable(Table *table){
    if(!table) return;

    printf("==========TABLE==========:\n");
    printf("> size: %d\n", table->size);
    printf("> max set bits: %d\n\n", table->max_setBits);
    for(int i = 0; i < table->size; i++){
        printGroup(table->groups[i]);
    }
    printf("*-Prime implicants:\n");
    for(int i = 0; i < table->unmin_terms_n; i++){
        printMinterm(table->unmin_terms[i]);
        printf("\n");
    }
    printf("=========================:\n");
    printf("\n");
}

void printPChart(bool **Petrick_chart, int rows, int colums){
    for(int i = 0; i < colums; i++){
        for(int j = 0; j < rows; j++){
            printf("%d ", Petrick_chart[colums][rows]);
        }
        printf("\n");
    }
    printf("\n");
}
