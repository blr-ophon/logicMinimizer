#include "ctx.h"

#define BUF_SIZE 256

//TODO: each panel has its lines as parameter
//at the end of printing, it gets cursor values,
//so to print at the end first

Panel *ctx_pan;

uint64_t *getTerms(int *n_out){
    char buf[BUF_SIZE] = {0};
    printf("\nNumber of terms:");
    fgets(buf, BUF_SIZE, stdin);
    int terms_n = atoi(buf);

    //Get minterms 
    printf("\nTerms:\n");
    uint64_t *terms = (uint64_t*) calloc(terms_n, sizeof(uint64_t));
    int valid_terms_n = 0;

    for(int i = 0; i < terms_n; i++){
        fgets(buf, BUF_SIZE, stdin);
        uint64_t newTerm = atoi(buf);
        if(!termIsPresent(newTerm, terms, terms_n)){
            //ignore repeated terms
            terms[valid_terms_n++] = newTerm;
        }
    }

    *n_out = valid_terms_n;
    return terms;
}

int main(void){
    //get input
    //Get number of minterms 
    int n = 0;
    uint64_t *terms = getTerms(&n);

    menu_init();



    Panel Table_p;
    Panel Circuit_p;

    menu_TableWindow(&Table_p);
    menu_CircuitWindow(&Circuit_p);

    /*
     * Fill table and circuit
     */
    QM_tables(&Table_p, &Circuit_p, terms, n);



    /*
     * Start program loop
     */

    //prerefresh: pad, y, x,  ret_ymin, ret_xmin, ret_ymax, ret_xmax
    prefresh(Table_p.buf, Table_p.buf_line, 0, 7, 2, 48, 48); 
    prefresh(Circuit_p.buf, Circuit_p.buf_line, 0, 7, 52, 48, 48+50); 
    
    //start at table
    ctx_pan = &Table_p;

    bool running = true;
    while(running){
        char c = getch();
        switch(c){
            case '1':
                //TODO: Highlight borders
                ctx_pan = &Table_p;
                break;
            case '2':
                //TODO: Highlight borders
                ctx_pan = &Circuit_p;
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
