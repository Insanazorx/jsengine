#pragma once

#include "AST.h"

namespace JSLib {

    class SyntaxValidator {
    public:
        SyntaxValidator() = default;
        ~SyntaxValidator() = default;
        static SyntaxValidator* Create() {return new SyntaxValidator();}
        int Validate(ASTNode* node) {
        }

    };
}

