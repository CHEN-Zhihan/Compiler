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


using position = int;
using id = int;
using scope = int;
using functionID = int;
using operatorID = int;

#define ADDSCOPE(node)\
    sList.push_back(++scopeCounter);\
    functionTable[scopeCounter] = map<functionID,nodeType*>();\
    variableTable[scopeCounter] = map<id, pair<position, valueEnum> >();\
    callTable[scopeCounter] = set<functionID>();\
    preCheck(node, sList, functionBase, mode);\
    sList.pop_back();


constexpr scope GLOBAL = 0;
constexpr int VALUE_CHECK = 1;
constexpr int REFERENCE_CHECK = 2;

extern vector<string> reverseLookup;/* maps a variable identifier to a name*/
extern int variableCounter;

static set<operatorID> comparator;/*determine whether an operator is a comparator. If yes, the resulting type should be BOOL*/
static map<operatorID, string> reverseSymbolLookup; /*maps an operator to its name*/
static map<id, set<pair<valueEnum, valueEnum> > > applicable; /*maps an operator to the set of operands it can perform on.*/
static vector<string> reverseTypeLookup; /*maps an valueEnum value to its name.*/
static vector<valueEnum> requiredType; /*maps a variable identifier to the type of arguments. Used for built-in functions.*/

static scope scopeCounter;
static map<scope, map<functionID, nodeType*> >functionTable; /*records all functions available in a specific function scope*/
static map<scope, set<functionID> > callTable; /*maps the ids of function called in a specific scope*/
static map<scope, map<id, pair<position, valueEnum> > > variableTable; /*map the id of a function to the variableTable a function holds*/
static map<scope, int> variableNumber;
static set<functionID> functionChecked;

void typeCheck(const nodeType * p, const callNodeType & c) {
    if (c.i >= 3 and c.i < 9 and p->valueType != requiredType[c.i]) {
        cerr << "Invalid call to function: " << reverseLookup[c.i]
            << " expect type: " << reverseTypeLookup[requiredType[c.i]]
            << " actual type: " << reverseTypeLookup[p->valueType] << endl;
        exit(-1);
    }
}

