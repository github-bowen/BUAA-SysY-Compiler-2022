#include <cassert>
#include "Parser.h"
#include "exceptions/ParseEndError.h"

Parser::Parser(std::vector<Token *> &tokens)
        : tokens(tokens), tokenPos(-1), curToken(nullptr), tokenLength(tokens.size()) {}

Node *Parser::parse() {
    this->nextItem();
    Node *root = parse_CompUnit(1);
    return root;
}

void Parser::nextItem() noexcept(false) {
    if (tokenPos < tokenLength - 1) {
        this->curToken = this->tokens[++tokenPos];
    } else {
        return;
    }
}

/* CompUnit → {Decl} {FuncDef} MainFuncDef */
Node *Parser::parse_CompUnit(int depth) {
    Node *root = new Node(GrammarItem::CompUnit, nullptr, 1);
    Node *child = nullptr;
    while (this->tokens[tokenPos + 2]->symbol != Symbol::LPARENT) {
        child = this->parse_Decl(depth + 1);
        root->addChild(child);
        child->setParent(root);
    }
    while (this->tokens[tokenPos + 1]->symbol != Symbol::MAINTK) {
        child = this->parse_FuncDef(depth + 1);
        root->addChild(child);
        child->setParent(root);
    }
    child = this->parse_MainFuncDef(depth + 1);
    root->addChild(child);
    child->setParent(root);
    return root;
}

/* Decl → ConstDecl | VarDecl */
Node *Parser::parse_Decl(int depth) {
    Node *decl = new Node(GrammarItem::Decl, depth);
    Node *child = nullptr;
    if (this->curToken->symbol == Symbol::CONSTTK) {
        child = this->parse_ConstDecl(depth + 1);
    } else {
        child = this->parse_VarDecl(depth + 1);
    }
    decl->addChild(child);
    child->setParent(decl);
    return decl;
}

/* FuncDef → FuncType Ident '(' [FuncFParams] ')' Block */
Node *Parser::parse_FuncDef(int depth) {
    Node *funcDef = new Node(GrammarItem::FuncDef, depth);
    Node *child = nullptr;
    // FuncType
    child = this->parse_FuncType(depth + 1);
    child->setParent(funcDef);
    funcDef->addChild(child);
    // Ident
    child = this->parse_Ident(depth + 1);
    child->setParent(funcDef);
    funcDef->addChild(child);
    // '('
    assert(this->curToken->symbol == Symbol::LPARENT);
    funcDef->addChild(new Node(this->curToken, funcDef, depth + 1));
    this->nextItem();
    // [FuncFParams]
    if (this->curToken->symbol != Symbol::RPARENT) {
        // FuncFParams
        child = this->parse_FuncFParams(depth + 1);
        child->setParent(funcDef);
        funcDef->addChild(child);
    }
    // ')'
    assert(this->curToken->symbol == Symbol::RPARENT);
    funcDef->addChild(new Node(this->curToken, funcDef, depth + 1));
    this->nextItem();
    // Block
    child = this->parse_Block(depth + 1);
    child->setParent(funcDef);
    funcDef->addChild(child);

    return funcDef;
}

/* MainFuncDef → 'int' 'main' '(' ')' Block */
Node *Parser::parse_MainFuncDef(int depth) {
    Node *mainFuncDef = new Node(GrammarItem::MainFuncDef, depth);
    Node *child = nullptr;
    // 'int'
    assert(this->curToken->symbol == Symbol::INTTK);
    mainFuncDef->addChild(new Node(this->curToken, mainFuncDef, depth + 1));
    this->nextItem();
    // 'main'
    assert(this->curToken->symbol == Symbol::MAINTK);
    mainFuncDef->addChild(new Node(this->curToken, mainFuncDef, depth + 1));
    this->nextItem();
    // '('
    assert(this->curToken->symbol == Symbol::LPARENT);
    mainFuncDef->addChild(new Node(this->curToken, mainFuncDef, depth + 1));
    this->nextItem();
    // ')'
    assert(this->curToken->symbol == Symbol::RPARENT);
    mainFuncDef->addChild(new Node(this->curToken, mainFuncDef, depth + 1));
    this->nextItem();
    // Block
    child = this->parse_Block(depth + 1);
    child->setParent(mainFuncDef);
    mainFuncDef->addChild(child);
    return mainFuncDef;
}

/* ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';' */
Node *Parser::parse_ConstDecl(int depth) {
    Node *constDecl = new Node(GrammarItem::ConstDecl, depth);
    Node *child = nullptr;
    // 'const'
    assert(this->curToken->symbol == Symbol::CONSTTK);
    constDecl->addChild(new Node(this->curToken, constDecl, depth));
    // BType
    this->nextItem();
    child = this->parse_BType(depth + 1);
    child->setParent(constDecl);
    constDecl->addChild(child);
    // ConstDef
    child = this->parse_ConstDef(depth + 1);
    child->setParent(constDecl);
    constDecl->addChild(child);
    // { ',' ConstDef }
    while (this->curToken->symbol == Symbol::COMMA) {
        constDecl->addChild(new Node(this->curToken, constDecl, depth + 1));
        this->nextItem();
        child = this->parse_ConstDef(depth + 1);
        constDecl->addChild(child);
    }
    // ';'
    assert(this->curToken->symbol == Symbol::SEMICN);
    constDecl->addChild(new Node(this->curToken, constDecl, depth + 1));
    this->nextItem();
    return constDecl;
}

/* VarDecl → BType VarDef { ',' VarDef } ';' */
Node *Parser::parse_VarDecl(int depth) {
    Node *varDecl = new Node(GrammarItem::VarDecl, depth);
    Node *child = nullptr;
    // BType
    child = parse_BType(depth + 1);
    child->setParent(varDecl);
    varDecl->addChild(child);
    // VarDef
    child = parse_VarDef(depth + 1);
    child->setParent(varDecl);
    varDecl->addChild(child);
    // { ',' VarDef }
    while (this->curToken->symbol == Symbol::COMMA) {
        varDecl->addChild(new Node(this->curToken, varDecl, depth + 1));
        this->nextItem();

        child = parse_VarDef(depth + 1);
        child->setParent(varDecl);
        varDecl->addChild(child);
    }
    // ';'
    assert(this->curToken->symbol == Symbol::SEMICN);
    varDecl->addChild(new Node(this->curToken, varDecl, depth + 1));
    this->nextItem();
    return varDecl;
}

/* BType → 'int' */
Node *Parser::parse_BType(int depth) {
    Node *bType = new Node(GrammarItem::BType, depth);
    assert(this->curToken->symbol == Symbol::INTTK);
    bType->addChild(new Node(this->curToken, bType, depth));
    this->nextItem();
    return bType;
}

/* ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal */
Node *Parser::parse_ConstDef(int depth) {
    Node *constDef = new Node(GrammarItem::ConstDef, depth);
    Node *child = nullptr;
    // Ident
    child = parse_Ident(depth + 1);
    child->setParent(constDef);
    constDef->addChild(child);
    // { '[' ConstExp ']' }
    while (this->curToken->symbol == Symbol::LBRACK) {
        // '['
        constDef->addChild(new Node(this->curToken, constDef, depth + 1));
        this->nextItem();
        // ConstExp
        child = this->parse_ConstExp(depth + 1);
        child->setParent(constDef);
        constDef->addChild(child);
        // ']'
        assert(this->curToken->symbol == Symbol::RBRACK);
        constDef->addChild(new Node(this->curToken, constDef, depth + 1));
        this->nextItem();
    }
    // '='
    assert(this->curToken->symbol == Symbol::ASSIGN);
    constDef->addChild(new Node(this->curToken, constDef, depth + 1));
    this->nextItem();
    // ConstInitVal
    child = parse_ConstInitVal(depth + 1);
    child->setParent(constDef);
    constDef->addChild(child);
    return constDef;
}

Node *Parser::parse_Ident(int depth) {
    assert(this->curToken->symbol == Symbol::IDENFR);
    Node *ident = new Node(this->curToken, depth);
    this->nextItem();
    return ident;
}

/* ConstExp → AddExp */
Node *Parser::parse_ConstExp(int depth) {
    Node *constExp = new Node(GrammarItem::ConstExp, depth);
    Node *child = this->parse_AddExp(depth + 1);
    child->setParent(constExp);
    constExp->addChild(child);
    return constExp;
}

