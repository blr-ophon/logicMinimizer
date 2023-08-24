#include "main.h"
#include "ui.h"
#include "math.h"

#define MAX_MINTERM_BITSIZE 64
#define BUF_SIZE 256

Panel Table_p;

int main(void){
    


    //Get number of minterms and minterms
    char buf[BUF_SIZE] = {0};
    printf("\nNumber of terms:");
    fgets(buf, BUF_SIZE, stdin);
    int terms_n = atoi(buf);
    int valid_terms_n = 0;
    uint64_t largest_mt = 0;

    uint64_t *terms = (uint64_t*) calloc(terms_n, sizeof(uint64_t));

    printf("\nTerms:\n");


    for(int i = 0; i < terms_n; i++){
        fgets(buf, BUF_SIZE, stdin);
        uint64_t newTerm = atoi(buf);
        if(!termIsPresent(newTerm, terms, terms_n)){
            //ignore repeated terms
            terms[valid_terms_n++] = newTerm;
            if(newTerm > largest_mt){
                largest_mt = newTerm;
            }
        }
    }

    Minterm **minterms = (Minterm**) calloc(valid_terms_n, sizeof(void*));

    for(int i = 0; i < valid_terms_n; i++){
        minterms[i] = IntToMinterm(terms[i], largest_mt);
    }

    for(int i = 0; i < valid_terms_n; i++){
        //printMinterm(minterms[i]); 
        printf("\n");
    }

    menu_init();
    menu_TableWindow(&Table_p);

    Implicants *implicants  = getImplicants(minterms, valid_terms_n);

    Implicants *primeImplicants = getPrimeImplicants(implicants, minterms, valid_terms_n);
    wprintw(Table_p.buf, "PRIME IMPLICANTS:\n");
    printImplicants(&Table_p, primeImplicants);

    char *equation = PrimeImplicantsToEquation(primeImplicants);
    wprintw(Table_p.buf, "\nEQUATION:\nZ = %s\n\n", equation);

    //printCircuit(equation);


    //Group minterms
    //Fill table with groups
    //Minimize table until not possible anymore
    //Petrick's algorithm
    //convert to boolean expression and print
    free(terms);
    free(equation);
    //TODO: free table and minterms
    
    
    
    
   
    int line = 0; 
    int column  = 0;
    prefresh(Table_p.buf, line, column, 7, 2, 45, 48); 

    bool running = true;
    while(running){
        char c = getch();
        switch(c){
            case 'j':
                if(line < PAD_LINES_SIZE)
                    line++;
                prefresh(Table_p.buf, line, 0, 7, 2, 45, 48); 
                break;
            case 'k':
                if(line > 0)
                    line--;
                prefresh(Table_p.buf, line, 0, 7, 2, 45, 48); 
                break;
            case 'q':
                running = false;
                break;
        }
    }
    endwin();
    return 0; 
}


char *PrimeImplicantsToEquation(Implicants *implicants){
    //All implicants become ex.: (x1)'(x43)(x64), (x12)(x5)' etc
    //add a "+" sign between them
    char *buf = (char*) calloc(BUF_SIZE, sizeof(char));
    int terms_count = 0;

    for(int i = 0; i <implicants->size; i++){
        Minterm *m = implicants->minterms[i];
        int bits_count = 0;
        if(terms_count > 0){
            snprintf(buf + strlen(buf), BUF_SIZE - strlen(buf), " + ");
        }
            
        for(int j = m->size-1; j >= 0 ; j--){
            if(m->bits[j] == BIT_X) continue;

            if(bits_count > 0){
                snprintf(buf + strlen(buf), BUF_SIZE - strlen(buf), "*");
            }

            //append xj or xj'
            snprintf(buf+ strlen(buf), BUF_SIZE - strlen(buf), "x%d", j);
            if(!m->bits[j]){
                snprintf(buf+ strlen(buf), BUF_SIZE - strlen(buf), "'");
            }

            bits_count++;
        }
        terms_count++;

    }

    return buf;
}


