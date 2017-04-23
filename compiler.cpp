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


using id = int;
using scope = int;
using functionID = int;
using operatorID = int;

#define ADDSCOPE(node)\
    sList.push_back(++scopeCounter);\
    variableTable[scopeCounter] = map<id, valueEnum>();\
    preCheck(node, sList, functionBase, mode);\
    sList.pop_back();



constexpr scope GLOBAL = 0;
constexpr int VALUE_CHECK = 1;
constexpr int REFERENCE_CHECK = 2;

extern vector<string> reverseLookup;/* maps a variable identifier to a name*/

static set<operatorID> comparator;/*determine whether an operator is a comparator. If yes, the resulting type should be BOOL*/
static map<operatorID, string> reverseSymbolLookup; /*maps an operator to its name*/
static map<id, set<pair<valueEnum, valueEnum> > > applicable; /*maps an operator to the set of operands it can perform on.*/
static vector<string> reverseTypeLookup; /*maps an valueEnum value to its name.*/
static vector<valueEnum> requiredType; /*maps a variable identifier to the type of arguments. Used for built-in functions.*/

static scope scopeCounter;
static map<functionID, nodeType *> functionTable; /*record all functions defined*/
static map<scope, map<id, valueEnum> > variableTable; /*map the id of a function to the variableTable a function holds*/
static set<functionID> functionChecked;
static map<functionID, int> functionLabel;
static map<functionID, map<id, int> > addressTable;
int ex(nodeType*, int, int, functionID);

scope getDefinitionScope(id target, const vector<scope>& sList, int functionBase) {
    for (auto i = sList.size() - 1; i != functionBase - 1; --i) {
        if (variableTable[sList[i]].count(target) != 0) {
            return sList[i];
        }
    }
    return variableTable[GLOBAL].count(target) != 0? GLOBAL:-1;
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
    if (p->valueType == NONE or p->valueType == FUNC) {
        cerr << "Invalid type of condition: " << reverseTypeLookup[p->valueType] << endl;
        exit(-1);
    }
}

void typeCheck(const oprNodeType &p) {
    if (p.nops == 1 and p.op[0]->valueType != INT) {
        cerr << "Invalid use of -" << " expected type: integer, actual type: " << reverseTypeLookup[p.op[0]->valueType] << endl;
        exit(-1);
    } 
    if (p.nops == 2 and applicable.count(p.oper) > 0) {
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
        if (variableTable[GLOBAL][variable] != UNSET and variableTable[GLOBAL][variable] != type) {
            cerr << "Assigning a " << reverseTypeLookup[type] << " to a(n) " << reverseTypeLookup[variableTable[GLOBAL][variable]] << " variable" << endl;
            exit(-1);
        }
    } else {
        auto s = getDefinitionScope(variable, sList, functionBase);
        if (s == -1) {
            variableTable[sList.back()][variable] = type;
            if (addressTable[sList[functionBase]].count(variable) == 0) {
                auto size = addressTable[sList[functionBase]].size();
                if (sList[functionBase] != GLOBAL) {
                    size -= functionTable[sList[functionBase]]->func.parameters->size();
                }
                addressTable[sList[functionBase]][variable] = size;
            }
        } else if (variableTable[s][variable] != UNSET and variableTable[s][variable] != type){
            cerr << "Assigning a " << reverseTypeLookup[type] << " to a(n) " << reverseTypeLookup[variableTable[s][variable]] << " variable" << endl;
            exit(-1);
        }
    }
}

void defineFunctions() {
    for (const auto& i : functionTable) {
        if (functionLabel.count(i.first) != 0) {
            int tempVariables = int(addressTable[i.first].size() - i.second->func.parameters->size());
            printf("L%03d:\n", functionLabel[i.first]);        
            if (tempVariables != 0) {
                printf("\tpush\t%d\n", tempVariables);
                printf("\tpush\tsp\n");   
                printf("\tadd\n");
                printf("\tpop\tsp\n");
            }
            ex(i.second->func.stmts, 998, 998, i.first);
        }
    }
}