/* ConstInitVal → ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}' */
Node *Parser::parse_ConstInitVal(int depth) {
    Node *constInitVal = new Node(GrammarItem::ConstInitVal, depth);
    Node *child = nullptr;

    if (this->curToken->symbol != Symbol::LBRACE) {  // ConstExp
        child = parse_ConstExp(depth + 1);
        child->setParent(constInitVal);
        constInitVal->addChild(child);
    } else {  // '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
        constInitVal->addChild(new Node(this->curToken, constInitVal, depth + 1));
        this->nextItem();
        if (this->curToken->symbol != Symbol::RBRACE) {
            // ConstInitVal
            child = parse_ConstInitVal(depth + 1);
            child->setParent(constInitVal);
            constInitVal->addChild(child);
            // { ',' ConstInitVal }
            while (this->curToken->symbol == Symbol::COMMA) {
                // ','
                constInitVal->addChild(new Node(this->curToken, constInitVal, depth + 1));
                this->nextItem();
                // ConstInitVal
                child = parse_ConstInitVal(depth + 1);
                child->setParent(constInitVal);
                constInitVal->addChild(child);
            }
        }
        constInitVal->addChild(new Node(this->curToken, constInitVal, depth + 1));
        this->nextItem();
    }
    return constInitVal;
}

/* VarDef → Ident { '[' ConstExp ']' }  |  Ident { '[' ConstExp ']' } '=' InitVal */
Node *Parser::parse_VarDef(int depth) {
    Node *varDef = new Node(GrammarItem::VarDef, depth);
    Node *child = nullptr;
    // Ident
    child = parse_Ident(depth + 1);
    child->setParent(varDef);
    varDef->addChild(child);
    // { '[' ConstExp ']' }
    while (this->curToken->symbol == Symbol::LBRACK) {
        varDef->addChild(new Node(this->curToken, varDef, depth + 1));
        this->nextItem();

        child = parse_ConstExp(depth + 1);
        child->setParent(varDef);
        varDef->addChild(child);

        assert(this->curToken->symbol == Symbol::RBRACK);
        varDef->addChild(new Node(this->curToken, varDef, depth + 1));
        this->nextItem();
    }
    if (this->curToken->symbol == Symbol::ASSIGN) {  // '=' InitVal
        varDef->addChild(new Node(this->curToken, varDef, depth + 1));
        this->nextItem();

        child = parse_InitVal(depth + 1);
        child->setParent(varDef);
        varDef->addChild(child);
    }
    return varDef;
}

/* InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}' */
Node *Parser::parse_InitVal(int depth) {
    Node *initVal = new Node(GrammarItem::InitVal, depth);
    Node *child = nullptr;
    if (this->curToken->symbol == Symbol::LBRACE) {  // '{' [ InitVal { ',' InitVal } ] '}'
        // '{'
        initVal->addChild(new Node(this->curToken, initVal, depth + 1));
        this->nextItem();
        // [ InitVal { ',' InitVal } ]
        if (this->curToken->symbol != Symbol::RBRACE) {
            // InitVal
            child = this->parse_InitVal(depth + 1);
            child->setParent(initVal);
            initVal->addChild(child);
            // { ',' InitVal }
            while (this->curToken->symbol == Symbol::COMMA) {
                // ','
                initVal->addChild(new Node(this->curToken, initVal, depth + 1));
                this->nextItem();
                // InitVal
                child = this->parse_InitVal(depth + 1);
                child->setParent(initVal);
                initVal->addChild(child);
            }
        }
        // '}'
        initVal->addChild(new Node(this->curToken, initVal, depth + 1));
        this->nextItem();
    } else {
        // Exp
        child = this->parse_Exp(depth + 1);
        child->setParent(initVal);
        initVal->addChild(child);
    }
    return initVal;
}

/*  Exp → AddExp */
Node *Parser::parse_Exp(int depth) {
    Node *exp = new Node(GrammarItem::Exp, depth);
    Node *child = this->parse_AddExp(depth + 1);
    child->setParent(exp);
    exp->addChild(child);
    return exp;
}

/* FuncType → 'void' | 'int' */
Node *Parser::parse_FuncType(int depth) {
    Node *funcType = new Node(GrammarItem::FuncType, depth);
    assert(this->curToken->symbol == Symbol::VOIDTK || this->curToken->symbol == Symbol::INTTK);
    funcType->addChild(new Node(this->curToken, funcType, depth + 1));
    this->nextItem();
    return funcType;
}

