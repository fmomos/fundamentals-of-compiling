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
    return TokenType::PLUS; // 默认返回（不会执行）)



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
