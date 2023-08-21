#include "main.h"
#include "ui.h"

#define MAX_MINTERM_BITSIZE 64
#define BUF_SIZE 256



int main(void){
    //Get number of minterms and minterms
    char buf[BUF_SIZE] = {0};
    printf("\nNumber of terms:");
    fgets(buf, BUF_SIZE, stdin);
    int terms_n = atoi(buf);
    int valid_terms_n = 0;

    uint64_t *terms = calloc(terms_n, sizeof(uint64_t));

    printf("\nTerms:\n");


    for(int i = 0; i < terms_n; i++){
        fgets(buf, BUF_SIZE, stdin);
        uint64_t newTerm = atoi(buf);
        if(!termIsPresent(newTerm, terms, terms_n)){
            //ignore repeated terms
            terms[valid_terms_n++] = newTerm;
        }
    }

    Minterm **minterms = calloc(valid_terms_n, sizeof(void*));

    for(int i = 0; i < valid_terms_n; i++){
        minterms[i] = IntToMinterm(terms[i]);
    }

    for(int i = 0; i < valid_terms_n; i++){
        printMinterm(minterms[i]); 
    }

    Table *table = createTable(minterms, valid_terms_n);
    printTable(table);

    Table *minimizedTable = minimizeTable(table);
    printTable(minimizedTable);



    //Group minterms
    //Fill table with groups
    //Minimize table until not possible anymore
    //Petrick's algorithm
    //convert to boolean expression and print
    free(terms);
    //TODO: free table and minterms
    return 0; 
}

bool termIsPresent(uint64_t newTerm, uint64_t *terms, int n){
    for(int i = 0; i < n; i++){
        if(terms[i] == newTerm) return true;
    }
    return false;
}

bool singleChangingBit(Minterm *m1, Minterm *m2, int *changing_bit_pos){
    int changed_bits = 0;

    int largest_size = m1->size >= m2->size? m1->size : m2->size;
    for(int i = 0; i < largest_size; i++){
        BitState m1_bit = i >= m1->size? BIT_0 : m1->bits[i];
        BitState m2_bit = i >= m2->size? BIT_0 : m2->bits[i];
        if(m1_bit != m2_bit){
            *changing_bit_pos = i;  //TODO: Probably wrong. bits are in reverse order
            changed_bits++;
        }
        if(changed_bits > 1){
            return false;
        }

    }

    return changed_bits == 1;
}

/*
 * Receives a table, minimizes it and return number of minimizations it
 * has made. When zero, no more minimizations are possible
 */
Table *minimizeTable(Table *table){
    Table *newTable = calloc(1, sizeof(Table));
    Group **all_groups = calloc(MAX_MINTERM_BITSIZE, sizeof(void*));

    for(int i = 0; i < table->size -1; i++){
        //compare minterms of group i to minterms of group i+1
        Group *g1 = table->groups[i];
        Group *g2 = table->groups[i+1];

        for(int j = 0; j < g1->size; j++){
            Minterm *m1 = g1->minterms[j];

            for(int k = 0; k < g2->size; k++){
                Minterm *m2 = g2->minterms[k];

                int changing_bit_pos;

                if(singleChangingBit(m1, m2, &changing_bit_pos)){
                    //create new minterm with bit[changing_bit_pos] = BIT_X
                    Minterm *largest_mt = m1->size >= m2->size? m1 : m2;
                    Minterm *newMinterm = calloc(1, sizeof(Minterm));
                    newMinterm->bits = calloc(largest_mt->size, sizeof(BitState));
                    memcpy(newMinterm->bits, largest_mt->bits, largest_mt->size*sizeof(BitState));
                    newMinterm->bits[changing_bit_pos] = BIT_X;
                    newMinterm->size = largest_mt->size;
                    newMinterm->set_bits = getSetBits(*newMinterm);

                    //create new group if needed and append minterm to group 
                    int set_bits = newMinterm->set_bits;
                    if(!all_groups[set_bits]){
                        all_groups[set_bits] = calloc(1, sizeof(Group));
                        all_groups[set_bits]->set_bits = set_bits;
                    }
                    Group_append(all_groups[set_bits], newMinterm);
                }
            }
        }
    }

    //pass all_groups to table->groups and free all_groups
    for(int i = 0; i < MAX_MINTERM_BITSIZE; i++){
        if(all_groups[i]){
            all_groups[i]->set_bits = i;
            Table_append(newTable, all_groups[i]);
        }
    }
    free(all_groups);

    //Old table is freed by caller
    return newTable;
}

void Group_append(Group *group, Minterm *mt){
    group->minterms = realloc(group->minterms, (group->size + 1)*sizeof(void*));
    group->minterms[group->size] = mt;
    group->size++;
}

void Table_append(Table *table, Group *group){
    table->groups = realloc(table->groups, (table->size + 1) *sizeof(void*));
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
            all_groups[set_bits]->set_bits = set_bits;
        }
        Group_append(all_groups[set_bits], minterms[i]);
    }

    //pass all_groups to table->groups and free all_groups
    for(int i = 0; i < MAX_MINTERM_BITSIZE; i++){
        if(all_groups[i]){
            all_groups[i]->set_bits = i;
            Table_append(table, all_groups[i]);
        }
    }
    free(all_groups);

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
