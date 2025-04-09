#include<map>
#include<cassert>
#include<string>
#include<iostream>
#include<vector>
#include<set>
#include<queue>

#define TODO assert(0 && "TODO")
// #define DEBUG_DFA
// #define DEBUG_PARSER

// enumerate for Status
enum class State {
    Empty,              // space, \n, \r ...
    IntLiteral,         // int literal, like '1' '01900', '0xAB', '0b11001'
    op                  // operators and '(', ')'
};
std::string toString(State s) {
    switch (s) {
    case State::Empty: return "Empty";
    case State::IntLiteral: return "IntLiteral";
    case State::op: return "op";
    default:
        assert(0 && "invalid State");
    }
    return "";
}

// enumerate for Token type
enum class TokenType{
    INTLTR,        // int literal
    PLUS,        // +
    MINU,        // -
    MULT,        // *
    DIV,        // /
    LPARENT,        // (
    RPARENT,        // )
};
std::string toString(TokenType type) {
    switch (type) {
    case TokenType::INTLTR: return "INTLTR";
    case TokenType::PLUS: return "PLUS";
    case TokenType::MINU: return "MINU";
    case TokenType::MULT: return "MULT";
    case TokenType::DIV: return "DIV";
    case TokenType::LPARENT: return "LPARENT";
    case TokenType::RPARENT: return "RPARENT";
    default:
        assert(0 && "invalid token type");
        break;
    }
    return "";
}

// definition of Token
struct Token {
    TokenType type;
    std::string value;
};

// definition of DFA
struct DFA {
    /**
     * @brief constructor, set the init state to State::Empty
     */
    DFA();
    
    /**
     * @brief destructor
     */
    ~DFA();
    
    // the meaning of copy and assignment for a DFA is not clear, so we do not allow them
    DFA(const DFA&) = delete;   // copy constructor
    DFA& operator=(const DFA&) = delete;    // assignment

    /**
     * @brief take a char as input, change state to next state, and output a Token if necessary
     * @param[in] input: the input character
     * @param[out] buf: the output Token buffer
     * @return  return true if a Token is produced, the buf is valid then
     */
    bool next(char input, Token& buf);

    /**
     * @brief reset the DFA state to begin
     */
    void reset();

private:
    State cur_state;    // record current state of the DFA
    std::string cur_str;    // record input characters
};


DFA::DFA(): cur_state(State::Empty), cur_str() {}

DFA::~DFA() {}

// helper function, you are not require to implement these, but they may be helpful
bool isoperator(char c) {
return c=='+'||c=='-'||c=='*'||c=='/'||c=='('||c==')';



}

TokenType get_op_type(std::string s) {
   if (s == "+") return TokenType::PLUS;
    if (s == "-") return TokenType::MINU;
    if (s == "*") return TokenType::MULT;
    if (s == "/") return TokenType::DIV;
    if (s == "(") return TokenType::LPARENT;
    if (s == ")") return TokenType::RPARENT;
    assert(0 && "invalid operator");
    return TokenType::PLUS; // 默认返回（不会执行）



}

bool DFA::next(char input, Token& buf) {
  switch(cur_state) {
        case State::Empty:
            if (isoperator(input)) {
                cur_str = std::string(1, input);
                cur_state = State::op;
                return false;
            } else if (isdigit(input)) {
                cur_str = std::string(1, input);
                cur_state = State::IntLiteral;
                return false;
            }
            return false;
            
        case State::op:
            buf.value = cur_str;
            buf.type = get_op_type(cur_str);
            if (isoperator(input)) {
                cur_str = std::string(1, input);
            } else if (isdigit(input)) {
                cur_state = State::IntLiteral;
                cur_str = std::string(1, input);
            } else {
                reset();
            }
            return true;
            
        case State::IntLiteral:
            if (isdigit(input) || 
               (input >= 'A' && input <= 'F') || 
               (input >= 'a' && input <= 'f') || 
               input == 'x' || input == 'X') {
                cur_str.push_back(input);
                return false;
            } else {
                buf.value = cur_str;
                buf.type = TokenType::INTLTR;
                if (isoperator(input)) {
                    cur_state = State::op;
                    cur_str = std::string(1, input);
                } else {
                    reset();
                }
                return true;
            }
            
        default:
            return false;
    }


}

