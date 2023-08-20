#ifndef MAIN_H
#define MAIN_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//TODO:
//ncurses interface with printed tables and scrolling
//when a new minimization starts, a windows pops up and prompts for
//number of minterms and its values
//Minimized circuit is printed in ascii
//
//3 tabs: Qune-mccluskey - Petrick - Circuit


/*
 * minterms are int arrays (unlimited length, can contain 1, 0 or -)
 * tables are minterms organized in groups
 */


typedef enum{
    BIT_1 = 1,
    BIT_0 = 0,
    BIT_X = 2,
}BitState;

typedef struct{
    int size;               //number of bits
    int set_bits;
    BitState *bits;
}Minterm;

typedef struct{
    int set_bits;           //number of '1' bits on members of this group
    int size;               //number of minterms in this group
    Minterm **minterms;     //array of pointers to minterms
}Group;

typedef struct{
    int size;           //number of groups
    int max_setBits;
    Group **groups;
}Table;



int minimizeTable(Table *table);
void fillTable(Minterm *minterms, int n, Table *table);

void Table_append(Table *table, Group *group);
void Group_append(Group *group, Minterm *mt);

int getSetBits(Minterm minterm);


#endif
