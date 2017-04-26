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

#include "Node.h"
#include "parser.tab.h"

int lbl;


using id = int;
using scope = int;
using functionID = int;
using operatorID = int;

scope GLOBAL = 0;
constexpr int VALUE_CHECK = 1;
constexpr int REFERENCE_CHECK = 2;

#define DEBUG true

extern vector<string> reverseLookup;/* maps a variable identifier to a name*/
map<functionID,  const FunctionNode* >functionTable;
map<int, set<int> > variableTable;
map<int, string> operatorInstruction;
/*
static set<operatorID> comparator;/*determine whether an operator is a comparator. If yes, the resulting type should be BOOL
static map<operatorID, string> reverseSymbolLookup; /*maps an operator to its name
static map<id, set<pair<valueEnum, valueEnum> > > applicable; /*maps an operator to the set of operands it can perform on.
static vector<string> reverseTypeLookup; /*maps an valueEnum value to its name.
static vector<valueEnum> requiredType; /*maps a variable identifier to the type of arguments. Used for built-in functions.*/

scope scopeCounter;
map<functionID, int> functionLabel;
map<functionID, map<id, int> > addressTable;


void defineFunctions() {
    for (const auto& FID : functionLabel) {
        auto func = functionTable[FID.first];
        int tempVariables = int(variableTable[FID.first].size() - func->getNumParameters());
        printf("L%03d:\n", functionLabel[FID.first]);
        if (tempVariables != 0) {
            printf("\tpush\t%d\n", tempVariables);
            printf("\tpush\tsp\n");   
            printf("\tadd\n");
            printf("\tpop\tsp\n");
        }
        func->exStmt(998, 998, FID.first);
        printf("\tpush\t0\n");
        printf("\tret\n");
    }
}

void run(shared_ptr<Node> p) {
    operatorInstruction = map<int, string> {{'+', "add"}, {'-', "sub"}, {'*', "mul"}, 
                                            {'/', "div"}, {'%', "mod"}, {'<', "compLT"},
                                            {'>', "compGT"}, {GE, "compGE"}, {LE, "compLE"},
                                            {NE, "compNE"}, {EQ, "compEQ"}, {AND, "and"},
                                            {OR, "or"}};
    variableTable[GLOBAL] = set<id>{};
    functionTable = map<int, const FunctionNode* >();    
    for (auto i = 0; i != 9; ++i) {
        variableTable[GLOBAL].insert(i);
    }
    scopeCounter = 1;
    vector<scope> sList{GLOBAL};
    #if DEBUG
        for (auto i = 0; i != reverseLookup.size(); ++i) {
            cerr << i << reverseLookup[i] << endl;
        }
    #endif
    p->check(sList, GLOBAL);

    if (addressTable[GLOBAL].size() != 0) {
        printf("\tpush\tsp\n");
        printf("\tpush\t%d\n", int(addressTable[GLOBAL].size()));
        printf("\tadd\n");
        printf("\tpop\tsp\n");
    }
    p->ex(998, 998, GLOBAL);
    printf("\tend\n");
    defineFunctions();
}

