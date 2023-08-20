#include <stdio.h>
#include <stdlib.h>

//TODO:
//ncurses interface with printed tables and scrolling
//when a new minimization starts, a windows pops up and prompts for
//number of minterms and its values
//Minimized circuit is printed in ascii
//


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
    BitState *bits;
}Minterm;

typedef struct{
    int set_bits;           //number of '1' bits on members of this group
    int size;               //number of minterms in this group
    Minterm *minterms;     
}Group;

typedef struct{
    int size;           //number of groups
    Group *groups;
}Table;



int main(void){
    //Get number of minterms and minterms
    //Group minterms
    //Fill table with groups
    //Minimize table until not possible anymore
    //Petrick's algorithm
    //convert to boolean expression and print
    return 0; 
}

/*
 * Receives a table, minimizes it and return number of minimizations it
 * has made. When zero, no more minimizations are possible
 */
int minimizeTable(Table *table){
    for(int i = 0; i < table->size -1; i++){
        //compare group i to group i+1
    }
    return 0;
}

Group *createTable(Minterm *minterms){
    //
    int n = 0;      //max number of set bits
    for(int i = 0; i < n; i++){
    }
}

static int getSetBits(Minterm minterm){
    int rv = 0;
    for(int i = 0; i < minterm.size; i++){
        if(minterm.bits[i] == BIT_1) rv++;
    }
    return rv;
}

static int getMaxBits(Minterm *minterms, int n){
    for(int i = 0; i < n; i++){ 
    }
}
