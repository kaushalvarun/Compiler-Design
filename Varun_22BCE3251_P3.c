/* slr.c
   Simple SLR parser implementation with tabular parse trace.
   Assumptions:
     - Non-terminals are uppercase letters A-Z
     - Terminals are single characters except uppercase letters
     - Epsilon = '#'
     - Productions like A->aB
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXP 100    // max productions
#define MAXRHS 50   // max rhs length
#define MAXSYMS 128 // ASCII
#define MAXITEMS 500
#define MAXSTATES 200
#define MAXCOL 128
#define MAXSTR 256

typedef struct {
    char lhs;               // single nonterminal
    char rhs[MAXRHS];       // string of rhs symbols, '#' for epsilon
} Production;

typedef struct {
    int prod;   // production index
    int dot;    // position of dot (0..len)
} Item;

typedef struct {
    Item items[MAXITEMS];
    int nitems;
} ItemSet;

Production prods[MAXP];
int nprods = 0;
char nonterms[26]; int nnon = 0;
char terms[MAXCOL]; int nterm = 0;
int augmented_index = -1;
char start_symbol = 0;

ItemSet states[MAXSTATES];
int nstates = 0;

int goto_table[MAXSTATES][MAXCOL]; // index of state or -1
int action_type[MAXSTATES][MAXCOL]; // 0 none, 1 shift, 2 reduce, 3 accept
int action_val[MAXSTATES][MAXCOL]; // state or production index

// Follow sets
int follow[MAXSYMS][MAXCOL]; // follow[A][t] = 1 if t in FOLLOW(A)
int firstset[MAXSYMS][MAXCOL];
int nullable[MAXSYMS];

// helper: map char to index
int cindex(char c){ return (int)(unsigned char)c; }

// string utility
int contains(char *s, char ch){
    for(int i=0;s[i];i++) if(s[i]==ch) return 1;
    return 0;
}

// add production (splits | into separate productions)
void add_production_line(char *line){
    // find lhs -> rhslist
    char lhs = line[0];
    start_symbol = start_symbol ? start_symbol : lhs;
    if(nprods==0){
        // will add augmented later
    }
    // find "->"
    char *arrow = strstr(line,"->");
    if(!arrow) return;
    char *rhslist = arrow + 2;
    // split by '|'
    char *p = rhslist;
    char buffer[MAXSTR];
    while(*p){
        char *q = p;
        int bi=0;
        while(*q && *q!='|'){ buffer[bi++]=*q; q++; }
        buffer[bi]=0;
        // trim
        int i=0; while(buffer[i]==' ') i++;
        memmove(buffer, buffer+i, strlen(buffer+i)+1);
        i=strlen(buffer)-1; while(i>=0 && buffer[i]==' ') buffer[i--]=0;
        // add production
        prods[nprods].lhs = lhs;
        if(strlen(buffer)==0) strcpy(prods[nprods].rhs, "#"); else strcpy(prods[nprods].rhs, buffer);
        nprods++;
        if(*q=='|') p = q+1; else break;
    }
}

// check and add non-terminals and terminals lists
void collect_symbols(){
    int seenNT[26]={0};
    int seenT[MAXCOL]={0};
    for(int i=0;i<nprods;i++){
        if(prods[i].lhs>='A' && prods[i].lhs<='Z'){
            if(!seenNT[prods[i].lhs-'A']){ nonterms[nnon++]=prods[i].lhs; seenNT[prods[i].lhs-'A']=1; }
        }
        for(int j=0;j<strlen(prods[i].rhs);j++){
            char c = prods[i].rhs[j];
            if(c=='#') continue;
            if(c>='A' && c<='Z'){
                if(!seenNT[c-'A']){ nonterms[nnon++]=c; seenNT[c-'A']=1; }
            } else {
                if(!seenT[(int)c]){ terms[nterm++]=c; seenT[(int)c]=1; }
            }
        }
    }
    // ensure $ is a terminal
    if(!contains(terms,'$')) terms[nterm++] = '$';
}

// production length helper
int prod_len(int idx){
    if(prods[idx].rhs[0]=='#') return 0;
    return strlen(prods[idx].rhs);
}

// item equality
int item_equal(Item *a, Item *b){
    return a->prod==b->prod && a->dot==b->dot;
}

// add item if not present
int add_item(ItemSet *S, Item it){
    for(int i=0;i<S->nitems;i++) if(item_equal(&S->items[i], &it)) return 0;
    S->items[S->nitems++] = it;
    return 1;
}

// closure
void closure(ItemSet *I){
    int changed = 1;
    while(changed){
        changed = 0;
        for(int i=0;i<I->nitems;i++){
            Item it = I->items[i];
            int len = prod_len(it.prod);
            if(it.dot < len){
                char B = prods[it.prod].rhs[it.dot];
                if(B>='A' && B<='Z'){
                    // for each production B -> gamma, add B .-> gamma with dot 0
                    for(int p=0;p<nprods;p++){
                        if(prods[p].lhs == B){
                            Item newit; newit.prod = p; newit.dot = 0;
                            if(add_item(I, newit)) changed=1;
                        }
                    }
                }
            }
        }
    }
}

// goto on symbol X (char)
ItemSet goto_set(ItemSet *I, char X){
    ItemSet J; J.nitems = 0;
    for(int i=0;i<I->nitems;i++){
        Item it = I->items[i];
        int len = prod_len(it.prod);
        if(it.dot < len){
            char a = prods[it.prod].rhs[it.dot];
            if(a == X){
                Item nit; nit.prod = it.prod; nit.dot = it.dot + 1;
                add_item(&J, nit);
            }
        }
    }
    closure(&J);
    return J;
}

// compare itemsets
int itemset_equal(ItemSet *a, ItemSet *b){
    if(a->nitems != b->nitems) return 0;
    for(int i=0;i<a->nitems;i++){
        int found=0;
        for(int j=0;j<b->nitems;j++) if(item_equal(&a->items[i], &b->items[j])){ found=1; break;}
        if(!found) return 0;
    }
    return 1;
}

// find state index if equal exists
int find_state(ItemSet *S){
    for(int i=0;i<nstates;i++){
        if(itemset_equal(S, &states[i])) return i;
    }
    return -1;
}

// generate canonical collection of LR(0) items
void build_states(){
    // initial item S'->.S (we added augmented production at index 0)
    ItemSet I0; I0.nitems=0;
    Item it0; it0.prod = 0; it0.dot = 0; add_item(&I0, it0);
    closure(&I0);
    states[0] = I0; nstates = 1;

    int changed = 1;
    while(changed){
        changed = 0;
        for(int i=0;i<nstates;i++){
            // for every grammar symbol X (nonterminals + terminals)
            // try goto
            for(int c = 0; c < MAXCOL; c++){
                if(c==0) continue;
                char X = (char)c;
                // consider only symbols that appear in grammar
                // compute goto(states[i], X)
                ItemSet J = goto_set(&states[i], X);
                if(J.nitems==0) continue;
                int idx = find_state(&J);
                if(idx == -1){
                    // new state
                    states[nstates] = J;
                    idx = nstates;
                    nstates++;
                    changed = 1;
                }
                // record mapping in goto_table later
            }
        }
    }
    // Populate goto_table with real mappings
    for(int i=0;i<nstates;i++){
        for(int c=0;c<MAXCOL;c++) goto_table[i][c] = -1;
    }
    for(int i=0;i<nstates;i++){
        for(int c=0;c<MAXCOL;c++){
            char X = (char)c;
            ItemSet J = goto_set(&states[i], X);
            if(J.nitems==0) continue;
            int idx = find_state(&J);
            if(idx!=-1) goto_table[i][c] = idx;
        }
    }
}

// compute FIRST sets (simple)
void compute_first(){
    for(int c=0;c<MAXCOL;c++) for(int t=0;t<MAXCOL;t++) firstset[c][t]=0;
    for(int i=0;i<nterm;i++){
        char t = terms[i];
        firstset[cindex(t)][cindex(t)] = 1;
    }
    // For nonterminals, initialize to empty
    for(int iter=0;iter<100;iter++){
        int changed = 0;
        for(int p=0;p<nprods;p++){
            char A = prods[p].lhs;
            int idxA = cindex(A);
            if(prods[p].rhs[0]=='#'){
                if(!nullable[idxA]){ nullable[idxA]=1; changed=1; }
            } else {
                int allnull = 1;
                for(int k=0;k<strlen(prods[p].rhs);k++){
                    char Y = prods[p].rhs[k];
                    if(Y>='A' && Y<='Z'){
                        // add FIRST(Y)-{epsilon} to FIRST(A)
                        for(int t=0;t<MAXCOL;t++){
                            if(firstset[cindex(Y)][t] && !firstset[idxA][t]){
                                firstset[idxA][t]=1; changed=1;
                            }
                        }
                        if(!nullable[cindex(Y)]){ allnull=0; break; }
                    } else {
                        if(!firstset[idxA][(int)Y]){ firstset[idxA][(int)Y]=1; changed=1; }
                        allnull = 0; break;
                    }
                }
                if(allnull){
                    if(!nullable[idxA]){ nullable[idxA]=1; changed=1; }
                }
            }
        }
        if(!changed) break;
    }
}

// helper: add follow terminal
int add_follow(char A, char t){
    int idxA = cindex(A), idxt = cindex(t);
    if(follow[idxA][idxt]) return 0;
    follow[idxA][idxt]=1;
    return 1;
}

// compute FOLLOW sets (simple)
void compute_follow(){
    for(int i=0;i<MAXSYMS;i++) for(int j=0;j<MAXCOL;j++) follow[i][j]=0;
    // follow(start) contains $
    add_follow(start_symbol, '$');

    for(int iter=0;iter<200;iter++){
        int changed = 0;
        for(int p=0;p<nprods;p++){
            char A = prods[p].lhs;
            int len = prod_len(p);
            for(int i=0;i<len;i++){
                char B = prods[p].rhs[i];
                if(!(B>='A' && B<='Z')) continue;
                // beta = rhs[i+1..]
                int allnullable = 1;
                // add FIRST(beta) - epsilon to FOLLOW(B)
                for(int j=i+1;j<len;j++){
                    char Y = prods[p].rhs[j];
                    if(Y>='A' && Y<='Z'){
                        // add FIRST(Y)
                        for(int t=0;t<MAXCOL;t++){
                            if(firstset[cindex(Y)][t]){
                                if(!follow[cindex(B)][t]){
                                    follow[cindex(B)][t]=1; changed=1;
                                }
                            }
                        }
                        if(!nullable[cindex(Y)]){ allnullable=0; break;}
                    } else {
                        // terminal
                        if(!follow[cindex(B)][cindex(Y)]){ follow[cindex(B)][cindex(Y)]=1; changed=1;}
                        allnullable=0; break;
                    }
                }
                if(allnullable){
                    // add FOLLOW(A) to FOLLOW(B)
                    for(int t=0;t<MAXCOL;t++){
                        if(follow[cindex(A)][t] && !follow[cindex(B)][t]){
                            follow[cindex(B)][t]=1; changed=1;
                        }
                    }
                }
            }
        }
        if(!changed) break;
    }
}

// Build SLR ACTION/GOTO table
void build_table(){
    // init
    for(int i=0;i<MAXSTATES;i++){
        for(int j=0;j<MAXCOL;j++){
            action_type[i][j]=0; action_val[i][j]= -1;
            // goto_table already populated
        }
    }

    // for each state i and each item [A->alpha . a beta], if goto(i,a)=j and a is terminal, action[i,a]=shift j
    for(int i=0;i<nstates;i++){
        for(int k=0;k<states[i].nitems;k++){
            Item it = states[i].items[k];
            int len = prod_len(it.prod);
            if(it.dot < len){
                char a = prods[it.prod].rhs[it.dot];
                if(!(a>='A' && a<='Z')){ // terminal
                    int j = goto_table[i][cindex(a)];
                    if(j!=-1){
                        action_type[i][cindex(a)] = 1; action_val[i][cindex(a)] = j;
                    }
                } else {
                    // nonterminal -> goto will set goto_table for GOTO table (we'll use goto_table directly)
                }
            } else {
                // dot at end: A->alpha.
                if(it.prod == 0){
                    // augmented production S'->S.
                    action_type[i][cindex('$')] = 3; // accept
                } else {
                    // for every terminal a in FOLLOW(A), set action[i,a] = reduce by prod it.prod
                    char A = prods[it.prod].lhs;
                    for(int t=0;t<nterm;t++){
                        char a = terms[t];
                        if(follow[cindex(A)][cindex(a)]){
                            // reduce
                            // check conflict rudimentary: prefer shift over reduce if shift exists
                            if(action_type[i][cindex(a)]==1){
                                // shift/reduce conflict -> keep shift (simple) ; in SLR ambiguous grammars this may be wrong
                            } else {
                                action_type[i][cindex(a)] = 2;
                                action_val[i][cindex(a)] = it.prod;
                            }
                        }
                    }
                }
            }
        }
    }
}

// pretty print production
void print_prod(int idx){
    printf("%c->%s", prods[idx].lhs, prods[idx].rhs);
}

// parse input string and print table of steps
void parse_input(char *input){
    // stack contains integers (states) and symbols; we'll implement states stack only and keep separate symbol stack
    int state_stack[MAXSTR]; int top = 0;
    char sym_stack[MAXSTR]; int stop = 0;
    state_stack[top++] = 0;
    sym_stack[stop++] = '#'; // bottom marker (not used)
    // append $ to input
    char inbuf[MAXSTR]; strcpy(inbuf, input);
    int L = strlen(inbuf);
    if(inbuf[L-1] != '$'){ strcat(inbuf, "$"); }
    int ip = 0;

    printf("\n%-20s | %-20s | %-30s\n", "Stack", "Remaining Input", "Action");
    printf("--------------------------------------------------------------------------------------\n");
    while(1){
        // build stack display: symbols and states (format: s0 A s1 b s2 ...)
        char stack_disp[512]; stack_disp[0]=0;
        for(int i=0;i<top;i++){
            char tmp[16];
            sprintf(tmp, "%d ", state_stack[i]);
            strcat(stack_disp, tmp);
        }
        char rem[128]; sprintf(rem, "%s", inbuf+ip);
        // determine action: look at current state and current input symbol
        int state = state_stack[top-1];
        char a = inbuf[ip];
        int atype = action_type[state][cindex(a)];
        int aval  = action_val[state][cindex(a)];
        if(atype==1){ // shift
            printf("%-20s | %-20s | shift %d (on '%c')\n", stack_disp, rem, aval, a);
            // push a then state
            sym_stack[stop++] = a;
            state_stack[top++] = aval;
            ip++;
        } else if(atype==2){ // reduce by production aval
            int p = aval;
            int len = prod_len(p);
            printf("%-20s | %-20s | reduce by ", stack_disp, rem);
            print_prod(p); printf("\n");
            // pop 2*len (for symbols & states) but since we store only states and syms separate:
            if(len > 0){
                stop -= len;
                top -= len;
            }
            // push lhs symbol
            char A = prods[p].lhs;
            sym_stack[stop++] = A;
            // goto from current top state on A
            int curstate = state_stack[top-1];
            int nxt = goto_table[curstate][cindex(A)];
            if(nxt == -1){
                printf("Error: no goto for state %d on %c\n", curstate, A);
                return;
            }
            state_stack[top++] = nxt;
        } else if(atype==3){
            printf("%-20s | %-20s | accept\n", stack_disp, rem);
            return;
        } else {
            printf("%-20s | %-20s | error -- no action\n", stack_disp, rem);
            return;
        }
    }
}

int main(){
    printf("SLR Parser (C) - Enter grammar productions.\n");
    printf("Conventions: use single-char nonterminals A-Z; terminals are other chars; epsilon=#\n");
    int pcount;
    printf("Enter number of productions: ");
    if(scanf("%d%*c", &pcount)!=1) return 0;
    char line[MAXSTR];
    for(int i=0;i<pcount;i++){
        printf("Prod %d: ", i+1);
        if(!fgets(line, sizeof(line), stdin)){ line[0]=0; }
        if(strlen(line)==0){ i--; continue; }
        // trim newline
        line[strcspn(line, "\n")] = 0;
        add_production_line(line);
    }
    // Augment grammar: S' -> S (make new production at index 0 by shifting existing)
    // We'll insert new prod at beginning
    for(int i=nprods;i>0;i--) prods[i]=prods[i-1];
    nprods++;
    prods[0].lhs = 'Z'; // use 'Z' as augmented start (unlikely user used it); but better make S' as ASCII not uppercase - keep 'Z' only if unused
    // choose a char not used as nonterminal
    char aug = 'Z';
    // ensure unique
    int used[256]={0};
    for(int i=1;i<nprods;i++) used[(int)prods[i].lhs]=1;
    for(char ch='A';ch<='Z';ch++) if(!used[(int)ch]){ aug=ch; break; }
    prods[0].lhs = aug;
    prods[0].rhs[0] = start_symbol; prods[0].rhs[1]=0;
    augmented_index = 0;
    start_symbol = start_symbol; // unchanged

    collect_symbols();
    compute_first();
    compute_follow();
    build_states();
    build_table();

    // print productions
    printf("\nProductions (numbered):\n");
    for(int i=0;i<nprods;i++){
        printf("%2d: ", i); print_prod(i); printf("\n");
    }
    printf("\nNumber of states: %d\n", nstates);

    // optional: print ACTION table summary (terminals only)
    printf("\nACTION (state x terminal) summary (non-empty entries):\n");
    for(int i=0;i<nstates;i++){
        for(int t=0;t<nterm;t++){
            char a = terms[t];
            int typ = action_type[i][cindex(a)];
            if(typ==1) printf("state %d, '%c' : shift %d\n", i, a, action_val[i][cindex(a)]);
            else if(typ==2) printf("state %d, '%c' : reduce by %d\n", i, a, action_val[i][cindex(a)]);
            else if(typ==3) printf("state %d, '%c' : accept\n", i, a);
        }
    }
    printf("\nNow enter input string to parse (no $ needed) : ");
    char input[MAXSTR];
    if(!fgets(input, sizeof(input), stdin)) return 0;
    input[strcspn(input, "\n")] = 0;
    if(strlen(input)==0) { printf("Empty input. Exiting.\n"); return 0; }
    parse_input(input);
    return 0;
}
