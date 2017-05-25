#include <iostream>
using std::cout;
using std::endl;
using std::printf;
using std::cerr;
#include <string>
using std::string;
#include <list>
using std::list;

#include <vector>
using std::vector;
using std::pair;
using std::tuple;

#include <unordered_map>
using std::unordered_map;

#include <unordered_set>
using std::unordered_set;

#include "Node.h"
#include "parser.tab.h"

using scope = int;

extern int GLOBAL;

#define DEBUG false

extern vector<string> reverseLookup; /* maps a variable identifier to a name*/
unordered_map<int, const FunctionNode *>
    functionTable; /* maps a function ID to the node it corresponds to*/
unordered_map<int, unordered_set<int> > variableTable; /*maps a scope to the set of visible variables*/
unordered_map<int, string>
    operatorInstruction; /*maps an operator to the instruction to be printed*/
unordered_map<int, int>
    functionLabel; /*maps a function id to the label number*/
unordered_map<int, pair<int, unordered_map<int, int>>>
    addressTable; /*maps a function to an address table it holds for
                     local/global variables*/

/*
define all functions called at the end of code.
*/
void defineFunctions() {
    for (const auto &FID : functionLabel) {
        auto func = functionTable[FID.first];
        int tempVariables =
            int(addressTable[FID.first].first - func->getNumParameters());
        printf("L%03d:\n", FID.second);
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
    operatorInstruction = unordered_map<int, string>{
        {'+', "add"},   {'-', "sub"},    {'*', "mul"},    {'/', "div"},
        {'%', "mod"},   {'<', "compLT"}, {'>', "compGT"}, {GE, "compGE"},
        {LE, "compLE"}, {NE, "compNE"},  {EQ, "compEQ"},  {AND, "and"},
        {OR, "or"}};
    variableTable[GLOBAL] = unordered_set<int>{};
    functionTable = unordered_map<int, const FunctionNode *>();
    for (auto i = 0; i != 9; ++i) {
        variableTable[GLOBAL].insert(i);
        functionTable[i] = nullptr;
    }
    vector<scope> sList{GLOBAL};
#if DEBUG
    for (auto i = 0; i != reverseLookup.size(); ++i) {
        cerr << i << reverseLookup[i] << endl;
    }
#endif
    p->check(sList, GLOBAL);

    if (addressTable[GLOBAL].first != 0) {
        printf("\tpush\tsp\n");
        printf("\tpush\t%d\n", addressTable[GLOBAL].first);
        printf("\tadd\n");
        printf("\tpop\tsp\n");
    }
    p->ex(GLOBAL, 998, 998);
    printf("\tend\n");
    defineFunctions();
}
