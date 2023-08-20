#include "main.h"
#include "ui.h"

#define MAX_MINTERM_BITSIZE 64
#define BUF_SIZE 256

int main(void){
    //Get number of minterms and minterms
    char buf[BUF_SIZE] = {0};
    printf("\nNumber of terms:");
    fgets(buf, BUF_SIZE, stdin);
    int n = atoi(buf);

    uint64_t *terms = calloc(n, sizeof(uint64_t));

    printf("\nTerms:\n");
    for(int i = 0; i < n; i++){
        fgets(buf, BUF_SIZE, stdin);
        terms[i] = atoi(buf);
    }

    Minterm **minterms = calloc(n, sizeof(void*));

    for(int i = 0; i < n; i++){
        minterms[i] = IntToMinterm(terms[i]);
    }

    for(int i = 0; i < n; i++){
        printMinterm(minterms[i]); 
    }



    //Group minterms
    //Fill table with groups
    //Minimize table until not possible anymore
    //Petrick's algorithm
    //convert to boolean expression and print
    free(terms);
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

void Group_append(Group *group, Minterm *mt){
    group->minterms = realloc(group->minterms, group->size + 1);
    group->minterms[group->size] = mt;
    group->size++;
}

void Table_append(Table *table, Group *group){
    table->groups = realloc(table->groups, table->size + 1);
    table->groups[table->size] = group;
    table->size++;
}

Table *createTable(Minterm **minterms, int n){
    Table *table = calloc(1, sizeof(Table));
    Group **all_groups = calloc(MAX_MINTERM_BITSIZE, sizeof(void*));

    //get maximum number of set bits and fill "existing groups"
    table->max_setBits = 0;
    for(int i = 0; i < n; i++){ 
        int set_bits = getSetBits(*minterms[i]);
        if(set_bits > table->max_setBits){ 
            table->max_setBits = set_bits;
        }

        //append to all_groups[set_bits]
        if(!all_groups[set_bits]){
            all_groups[set_bits] = calloc(1, sizeof(Group));
        }
        Group_append(all_groups[set_bits], minterms[i]);
    }

    //pass all_groups to table->groups and free all_groups
    for(int i = 0; i < MAX_MINTERM_BITSIZE; i++){
        if(all_groups[i]){
            Table_append(table, all_groups[i]);
        }
    }

    return table;
}

/*
 * Gets a 64bits minterm and converts it to a Minterm struct
 */
Minterm *IntToMinterm(uint64_t num){
    Minterm *minterm = calloc(1, sizeof(Minterm));

    //get size and number of set bits
    for(int i = 0; i < MAX_MINTERM_BITSIZE; i++){
        if((num >> i) & BIT_1){
            minterm->size = i+1;
            minterm->set_bits ++;
        }
    }

    //fill bit array
    minterm->bits = calloc(minterm->size, sizeof(BitState));
    for(int i = 0; i < minterm->size; i++){
        minterm->bits[i] = (num >> i) & 0x1;
    }

    return minterm;
}



int getSetBits(Minterm minterm){
    int rv = 0;
    for(int i = 0; i < minterm.size; i++){
        if(minterm.bits[i] == BIT_1) rv++;
    }
    return rv;
}
