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
    virtual void ex(vector<int>&, int, int, int) const = 0;
    virtual void check(vector<int>&, int) const = 0;
    virtual ~Node() {;};
};

class ConNode : public Node {
public:
    virtual void ex(vector<int>&, int, int, int) const = 0;
    void check(vector<int>&, int) const;
};

class StrNode:public ConNode {
public:
    StrNode(const string&);
    void ex(vector<int>&, int, int, int) const;
private:
    const string value;
};

class CharNode:public ConNode {
public:
    CharNode(const string&);
    void ex(vector<int>&, int, int, int) const;
    int getValue() const;
private:
    const string value;
};

class IntNode:public ConNode {
public:
    IntNode(const int&);
    void ex(vector<int>&, int, int, int) const;
    int getValue() const;
private:
    const int value;
};

class BoolNode: public ConNode {
public:
    BoolNode(bool);
    void ex(vector<int>&, int, int, int) const;
    int getValue() const;
private:
    const bool value;
};


class ExprNode: public Node {
public:
    ExprNode(int, const vector<shared_ptr<Node> >&);
    void ex(vector<int>&, int, int, int) const;
    void check(vector<int>&, int) const;
    int getOper() const;
    void inStmt();
    void evaluate();
    bool isKnown() const;
    int getValue() const;
private:
    int oper;
    bool inStatement;
    int value;
    bool known;
    array<shared_ptr<Node>, 2> op;
};

class StmtNode: public Node {
public:
    StmtNode(int, const vector<shared_ptr<Node> >&);
    void ex(vector<int>&, int, int, int) const;
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
    VarNode(int, bool, const vector<shared_ptr<ExprNode> >& subscriptions);
    void ex(vector<int>&, int, int, int) const;
    void check(vector<int>&, int) const;
    void assign(vector<int>&, int) const;
    void pop(vector<int>&, int) const;
    void push(vector<int>&, int) const;
    void declare(vector<int>&, int) const;
private:
    bool global;
    vector<shared_ptr<ExprNode> > subscriptions;
    int getDefinitionScope(const vector<int>&, int) const;
    void getOffSet(vector<int>&, int, int, int) const;
    void pushPop(vector<int>&, int, bool) const;
    vector<int> getDimension() const;
};

class DeclareNode: public Node {
public:
    DeclareNode(const shared_ptr<VarNode>&, const shared_ptr<ExprNode> =nullptr);
    void ex(vector<int>&, int, int, int) const;
    void check(vector<int>&, int) const;
private:
    shared_ptr<VarNode> variable;
    shared_ptr<ExprNode> initializer;
};

class FunctionNode: public IDNode {
public:
    FunctionNode(int, const vector<shared_ptr<VarNode> >&, const vector<shared_ptr<Node>>&);
    void ex(vector<int>&, int, int, int) const;
    void check(vector<int>&, int) const;
    size_t getNumParameters() const;
    void checkStmt(vector<int>&, int) const;
    void exStmt(vector<int>&, int, int, int) const;
private:
    vector<shared_ptr<VarNode> > parameters;
    vector<shared_ptr<Node> > stmts;
};

class CallNode: public IDNode {
public:
    CallNode(int, const vector<shared_ptr<ExprNode>>&);
    void ex(vector<int>&, int, int, int) const;
    void check(vector<int>&, int) const;
private:
    vector<shared_ptr<ExprNode> > arguments;
};
#endif