/* FuncFParams → FuncFParam { ',' FuncFParam } */
Node *Parser::parse_FuncFParams(int depth) {
    Node *funcFParams = new Node(GrammarItem::FuncFParams, depth);
    Node *child = nullptr;
    // FuncFParam
    child = this->parse_FuncFParam(depth + 1);
    child->setParent(funcFParams);
    funcFParams->addChild(child);
    // { ',' FuncFParam }
    while (this->curToken->symbol == Symbol::COMMA) {
        // ','
        funcFParams->addChild(new Node(this->curToken, funcFParams, depth + 1));
        this->nextItem();
        // FuncFParam
        child = this->parse_FuncFParam(depth + 1);
        child->setParent(funcFParams);
        funcFParams->addChild(child);
    }
    return funcFParams;
}

/* Block → '{' { BlockItem } '}' */
Node *Parser::parse_Block(int depth) {
    Node *block = new Node(GrammarItem::Block, depth);
    Node *child = nullptr;
    // '{'
    assert(this->curToken->symbol == Symbol::LBRACE);
    block->addChild(new Node(this->curToken, block, depth + 1));
    this->nextItem();
    // { BlockItem }
    while (this->curToken->symbol != Symbol::RBRACE) {
        child = this->parse_BlockItem(depth + 1);
        child->setParent(block);
        block->addChild(child);
    }
    // '}'
    block->addChild(new Node(this->curToken, block, depth + 1));
    this->nextItem();
    return block;
}

/* FuncFParam → BType Ident ['[' ']' { '[' ConstExp ']' }] */
Node *Parser::parse_FuncFParam(int depth) {
    Node *funcFParam = new Node(GrammarItem::FuncFParam, depth);
    Node *child = nullptr;
    // BType
    child = this->parse_BType(depth + 1);
    child->setParent(funcFParam);
    funcFParam->addChild(child);
    // Ident
    child = this->parse_Ident(depth + 1);
    child->setParent(funcFParam);
    funcFParam->addChild(child);
    // ['[' ']' { '[' ConstExp ']' }]
    if (this->curToken->symbol == Symbol::LBRACK) {
        // '['
        funcFParam->addChild(new Node(this->curToken, funcFParam, depth + 1));
        this->nextItem();
        // ']'
        assert(this->curToken->symbol == Symbol::RBRACK);
        funcFParam->addChild(new Node(this->curToken, funcFParam, depth + 1));
        this->nextItem();
        while (this->curToken->symbol == Symbol::LBRACK) {
            funcFParam->addChild(new Node(this->curToken, funcFParam, depth + 1));
            this->nextItem();
            // ConstExp
            child = this->parse_ConstExp(depth + 1);
            child->setParent(funcFParam);
            funcFParam->addChild(child);
            // ']'
            assert(this->curToken->symbol == Symbol::RBRACK);
            funcFParam->addChild(new Node(this->curToken, funcFParam, depth + 1));
            this->nextItem();
        }
    }
    return funcFParam;
}

/* BlockItem → Decl | Stmt */
Node *Parser::parse_BlockItem(int depth) {
    Node *blockItem = new Node(GrammarItem::BlockItem, depth);
    Node *child = nullptr;
    // Decl → ConstDecl | VarDecl, 以 const 或 int 开头
    if (this->curToken->symbol == Symbol::CONSTTK || this->curToken->symbol == Symbol::INTTK) {
        child = this->parse_Decl(depth + 1);
    } else {
        child = this->parse_Stmt(depth + 1);
    }
    child->setParent(blockItem);
    blockItem->addChild(child);
    return blockItem;
}

/*
 * Stmt → LVal '=' Exp ';'
 *      | [Exp] ';'
 *      | Block
 *      | 'if' '(' Cond ')' Stmt [ 'else' Stmt ]
 *      | 'while' '(' Cond ')' Stmt
 *      | 'break' ';'
 *      | 'continue' ';'
 *      | 'return' [Exp] ';'
 *      | LVal '=' 'getint''('')'';'
 *      | 'printf''('FormatString{','Exp}')'';'
 */