void conditionCheck(const nodeType * p) {
    if (p->valueType == NONE or p->valueType == FUNC) {
        cerr << "Invalid type of condition: " << reverseTypeLookup[p->valueType] << endl;
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



void preAssign(idNodeType node, valueEnum type, const vector<scope>& sList, int functionBase) {
    int variable = node.i;
    if (type == NONE) {
        cerr << "Assigning None to variable: " << reverseLookup[variable] << endl;
        exit(-1);
    }
    if (node.global) {
        if (variableTable[GLOBAL].count(variable) == 0) {
            cerr << "Reference to undefined global variable: " << reverseLookup[variable] << endl;
            exit(-1);
        }
        variableTable[GLOBAL][variable].second = type;
    } else {
        bool found = false;
        for (int i = sList.size() - 1; i != functionBase - 1; --i) {
            if (variableTable[sList[i]].count(variable) != 0) {
                variableTable[sList[i]][variable].second = type;
                found = true;
                break;
            }
        }
        if (!found) {
            if (functionTable[GLOBAL].count(variable) != 0) {
                cerr << "assigning value to a global function: " << reverseLookup[variable] <<  endl;
                exit(-1);
            }
            auto size = variableTable[sList.back()].size();
            variableTable[sList.back()][variable] = pair<int, valueEnum>{size, type};
        }
    }
}

void defineFunctions() {

}

void preCheck(nodeType *&p, vector<scope>& sList, int functionBase, int mode) {
    if (p == nullptr) {
        return;
    }
    switch (p->type) {
        case typeCon: {
            break;
        } case typeId: {
            if (mode == REFERENCE_CHECK) {
                if (p->id.global) {
                    if (variableTable[GLOBAL].count(p->id.i) == 0) {
                        cerr << "Undefined reference to global variable: " << reverseLookup[p->id.i] << endl;
                        exit(-1);
                    }
                    if (sList.back() == GLOBAL) {
                        cerr << "Refering to global variable: " << reverseLookup[p->id.i] << " in the global scope" << endl;
                        exit(-1);
                    }
                    p->valueType = variableTable[GLOBAL][p->id.i].second;
                } else {
                    int found = false;
                    scope s;
                    for (int i = sList.size() - 1; i != functionBase - 1; --i) {
                        if (variableTable[sList[i]].count(p->id.i) != 0) {
                            p->valueType = variableTable[sList[i]][p->id.i].second;
                            s = sList[i];
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        if (functionTable[GLOBAL].count(p->id.i) != 0) {
                            p->valueType = variableTable[GLOBAL][p->id.i].second;
                            s = GLOBAL;
                        } else {
                            cerr << "Undefined reference to variable: " << reverseLookup[p->id.i] << endl;
                            exit(-1);
                        }
                    }
                }
            }
            break;
        } case typeFunc: {
            funcNodeType function = p->func;
            if (mode == REFERENCE_CHECK) {
                variableTable[sList.back()][function.i] = pair<int, valueEnum>{-1, FUNC};
                functionTable[sList.back()][function.i] = p;
                variableTable[function.i] = map<scope, pair<int, valueEnum>>();
            }
            int counter = 0;
            for (auto i = function.parameters->begin(); i != function.parameters->end(); ++i) {
                variableTable[function.i][(*i)->id.i] = pair<int, valueEnum>{counter++, UNSET};
            }
            break;
        } case typeCall: {
            functionID FID = p->call.i;
            callNodeType callNode = p->call;
            auto& firstArgument = callNode.arguments->front();
            if (FID < 9) {
                if (callNode.arguments->size() != 1 or (FID < 3 and firstArgument->type != typeId)) {
                    cerr << "Invalid call to " << reverseLookup[FID] << endl;
                    exit(-1);
                }
                if (FID >= 3) {
                    preCheck(firstArgument, sList, functionBase, mode);
                    typeCheck(firstArgument, callNode);
                } else {
                    preAssign(firstArgument->id, requiredType[FID], sList, functionBase);
                }
            } else {
                nodeType* target = nullptr;
                scope scopeDefined = -1;
                for (int i = sList.size() - 1; i != functionBase - 1; --i) {
                    if (functionTable[sList[i]].count(FID) != 0) {
                        target = functionTable[sList[i]][FID];
                        scopeDefined = sList[i];
                        break;
                    }
                }
                if (target == nullptr) {
                    if (functionTable[GLOBAL].count(FID) == 0) {
                        cerr << "Call to undefined function: " << reverseLookup[FID] << endl;
                        exit(-1);
                    }
                    target = functionTable[GLOBAL][FID];
                    scopeDefined = GLOBAL;
                }
                for (auto& i: *callNode.arguments) {
                    preCheck(i, sList, functionBase, mode);
                }
                if (target->func.parameters->size() != callNode.arguments->size()) {
                    cerr << "Incorrect argument number for function " << reverseLookup[callNode.i]
                        << " expect " << target->func.parameters->size() << ", got " << callNode.arguments->size() << endl;
                    exit(-1);
                } else {
                    cout << "calling " << reverseLookup[FID] << endl;
                    sList.push_back(target->func.i);
                    if (mode == REFERENCE_CHECK) {
                        variableTable[target->func.i] = map<id, pair<int, valueEnum> >();
                        int counter = 0;
                        for (auto pi = target->func.parameters->begin(); pi != target->func.parameters->end(); ++pi) {
                            if (functionTable[GLOBAL].count((*pi)->id.i) != 0) {
                                cerr << "passing global function " << reverseLookup[(*pi)->id.i] << " as argument" << endl;
                                exit(-1);
                            }
                            variableTable[target->func.i][(*pi)->id.i] = pair<int, valueEnum>{counter++, UNSET};
                        }
                    }
                    for (auto ai = callNode.arguments->begin(), pi = target->func.parameters->begin(); ai != callNode.arguments->end(); ++ai, ++pi) {
                        variableTable[target->func.i][(*pi)->id.i].second = (*ai)->valueType;
                    }
                    if (target->func.i != sList[functionBase]) {
                        preCheck(target->func.stmts, sList, sList.size() - 1, mode);
                        if (mode == REFERENCE_CHECK) {
                            functionChecked.insert(FID);                        
                        }
                    }
                    sList.pop_back();
                    p->valueType = target->valueType;
                }
            }
            break;
        } case typeOpr: {
            switch (p->opr.oper) {
                case BREAK: {
                    break;
                } case CONTINUE: {
                    break;
                } case RETURN: {
                    if (functionBase == 0) {
                        cerr << "return occurs in the global scope" << endl;
                        exit(-1);
                    }
                    scope s = 0;
                    for (scope i = functionBase - 1; i != -1; --i) {
                        if (functionTable[sList[i]].count(sList[functionBase]) > 0) {
                            s = sList[i];
                            break;
                        }
                    }
                    nodeType * node = functionTable[s][sList[functionBase]];                    
                    if (p->opr.nops != 0) {
                        preCheck(p->opr.op[0], sList, functionBase, mode);
                        if (node->valueType != UNSET and node->valueType != p->opr.op[0]->valueType) {
                            cerr << "returning multiple types from function " << reverseLookup[sList[functionBase]] << endl;
                            exit(-1);
                        }
                        node->valueType = p->opr.op[0]->valueType;
                    } else {
                        node->valueType = NONE;
                    }
                    break;
                } case UMINUS: {
                    preCheck(p->opr.op[0], sList, functionBase, mode);
                    typeCheck(p->opr);
                    break;
                } case FOR: {
                    sList.push_back(scopeCounter++);
                    preCheck(p->opr.op[0], sList, functionBase, mode);
                    conditionCheck(p->opr.op[0]);
                    preCheck(p->opr.op[1], sList, functionBase, mode);
                    preCheck(p->opr.op[3], sList, functionBase, mode);
                    preCheck(p->opr.op[2], sList, functionBase, mode);
                    sList.pop_back();
                    break;
                } case WHILE: {
                    preCheck(p->opr.op[0], sList, functionBase, mode);
                    conditionCheck(p->opr.op[0]);
                    ADDSCOPE(p->opr.op[1]);
                    break;
                } case IF: {
                    preCheck(p->opr.op[0], sList, functionBase, mode);
                    conditionCheck(p->opr.op[0]);
                    ADDSCOPE(p->opr.op[1]);
                    if (p->opr.nops > 2) {
                        ADDSCOPE(p->opr.op[2]);
                    }
                    break;
                } case '=': {
                    preCheck(p->opr.op[1], sList, functionBase, mode);
                    preAssign(p->opr.op[0]->id, p->opr.op[1]->valueType, sList, functionBase);
                } default: {
                    if (p->opr.op[0] != nullptr and p->opr.op[1] != nullptr) {
                        preCheck(p->opr.op[0], sList, functionBase, mode);
                        preCheck(p->opr.op[1], sList, functionBase, mode);
                        typeCheck(p->opr);
                        auto left = p->opr.op[0]->valueType;
                        auto right = p->opr.op[1]->valueType;
                        cout << reverseSymbolLookup[p->opr.oper] << " " << reverseTypeLookup[left] << " " << reverseTypeLookup[right] << endl;
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
                    }

                    break;
                }
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
                    printf("\tpush\t%s\n", p->con.sValue->c_str());
                    break;
                }
            }
            break;
        case typeId: {
            int i = p->id.i;
            if (p->id.global or variableTable.size() == 1) {
                p->valueType = variableTable[0][i].second;
                printf("\tpush\tsb[%d]\n", variableTable[0][i].first);
            } else {
            }
            break;
        }
        case typeFunc: {
            lblx = lbl++;
            printf("L%03d:\n", lblx);
            for (int i = p->func.parameters->size() - 1; i != -1; --i) {
                printf("\tpush\tfp[%d]\n", -3 - i);
            }
            ex(p->func.stmts, blbl, clbl);
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
                } else {
                    ex(argument, blbl, clbl);
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
                case RETURN: {
                    break;
                }
                case '=': {
                    ex(p->opr.op[1], blbl, clbl);
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


void run(nodeType *p) {
    variableTable[GLOBAL] = (map<int, pair<int, valueEnum>>{});
    reverseSymbolLookup = map<int, string>{{'+', "+"}, {'-', "-"}, {'*', "*"},
                                            {'/', "/"}, {'%', "%"}, {'<', "<"}, {'>', ">"}, {GE, ">="}, {LE, "<="},
                                            {EQ, "=="}, {NE, "!="}, {AND, "&&"}, {OR, "||"}, {UMINUS, "-"}};
    reverseTypeLookup = vector<string>{"unset", "None", "string", "character", "integer", "bool", "function"};
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
    vector<scope> sList{GLOBAL};
    preCheck(p, sList, GLOBAL, REFERENCE_CHECK);
    cout << "preCheck succeeded" << endl;
    printf("\tjmp\tL999\n");
    defineFunctions();
    printf("L999:\n");
}