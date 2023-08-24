#ifndef UI_H
#define UI_H

#define PAD_LINES_SIZE 1024

#include "main.h"


void menu_init(void);
void menu_TableWindow(Panel *pan);

void printMinterm(Panel *pan, Minterm *minterm);
void printGroup(Panel *pan, Group *group);
void printTable(Panel *pan, Table *table);
void printPChart(bool **Petrick_chart, int rows, int colums);
void printImplicants(Panel *pan, Implicants *implicants);


#endif