Node *Parser::parse_Stmt(int depth) {
    Node *stmt = new Node(GrammarItem::Stmt, depth);
    Node *child = nullptr;
    if (this->curToken->symbol == Symbol::PRINTFTK) {  /* 'printf''('FormatString{','Exp}')'';' */
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
        // '('
        assert(this->curToken->symbol == Symbol::LPARENT);
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
        // FormatString
        child = this->parse_FormatString(depth + 1);
        child->setParent(stmt);
        stmt->addChild(child);
        // {','Exp}
        while (this->curToken->symbol == Symbol::COMMA) {
            stmt->addChild(new Node(this->curToken, stmt, depth + 1));
            this->nextItem();
            // Exp
            child = this->parse_Exp(depth + 1);
            child->setParent(stmt);
            stmt->addChild(child);
        }
        // ')'
        assert(this->curToken->symbol == Symbol::RPARENT);
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
        // ';'
        assert(this->curToken->symbol == Symbol::SEMICN);
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
    } else if (this->curToken->symbol == Symbol::RETURNTK) {  /* 'return' [Exp] ';' */
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
        // [Exp]
        if (this->curToken->symbol != Symbol::SEMICN) {
            child = this->parse_Exp(depth + 1);
            child->setParent(stmt);
            stmt->addChild(child);
        }
        // ';'
        assert(this->curToken->symbol == Symbol::SEMICN);
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
    } else if (this->curToken->symbol == Symbol::BREAKTK ||
               this->curToken->symbol == Symbol::CONTINUETK) {  /* 'break' ';' | 'continue' ';' */
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
        // ';'
        assert(this->curToken->symbol == Symbol::SEMICN);
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
    } else if (this->curToken->symbol == Symbol::WHILETK) {  /* 'while' '(' Cond ')' Stmt */
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
        // '('
        assert(this->curToken->symbol == Symbol::LPARENT);
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
        // Cond
        child = this->parse_Cond(depth + 1);
        child->setParent(stmt);
        stmt->addChild(child);
        // ')'
        assert(this->curToken->symbol == Symbol::RPARENT);
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
        // Stmt
        child = this->parse_Stmt(depth + 1);
        child->setParent(stmt);
        stmt->addChild(child);
    } else if (this->curToken->symbol == Symbol::IFTK) {  /* 'if' '(' Cond ')' Stmt [ 'else' Stmt ] */
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
        // '('
        assert(this->curToken->symbol == Symbol::LPARENT);
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
        // Cond
        child = this->parse_Cond(depth + 1);
        child->setParent(stmt);
        stmt->addChild(child);
        // ')'
        assert(this->curToken->symbol == Symbol::RPARENT);
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
        // Stmt
        child = this->parse_Stmt(depth + 1);
        child->setParent(stmt);
        stmt->addChild(child);
        // [ 'else' Stmt ]
        if (this->curToken->symbol == Symbol::ELSETK) {
            stmt->addChild(new Node(this->curToken, stmt, depth + 1));
            this->nextItem();
            // Stmt
            child = this->parse_Stmt(depth + 1);
            child->setParent(stmt);
            stmt->addChild(child);
        }
    } else if (this->curToken->symbol == Symbol::LBRACE) {  /* Block */
        child = this->parse_Block(depth + 1);
        child->setParent(stmt);
        stmt->addChild(child);
    } else if (this->semicn_before_assign()) {  /* 分号比等号先出现: [Exp] ';' */
        if (this->curToken->symbol != Symbol::SEMICN) {
            child = this->parse_Exp(depth + 1);
            child->setParent(stmt);
            stmt->addChild(child);
        }
        // ';'
        assert(this->curToken->symbol == Symbol::SEMICN);
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
    } else {  /* LVal '=' Exp ';'  | LVal '=' 'getint''('')'';' */
        // LVal
        child = this->parse_LVal(depth + 1);
        child->setParent(stmt);
        stmt->addChild(child);
        // '='
        assert(this->curToken->symbol == Symbol::ASSIGN);
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
        if (this->curToken->symbol != Symbol::GETINTTK) {  // LVal '=' Exp ';'
            // Exp
            child = this->parse_Exp(depth + 1);
            child->setParent(stmt);
            stmt->addChild(child);
        } else {  // LVal '=' 'getint''('')'';'
            stmt->addChild(new Node(this->curToken, stmt, depth + 1));
            this->nextItem();
            // '('
            assert(this->curToken->symbol == Symbol::LPARENT);
            stmt->addChild(new Node(this->curToken, stmt, depth + 1));
            this->nextItem();
            // ')'
            assert(this->curToken->symbol == Symbol::RPARENT);
            stmt->addChild(new Node(this->curToken, stmt, depth + 1));
            this->nextItem();
        }
        // ';'
        assert(this->curToken->symbol == Symbol::SEMICN);
        stmt->addChild(new Node(this->curToken, stmt, depth + 1));
        this->nextItem();
    }
    return stmt;
}

