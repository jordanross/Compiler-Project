#pragma once
#include "token.h" 
#include "errhandler.h"
#include "symboltable.h"
#include "scanner.h"

#include <iostream>
#include <sstream>

class Parser
{
public:
    Parser(ErrHandler* handler, SymbolTableManager* manager, Scanner* scan);
    void parse();
private:
    ErrHandler* err_handler;
    SymbolTableManager* symtable_manager;
    Scanner* scanner;

    // Get a token from scanner and store in curr_token if !curr_token_valid
    TokenType token();
    // Consume the token; subsequent calls to getToken will return a new token.
    Token advance();
    // Ensure current_token has type t, 
    //  if not, report err (if error=true) or warning 
    Token require(TokenType t, bool error=true);

    Token curr_token;
    bool curr_token_valid;

    void program();
    void program_header();
    void program_body();

    void declaration();

    void proc_declaration(bool is_global);
    void proc_header();
    void proc_body();
    void parameter_list();
    void parameter();

    SymTableEntry* var_declaration(bool is_global);
    void type_mark();
    Value lower_bound();
    Value upper_bound();

    bool statement();
    void identifier_statement();
    void assignment_statement(std::string);
    void proc_call(std::string);
    void argument_list(SymTableEntry* proc_entry);

    void if_statement();
    void loop_statement();
    void return_statement();

    //void destination();

    Value expression();
    Value expression_pr(Value lhs); // needed for eliminating left recursion
    Value arith_op();
    Value arith_op_pr(Value lhs); // needed for eliminating left recursion
    Value relation();
    Value relation_pr(Value lhs); // needed for eliminating left recursion
    Value term();
    Value term_pr(Value lhs);
    Value factor();
    Value name();
};