bool equalMinterms(Minterm *m1, Minterm *m2){
    if(!m1 || !m2){
        return false;
    }
    if(m1->size != m2->size){
        //return false;
    }

    for(int i = 0; i < m1->size; i++){
        if(m1->bits[i] != m2->bits[i]){
            return false;
        }
    }

    return true;
}


void append_implicant(Implicants *implicants, Minterm *m){
    implicants->minterms = (Minterm**) realloc(implicants->minterms, (implicants->size+ 1) *sizeof(void*));
    implicants->minterms[implicants->size] = m;
    implicants->size++;
}


Implicants *getImplicants(Minterm **minterms, int n){
    Implicants *implicants = (Implicants*) calloc(1, sizeof(Implicants));

    Table *table = createTable(minterms, n);
    printTable(&Table_p, table);
    //minimize table until no minimization is possible
    int minimized_terms;
    while(table->size){
        table = minimizeTable(table, &minimized_terms);
        if(!minimized_terms) 
            break;
        printTable(&Table_p, table);
    }

    printTable(&Table_p, table);


    //Implicants from table to struct
    Minterm *previous = NULL;
    for(int i = 0; i < table->size; i++){
        Group *group = table->groups[i];
        for(int j = 0; j < group->size; j++){
            if(!equalMinterms(group->minterms[j], previous)){
                append_implicant(implicants, group->minterms[j]);
                previous = group->minterms[j];
            }
        }
    }


    previous = NULL;
    //Append unminimized implicants from table
    Implicants t_implicants = table->implicants;
    for(int i = 0; i < t_implicants.size; i++){
        if(!equalMinterms(t_implicants.minterms[i], previous)){
            append_implicant(implicants, t_implicants.minterms[i]);
            previous = t_implicants.minterms[i];
        }
    }

     
    //TODO: free table
    wprintw(Table_p.buf, "IMPLICANTS: \n");
    printImplicants(&Table_p, implicants);

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

    Implicants *primeImplicants = (Implicants*) calloc(1, sizeof(Implicants));

    //create Petrick table with all implicants and provided minterms
    int Petrick_chart[implicants->size][n];
    memset(Petrick_chart, 0, sizeof(int)*implicants->size * n);

    //for each implicant compare non-x bits to all the minterms provided
    for(int i = 0; i < implicants->size; i++){
        Minterm *Prime_implicant = implicants->minterms[i];
        for(int j = 0; j < n; j++){
            Minterm *m = minterms[j];
            if(PI_in_minterm(Prime_implicant, m)){
                Petrick_chart[i][j] = 1;
            }
        }
    }
        
    //find prime implicants from the chart
    for(int j = 0; j < n; j++){
        int count = 0;
        int impl_index = -1;
        for(int i = 0; i < implicants->size; i++){
            if(Petrick_chart[i][j]){
                impl_index = i;
                count++;
            }
            if(count > 1) break;
        }

        if(count == 1){
            Petrick_chart[impl_index][j] = 2;
            append_implicant(primeImplicants, implicants->minterms[impl_index]);
        }
    }

    //printf("\nPETRICK CHART\n");
    //for(int i = 0; i < implicants->size; i++){
    //    printf("(");
    //    printMinterm(implicants->minterms[i]);
    //    printf("): ");
    //    for(int j = 0; j < n; j++){
    //        printf("%d ", Petrick_chart[i][j]);
    //    }
    //    printf("\n");
    //}
    //printf("\n");

    //get prime implicants 
    return primeImplicants;
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

void Group_append(Group *group, Minterm *mt){
    group->minterms = (Minterm**) realloc(group->minterms, (group->size + 1)*sizeof(void*));
    group->minterms[group->size] = mt;
    group->size++;
}

void Table_append(Table *table, Group *group){
    table->groups = (Group**) realloc(table->groups, (table->size + 1) *sizeof(void*));
    table->groups[table->size] = group;
    table->size++;
}


/*
 * Receives a table, minimizes it and return number of minimizations it
 * has made. When zero, no more minimizations are possible
 */
Table *minimizeTable(Table *table, int *minimized_terms){
    //TODO: minimized_terms is useless now
    *minimized_terms = 0;

    Table *newTable = (Table*) calloc(1, sizeof(Table));
    Group **all_groups = (Group**) calloc(MAX_MINTERM_BITSIZE, sizeof(void*));



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
                    m1->isImplicant = false;
                    m2->isImplicant = false;
                    (*minimized_terms)++;

                    //create new minterm with bit[changing_bit_pos] = BIT_X
                    Minterm *largest_mt = m1->size >= m2->size? m1 : m2;
                    Minterm *newMinterm = (Minterm*) calloc(1, sizeof(Minterm));
                    newMinterm->bits = (BitState*) calloc(largest_mt->size, sizeof(BitState));
                    memcpy(newMinterm->bits, largest_mt->bits, largest_mt->size*sizeof(BitState));
                    newMinterm->bits[changing_bit_pos] = BIT_X;
                    newMinterm->size = largest_mt->size;
                    newMinterm->set_bits = getSetBits(*newMinterm);

                    //create new group if needed and append minterm to group 
                    int set_bits = newMinterm->set_bits;
                    if(!all_groups[set_bits]){
                        all_groups[set_bits] = (Group*) calloc(1, sizeof(Group));
                        all_groups[set_bits]->set_bits = set_bits;
                    }
                    Group_append(all_groups[set_bits], newMinterm);
                }
            }
        }
    }


    //pass all unminimized terms from old table to implicants of newTable
    memcpy(&newTable->implicants, &table->implicants, sizeof(Implicants));
    for(int i = 0; i < table->size; i++){
        Group *g = table->groups[i];

        for(int j = 0; j < g->size; j++){
            Minterm *m = g->minterms[j];
            if(m->isImplicant){
                append_implicant(&newTable->implicants, m);
            }
        }
    }


    //pass all_groups to table->groups and free all_groups
    for(int i = 0; i < MAX_MINTERM_BITSIZE; i++){
        if(all_groups[i]){
            Group *g = all_groups[i];
            //set all minimized terms as prime implicants for the next minimization
            for(int j = 0; j < g->size; j++){
                Minterm *m = g->minterms[j];
                m->isImplicant = true;
            }

            g->set_bits = i;
            Table_append(newTable, g);
        }
    }
    free(all_groups);
    //Old table is freed by caller
    //TODO: free old table
    
    return newTable;
}