void DFA::reset() {
    cur_state = State::Empty;
    cur_str = "";
}

// hw2
enum class NodeType {
    TERMINAL,       // terminal lexical unit
    EXP,
    NUMBER,
    PRIMARYEXP,
    UNARYEXP,
    UNARYOP,
    MULEXP,
    ADDEXP,
    NONE
};
std::string toString(NodeType nt) {
    switch (nt) {
    case NodeType::TERMINAL: return "Terminal";
    case NodeType::EXP: return "Exp";
    case NodeType::NUMBER: return "Number";
    case NodeType::PRIMARYEXP: return "PrimaryExp";
    case NodeType::UNARYEXP: return "UnaryExp";
    case NodeType::UNARYOP: return "UnaryOp";
    case NodeType::MULEXP: return "MulExp";
    case NodeType::ADDEXP: return "AddExp";
    case NodeType::NONE: return "NONE";
    default:
        assert(0 && "invalid node type");
        break;
    }
    return "";
}

// tree node basic class
struct AstNode{
    int value;
    NodeType type;  // the node type
    AstNode* parent;    // the parent node
    std::vector<AstNode*> children;     // children of node

    /**
     * @brief constructor
     */
    AstNode(NodeType t = NodeType::NONE, AstNode* p = nullptr): type(t), parent(p), value(0) {} 

    /**
     * @brief destructor
     */
    virtual ~AstNode() {
        for(auto child: children) {
            delete child;
        }
    }

    // rejcet copy and assignment
    AstNode(const AstNode&) = delete;
    AstNode& operator=(const AstNode&) = delete;
};

// definition of Parser
// a parser should take a token stream as input, then parsing it, output a AST
struct Parser {
    uint32_t index; // current token index
    const std::vector<Token>& token_stream;

    /**
     * @brief constructor
     * @param tokens: the input token_stream
     */
    Parser(const std::vector<Token>& tokens): index(0), token_stream(tokens) {}

    /**
     * @brief destructor
     */
    ~Parser() {}
    
    /**
     * @brief creat the abstract syntax tree
     * @return the root of abstract syntax tree
     */
    AstNode* get_abstract_syntax_tree() {
return OP_Exp();


    
    }

    // u can define member funcition of Parser here
	AstNode* OP_Exp() {
		AstNode* root = new AstNode(NodeType::EXP);
		AstNode* child = OP_AddExp();		// construct subtree
		root->children.push_back(child);
		child->parent = root;
		return root;
	}

	/**
	 *  @brief creat AST by `Number -> IntConst | floatConst`
	 *  @return the root of current subAST
	 */
	AstNode* OP_Number() {
		AstNode* root = new AstNode(NodeType::NUMBER);
		if(token_stream[index].type != TokenType::INTLTR){
			assert(0 && "invalid token type in function OP_Number");
			return nullptr;
		}
		// determine radix
        AstNode* child = new AstNode(NodeType::TERMINAL);       // terminal node
        root->children.push_back(child);
		std::string val = token_stream[index].value;
        if(val.length()>=3 && val[0]=='0' && (val[1]=='x' || val[1]=='X')){ // hexadecimal integer
            child->value = std::stoi(val.substr(2),nullptr,16);
            child->parent = root;
        }
        else if(val.length()>=3 && val[0]=='0' && (val[1]=='b' || val[1]=='B')){    // binary integer
            child->value = std::stoi(val.substr(2),nullptr,2);
            child->parent = root;
        }
        else if(val.length()>=2 && val[0]=='0'){                            // octal integer
            child->value = std::stoi(val,nullptr,8);
            child->parent = root;
        }
        else{           // decimal integer (or float)
            child->value = std::stoi(val,nullptr,10);
            child->parent = root;
        }
        index++;        // no more calls to other OP functions for UnaryOp
		return root;
	}

