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

    Implicants *implicants  = getImplicants(minterms, valid_terms_n);
    getPrimeImplicants(implicants, minterms, valid_terms_n);




    //Group minterms
    //Fill table with groups
    //Minimize table until not possible anymore
    //Petrick's algorithm
    //convert to boolean expression and print
    free(terms);
    //TODO: free table and minterms
    return 0; 
}


bool equalMinterms(Minterm *m1, Minterm *m2){
    if(!m1 || !m2){
        return false;
    }
    if(m1->size != m2->size){
        return false;
    }

    for(int i = 0; i < m1->size; i++){
        if(m1->bits[i] != m2->bits[i]){
            return false;
        }
    }

    return true;
}



Implicants *getImplicants(Minterm **minterms, int n){
    Implicants *implicants = calloc(1, sizeof(Implicants));

    Table *table = createTable(minterms, n);
    printTable(table);
    //minimize table until no minimization is possible
    int minimized_terms;
    do{
        table = minimizeTable(table, &minimized_terms);
        if(!minimized_terms) 
            break;
        printTable(table);

    }while(minimized_terms);


    //Implicants from table to struct
    Minterm *previous = NULL;
    for(int i = 0; i < table->size; i++){
        Group *group = table->groups[i];
        for(int j = 0; j < group->size; j++){
            if(!equalMinterms(group->minterms[j], previous)){
                implicants->size++;
                implicants->minterms = realloc(implicants->minterms, 
                        (implicants->size) * sizeof(void*));
                implicants->minterms[implicants->size-1] = group->minterms[j];
                previous = group->minterms[j];
            }
        }
    }
     
    //TODO: free table
    for(int i = 0; i < implicants->size; i++){
        printMinterm(implicants->minterms[i]);
    }
    return implicants;
}


bool PI_in_minterm(Minterm *Prime_implicant, Minterm *minterm){ 

    for(int i = 0; i < Prime_implicant->size; i++){
        if(Prime_implicant->bits[i] == BIT_X)
            continue;

        if(i >= minterm->size)
            return false;

        if(Prime_implicant->bits[i] != minterm->bits[i])
            return false;
    }
    return true;
}


//Expects minterms sorted
Implicants *getPrimeImplicants(Implicants *implicants, Minterm **minterms, int n){
    //create Petrick table with all implicants and provided
    //minterms

    //convert all leading 0s to dont cares
    for(int i = 0; i < implicants->size; i++){
        //TODO: problably unnecessary, there are no leading 0s
        Minterm *m = implicants->minterms[i];
        for(int j = m->size-1; j >= 0; j--){
            if(m->bits[j] != BIT_0){
                break;
            }
            m->bits[j] = BIT_X;
        }
    }
    
    int rows = n;
    int colums = implicants->size;
    bool Petrick_chart[implicants->size][n];
    memset(Petrick_chart, 0, sizeof(bool)*rows*colums);

    for(int i = 0; i < implicants->size; i++){
        Minterm *Prime_implicant = implicants->minterms[i];
        for(int j = 0; j < n; j++){
            Minterm *m = minterms[j];
            if(PI_in_minterm(Prime_implicant, m)){
                Petrick_chart[j][i] = true;
            }
        }
    }

    printf("\n");
    for(int i = 0; i < colums; i++){
        for(int j = 0; j < rows; j++){
            printf("%d ", Petrick_chart[j][i]);
        }
        printf("\n");
    }
    printf("\n");

    //
    //for each implicant
    //compare non-x bits to all the minterms provided
    //Important: all leading 0s become leading dont cares
    //fill table
    //
    //get prime implicants and return them
    return NULL;
}

void PrimeImplicantsToEquation(void){
    //All implicants become ex.: (x1)'(x43)(x64), (x12)(x5)' etc
    //add a "+" sign between them
    //
    //Print equation and circuit
}

bool termIsPresent(uint64_t newTerm, uint64_t *terms, int n){
    //Temporary workaround 
    if(newTerm == 0) return false;


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
            *changing_bit_pos = i;  
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
Table *minimizeTable(Table *table, int *minimized_terms){
    *minimized_terms = 0;

    Table *newTable = calloc(1, sizeof(Table));
    Group **all_groups = calloc(MAX_MINTERM_BITSIZE, sizeof(void*));

    for(int i = 0; i < table->size -1; i++){
        //compare minterms of group i to minterms of group i+1
        Group *g1 = table->groups[i];
        Group *g2 = table->groups[i+1];

        if(g2->set_bits - g1->set_bits > 1) continue;

        for(int j = 0; j < g1->size; j++){
            Minterm *m1 = g1->minterms[j];

            for(int k = 0; k < g2->size; k++){
                Minterm *m2 = g2->minterms[k];

                int changing_bit_pos;

                if(singleChangingBit(m1, m2, &changing_bit_pos)){
                    (*minimized_terms)++;

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
    //TODO: free old table
    
    if(*minimized_terms == 0){
        //TODO: free newTable
        return table;
    }
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
