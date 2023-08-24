#ifndef UI_H
#define UI_H

#define PAD_LINES_SIZE 1024

#include <ncurses.h>
#include "quine.h"

typedef struct panel{
    WINDOW *borders;
    WINDOW *buf;
    int buf_line;
    int buf_xPos;
    int buf_yPos;
}Panel;


void menu_init(void);
void menu_TableWindow(Panel *pan);
void menu_CircuitWindow(Panel *pan);

void printMinterm(Panel *pan, Minterm *minterm);
void printGroup(Panel *pan, Group *group);
void printTable(Panel *pan, Table *table);
void printPChart(bool **Petrick_chart, int rows, int colums);
void printImplicants(Panel *pan, Implicants *implicants);

void printCircuit(Panel *pan, char *str);


#endif