void preCheck(nodeType * const p, vector<scope>& sList, int functionBase, int mode) {
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
                        cerr << "Refer to a global variable: " << reverseLookup[p->id.i] << " in the global scope" << endl;
                        exit(-1);
                    }
                    p->valueType = variableTable[GLOBAL][p->id.i];
                } else {
                    auto s = getDefinitionScope(p->id.i, sList, functionBase);
                    if ((s == -1) || (s == 0 and sList[functionBase] != 0)) {
                        cerr << "Undefined reference to variable " << reverseLookup[p->id.i] << endl;
                        exit(-1);
                    }
                    p->valueType = variableTable[s][p->id.i];
                }
            } else {
                auto s = getDefinitionScope(p->id.i, sList, functionBase);
                p->valueType = variableTable[s][p->id.i];
            }
            break;
        } case typeFunc: {
            funcNodeType function = p->func;
            functionTable[function.i] = p;
            for (auto i = function.parameters->begin(); i != function.parameters->end(); ++i) {
                variableTable[function.i][(*i)->id.i] = UNSET;
            }
            break;
        } case typeCall: {
            functionID FID = p->call.i;

            callNodeType callNode = p->call;
            auto firstArgument = callNode.arguments->front();
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
                if (functionTable.count(FID) == 0) {
                    cerr << "Call to undefined function: " << reverseLookup[FID] << endl;
                    exit(-1);
                }
                funcNodeType target = functionTable[FID]->func;
                for (const auto& i: *callNode.arguments) {
                    preCheck(i, sList, functionBase, mode);
                }
                if (functionLabel.count(FID) == 0) {
                    functionLabel[FID] = lbl++;
                }
                if (mode == REFERENCE_CHECK and target.i != sList[functionBase]) {
                    variableTable[FID] = map<id, valueEnum>();
                    addressTable[target.i] = map<id, int>();
                }
                int size = 0;
                for (auto ai = callNode.arguments->begin(), pi = target.parameters->begin(); ai != callNode.arguments->end(); ++ai, ++pi) {
                    variableTable[target.i][(*pi)->id.i] = (*ai)->valueType;
                    addressTable[target.i][(*pi)->id.i] = - 3 + (size++) - target.parameters->size();
                }
                if (target.i != sList[functionBase]) {
                    sList.push_back(FID);
                    if (functionChecked.count(FID) != 0) {
                        mode = VALUE_CHECK;
                    }
                    preCheck(target.stmts, sList, sList.size() - 1, mode);
                    sList.pop_back();
                    if (mode == REFERENCE_CHECK) {
                        functionChecked.insert(FID);
                    }
                }
                p->valueType = functionTable[FID]->valueType;
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
                    nodeType * node = functionTable[sList[functionBase]];
                    if (p->opr.nops != 0) {
                        preCheck(p->opr.op[0], sList, functionBase, mode);
                    }
                    valueEnum t = p->opr.nops != 0 ? p->opr.op[0]->valueType : NONE;
                    node->valueType = t;
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
                        if (applicable.count(p->opr.oper) > 0 and applicable[p->opr.oper].count({left, right})) {
                            if (comparator.count(p->opr.oper) > 0) {
                                p->valueType = BOOL;
                            } else if (left != right) {
                                p->valueType = INT;
                            } else {
                                p->valueType = left;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}

void get(int i, functionID function, bool global=false) {
    if (global or function == GLOBAL) {
        printf("\tpush\tsb[%d]\n", addressTable[GLOBAL][i]);
    } else {
        printf("\tpush\tfp[%d]\n", addressTable[function][i]);
    }
}

void put(int i, functionID function, bool global=false) {
    if (global or function == GLOBAL) {
        printf("\tpop\tsb[%d]\n", addressTable[GLOBAL][i]);
    } else {
        printf("\tpop\tfp[%d]\n", addressTable[function][i]);
    }
}

int ex(nodeType *p, int blbl, int clbl, functionID function) {
    int lblx, lbly, lblz;
    if (!p) return 0;
    switch(p->type) {
        case typeCon:
            switch (p->valueType) {
                case INT: {
                    printf("\tpush\t%d\n", p->con.iValue);
                    break;
                } case CHAR: {
                    printf("\tpush\t%s\n", p->con.cValue->c_str());
                    break;
                } case STR: {
                    printf("\tpush\t%s\n", p->con.sValue->c_str());
                    break;
                }
            }
            break;
        case typeId: {
            get(p->id.i, function, p->id.global);
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
                    cout << "\t" << reverseLookup[call.i] << "\n";
                    put(argument->id.i, function, argument->id.global);
                } else {
                    ex(argument, blbl, clbl, function);
                    cout << "\t" << reverseLookup[call.i] << "\n";
                }
            } else {
                for (const auto& i: *p->call.arguments) {
                    ex(i, blbl, clbl, function);
                }
                printf("\tcall\tL%03d, %d\n", functionLabel[call.i], int(call.arguments->size()));
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
                    ex(p->opr.op[0], blbl, clbl, function);
                    printf("L%03d:\n", lblx);
                    ex(p->opr.op[1], blbl, clbl, function);
                    printf("\tj0\tL%03d\n", lbly);
                    ex(p->opr.op[3], lbly, lblz, function);
                    printf("L%03d:\n", lblz);
                    ex(p->opr.op[2], blbl, clbl, function);
                    printf("\tjmp\tL%03d\n", lblx);
                    printf("L%03d:\n", lbly);
                    break;
                case WHILE:
                    lblx = lbl++;
                    lbly = lbl++;
                    printf("L%03d:\n", lblx);
                    ex(p->opr.op[0], blbl, clbl, function);
                    printf("\tj0\tL%03d\n", lbly);
                    ex(p->opr.op[1], lbly, lblx, function);
                    printf("\tjmp\tL%03d\n", lblx);
                    printf("L%03d:\n", lbly);
                    break;
                case IF:
                    ex(p->opr.op[0], blbl, clbl, function);
                    if (p->opr.nops > 2) {
                        /* if else */
                        printf("\tj0\tL%03d\n", lblx = lbl++);
                        ex(p->opr.op[1], blbl, clbl, function);
                        printf("\tjmp\tL%03d\n", lbly = lbl++);
                        printf("L%03d:\n", lblx);
                        ex(p->opr.op[2], blbl, clbl, function);
                        printf("L%03d:\n", lbly);
                    } else {
                        /* if */
                        printf("\tj0\tL%03d\n", lblx = lbl++);
                        ex(p->opr.op[1], blbl, clbl, function);
                        printf("L%03d:\n", lblx);
                    }
                    break;
                case RETURN: {
                    if (p->opr.nops == 0) {
                        printf("\tpush\t1\n");
                    } else {
                        ex(p->opr.op[0], blbl, clbl, function);
                    }
                    printf("\tret\n");
                    break;
                }
                case '=': {
                    ex(p->opr.op[1], blbl, clbl, function);
                    put(p->opr.op[0]->id.i, function, p->opr.op[0]->id.global);
                    break;
                }
                case UMINUS:
                    ex(p->opr.op[0], blbl, clbl, function);
                    printf("\tneg\n");
                    break;
                default:
                    ex(p->opr.op[0], blbl, clbl, function);
                    ex(p->opr.op[1], blbl, clbl, function);
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
    variableTable[GLOBAL] = map<int, valueEnum>{};
    reverseSymbolLookup = map<int, string>{{'+', "+"}, {'-', "-"}, {'*', "*"},
                                            {'/', "/"}, {'%', "%"}, {'<', "<"}, {'>', ">"}, {GE, ">="}, {LE, "<="},
                                            {EQ, "=="}, {NE, "!="}, {AND, "&&"}, {OR, "||"}, {UMINUS, "-"}};
    reverseTypeLookup = vector<string>{"unset", "None", "string", "character", "integer", "bool", "function"};
    for (const auto& i : {'+', '-'}) {
        applicable[i] = set<pair<valueEnum, valueEnum> >{{INT, INT}, {INT, CHAR}, {CHAR, INT}};
    }
    for (const auto& i : {'*', '/', '%'}) {
        applicable[i] = set<pair<valueEnum, valueEnum> >{{INT, INT}};
    }
    for (const auto& i : vector<int>{GE, LE, '<', '>', EQ, NE}) {
        applicable[i] = set<pair<valueEnum, valueEnum> >{{CHAR, CHAR}, {INT, INT}};
    }
    for (const auto& i : {EQ, NE}) {
        applicable[i].insert(pair<valueEnum, valueEnum>{BOOL, BOOL});
    }
    for (const auto& i : {AND, OR}) {
        applicable[i] = set<pair<valueEnum, valueEnum> >{{BOOL, BOOL}};
    }
    comparator = set<int>{GE, LE, '<', '>', NE, EQ};
    requiredType = {STR, CHAR, INT, STR, CHAR, INT, STR, CHAR, INT};
    
    scopeCounter = 1;
    vector<scope> sList{GLOBAL};
    preCheck(p, sList, GLOBAL, REFERENCE_CHECK);
    if (addressTable[GLOBAL].size() != 0) {
        printf("\tpush\tsp\n");
        printf("\tpush\t%d\n", int(addressTable[GLOBAL].size()));
        printf("\tadd\n");
        printf("\tpop\tsp\n");
    }
    ex(p, 998, 998, GLOBAL);
    printf("\tend\n");
    defineFunctions();
}