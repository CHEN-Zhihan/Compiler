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

enum VALUE_TYPE {
    UNKNOWN = -2, STRING = -3, INT = -4, BOOL = -5, CHAR = -6, VOID = -7
};


class TypeNode {
  public:
    TypeNode(VALUE_TYPE, const vector<int>&, bool declare);
    bool operator==(const TypeNode&);
private:
    VALUE_TYPE type;
    const vector<int> dimensions;
    bool declare;
};


class Node {
  public:
    Node() = default;
    virtual void ex(int, int, int) const = 0;
    virtual void check(vector<int> &, int) const = 0;
    virtual ~Node() { ; };
private:
    VALUE_TYPE type; 
};

class ConNode : public Node {
  public:
    virtual void ex(int, int, int) const = 0;
    void check(vector<int> &, int) const { ; }
};

class StrNode : public ConNode {
  public:
    StrNode(const string &);
    void ex(int, int, int) const;

  private:
    const string value;
};

class CharNode : public ConNode {
  public:
    CharNode(const string &);
    void ex(int, int, int) const;

  private:
    const string value;
};

class IntNode : public ConNode {
  public:
    IntNode(const int &);
    void ex(int, int, int) const;

  private:
    const int value;
};

class BoolNode: public ConNode {
  public:
    BoolNode(bool);
    void ex(int, int, int) const;
  private:
    const bool value;
};

class OprNode : public Node {
  public:
    OprNode(int, const vector<shared_ptr<Node>> &);
    virtual void ex(int, int, int) const = 0;
    virtual void check(vector<int> &, int) const = 0;
  protected:
    int oper;
    vector<shared_ptr<Node>> op;
};

class IDNode : public Node {
  public:
    IDNode(int);
    int getID() const;
  protected:
    int i;
};

class DeclareNode : public IDNode { // denote the local variable declared.
  public:
    DeclareNode(Node * variable, Node * initializer=nullptr);
    void setType(VALUE_TYPE);
  private:
    const vector<int> dimensions;
    shared_ptr<Node> initializer;
};

class FunctionParameterDeclareNode : public IDNode { // denote the parameter in function definition.
  public:
    FunctionParameterDeclareNode(VALUE_TYPE, int, int);

  private:
    VALUE_TYPE type;

};


class VarNode : public IDNode {
  public:
    VarNode(int, bool, const list<shared_ptr<Node>> &);
    virtual void ex(int, int, int) const;
    virtual void check(vector<int> &, int) const;
    virtual void assign(vector<int> &, int) const;
    virtual void pop(int) const;
    void setType(VALUE_TYPE);
  private:
    bool global;
    shared_ptr<Node> initializer;
    list<shared_ptr<Node>> subscriptions;
    int getDefinitionScope(const vector<int> &, int) const;
    void push(int) const;
};

class ArrayNode: public VarNode {

};

class ExprNode : public OprNode {
  public:
    ExprNode(int, const vector<shared_ptr<Node>> &);
    void ex(int, int, int) const;
    void check(vector<int> &, int) const;
    void inStmt();
    void initialize();
  private:
    bool inStatement;
};


class StmtNode : public OprNode {
  public:
    StmtNode(int, const vector<shared_ptr<Node>> &);
    void ex(int, int, int) const;
    void check(vector<int> &, int) const;
};



class FunctionNode : public IDNode {
  public:
    FunctionNode(int, const list<shared_ptr<VarNode>>,
                 const shared_ptr<Node> &);
    void ex(int, int, int) const;
    void check(vector<int> &, int) const;
    size_t getNumParameters() const;
    void checkStmt(vector<int> &, int) const;
    void exStmt(int, int, int) const;

  private:
    int origin;
    list<shared_ptr<VarNode>> parameters;
    shared_ptr<Node> stmts;
};

class CallNode : public IDNode {
  public:
    CallNode(int, const list<shared_ptr<Node>>);
    void ex(int, int, int) const;
    void check(vector<int> &, int) const;

  private:
    int origin;
    list<shared_ptr<Node>> arguments;
};

#endif