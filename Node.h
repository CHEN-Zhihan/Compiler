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

#include <array>
using std::array;

class Node {
public:
    Node()=default;
    virtual void ex(int, int, int) const = 0;
    virtual void check(vector<int>&, int) const = 0;
    virtual ~Node() {;};
};

class StrNode:public Node {
public:
    StrNode(const string&);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;
private:
    const string value;
};

class CharNode:public Node {
public:
    CharNode(const string&);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;    
private:
    const string value;
};

class IntNode:public Node {
public:
    IntNode(const int&);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;    
private:
    const int value;
};


class ExprNode: public Node {
public:
    ExprNode(int, const vector<shared_ptr<Node> >&);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;
    int getOper() const;
private:
    int oper;
    array<shared_ptr<Node>, 2> op;
};

class StmtNode: public Node {
public:
    StmtNode(int, const vector<shared_ptr<Node> >&);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;
private:
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
    VarNode(int, bool, vector<shared_ptr<ExprNode> > subscriptions);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;
    void assign(vector<int>&, int) const;
    void pop(int) const;
private:
    bool global;
    vector<shared_ptr<ExprNode> > subscriptions;
    int getDefinitionScope(const vector<int>&, int) const;
    void push(int) const;
};

class ArrayDeclareNode: public Node {
public:
    ArrayDeclareNode(const shared_ptr<VarNode>&, const shared_ptr<ExprNode>&);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;
private:
    shared_ptr<VarNode> variable;
    shared_ptr<ExprNode> initializer;
};

class FunctionNode: public IDNode {
public:
    FunctionNode(int, const vector<shared_ptr<VarNode> >, const vector<shared_ptr<StmtNode>>&);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;
    size_t getNumParameters() const;
    void checkStmt(vector<int>&, int) const;
    void exStmt(int, int, int) const;
private:
    vector<shared_ptr<VarNode> > parameters;
    vector<shared_ptr<StmtNode> > stmts;
};

class CallNode: public IDNode {
public:
    CallNode(int, const vector<shared_ptr<ExprNode>>);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;
private:
    vector<shared_ptr<ExprNode> > arguments;
};
#endif