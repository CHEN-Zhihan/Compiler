#include <cstring>
using std::strlen;
#include <iostream>
using std::cout;    using std::endl;
using std::printf;  using std::cerr;
#include <string>
using std::string;
#include <list>
using std::list;

#include <vector>
using std::vector;

#include <map>
using std::map;

#include "c6.h"
#include "parser.tab.h"

static int lbl;
static int sb;
static int fb;
static int sp;

#define TOP(v) v[v.size() - 1]

extern vector<string> reverseLookup;
vector<map<int, int> > variableStack;
map<int, int> functionMap;
vector<nodeType *> functionTable;

void variableCheck(int i, bool global) {
    if (global) {
        if (variableStack[0].count(i) == 0) {
            cerr << "Undefined reference to global variable: " << reverseLookup[i] << endl;
            exit(-1);
        }
        if (variableStack.size() == 1) {
            cerr << "Refering to global variable: " << reverseLookup[i] << " in the global scope" << endl;
            exit(-1);
        }
    } else {
        if (TOP(variableStack).count(i) == 0) {
            cerr << "Undefined reference to variable: " << reverseLookup[i] << endl;
            exit(-1);
        }
    }
}



int ex(nodeType *p, int blbl, int clbl) {
    int lblx, lbly, lblz;
    if (!p) return 0;
    switch(p->type) {
        case typeCon:
            switch (p->con.type) {
                case INT: {
                    printf("\tpush\t%d\n", p->con.iValue);
                    break;
                } case CHAR: {
                    printf("\tpush\t%c\n", p->con.cValue);
                    break;
                } case STR: {
                    printf("\tpush\t%s\n", p->con.sValue);
                    break;
                }
            }
            break;
        case typeId: {
            int i = p->id.i;
            variableCheck(i, p->id.global);
            if (p->id.global or variableStack.size() == 1) {
                printf("\tpush\tsb[%d]\n", variableStack[0][i]);
            } else {
                printf("\tpush\tfp[%d]\n", TOP(variableStack)[i]);
            }
            break;
        }
        case typeFunc: {
            ;
        }
        case typeOpr:
            switch(p->opr.oper) {
                case BREAK:
                    printf("\tjmp\tL%03d\n", blbl);
                    break;
                case CONTINUE:
                    printf("\tjmp\tL%03d\n", clbl);
                    break;
                case FOR:
                    lblx = lbl++;
                    lbly = lbl++;
                    lblz = lbl++;
                    ex(p->opr.op[0], blbl, clbl);
                    printf("L%03d:\n", lblx);
                    ex(p->opr.op[1], blbl, clbl);
                    printf("\tj0\tL%03d\n", lbly);
                    ex(p->opr.op[3], lbly, lblz);
                    printf("L%03d:\n", lblz);
                    ex(p->opr.op[2], blbl, clbl);
                    printf("\tjmp\tL%03d\n", lblx);
                    printf("L%03d:\n", lbly);
                    break;
                case WHILE:
                    lblx = lbl++;
                    lbly = lbl++;
                    printf("L%03d:\n", lblx);
                    ex(p->opr.op[0], blbl, clbl);
                    printf("\tj0\tL%03d\n", lbly);
                    ex(p->opr.op[1], lbly, lblx);
                    printf("\tjmp\tL%03d\n", lblx);
                    printf("L%03d:\n", lbly);
                    break;
                case IF:
                    ex(p->opr.op[0], blbl, clbl);
                    if (p->opr.nops > 2) {
                        /* if else */
                        printf("\tj0\tL%03d\n", lblx = lbl++);
                        ex(p->opr.op[1], blbl, clbl);
                        printf("\tjmp\tL%03d\n", lbly = lbl++);
                        printf("L%03d:\n", lblx);
                        ex(p->opr.op[2], blbl, clbl);
                        printf("L%03d:\n", lbly);
                    } else {
                        /* if */
                        printf("\tj0\tL%03d\n", lblx = lbl++);
                        ex(p->opr.op[1], blbl, clbl);
                        printf("L%03d:\n", lblx);
                    }
                    break;
                case '=': {
                    ex(p->opr.op[1], blbl, clbl);
                    int i = p->opr.op[0]->id.i;
                    if (p->opr.op[0]->id.global) {
                        if (variableStack[0].count(i) == 0) {
                            cerr << "Reference to undefined global variable: " << reverseLookup[i] << endl;
                            exit(-1);
                        }
                        printf("\tpop\tsb[%d]\n", variableStack[0][i]);
                    } else {
                        if (TOP(variableStack).count(i) == 0) {
                            TOP(variableStack)[i] = TOP(variableStack).size() - 1;
                        } else {
                            if (variableStack.size() == 1) {
                                printf("\tpop\tsb[%d]\n", variableStack[0][i]);
                            } else {
                                printf("\tpop\tfb[%d]\n", TOP(variableStack)[i]);
                            }
                        }
                    }
                    break;
                }
                
                case UMINUS:
                    ex(p->opr.op[0], blbl, clbl);
                    printf("\tneg\n");
                    break;
                default:
                    ex(p->opr.op[0], blbl, clbl);
                    ex(p->opr.op[1], blbl, clbl);
                    switch(p->opr.oper) {
                        case '+':   printf("\tadd\n"); break;
                        case '-':   printf("\tsub\n"); break;
                        case '*':   printf("\tmul\n"); break;
                        case '/':   printf("\tdiv\n"); break;
                        case '%':   printf("\tmod\n"); break;
                        case '<':   printf("\tcompLT\n"); break;
                        case '>':   printf("\tcompGT\n"); break;
                        case GE:    printf("\tcompGE\n"); break;
                        case LE:    printf("\tcompLE\n"); break;
                        case NE:    printf("\tcompNE\n"); break;
                        case EQ:    printf("\tcompEQ\n"); break;
                        case AND:   printf("\tand\n"); break;
                        case OR:    printf("\tor\n"); break;
                    }
            }
    }

    return 0;
}

void eop() {
    // add end of program label
    printf("\tjmp\tL999\n");
    printf("L998:\n");
    printf("\tpush\t999999\n");
    printf("\tprint\n");
    printf("\tpush\t999999\n");
    printf("\tprint\n");
    printf("\tpush\t999999\n");
    printf("\tprint\n");
    printf("L999:\n");
}

void run(nodeType *p) {
    variableStack.push_back(map<int, int>{});
    ex(p, 998, 998);
}