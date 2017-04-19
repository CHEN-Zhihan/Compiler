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

int ex(nodeType *, int, int);
vector<map<string, int> > variableStack;
map<string, int> functionMap;
vector<nodeType *> functionTable;


void run(list<nodeType *>* l) {
    list<nodeType *> definitionList;
    list<nodeType *> functionList;
    for (auto i: *l) {
        if (i->type == typeOpr) {
            definitionList.push_front(i);
        } else {
            functionList.push_front(i);
        }
    }
    map<string, int> global;
    for (auto i:definitionList) {
        ex(i->opr.op[1], 998, 998);
        global[*i->opr.op[0]->id.name] = sb++;
        ++sp;
    }
    variableStack.push_back(global);
    fb = sb + 1;
    nodeType * entry = nullptr;
    for (auto i:functionList) {
        if (*i->func.name == "main" and (i->func.parameters->size() == 0)) {
            entry = i->func.stmts;
        }
    }
    if (entry == nullptr) {
        cerr << "main() function not found" << endl;
        exit(-1);
    }
    variableStack.push_back(map<string, int>());
    ex(entry, 998, 998);
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
            string name = *(p->id.name);
            if (p->id.global) {
                if (variableStack[0].count(name) == 0) {
                    cerr << "Undefined reference to global variable: " << name << endl;
                    exit(-1);
                }
                printf("\tpush\tsb[%d]\n", variableStack[0][name]);
            } else {
                if (TOP(variableStack).count(name) == 0) {
                    cerr << "Undefined reference to variable: " << name << endl;
                    exit(-1);
                }
                printf("\tpush\tfp[%d]\n", TOP(variableStack)[name]);
            }
            break;
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
                    printf("\tjz\tL%03d\n", lbly);
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
                    printf("\tjz\tL%03d\n", lbly);
                    ex(p->opr.op[1], lbly, lblx);
                    printf("\tjmp\tL%03d\n", lblx);
                    printf("L%03d:\n", lbly);
                    break;
                case IF:
                    ex(p->opr.op[0], blbl, clbl);
                    if (p->opr.nops > 2) {
                        /* if else */
                        printf("\tjz\tL%03d\n", lblx = lbl++);
                        ex(p->opr.op[1], blbl, clbl);
                        printf("\tjmp\tL%03d\n", lbly = lbl++);
                        printf("L%03d:\n", lblx);
                        ex(p->opr.op[2], blbl, clbl);
                        printf("L%03d:\n", lbly);
                    } else {
                        /* if */
                        printf("\tjz\tL%03d\n", lblx = lbl++);
                        ex(p->opr.op[1], blbl, clbl);
                        printf("L%03d:\n", lblx);
                    }
                    break;
                case '=': {
                    ex(p->opr.op[1], blbl, clbl);
                    string name = *(p->opr.op[0]->id.name);
                    if (p->opr.op[0]->id.global) {
                        if (variableStack[0].count(name) == 0) {
                            cerr << "Reference to undefined global variable: " << name << endl;
                            exit(-1);
                        }
                        printf("\tpop\tsb[%d]\n", variableStack[0][name]);
                    } else {
                        if (TOP(variableStack).count(name) == 0) {
                            TOP(variableStack)[name] = TOP(variableStack).size() - 1;
                        } else {
                            printf("\tpop\tfb[%d]\n", TOP(variableStack)[name]);
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
