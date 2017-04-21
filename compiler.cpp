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
using std::map; using std::pair;

#include <set>
using std::set;

#include "c6.h"
#include "parser.tab.h"

static int lbl;
static int sb;
static int fb;
static int sp;

#define TOP(v) v[v.size() - 1]

extern vector<string> reverseLookup;
static set<int> comparator;
static map<int, string> reverseSymbolLookup;
static vector<map<int, pair<int, valueEnum> > > variableStack;
static vector<map<int, int> > functionStack;
static map<int, set<pair<valueEnum, valueEnum> > > applicable;
static vector<string> reverseTypeLookup;
static vector<valueEnum> requiredType;
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

void typeCheck(const nodeType * p, const callNodeType & c) {
    if (c.i >= 3 and c.i < 9 and p->valueType != requiredType[c.i]) {
        cerr << "Invalid call to function: " << reverseLookup[c.i]
            << " expect type: " << reverseTypeLookup[requiredType[c.i]]
            << " actual type: " << reverseTypeLookup[p->valueType] << endl;
        exit(-1);
    }
}

void conditionCheck(const nodeType * p) {
    if (p->valueType == NONE) {
        cerr << "Invalid type of condition: None" << endl;
        exit(-1);
    }
}

void typeCheck(const oprNodeType &p) {
    if (p.nops == 1 and p.op[0]->valueType != INT) {
        cerr << "Invalid use of -" << " expected type: integer, actual type: " << reverseTypeLookup[p.op[0]->valueType] << endl;
        exit(-1);
    } else if (p.nops == 2 and applicable.count(p.oper) > 0){
        valueEnum left = p.op[0]->valueType;
        valueEnum right = p.op[1]->valueType;
        if (applicable[p.oper].count(pair<valueEnum, valueEnum>{left, right}) == 0) {
            cerr << "Invalid use of " << reverseSymbolLookup[p.oper] << " on "
                << reverseTypeLookup[p.op[0]->valueType] << " and " << reverseTypeLookup[p.op[1]->valueType] << endl;
            exit(-1);
        }
    }
}

void assign(idNodeType& id, valueEnum type) {
    int i = id.i;
    if (type == NONE) {
        cerr << "Assigning None to variable: " << reverseLookup[i] << endl;
        exit(-1);
    }
    if (id.global) {
        if (variableStack[0].count(i) == 0) {
            cerr << "Reference to undefined global variable: " << reverseLookup[i] << endl;
            exit(-1);
        }
        variableStack[0][i].second = type;
        printf("\tpop\tsb[%d]\n", variableStack[0][i].first);
    } else {
        if (TOP(variableStack).count(i) == 0) {
            int size = TOP(variableStack).size();
            TOP(variableStack)[i] = pair<int, valueEnum>{size, type};
        } else {
            TOP(variableStack)[i].second = type;
            if (variableStack.size() == 1) {
                printf("\tpop\tsb[%d]\n", variableStack[0][i].first);
            } else {
                printf("\tpop\tfb[%d]\n", TOP(variableStack)[i].first);
            }
        }
    }
}

int ex(nodeType *p, int blbl, int clbl) {
    int lblx, lbly, lblz;
    if (!p) return 0;
    switch(p->type) {
        case typeCon:
            switch (p->valueType) {
                case INT: {
                    printf("\tpush\t%d\n", p->con.iValue);
                    break;
                } case CHAR: {
                    printf("\tpush\t'%c'\n", p->con.cValue);
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
                p->valueType = variableStack[0][i].second;
                printf("\tpush\tsb[%d]\n", variableStack[0][i].first);
            } else {
                p->valueType = TOP(variableStack)[i].second;
                printf("\tpush\tfp[%d]\n", TOP(variableStack)[i].first);
            }
            break;
        }
        case typeFunc: {
            break;
        }
        case typeCall: {
            callNodeType& call = p->call;
            if (call.i < 9) {
                auto argument = call.arguments->front();                
                if (call.i < 3) {
                    if (argument->type != typeId) {
                        cerr << "Invalid call to " << reverseLookup[call.i] << endl;
                        exit(-1); 
                    }
                    cout << "\t" << reverseLookup[call.i] << "\n";
                    assign(argument->id, requiredType[call.i]);
                } else {
                    ex(argument, blbl, clbl);
                    typeCheck(argument, call);
                    cout << "\t" << reverseLookup[call.i] << "\n";
                }
            } else {
                ;
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
                    conditionCheck(p->opr.op[0]);
                    printf("\tj0\tL%03d\n", lbly);
                    ex(p->opr.op[1], lbly, lblx);
                    printf("\tjmp\tL%03d\n", lblx);
                    printf("L%03d:\n", lbly);
                    break;
                case IF:
                    ex(p->opr.op[0], blbl, clbl);
                    conditionCheck(p->opr.op[0]);
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
                    assign(p->opr.op[0]->id, p->opr.op[1]->valueType);
                    break;
                }
                case UMINUS:
                    ex(p->opr.op[0], blbl, clbl);
                    typeCheck(p->opr);
                    printf("\tneg\n");
                    break;
                default:
                    ex(p->opr.op[0], blbl, clbl);
                    ex(p->opr.op[1], blbl, clbl);
                    typeCheck(p->opr);
                    auto left = p->opr.op[0]->valueType;
                    auto right = p->opr.op[1]->valueType;
                    if (applicable.count(p->opr.oper) > 0 and applicable[p->opr.oper].count(pair<valueEnum, valueEnum>{left, right})) {
                        if (comparator.count(p->opr.oper) > 0) {
                            p->valueType = BOOL;
                        } else {
                            if (left != right) {
                                p->valueType = CHAR;
                            } else {
                                p->valueType = left;
                            }
                        }
                    }
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
    variableStack.push_back(map<int, pair<int, valueEnum>>{});
    reverseSymbolLookup = map<int, string>{{'+', "+"}, {'-', "-"}, {'*', "*"},
                                            {'/', "/"}, {'%', "%"}, {'<', "<"}, {'>', ">"}, {GE, ">="}, {LE, "<="},
                                            {EQ, "=="}, {NE, "!="}, {AND, "&&"}, {OR, "||"}, {UMINUS, "-"}};
    reverseTypeLookup = vector<string>{"None", "string", "character", "integer", "bool", "function"};
    applicable['+'] = set<pair<valueEnum, valueEnum> >{{INT, INT}, {INT, CHAR}, {CHAR, INT}};
    applicable['-'] = set<pair<valueEnum, valueEnum> >{{INT, INT}, {INT, CHAR}, {CHAR, INT}};
    applicable['*'] = set<pair<valueEnum, valueEnum> >{{INT, INT}};
    applicable['/'] = applicable['*'];
    applicable['%'] = applicable['*'];
    applicable[GE] = set<pair<valueEnum, valueEnum> >{{CHAR, CHAR}, {INT, INT}};
    applicable[LE] = applicable[GE];
    applicable['>'] = applicable[GE];
    applicable['<'] = applicable[GE];
    applicable[EQ] = applicable[GE];
    applicable[EQ].insert(pair<valueEnum, valueEnum>{BOOL, BOOL});
    applicable[NE] = applicable[EQ];
    applicable[AND] = set<pair<valueEnum, valueEnum> >{{BOOL, BOOL}};
    applicable[OR] = applicable[AND];
    comparator = set<int>{GE, LE, '<', '>', NE, EQ};
    requiredType = {STR, CHAR, INT, STR, CHAR, INT, STR, CHAR, INT};
    ex(p, 998, 998);
    ;
}