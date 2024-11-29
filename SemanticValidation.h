#pragma once

#include "Visitor.h"
#include "StdLib.h"
#include "AST.h"
#include "Stack.h"

namespace JSLib{

class SemanticValidator {
    class ValidationVisitor : public Visitor {

    };
    class Encapsulator {
    public:
        using Variable = int; //variable tanımla
        void InitScope() {}
        void EncapsulateWithScope (NodeBranchInfo& info) {}
        void EncapsulateWithScope (Node* node) {}
        void PushLocal() {}
        class ScopeChecker : public Visitor {
        public:
            bool CheckScopeViolation() {}
        };
        ScopeChecker& checker() {return m_Checker;}
        std::list<Variable> ExtractLocals() {return Locals.ConvertTostd::list();}
    private:
        Util::Stack<Variable> Locals;
        ScopeChecker m_Checker;
    };
};

}