Node *Parser::parse_FormatString(int depth) {
    assert(this->curToken->symbol == Symbol::STRCON);
    Node *formatString = new Node(this->curToken, depth);
    this->nextItem();
    return formatString;
}

/* Cond → LOrExp */
Node *Parser::parse_Cond(int depth) {
    Node *cond = new Node(GrammarItem::Cond, depth);
    Node *child = this->parse_LOrExp(depth + 1);
    child->setParent(cond);
    cond->addChild(child);
    return cond;
}

bool Parser::semicn_before_assign() {
    /* 判断接下来先出现 ';'还是 '=' */
    /* 用于Stmt解析中判断是表达式还是赋值 */
    int i = this->tokenPos;
    while (this->tokens[i]->symbol != Symbol::SEMICN &&
           this->tokens[i]->symbol != Symbol::ASSIGN)
        ++i;
    if (this->tokens[i]->symbol == Symbol::SEMICN) return true;
    return false;
}

/* LVal → Ident {'[' Exp ']'} */
Node *Parser::parse_LVal(int depth) {
    Node *lVal = new Node(GrammarItem::LVal, depth);
    Node *child = this->parse_Ident(depth + 1);  // Ident
    child->setParent(lVal);
    lVal->addChild(child);
    // {'[' Exp ']'}
    while (this->curToken->symbol == Symbol::LBRACK) {
        lVal->addChild(new Node(this->curToken, lVal, depth + 1));
        this->nextItem();
        // Exp
        child = this->parse_Exp(depth + 1);
        child->setParent(lVal);
        lVal->addChild(child);
        // ']'
        assert(this->curToken->symbol == Symbol::RBRACK);
        lVal->addChild(new Node(this->curToken, lVal, depth + 1));
        this->nextItem();
    }
    return lVal;
}

/* PrimaryExp → '(' Exp ')' | LVal | Number */
Node *Parser::parse_PrimaryExp(int depth) {
    Node *primaryExp = new Node(GrammarItem::PrimaryExp, depth);
    Node *child = nullptr;
    if (this->curToken->symbol == Symbol::LPARENT) {  // '(' Exp ')'
        primaryExp->addChild(new Node(this->curToken, primaryExp, depth + 1));
        this->nextItem();
        // Exp
        child = this->parse_Exp(depth + 1);
        child->setParent(primaryExp);
        primaryExp->addChild(child);
        // ')'
        assert(this->curToken->symbol == Symbol::RPARENT);
        primaryExp->addChild(new Node(this->curToken, primaryExp, depth + 1));
        this->nextItem();
    } else if (this->curToken->symbol == Symbol::IDENFR) {  // LVal  (LVal → Ident {'[' Exp ']'})
        child = this->parse_LVal(depth + 1);
        child->setParent(primaryExp);
        primaryExp->addChild(child);
    } else {  // Number
        child = this->parse_Number(depth + 1);
        child->setParent(primaryExp);
        primaryExp->addChild(child);
    }
    return primaryExp;
}

/* Number → IntConst */
Node *Parser::parse_Number(int depth) {
    Node *number = new Node(GrammarItem::Number, depth);
    Node *child = this->parse_IntConst(depth + 1);
    child->setParent(number);
    number->addChild(child);
    return number;
}

Node *Parser::parse_IntConst(int depth) {
    assert(this->curToken->symbol == Symbol::INTCON);
    Node *intConst = new Node(this->curToken, depth);
    this->nextItem();
    return intConst;
}

/* UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp  */
Node *Parser::parse_UnaryExp(int depth) {
    Node *unaryExp = new Node(GrammarItem::UnaryExp, depth);
    Node *child = nullptr;
    if (this->curToken->symbol == Symbol::PLUS
        || this->curToken->symbol == Symbol::MINU
        || this->curToken->symbol == Symbol::NOT) {  // UnaryOp UnaryExp
        child = this->parse_UnaryOp(depth + 1);
        child->setParent(unaryExp);
        unaryExp->addChild(child);
        child = this->parse_UnaryExp(depth + 1);
        child->setParent(unaryExp);
        unaryExp->addChild(child);
    } else if (this->curToken->symbol == Symbol::IDENFR
               && this->tokens[tokenPos + 1]->symbol == Symbol::LPARENT) {  // Ident '(' [FuncRParams] ')'
        child = this->parse_Ident(depth + 1);
        child->setParent(unaryExp);
        unaryExp->addChild(child);
        // '('
        assert(this->curToken->symbol == Symbol::LPARENT);
        unaryExp->addChild(new Node(this->curToken, unaryExp, depth + 1));
        this->nextItem();
        if (this->curToken->symbol != Symbol::RPARENT) {  // [FuncRParams]
            child = this->parse_FuncRParams(depth + 1);
            child->setParent(unaryExp);
            unaryExp->addChild(child);
        }
        // ')'
        assert(this->curToken->symbol == Symbol::RPARENT);
        unaryExp->addChild(new Node(this->curToken, unaryExp, depth + 1));
        this->nextItem();
    } else {  // PrimaryExp
        child = this->parse_PrimaryExp(depth + 1);
        child->setParent(unaryExp);
        unaryExp->addChild(child);
    }
    return unaryExp;
}

/* UnaryOp → '+' | '−' | '!' */
Node *Parser::parse_UnaryOp(int depth) {
    Node *unaryOp = new Node(GrammarItem::UnaryOp, depth);
    assert(this->curToken->symbol == Symbol::PLUS
           || this->curToken->symbol == Symbol::MINU
           || this->curToken->symbol == Symbol::NOT);
    unaryOp->addChild(new Node(this->curToken, unaryOp, depth + 1));
    this->nextItem();
    return unaryOp;
}

/* FuncRParams → Exp { ',' Exp } */
Node *Parser::parse_FuncRParams(int depth) {
    Node *exp = new Node(GrammarItem::FuncRParams, depth);
    Node *child = nullptr;
    // Exp
    child = this->parse_Exp(depth + 1);
    child->setParent(exp);
    exp->addChild(child);
    // { ',' Exp }
    while (this->curToken->symbol == Symbol::COMMA) {
        exp->addChild(new Node(this->curToken, exp, depth + 1));
        this->nextItem();
        // Exp
        child = this->parse_Exp(depth + 1);
        child->setParent(exp);
        exp->addChild(child);
    }
    return exp;
}

/* MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp  FIXME: 左递归 */
/* 改写后： MulExp -> UnaryExp {('*' | '/' | '%') UnaryExp} */
Node *Parser::parse_MulExp(int depth) {
    Node *current = new Node(GrammarItem::MulExp, depth);
    Node *child = this->parse_UnaryExp(depth + 1);
    current->addChild(child);
    child->setParent(current);
    while (this->curToken->symbol == Symbol::MULT
           || this->curToken->symbol == Symbol::DIV
           || this->curToken->symbol == Symbol::MOD) {
        // 在发现右侧还有成分时，先将左侧向上打包一层, FIXME: 这里depth有误
        Node *temp = new Node(GrammarItem::MulExp, depth);
        temp->addChild(current);
        current->setParent(temp);
        current = temp;
        // ('*' | '/' | '%')
        current->addChild(new Node(this->curToken, current, depth + 1));
        this->nextItem();
        // UnaryExp
        child = this->parse_UnaryExp(depth + 1);
        current->addChild(child);
    }
    // TODO: 递归修改 current 的depth
    return current;
}

/* AddExp → MulExp | AddExp ('+' | '−') MulExp FIXME: 左递归 */
/* 改写后: AddExp → MulExp {('+' | '−') MulExp} */
Node *Parser::parse_AddExp(int depth) {
    Node *current = new Node(GrammarItem::AddExp, depth);
    Node *child = this->parse_MulExp(depth + 1);
    current->addChild(child);
    child->setParent(current);
    while (this->curToken->symbol == Symbol::PLUS
           || this->curToken->symbol == Symbol::MINU) {
        // 在发现右侧还有成分时，先将左侧向上打包一层, FIXME: 这里depth有误
        Node *temp = new Node(GrammarItem::AddExp, depth);
        temp->addChild(current);
        current->setParent(temp);
        current = temp;
        // ('+' | '−')
        current->addChild(new Node(this->curToken, current, depth + 1));
        this->nextItem();
        // MulExp
        child = this->parse_MulExp(depth + 1);
        current->addChild(child);
    }
    // TODO: 递归修改 current 的depth
    return current;
}

/* RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp FIXME: 左递归 */
/* 改写后： RelExp → AddExp {('<' | '>' | '<=' | '>=') AddExp} */
Node *Parser::parse_RelExp(int depth) {
    Node *current = new Node(GrammarItem::RelExp, depth);
    Node *child = this->parse_AddExp(depth + 1);
    current->addChild(child);
    child->setParent(current);
    while (this->curToken->symbol == Symbol::LEQ
           || this->curToken->symbol == Symbol::LSS
           || this->curToken->symbol == Symbol::GEQ
           || this->curToken->symbol == Symbol::GRE) {
        // 在发现右侧还有成分时，先将左侧向上打包一层, FIXME: 这里depth有误
        Node *temp = new Node(GrammarItem::RelExp, depth);
        temp->addChild(current);
        current->setParent(temp);
        current = temp;
        // ('<' | '>' | '<=' | '>=')
        current->addChild(new Node(this->curToken, current, depth + 1));
        this->nextItem();
        // AddExp
        child = this->parse_AddExp(depth + 1);
        current->addChild(child);
    }
    // TODO: 递归修改 current 的depth
    return current;
}

/* EqExp → RelExp | EqExp ('==' | '!=') RelExp FIXME: 左递归 */
/* 改写后： EqExp → RelExp {('==' | '!=') RelExp} */
Node *Parser::parse_EqExp(int depth) {
    Node *current = new Node(GrammarItem::EqExp, depth);
    Node *child = this->parse_RelExp(depth + 1);
    current->addChild(child);
    child->setParent(current);
    while (this->curToken->symbol == Symbol::EQL
           || this->curToken->symbol == Symbol::NEQ) {
        // 在发现右侧还有成分时，先将左侧向上打包一层, FIXME: 这里depth有误
        Node *temp = new Node(GrammarItem::EqExp, depth);
        temp->addChild(current);
        current->setParent(temp);
        current = temp;
        // ('==' | '!=')
        current->addChild(new Node(this->curToken, current, depth + 1));
        this->nextItem();
        // RelExp
        child = this->parse_RelExp(depth + 1);
        current->addChild(child);
    }
    // TODO: 递归修改 current 的depth
    return current;
}

/* LAndExp → EqExp | LAndExp '&&' EqExp FIXME: 左递归 */
/* 改写后: LAndExp → EqExp {'&&' EqExp} */
Node *Parser::parse_LAndExp(int depth) {
    Node *current = new Node(GrammarItem::LAndExp, depth);
    Node *child = this->parse_EqExp(depth + 1);
    current->addChild(child);
    child->setParent(current);
    while (this->curToken->symbol == Symbol::AND) {
        // 在发现右侧还有成分时，先将左侧向上打包一层, FIXME: 这里depth有误
        Node *temp = new Node(GrammarItem::LAndExp, depth);
        temp->addChild(current);
        current->setParent(temp);
        current = temp;
        // '&&'
        current->addChild(new Node(this->curToken, current, depth + 1));
        this->nextItem();
        // EqExp
        child = this->parse_EqExp(depth + 1);
        current->addChild(child);
    }
    // TODO: 递归修改 current 的depth
    return current;
}

/* LOrExp → LAndExp | LOrExp '||' LAndExp FIXME: 左递归 */
/* 改写后： LOrExp → LAndExp {'||' LAndExp} */
Node *Parser::parse_LOrExp(int depth) {
    Node *current = new Node(GrammarItem::LOrExp, depth);
    Node *child = this->parse_LAndExp(depth + 1);
    current->addChild(child);
    child->setParent(current);
    while (this->curToken->symbol == Symbol::OR) {
        // 在发现右侧还有成分时，先将左侧向上打包一层, FIXME: 这里depth有误
        Node *temp = new Node(GrammarItem::LOrExp, depth);
        temp->addChild(current);
        current->setParent(temp);
        current = temp;
        // '||'
        current->addChild(new Node(this->curToken, current, depth + 1));
        this->nextItem();
        // LAndExp
        child = this->parse_LAndExp(depth + 1);
        current->addChild(child);
    }
    // TODO: 递归修改 current 的depth
    return current;
}