	/**
	 *  @brief creat AST by `PrimaryExp -> '(' Exp ')' | Number`
	 *  @return the root of current subAST
	 */
	AstNode* OP_PrimaryExp() {
		AstNode* root = new AstNode(NodeType::PRIMARYEXP);
		if(token_stream[index].type == TokenType::LPARENT) {    // PrimaryExp -> '(' Exp ')'
            index++;            // discard '('
            AstNode* child = OP_Exp();
            if(token_stream[index].type == TokenType::RPARENT) {
                index++;        // discard ')'
            } else {
                assert(0 && "Missing RPARENT");
            }
            root->children.push_back(child);
            child->parent = root;
        }
        else{   // PrimaryExp -> Number
            AstNode *child = OP_Number();
            root->children.push_back(child);
            child->parent = root;
        }
		return root;
	}

	/**
	 *  @brief creat AST by `UnaryExp -> PrimaryExp | UnaryOp UnaryExp`
	 *  @return the root of current subAST
	 */
	AstNode* OP_UnaryExp() {
		AstNode* root = new AstNode(NodeType::UNARYEXP);
        if (token_stream[index].type == TokenType::PLUS || token_stream[index].type == TokenType::MINU) {   // UnaryExp -> UnaryOp UnaryExp
            AstNode *child1 = OP_UnaryOp();
            AstNode *child2 = OP_UnaryExp();
            child1->parent = root;
            child2->parent = root;
            root->children.push_back(child1);
            root->children.push_back(child2);
        }
        else {      // UnaryExp -> PrimaryExp
            AstNode *child = OP_PrimaryExp();
            child->parent = root;
            root->children.push_back(child);
        }
		return root;
	}

	/**
	 *  @brief creat AST by `UnaryOp -> '+' | '-'`
	 *  @return the root of current subAST
	 */
	AstNode* OP_UnaryOp() {
		AstNode* root = new AstNode(NodeType::UNARYOP);
		if(token_stream[index].type != TokenType::PLUS && token_stream[index].type != TokenType::MINU){
			assert(0 && "invalid token type in function OP_UnaryOp");
			return nullptr;
		}
        AstNode* child = new AstNode(NodeType::TERMINAL);       // terminal node
        root->children.push_back(child);
		std::string val = token_stream[index].value;
        if(token_stream[index].type==TokenType::PLUS){
            child->value = static_cast<int>(TokenType::PLUS);
            child->parent = root;
        }
        else{
            child->value = static_cast<int>(TokenType::MINU);
            child->parent = root;
        }
        index++;        // no more calls to other OP functions for UnaryOp
		return root;
	}

	/**
	 *  @brief creat AST by `MulExp -> UnaryExp { ('*' | '/') UnaryExp }`
	 *  @return the root of current subAST
	 */
	AstNode* OP_MulExp() {
		AstNode* root = new AstNode(NodeType::MULEXP);
        AstNode *child1 = OP_UnaryExp();
        root->children.push_back(child1);
        child1->parent = root;
        while (index < token_stream.size() && (token_stream[index].type == TokenType::MULT || token_stream[index].type == TokenType::DIV)) {
            // '*' and '/' have no intermediate node
            AstNode* child2 = new AstNode(NodeType::TERMINAL);
            child2->value = static_cast<int>(token_stream[index].type);
            child2->parent = root;
            root->children.push_back(child2);
            index++;    // no more calls to other OP functions for '*' and '/'
            AstNode *child3 = OP_UnaryExp();
            root->children.push_back(child3);
            child3->parent = root;
        }
		return root;
	}

