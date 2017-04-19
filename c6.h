#ifndef C6_H
#define C6_H

#include <list>
using std::list;

#include <string>
using std::string;

enum nodeEnum{ typeCon, typeId, typeOpr, typeFunc };


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
    string * name;                      /* variable identifier */
    bool global;
    idType type;
};

struct funcNodeType {
    string * name;
    list<nodeType *> * parameters;
    nodeType * stmts;
};

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
    };
};

#endif // C6_H