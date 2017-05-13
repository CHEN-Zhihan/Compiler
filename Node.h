#ifndef NODE_H
#define NODE_H

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;

#include <list>
using std::list;

class Node {
public:
    Node()=default;
    virtual void ex(int, int, int) const = 0;
    virtual void check(vector<int>&, int) const = 0;
    virtual ~Node() {;};
    enum type {
        STRING,
        INT,
        BOOL,
        CHAR
    };
};

class ConNode: public Node {
public:
    virtual void ex(int, int, int) const = 0;
    void check(vector<int>&, int) const{
        ;
    }
};


class StrNode:public ConNode {
public:
    StrNode(const string&);
    void ex(int, int, int) const;
private:
    const string value;
};

class CharNode:public ConNode {
public:
    CharNode(const string&);
    void ex(int, int, int) const;
private:
    const string value;
};

class IntNode:public ConNode {
public:
    IntNode(const int&);
    void ex(int, int, int) const;
private:
    const int value;
};

class OprNode:public Node {
public:
    OprNode(int, const vector<shared_ptr<Node>>&);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;
protected:
    int oper;
    vector<shared_ptr<Node>> op;
};

class IDNode:public Node {
public:
    IDNode(int);
    int getID() const;
protected:
    int i;
};
class VarNode: public IDNode {
public:
    VarNode(int, bool, const list<shared_ptr<Node> >&);
    virtual void ex(int, int, int) const;
    virtual void check(vector<int>&, int) const;
    virtual void assign(vector<int>&, int) const;
    virtual void pop(int) const;
private:
    bool global;
    list<shared_ptr<Node>> subscriptions;
    int getDefinitionScope(const vector<int>&, int) const;    
    void push(int) const;
};

class ArrayNode:public VarNode {
public:
    ArrayNode(int, const vector<int>&, bool);
};

class ExprNode: public OprNode {
public:
    ExprNode(int, const vector<shared_ptr<Node>>&);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;
    void inStmt();
private:
    bool inStatement;
};

class StmtNode: public OprNode {
public:
    StmtNode(int, const vector<shared_ptr<Node>>&);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;
};


class FunctionNode: public IDNode {
public:
    FunctionNode(int, const list<shared_ptr<VarNode> >, const shared_ptr<Node>&);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;
    size_t getNumParameters() const;
    void checkStmt(vector<int>&, int) const;
    void exStmt(int, int, int) const;
private:
    int origin;
    list<shared_ptr<VarNode> > parameters;
    shared_ptr<Node> stmts;
};

class CallNode: public IDNode {
public:
    CallNode(int, const list<shared_ptr<Node>>);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;
private:
    int origin;
    list<shared_ptr<Node> > arguments;
};



#endif