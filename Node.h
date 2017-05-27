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

#define DEBUG false

class FunctionNode;
class CallNode;
class Node {
public:
    Node()=default;
    virtual void ex(vector<int>&, int, int, int) const = 0;
    virtual void check(vector<int>&, int) = 0;
    virtual ~Node() {;};
};

class ConNode : public Node {
public:
    virtual void ex(vector<int>&, int, int, int) const = 0;
    void check(vector<int>&, int);
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
    void check(vector<int>&, int);
    void checkArray(vector<int>&, int, const shared_ptr<Node>&) const;
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
friend class ::FunctionNode;
friend class ::CallNode;
};

class StmtNode: public Node {
public:
    StmtNode(int, const vector<shared_ptr<Node> >&);
    void ex(vector<int>&, int, int, int) const;
    void check(vector<int>&, int);
private:
    int oper;
    vector<int> scopes;
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
    void check(vector<int>&, int);
    void assign(vector<int>&, int) const;
    void pop(vector<int>&, int) const;
    void push(vector<int>&, int) const;
    void declare(vector<int>&, int) const;
    bool isGlobal() const;
    int getDefinitionScope(const vector<int>&, int) const;
    int getDimensions() const;
    void match();
private:
    bool global;
    bool matched;
    vector<shared_ptr<ExprNode> > subscriptions;
    void getOffSet(vector<int>&, int, int, int) const;
    void pushPop(vector<int>&, int, bool) const;
    vector<int> getDimension() const;
friend class ::ExprNode;
friend class ::FunctionNode;
};

class DeclareNode: public Node {
public:
    DeclareNode(const shared_ptr<VarNode>&, const shared_ptr<ExprNode> =nullptr);
    void ex(vector<int>&, int, int, int) const;
    void check(vector<int>&, int);
private:
    shared_ptr<VarNode> variable;
    shared_ptr<ExprNode> initializer;
};

class FunctionNode: public IDNode {
public:
    FunctionNode(int, const vector<shared_ptr<VarNode> >&, const vector<shared_ptr<Node>>&);
    void ex(vector<int>&, int, int, int) const;
    void check(vector<int>&, int);
    size_t getNumParameters() const;
    void checkStmt(vector<int>&, int) ;
    void exStmt(vector<int>&, int, int, int) const;
    void match(const vector<shared_ptr<ExprNode> >&, const vector<int>&, int) const;
private:
    vector<shared_ptr<VarNode> > parameters;
    vector<shared_ptr<Node> > stmts;
    int scope;
};

class CallNode: public IDNode {
public:
    CallNode(int, const vector<shared_ptr<ExprNode>>&);
    void ex(vector<int>&, int, int, int) const;
    void check(vector<int>&, int);
private:
    vector<shared_ptr<ExprNode> > arguments;
};
#endif