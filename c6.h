#ifndef C6_H
#define C6_H

#include <list>
using std::list;

#include <string>
using std::string;

enum nodeEnum{ typeCon, typeId, typeOpr, typeFunc, typeCall };


enum idType {
    str,
    character,
    integer,
    function
};

struct nodeType;
enum constType {
    STR,
    CHAR,
    INT
};

/* constants */
struct conNodeType {
    constType type;
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
    idType type;
};

struct funcNodeType {
    int i;
    list<nodeType *> * parameters;
    list<nodeType *> * arguments;
    nodeType * stmts;
};

struct callNodeType {
    
}

/* operators */
struct oprNodeType {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    idType resultType;
    nodeType *op[1];  /* operands (expandable) */
};


struct nodeType {
    nodeEnum type;              /* type of node */

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