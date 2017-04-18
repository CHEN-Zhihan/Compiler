typedef enum { typeCon, typeId, typeOpr } nodeEnum;

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
typedef struct {
    int i;                      /* subscript to fp */
} idNodeType;

/* operators */
typedef struct {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    struct nodeTypeTag *op[1];  /* operands (expandable) */
} oprNodeType;


typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

    /* union must be last entry in nodeType */
    /* because operNodeType may dynamically increase */
    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
    };
} nodeType;

