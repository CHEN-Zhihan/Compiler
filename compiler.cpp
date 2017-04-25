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
    variableTable[scopeCounter] = set<id>();\
    functionTable[scopeCounter] = map<functionID, nodeType *>();\
    preCheck(node, sList, functionBase);\
    sList.pop_back();



constexpr scope GLOBAL = 0;
constexpr int VALUE_CHECK = 1;
constexpr int REFERENCE_CHECK = 2;

extern vector<string> reverseLookup;/* maps a variable identifier to a name*/
map<scope, map<functionID, nodeType *> > functionTable;
static map<scope, set<id> > variableTable;
/*
static set<operatorID> comparator;/*determine whether an operator is a comparator. If yes, the resulting type should be BOOL
static map<operatorID, string> reverseSymbolLookup; /*maps an operator to its name
static map<id, set<pair<valueEnum, valueEnum> > > applicable; /*maps an operator to the set of operands it can perform on.
static vector<string> reverseTypeLookup; /*maps an valueEnum value to its name.
static vector<valueEnum> requiredType; /*maps a variable identifier to the type of arguments. Used for built-in functions.*/

static scope scopeCounter;
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
/*
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

*/

void preAssign(idNodeType& node, const vector<scope>& sList, int functionBase, bool function=false) {
    int variable = node.i;
    cout << functionTable[GLOBAL].size() << endl;
    if (functionTable[GLOBAL].count(variable) != 0) {
        auto temp = functionTable[GLOBAL][variable];
        if (!temp or temp->id.function.origin == temp->id.i) {
            cerr << "Redefinition of function " << reverseLookup[variable] << endl;
            exit(-1);
        }
    }
    if (node.type == VAR and node.global) {
        if (variableTable[GLOBAL].count(variable) == 0) {
            cerr << "Reference to undefined global variable: " << reverseLookup[variable] << endl;
            exit(-1);
        }
    } else {
        auto s = getDefinitionScope(variable, sList, functionBase);
        if (s == -1) {
            if (node.type == VAR and addressTable[sList[functionBase]].count(variable) == 0) {
                auto size = addressTable[sList[functionBase]].size();
                if (sList[functionBase] != GLOBAL) {
                    size -= functionTable[GLOBAL][sList[functionBase]]->id.function.parameters->size();
                }
                if (!function) {
                    addressTable[sList[functionBase]][variable] = size;
                }
                variableTable[sList.back()].insert(variable);
                cout << "add variable " << variable << " to " << sList.back() << endl;
            } //else if (node.type == ) // TODO
        }
    }
}


void defineFunctions() {
    for (const auto& FID : functionLabel) {
        nodeType * func = functionTable[GLOBAL][FID.first];
        func = functionTable[GLOBAL][func->id.function.origin];
        int tempVariables = int(variableTable[FID.first].size() - func->id.function.parameters->size());
        printf("L%03d:\n", functionLabel[FID.first]);
        if (tempVariables != 0) {
            printf("\tpush\t%d\n", tempVariables);
            printf("\tpush\tsp\n");   
            printf("\tadd\n");
            printf("\tpop\tsp\n");
        }
//        ex(func->id.function.stmts, 998, 998, FID);
    }
}

