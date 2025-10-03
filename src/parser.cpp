#include <Parser.hpp>
#include <Scanner.hpp>
#include <Operation.hpp>

InternalRepresentation Parser::parse() {
    LinkedList<Operation>* representation = new LinkedList<Operation>();
    int error = 0;
    int count = 0;

    Token token = scanner.nextToken();    
    while (token.category != Category::CAT_EOF) {
        this->line++;
        switch (token.category) {
            case Category::CAT_MEMOP:
                try {
                    representation->append(this->finishMEMOP(static_cast<Opcode>(token.lexeme)));
                    count++;
                } catch (const InvalidTokenException& e) {
                    this->handleInvalidToken(e);
                    error++;
                }    
                break;
            case Category::CAT_LOADI:
                try {
                    representation->append(finishLOADI(static_cast<Opcode>(token.lexeme)));
                    count++;
                } catch (const InvalidTokenException& e) {
                    this->handleInvalidToken(e);
                    error++;
                }  
                break;
            case Category::CAT_ARITHOP:
                try {
                    representation->append(finishARITHOP(static_cast<Opcode>(token.lexeme)));
                    count++;
                } catch (const InvalidTokenException& e) {
                    this->handleInvalidToken(e);
                    error++;
                }  
                break;
            case Category::CAT_OUTPUT:
                try {
                    representation->append(finishOUTPUT(static_cast<Opcode>(token.lexeme)));
                    count++;
                } catch (const InvalidTokenException& e) {
                    this->handleInvalidToken(e);
                    error++;
                }  
                break;
            case Category::CAT_NOP:
                try {
                    representation->append(finishNOP(static_cast<Opcode>(token.lexeme)));
                    count++;
                } catch (const InvalidTokenException& e) {
                    this->handleInvalidToken(e);
                    error++;
                }  
                break;
            case Category::CAT_EOL:
                break;
            default:
                this->handleInvalidToken(InvalidTokenException(token, "Operation starts with an invalid opcode."));
                error++;
        }
        token = scanner.nextToken();
    }

    if (error > 0) {
        throw ParseFailedException("Parse failed with " + std::to_string(error) + " errors.");
    }
    return {count, representation};
}

Operation Parser::finishMEMOP(Opcode opcode) {
    
    Operation op;
    op.line = this->line;
    op.opcode = opcode;

    Token token = scanner.nextToken();
    if (token.category != Category::CAT_REGISTER) {
        throw InvalidTokenException(token, "Missing source register in MEMOP.");
    }
    op.op1.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_INTO) {
        throw InvalidTokenException(token, "Missing \"=>\" in MEMOP.");
    }

    token = scanner.nextToken();
    if (token.category != Category::CAT_REGISTER) {
        throw InvalidTokenException(token, "Missing target register in MEMOP.");
    }
    op.op3.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_EOL) {
        throw InvalidTokenException(token, "Extra token at end of line in MEMOP.");
    }

    return op;
}

Operation Parser::finishLOADI(Opcode opcode) {
    
    Operation op;
    op.line = this->line;
    op.opcode = opcode;

    Token token = scanner.nextToken();
    if (token.category != Category::CAT_CONSTANT) {
        throw InvalidTokenException(token, "Missing constant in LOADI.");
    }
    op.op1.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_INTO) {
        throw InvalidTokenException(token, "Missing \"=>\" in LOADI.");
    }

    token = scanner.nextToken();
    if (token.category != Category::CAT_REGISTER) {
        throw InvalidTokenException(token, "Missing target register in LOADI.");
    }
    op.op3.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_EOL) {
        throw InvalidTokenException(token, "Extra token at end of line in LOADI.");
    }

    return op;
}

Operation Parser::finishARITHOP(Opcode opcode) {
    
    Operation op;
    op.line = this->line;
    op.opcode = opcode;

    Token token = scanner.nextToken();
    if (token.category != Category::CAT_REGISTER) {
        throw InvalidTokenException(token, "Missing first source register in ARITHOP.");
    }
    op.op1.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_COMMA) {
        throw InvalidTokenException(token, "Missing \",\" in ARITHOP.");
    }

    token = scanner.nextToken();
    if (token.category != Category::CAT_REGISTER) {
        throw InvalidTokenException(token, "Missing second source register in ARITHOP.");
    }
    op.op2.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_INTO) {
        throw InvalidTokenException(token, "Missing \"=>\" in ARITHOP.");
    }

    token = scanner.nextToken();
    if (token.category != Category::CAT_REGISTER) {
        throw InvalidTokenException(token, "Missing target register in ARITHOP.");
    }
    op.op3.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_EOL) {
        throw InvalidTokenException(token, "Extra token at end of line in ARITHOP.");
    }

    return op;
}

Operation Parser::finishOUTPUT(Opcode opcode) {
    
    Operation op;
    op.line = this->line;
    op.opcode = opcode;

    Token token = scanner.nextToken();
    if (token.category != Category::CAT_CONSTANT) {
        throw InvalidTokenException(token, "Missing constant in OUTPUT.");
    }
    op.op1.SR = token.lexeme;

    token = scanner.nextToken();
    if (token.category != Category::CAT_EOL) {
        throw InvalidTokenException(token, "Extra token at end of line in OUTPUT.");
    }

    return op;
}

Operation Parser::finishNOP(Opcode opcode) {
    Operation op;
    op.line = this->line;
    op.opcode = opcode;

    Token token = scanner.nextToken();
    if (token.category != Category::CAT_EOL) {
        throw InvalidTokenException(token, "Extra token at end of line in NOP.");
    }

    return op;
}

void Parser::readToNextLine () {
    Token token = scanner.nextToken();
    while (token.category != Category::CAT_EOL) {
        if (token.category == Category::CAT_EOF) {
            throw new UnexpectedEOFException("Unexpected EOF at line " + std::to_string(this->line));
        }
        token = scanner.nextToken();
    }
}

void Parser::handleInvalidToken(const InvalidTokenException& e) {
    std::cerr << "ERROR " << this->line << ": "  << e.what() << std::endl;
    if (e.getToken().category == Category::CAT_EOF) {
        throw new UnexpectedEOFException("Unexpected EOF at line " + std::to_string(this->line));
    } else if (e.getToken().category != Category::CAT_EOL) {
        this->readToNextLine();
    }
}
