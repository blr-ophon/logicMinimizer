#include "ctx.h"

#define BUF_SIZE 256

//TODO: each panel has its lines as parameter
//at the end of printing, it gets cursor values,
//so to print at the end first

Panel *ctx_pan;


/*
 * gets terms and number of terms from a string
 * a,b,c,d,e,f
 */
uint64_t *parseTerms(char *str, int *n_out){
    int count = 0;

    const char s[2] = ",";

    uint64_t *terms = (uint64_t*) calloc(1, sizeof(uint64_t));

    // get the first token
    char *token;
    token = strtok(str, s);

    /* walk through other tokens */
    while(token != NULL) {
        count++;
        terms = (uint64_t*) realloc(terms, count*sizeof(uint64_t));
        terms[count-1] = atoi(token);
        token = strtok(NULL, s);
    }

    *n_out = count;
    return terms;
}



uint64_t *getTerms(Panel *pan, int *n_out){
    echo();
    char buf[BUF_SIZE] = {0};
    menu_PromptWindow(pan);
    wgetnstr(pan->buf, buf, BUF_SIZE);
    noecho();
    return parseTerms(buf, n_out);
}

void newOperation(Panel *Table_p, Panel *Circuit_p, Panel *Prompt_p){
    /* 
     * Prompt for terms
     */
    int n = 0;
    uint64_t *terms = getTerms(Prompt_p, &n);

    /*
     * Fill table and circuit
     */
    menu_TableWindow(Table_p);
    menu_CircuitWindow(Circuit_p);
    QM_tables(Table_p, Circuit_p, terms, n);

    //prerefresh: pad, y, x,  ret_ymin, ret_xmin, ret_ymax, ret_xmax
    prefresh(Table_p->buf, Table_p->buf_line, 0, 7, 2, 48, 48); 
    prefresh(Circuit_p->buf, Circuit_p->buf_line, 0, 7, 52, 48, 48+50); 
}



int main(void){
    menu_init();

    Panel Table_p;
    Panel Circuit_p;
    Panel Prompt_p;

    newOperation(&Table_p, &Circuit_p, &Prompt_p);
    
    //start at table
    ctx_pan = &Table_p;
    menu_highlightBorders(ctx_pan);

    bool running = true;
    while(running){
        char c = getch();
        switch(c){
            case '0':
                newOperation(&Table_p, &Circuit_p, &Prompt_p);
                ctx_pan = &Table_p;
                menu_highlightBorders(ctx_pan);
                break;
            case '1':
                menu_printBorders(ctx_pan);         //unhighlight previous
                ctx_pan = &Table_p;
                menu_highlightBorders(ctx_pan);
                break;
            case '2':
                menu_printBorders(ctx_pan);         //unhighlight previous
                ctx_pan = &Circuit_p;
                menu_highlightBorders(ctx_pan);
                break;
            case 'j':
                if(ctx_pan->buf_line < PAD_LINES_SIZE)
                    ctx_pan->buf_line += 2;
                prefresh(ctx_pan->buf, ctx_pan->buf_line, 0, 
                        ctx_pan->buf_yPos, ctx_pan->buf_xPos, 
                        48, ctx_pan->buf_xPos+46); 
                break;
            case 'k':
                if(ctx_pan->buf_line > 0)
                    ctx_pan->buf_line -= 2;
                prefresh(ctx_pan->buf, ctx_pan->buf_line, 0, 
                        ctx_pan->buf_yPos, ctx_pan->buf_xPos, 
                        48, ctx_pan->buf_xPos+46); 
                break;
            case 'q':
                running = false;
                break;
        }
    }

    endwin();
    return 0;
}