void preCheck(nodeType * & p, vector<scope>& sList, int functionBase) {
    if (p == nullptr) {
        return;
    }
    switch (p->node) {
        case nodeCon: {
            break;
        } case nodeId: {
            idNodeType& ID = p->id;
            auto VID = ID.i;
            switch (ID.type) {
                case FUNCTION: {
                    if (sList.back() != GLOBAL) {
                        cerr << "Function " << reverseLookup[VID] << " can only be defined in the global scope" << endl;
                        exit(-1);
                    }
                    if (variableTable.count(ID.i) == 0) {
                        variableTable[ID.i] = set<int>();
                        addressTable[ID.i] = map<id, int>();
                        int size = 0;
                        for (auto i = ID.function.parameters->begin(); i != ID.function.parameters->end(); ++i) {
                            if (functionTable[GLOBAL].count((*i)->id.i) != 0) {
                                cerr << "Redefinition of function: " << reverseLookup[(*i)->id.i] << endl;
                                exit(-1);
                            }
                            variableTable[ID.i].insert((*i)->id.i);
                            cout << "add variable " << (*i)->id.i << " to " << ID.i << endl;
                            addressTable[ID.i][(*i)->id.i] = - 3 + (size++) - ID.function.parameters->size();
                        }
                        functionTable[GLOBAL][ID.i] = p;
                        cout << "adding " << ID.i << " to " << GLOBAL << endl;
                        variableTable[GLOBAL].insert(ID.i);
                    }
                    break;
                } case VAR: {
                    if (ID.global) {
                        if (variableTable[GLOBAL].count(ID.i) == 0) {
                            cerr << "Undefined reference to global variable: " << reverseLookup[ID.i] << endl;
                            exit(-1);
                        }
                        if (sList.back() == GLOBAL) {
                            cerr << "Refer to a global variable: " << reverseLookup[ID.i] << " in the global scope" << endl;
                            exit(-1);
                        }
                    } else {
                            auto s = getDefinitionScope(ID.i, sList, functionBase);
                        if ((s == -1) || (s == 0 and sList[functionBase] != 0)) {
                            cerr << "Undefined reference to variable " << reverseLookup[p->id.i] << endl;
                            exit(-1);
                        }
                    }
                    break;
                } case CALL: {
                    auto FID = p->id.i;
                    if (FID < 9) {
                        if (ID.call.arguments->size() != 1) {
                            cerr << reverseLookup[FID] << " expects 1 argument, got " << ID.call.arguments->size() << endl;
                            exit(-1); 
                        }
                        auto first = ID.call.arguments->front();                    
                        if (FID < 3) {
                            if (first->node != nodeId) {
                                cerr << reverseLookup[FID] << " got invalid argument" << endl;
                                exit(-1);
                            }
                            preAssign(first->id, sList, functionBase);
                        } else {
                            preCheck(first, sList, functionBase);
                        }
                    } else {
                        scope s = getDefinitionScope(FID, sList, functionBase);
                        if (s == -1) {
                            cerr << "Call to undefined function: " << reverseLookup[FID] << endl;
                            exit(-1);
                        }
                        if (functionTable[s].count(FID) == 0) {
                            cerr << "Call to undefined function: " << reverseLookup[FID] << endl;
                            exit(-1);
                        }
                        auto target = functionTable[s][FID];
                        if (target->id.i != target->id.function.origin and s == GLOBAL) {
                            cerr << "Call to undefined function: " << reverseLookup[FID] << endl;
                            exit(-1);
                        }
                        nodeType* temp = new nodeType(*target);
                        cout << temp->id.function.arguments->size() << endl;
                        if (temp->id.function.arguments->size() != temp->id.function.parameters->size()) {
                            for (auto k = p->id.call.arguments->rbegin(); k != p->id.call.arguments->rend(); k++) {
                                temp->id.function.arguments->push_front(*k);
                            //    ID.call.arguments->push_back(nullptr);
                            }
                        }
                        if (temp->id.function.parameters->size() < temp->id.function.arguments->size()) {
                            cerr << reverseLookup[ID.i] << " expects " << temp->id.function.parameters->size() -  temp->id.function.arguments->size()
                             +p->id.call.arguments->size() << " argument(s), got " << p->id.call.arguments->size() << endl;
                            exit(-1);
                        }
                        for (auto & i : *ID.call.arguments) {
                            preCheck(i, sList, functionBase);
                        }

                        if (temp->id.function.parameters->size() == temp->id.function.arguments->size()) {
                            if (functionLabel.count(temp->id.function.origin) == 0) {
                                functionLabel[temp->id.function.origin] = lbl++;
                            }
                            if (temp->id.i != sList[functionBase] and functionChecked.count(temp->id.i) == 0) {
                                sList.push_back(temp->id.function.origin);
                                preCheck(temp->id.function.stmts, sList, sList.size() - 1);
                                sList.pop_back();
                                functionChecked.insert(temp->id.function.origin);
                            }
                        } else if (ID.call.arguments->size() != 0) {
                            p = temp;
                        } else {
                            cerr << reverseLookup[ID.i] << " expects " << temp->id.function.parameters->size() - temp->id.function.arguments->size()
                             << " argument(s), got " << ID.call.arguments->size() << endl;
                            exit(-1);
                        }
                    }
                }
            }
            break;
        } case nodeOpr: {
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
                    nodeType * node = functionTable[GLOBAL][sList[functionBase]];
                    if (p->opr.nops != 0) {
                        preCheck(p->opr.op[0], sList, functionBase);
                    }
                    break;
                } case UMINUS: {
                    preCheck(p->opr.op[0], sList, functionBase);
                    break;
                } case FOR: {
                    sList.push_back(scopeCounter++);
                    preCheck(p->opr.op[0], sList, functionBase);
   //                 conditionCheck(p->opr.op[0]);
                    preCheck(p->opr.op[1], sList, functionBase);
                    preCheck(p->opr.op[3], sList, functionBase);
                    preCheck(p->opr.op[2], sList, functionBase);
                    sList.pop_back();
                    break;
                } case WHILE: {
                    preCheck(p->opr.op[0], sList, functionBase);
  //                  conditionCheck(p->opr.op[0]);
                    ADDSCOPE(p->opr.op[1]);
                    break;
                } case IF: {
                    preCheck(p->opr.op[0], sList, functionBase);
  //                  conditionCheck(p->opr.op[0]);
                    ADDSCOPE(p->opr.op[1]);
                    if (p->opr.nops > 2) {
                        ADDSCOPE(p->opr.op[2]);
                    }
                    break;
                } case '=': {
                    preCheck(p->opr.op[1], sList, functionBase);
                    if (p->opr.op[1]->node == nodeCon or p->opr.op[1]->node == nodeOpr) {
                        preAssign(p->opr.op[0]->id, sList, functionBase);
                    } else {
                        preAssign(p->opr.op[0]->id, sList, functionBase);
                        scope s = getDefinitionScope(p->opr.op[1]->id.i, sList, functionBase);
                        if (p->opr.op[1]->id.type == FUNCTION) {
                            p->opr.op[0]->id.type = p->opr.op[1]->id.type;                        
                            p->opr.op[0]->id.function = p->opr.op[1]->id.function;
                            functionTable[sList.back()][p->opr.op[0]->id.i] = p->opr.op[0];
                            cout << "adding " << p->opr.op[0]->id.i << " to " << sList.back() << endl;
                        }
                    }
                    break;
                } default: {
                    if (p->opr.nops == 2 and p->opr.op[0] != nullptr and p->opr.op[1] != nullptr) {
                        preCheck(p->opr.op[0], sList, functionBase);
                        preCheck(p->opr.op[1], sList, functionBase);
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
/*
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
                        /* if else *\/
                        printf("\tj0\tL%03d\n", lblx = lbl++);
                        ex(p->opr.op[1], blbl, clbl, function);
                        printf("\tjmp\tL%03d\n", lbly = lbl++);
                        printf("L%03d:\n", lblx);
                        ex(p->opr.op[2], blbl, clbl, function);
                        printf("L%03d:\n", lbly);
                    } else {
                        /* if *\/
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
*/
void run(nodeType *p) {
    variableTable[GLOBAL] = set<id>{};
    functionTable[GLOBAL] = map<functionID, nodeType*>();    
    for (auto i = 0; i != 9; ++i) {
        functionTable[GLOBAL][i] = nullptr;
    }
/*    reverseSymbolLookup = map<int, string>{{'+', "+"}, {'-', "-"}, {'*', "*"},
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
    */
    scopeCounter = 1;
    vector<scope> sList{GLOBAL};
    for (auto i = 0; i != reverseLookup.size(); ++i) {
        cout << i << reverseLookup[i] << endl;
    }
    preCheck(p, sList, GLOBAL);
    if (addressTable[GLOBAL].size() != 0) {
        printf("\tpush\tsp\n");
        printf("\tpush\t%d\n", int(addressTable[GLOBAL].size()));
        printf("\tadd\n");
        printf("\tpop\tsp\n");
    }
 //   ex(p, 998, 998, GLOBAL);
    printf("\tend\n");
    defineFunctions();
}