Table *createTable(Minterm **minterms, int n){
    Table *table = (Table*) calloc(1, sizeof(Table));
    Group **all_groups = (Group**) calloc(MAX_MINTERM_BITSIZE, sizeof(void*));

    //get maximum number of set bits and fill "existing groups"
    table->max_setBits = 0;
    for(int i = 0; i < n; i++){ 
        int set_bits = getSetBits(*minterms[i]);
        if(set_bits > table->max_setBits){ 
            table->max_setBits = set_bits;
        }

        //append to all_groups[set_bits]
        if(!all_groups[set_bits]){
            all_groups[set_bits] = (Group*) calloc(1, sizeof(Group));
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
Minterm *IntToMinterm(uint64_t num, int largest_size){
    Minterm *minterm = (Minterm*) calloc(1, sizeof(Minterm));
    minterm->size = (int)(log2(largest_size)) + 1;
    minterm->isImplicant = true;    //This will be set to 0 if minimization
                                        //occurs in minimize_table

    //get number of set bits
    for(int i = 0; i < MAX_MINTERM_BITSIZE; i++){
        if((num >> i) & BIT_1){
            minterm->set_bits ++;
        }
    }

    //fill bit array
    minterm->bits = (BitState*) calloc(minterm->size, sizeof(BitState));
    for(int i = 0; i < minterm->size; i++){
        minterm->bits[i] = (BitState) ((num >> i) & 0x1);
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
