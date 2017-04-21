#ifndef C6_H
#define C6_H

#include <list>
using std::list;

#include <string>
using std::string;

enum nodeEnum{ typeCon, typeId, typeOpr, typeFunc, typeCall };


enum valueEnum {
    NONE,
    STR,
    CHAR,
    INT,
    BOOL,
    FUNC
};

struct nodeType;


/* constants */
struct conNodeType {
    union {
        int iValue;
        char cValue;
        const char * sValue;
    };
};


/* identifiers */
struct idNodeType{
    int i;                      /* variable identifier */
    bool global;
};

struct funcNodeType {
    int i;
    list<nodeType *> * parameters;
    nodeType * stmts;
};

struct callNodeType {
    int i;
    list<nodeType *> * arguments;
};

/* operators */
struct oprNodeType {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    nodeType *op[1];  /* operands (expandable) */
};


struct nodeType {
    nodeEnum type;              /* type of node */
    valueEnum valueType;
    /* union must be last entry in nodeType */
    /* because operNodeType may dynamically increase */
    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        funcNodeType func;      /* functions */        
        oprNodeType opr;        /* operators */
        callNodeType call;      
    };
};

#endif // C6_H