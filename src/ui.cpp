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

void menu_printContent(Panel *pan){
    //prerefresh: pad, y, x,  ret_ymin, ret_xmin, ret_ymax, ret_xmax
    prefresh(pan->buf, pan->buf_line, 0, 
            pan->buf_yPos, pan->buf_xPos, 
            pan->buf_yPos + pan->height - 4, 
            pan->buf_xPos + pan->width - 3); 
}

void menu_highlightBorders(Panel *pan){
    wclear(pan->borders);
    wattr_on(pan->borders, A_REVERSE, NULL);
    menu_printBorders(pan);
    wattr_off(pan->borders, A_REVERSE, NULL);
    wrefresh(pan->borders);
}

void menu_printBorders(Panel *pan){
    wclear(pan->borders);
    box(pan->borders, 0, 0);
    wprintw(pan->borders, pan->title);
    wrefresh(pan->borders);

    //reprint the content above the border box
    menu_printContent(pan);
}

void menu_PromptWindow(Panel *pan){
    int y,x;
    getmaxyx(stdscr,y,x);

    char *str = "|Enter terms (a,b,c...)|";
    pan->height = y/3;
    pan->width = x/4;
    pan->yPos = y/2 - (pan->height/2);
    pan->xPos = x/2 - (pan->width/2);

    pan->borders = newwin(pan->height, pan->width,
            pan->yPos, pan->xPos);         //height, width, y, x
    pan->title = strdup(str);
                                      
                                      
    pan->buf = newwin(pan->height-2, pan->width-2,
            pan->yPos+1, pan->xPos+1);
    pan->buf_yPos = pan->yPos+2;
    pan->buf_xPos = pan->xPos+1;

    refresh();
    menu_printBorders(pan);
}

void menu_TableWindow(Panel *pan){
    int y,x;
    getmaxyx(stdscr,y,x);

    char *str = "|1|-|Quine-McCluskey Minimization|";
    pan->height = (4*y)/5;
    pan->width = x/2-1;
    pan->yPos = y/5;
    pan->xPos = 1;

    pan->borders = newwin(pan->height,pan->width, pan->yPos, pan->xPos);    //height, width, y, x
    pan->title = strdup(str);
                                      
    pan->buf = newpad(PAD_LINES_SIZE, pan->width-2);      //height, width
    pan->buf_line = 0;
    pan->buf_yPos = pan->yPos+2;
    pan->buf_xPos = pan->xPos+1;

    refresh();
    menu_printBorders(pan);
}

void menu_CircuitWindow(Panel *pan){
    int y,x;
    getmaxyx(stdscr,y,x);

    char *str = "|2|-|Circuit Visualization|";
    pan->height = (4*y)/5;
    pan->width = x/2-1;
    pan->yPos = y/5;
    pan->xPos = x/2;

    pan->borders = newwin(pan->height,pan->width, pan->yPos, pan->xPos);    //height, width, y, x
    pan->title = strdup(str);

    pan->buf = newpad(PAD_LINES_SIZE, pan->width-2);      //height, width
    pan->buf_line = 0;
    pan->buf_yPos = pan->yPos+2;
    pan->buf_xPos = pan->xPos+1;

    refresh();
    menu_printBorders(pan);
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
