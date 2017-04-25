#ifndef C6_H
#define C6_H

#include <list>
using std::list;

#include <string>
using std::string;

#include <vector>
using std::vector;

enum nodeEnum{nodeCon, nodeId, nodeOpr};

enum typeEnum{
    STR,
    CHAR,
    INT
};

enum idEnum {
    VAR,
    FUNCTION,
    CALL
};


struct nodeType;


/* constants */
struct conNodeType {
    typeEnum conType;
    conNodeType(const conNodeType& another) {
        conType = another.conType;
        if (conType == STR) {
            sValue = another.sValue;
        } else if(conType == INT) {
            iValue = another.iValue;
        } else {
            cValue = another.cValue;
        }
    }
    conNodeType(typeEnum t, int i):iValue(i), conType(t){;}
    conNodeType(typeEnum t, const string * c, bool isString):conType(t) {
        if (isString) {
            sValue = c;
        } else {
            cValue = c;
        }
    }
    union {
        int iValue;
        const string * cValue;
        const string * sValue;
    };
};

struct callType {
    callType(list<nodeType*> * a):arguments(a) {;}
    list<nodeType *> * arguments;
};

struct funcType {
    funcType() {
        this->parameters = nullptr;
        this->arguments = nullptr;
    }
    funcType(list<nodeType * > * p, nodeType * stmts, int o) {
        this->parameters = p;
        this->stmts = stmts;
        this->arguments = new list<nodeType*>();
        origin = o;
    }
    funcType(const funcType& another) {
        this->parameters = another.parameters;
        this->stmts = another.stmts;
        this->arguments = new list<nodeType*>();
        for (const auto& i: *another.arguments) {
            this->arguments->push_back(i);
        }
        origin = another.origin;
    }
    funcType& operator=(const funcType& another) {
  /*      if (this->parameters != nullptr) {
            delete this->parameters;
            delete this->arguments;
            this->parameters = nullptr;
            this->arguments = nullptr;
        }*/

        this->parameters = another.parameters;
        this->stmts = another.stmts;
        this->arguments = new list<nodeType*>();
        for (const auto& i: *another.arguments) {
            this->arguments->push_back(i);
        }
        origin = another.origin;
        return *this;
    }
    int origin;
    const list<nodeType *> * parameters;
    list<nodeType *> * arguments;
    nodeType * stmts;
};

/* identifiers */
struct idNodeType{
    int i;                      /* variable identifier */
    idEnum type;
    idNodeType(const idNodeType& another):i(another.i), type(another.type) {
        if (another.type == FUNCTION) {
            function = another.function;
        } else if (another.type == CALL) {
            call = another.call;
        } else {
            global = another.global;
        }
        type = another.type;
    }
    idNodeType& operator=(const idNodeType& another) {
        i = another.i;
        type = another.type;
        if (&another == this) {
            return *this;
        }
        if (another.type == FUNCTION) {
            function = another.function;
        } else if (another.type == CALL) {
            call = another.call;
        } else {
            global = another.global;
        }
        return *this;
    }
    idNodeType(int i, bool global):i(i), type(VAR), global(global) {
        ;
    }
    idNodeType(int i, list<nodeType* > * p, nodeType* stmts, int origin): i(i), type(FUNCTION), function(funcType(p, stmts, origin)) {
        ;
    }
    idNodeType(int i, list<nodeType* > * p):i(i), type(CALL), call(callType(p)) {
        ;
    }
    union {
        funcType function;
        callType call;
        bool global;
    };
};

/* operators */
struct oprNodeType {
    oprNodeType(int opr, int nop, nodeType** op): oper(opr), op(op), nops(nop) {;}
    int oper;                   /* operator */
    int nops;
    nodeType** op;  /* operands (expandable) */
    oprNodeType(const oprNodeType& another) {
        oper = another.oper;
        nops = another.nops;
        op = another.op;
    }
};


struct nodeType {
    nodeType(nodeEnum n, const conNodeType&& c): node(n) {
        con = c;
    }
    nodeType(nodeEnum n, const idNodeType&& i): node(n) {
        id = i;
    }
    nodeType(nodeEnum n, const oprNodeType&& o): node(n) {
        opr = o;
    }
    nodeType(const nodeType& another) {
        node = another.node;
        if (node == nodeCon) {
            con = another.con;
        } else if (node == nodeOpr) {
            opr = another.opr;
        } else {
            id = another.id;
        }
    }
    nodeType& operator=(const nodeType& another) {
        node = another.node;
        if (node == nodeCon) {
            con = another.con;
        } else if (node == nodeOpr) {
            opr = another.opr;
        } else {
            id = another.id;
        }
        return *this;
    }
    nodeEnum node;              /* type of node */
    /* union must be last entry in nodeType */
    /* because operNodeType may dynamically increase */
    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
    };
};

#endif // C6_H