	/**
	 *  @brief creat AST by `AddExp -> MulExp { ('+' | '-') MulExp }`
	 *  @return the root of current subAST
	 */
	AstNode* OP_AddExp() {
		AstNode* root = new AstNode(NodeType::ADDEXP);
		AstNode* child1 = OP_MulExp();
		root->children.push_back(child1);
		child1->parent = root;
		while(index < token_stream.size() && (token_stream[index].type == TokenType::PLUS || token_stream[index].type == TokenType::MINU)){
            // no need to call function OP_UnaryOP, because that will add more intermediate nodes in AST
            AstNode* child2 = new AstNode(NodeType::TERMINAL);
            child2->value = static_cast<int>(token_stream[index].type);
            child2->parent = root;
            root->children.push_back(child2);
            index++;    // no more calls to other OP functions for '+' and '-'
            AstNode *child3 = OP_MulExp();
            root->children.push_back(child3);
            child3->parent = root;
		}
		return root;
	}





    
};

// u can define funcition here
int get_value(AstNode* root) {
    if(root->type == NodeType::TERMINAL){       // value has been assigned
        return root->value;
    }
    else if(root->type == NodeType::EXP){       // Exp -> AddExp
        root->value = get_value(root->children[0]);         // AddExp
        return root->value;
    }
    else if(root->type == NodeType::NUMBER){    // Number -> IntConst | floatConst
        root->value = get_value(root->children[0]);         // IntConst or floatConst
        return root->value;
    }
    else if(root->type == NodeType::PRIMARYEXP){// PrimaryExp -> '(' Exp ')' | Number
        root->value = get_value(root->children[0]);         // Exp or Number
        return root->value;
    }
    else if(root->type == NodeType::UNARYEXP){  // UnaryExp -> PrimaryExp | UnaryOp UnaryExp
        if(root->children[0]->type == NodeType::PRIMARYEXP){// UnaryExp -> PrimaryExp
            root->value = get_value(root->children[0]);     // PrimaryExp
        }
        else{                                               // UnaryExp -> UnaryOp UnaryExp
            int unaryop = get_value(root->children[0]);
            if (unaryop == static_cast<int>(TokenType::PLUS)){      // '+'
                root->value = get_value(root->children[1]);         // UnaryExp
            }
            else{                                                   // '-'
                root->value = -get_value(root->children[1]);        // UnaryExp
            }
        }
        return root->value;
    }
    else if(root->type == NodeType::UNARYOP){   // UnaryOp -> '+' | '-'
        root->value = get_value(root->children[0]);         // '+' or '-'
        return root->value;
    }
    else if(root->type == NodeType::MULEXP){    // MulExp -> UnaryExp { ('*' | '/') UnaryExp }
        int result = get_value(root->children[0]);          // UnaryExp
        for(int i = 1; i < root->children.size(); i += 2){  // ('*' | '/') UnaryExp
            int val = root->children[i]->value;
            if (val == static_cast<int>(TokenType::MULT)){  // '*'
                result *= get_value(root->children[i+1]);   // UnaryExp
            }
            else{                                           // '/'
                result /= get_value(root->children[i+1]);   // UnaryExp
            }
        }
        root->value = result;
        return root->value;
    }
    else if(root->type == NodeType::ADDEXP){    // AddExp -> MulExp { ('+' | '-') MulExp }
        int result = get_value(root->children[0]);          // MULEXP
        for(int i = 1; i < root->children.size(); i += 2){  // ('+' | '-') MulExp
            int val = root->children[i]->value;
            if (val == static_cast<int>(TokenType::PLUS)){  // '+'
                result += get_value(root->children[i+1]);   // MulExp
            }
            else{                                           // '-'
                result -= get_value(root->children[i+1]);   // MulExp
            }
        }
        root->value = result;
        return root->value;
    }
    else{
        assert(0 && "invalid  NodeType");
        return -1; 
    }
}


int main(){
    std::string stdin_str;
    std::getline(std::cin, stdin_str);
    stdin_str += "\n";
    DFA dfa;
    Token tk;
    std::vector<Token> tokens;
    for (size_t i = 0; i < stdin_str.size(); i++) {
        if(dfa.next(stdin_str[i], tk)){
            tokens.push_back(tk); 
        }
    }

    // hw2
    Parser parser(tokens);
    auto root = parser.get_abstract_syntax_tree();
    // u may add function here to analysis the AST, or do this in parsing
    // like get_value(root);
    
get_value(root);



    std::cout << root->value;

    return 0;
}
