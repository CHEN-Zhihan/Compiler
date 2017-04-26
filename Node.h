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
    virtual void ex(int, int, int) = 0;
    virtual void check(vector<int>&, int) = 0;
};

class ConNode: public Node {
public:
    virtual void ex(int, int, int) = 0;
    virtual void check(vector<int>&, int) {
        ;
    }
};

class StrNode:public ConNode {
public:
    StrNode(const string&);
    void ex(int, int, int);
private:
    const string value;
};

class CharNode:public ConNode {
public:
    CharNode(const string&);
    void ex(int, int, int);
private:
    const string value;
};

class IntNode:public ConNode {
public:
    IntNode(const int&);
    void ex(int, int, int);
private:
    const int value;
};

class OprNode:public Node {
public:
    OprNode(int, const vector<shared_ptr<Node>>&);
    void ex(int, int, int);
    void check(vector<int>&, int);
private:
    int oper;
    vector<shared_ptr<Node>> op;
};

class IDNode:public Node {
public:
    IDNode(int);

protected:
    int i;
};

class FunctionNode: public IDNode {
public:
    FunctionNode(int, const list<shared_ptr<Node> >, const shared_ptr<Node>&);
    void ex(int, int, int);
    void check(vector<int>&, int);
private:
    int origin;
    list<shared_ptr<Node> > parameters;
    shared_ptr<Node> stmts;
};

class CallNode: public IDNode {
public:
    CallNode(int, const list<shared_ptr<Node>>);
    void ex(int, int, int);
    void check(vector<int>&, int);
private:
    int origin;
    list<shared_ptr<Node> > arguments;
};

class VarNode: public IDNode {
public:
    VarNode(int, bool);
    void ex(int, int, int);
    void check(vector<int>&, int);
    void push(int);
    void pop(int);
    void assign(vector<int>&, int);
private:
    bool global;
};

#endif