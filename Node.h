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
    UNKNOWN = -2, STR = -3, INT = -4, BOOL = -5, CHAR = -6, VOID = -7
};

class Node {
  public:
    Node();
    Node(VALUE_TYPE);
    virtual void ex(int, int, int) const = 0;
    virtual void check(vector<int> &, int) const = 0;
    virtual ~Node() { ; };
    VALUE_TYPE getType() const;
protected:
    VALUE_TYPE type; 
};

class ValueNode {
  public:
    void inStmt();
  protected:
    bool inStatement;
};

class ConNode : public ValueNode, public Node {
  public:
    ConNode(VALUE_TYPE);
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
    int getValue() const;
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
    IDNode(int, VALUE_TYPE t=UNKNOWN);
    int getID() const;
  protected:
    int i;
};



class ParameterNode : public IDNode { // denote the parameter in function definition.
  public:
    ParameterNode(VALUE_TYPE, int, int);
    void ex(int, int, int) const;
    void check(vector<int> &, int) const;
  private:
    int dimensions;
};
class ExprNode : public OprNode, public ValueNode {
  public:
    ExprNode(int, const vector<shared_ptr<Node>> &);
    void ex(int, int, int) const;
    void check(vector<int> &, int) const;
    void evaluate(vector<int> &, int);
    int getValue() const;
    bool isKnown() const;
  private:
    int value;
    bool known;
};

class VarNode : public IDNode {
  public:
    VarNode(int, bool, const list<shared_ptr<Node>> &);
    void ex(int, int, int) const;
    void check(vector<int> &, int) const;
    void declare(vector<int> &, int) const;
    void pop(int) const;
    void setType(VALUE_TYPE);
    void push(int) const;   
  private:
    bool global;
    list<shared_ptr<Node>> subscriptions;
    int getDefinitionScope(const vector<int> &, int) const;
};



class DeclareNode : public Node { // denote the local variable declared.
  public:
    DeclareNode(Node * variable, Node * initializer=nullptr);
    void setType(VALUE_TYPE);
    void ex(int, int, int) const;
    void check(vector<int>&, int) const;
  private:
    shared_ptr<VarNode> variable;
    shared_ptr<Node> initializer;
};

class StmtNode : public OprNode {
  public:
    StmtNode(int, const vector<shared_ptr<Node>> &);
    void ex(int, int, int) const;
    void check(vector<int> &, int) const;
};



class FunctionNode : public IDNode {
  public:
    FunctionNode(VALUE_TYPE, int, const list<shared_ptr<VarNode>>,
                 const vector<shared_ptr<Node> > &);
    void ex(int, int, int) const;
    void check(vector<int> &, int) const;
    size_t getNumParameters() const;
    void checkStmt(vector<int> &, int) const;
    void exStmt(int, int, int) const;

  private:
    int origin;
    list<shared_ptr<VarNode>> parameters;
    const vector<shared_ptr<Node>> stmts;
};

class CallNode : public IDNode, public ValueNode {
  public:
    CallNode(int, const list<shared_ptr<Node>>);
    void ex(int, int, int) const;
    void check(vector<int> &, int) const;

  private:
    int origin;
    list<shared_ptr<Node>> arguments;
};

#endif