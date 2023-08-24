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



void menu_TableWindow(Panel *pan){
    pan->borders = newwin(48,50,5,1); 
    pan->buf = newpad(PAD_LINES_SIZE, 48);
    pan->buf_line = 0;

    refresh();
    box(pan->borders, 0, 0);
    wprintw(pan->borders, "|1|-|Quine-McCluskey Minimization|");
    wrefresh(pan->borders);
}

void menu_CircuitWindow(Panel *pan){
    pan->borders = newwin(48,50,5,51); 
    pan->buf = newpad(PAD_LINES_SIZE, 48);
    pan->buf_line = 0;


    refresh();
    box(pan->borders, 0, 0);
    wprintw(pan->borders, "|2|-|Circuit Visualization|");
    wrefresh(pan->borders);
}


void menu_init(void){
    initscr();
    cbreak();   //ctrl c exits the program
    noecho();     //characters on screen
}

void printMinterm(Panel *pan, Minterm *minterm){
    if(!minterm) return;

    if(minterm->size == 0){
        wprintw(pan->buf, "0");
    }

    //int leading_spaces = 25 - minterm->size;
    //wprintw(pan->contents, "%0*d", leading_spaces, 0);

    for(int i = minterm->size-1; i >= 0; i--){
        if(minterm->bits[i] == BIT_X){
            wprintw(pan->buf, "-");
        }else{
            wprintw(pan->buf, "%d", minterm->bits[i]);
        }
    }
}

void printGroup(Panel *pan, Group *group){
    if(!group) return;

    wprintw(pan->buf, "*-%d BITS GROUP:\n| size %d\n", group->set_bits, group->size);
    for(int i = 0; i < group->size; i++){
        wprintw(pan->buf, "| ");
        printMinterm(pan, group->minterms[i]);
        wprintw(pan->buf, "\n");
    }
    wprintw(pan->buf, "*--\n");
}

void printTable(Panel *pan, Table *table){
    if(!table) return;

    wprintw(pan->buf, "==========TABLE==========:\n");
    wprintw(pan->buf, "> size: %d\n", table->size);
    wprintw(pan->buf, "> max set bits: %d\n\n", table->max_setBits);
    for(int i = 0; i < table->size; i++){
        printGroup(pan, table->groups[i]);
    }
    wprintw(pan->buf, "*-Implicants:\n");
    for(int i = 0; i < table->implicants.size; i++){
        printMinterm(pan, table->implicants.minterms[i]);
        wprintw(pan->buf, "\n");
    }
    wprintw(pan->buf, "=========================:\n\n");
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

void printImplicants(Panel *pan, Implicants *implicants){
    for(int i = 0; i < implicants->size; i++){
        printMinterm(pan, implicants->minterms[i]);
        wprintw(pan->buf, "\n");
    }
}
