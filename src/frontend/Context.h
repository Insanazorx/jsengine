#pragma once
#include "../Util.h"
#include <optional>
#include "Forward.h"
#include "../Validator.h"

namespace js{

class Context{};
    class ParserContext : public Context {
        friend class Parser;
    private:
        ParserContext(Parser* obj): parserObj(obj) , m_syntax_validator(new SyntaxValidator()) {}
    public:
        static ParserContext *Create(Parser* obj) {
            return new ParserContext(obj);
        }
        bool isAnalyzingLoopDone() {return m_MainCallStack.Size() <= 2 && m_MainCallStack.Peek() == "StatementParser";}
        void PushCallStack(std::string funcName) {m_MainCallStack.Push(std::move(funcName));}
        void PopCallStack() {m_MainCallStack.Pop();}
        int CallStackCount() {return m_MainCallStack.Size();}
        void incConsumeCount() {m_ConsumeCountFromMinorCounters++;}
        void decConsumeCount() {m_ConsumeCountFromMinorCounters--;}
        int ConsumeCountFromMinorCounters() const {return m_ConsumeCountFromMinorCounters;}


        void PushToWaitingStack(Statement* statement) {m_WaitingReceiverStatementStack.Push(statement);}
        void PopFromWaitingStack() {m_WaitingReceiverStatementStack.Pop();}

        Statement* TopOfWaitingStack() {return m_WaitingReceiverStatementStack.Peek();}

        Statement* RedirectWaitingStatement() {
            if (m_WaitingReceiverStatementStack.Size() > 0) {
                return m_WaitingReceiverStatementStack.Pop();
            }
            return nullptr;
        }

        bool isThereAnyWaitingStatement () {
            if (m_WaitingReceiverStatementStack.Size() > 0) {
                return true;
            }
            return false;
        }

        Statement* SendStatement() {
            if (m_StatementStackToSendReceiver.size() > 0) {
                auto ret = m_StatementStackToSendReceiver.back();
                m_StatementStackToSendReceiver.pop_back();
                return ret;
            }
            VERIFY_NOT_REACHED();
        }

        Validator* SyntacticValidator() const {return m_syntax_validator;}

        void PushScopeStatementBody(std::vector<Token> tokens) {m_ScopeStatementBodyStack.Push(tokens);}
        std::vector<Token> PopScopeStatementBody() {return std::move(m_ScopeStatementBodyStack.Pop());}
        std::vector<std::vector<Token>> MoveScopeStatementBodyToVariable() {
            std::vector<std::vector<Token>> result;
            for (auto tokenVector : m_ScopeStatementBodyStack.asVector()) {
                result.push_back(std::move(tokenVector));
            }
            return result;
        }

        void PushSendingStack(Statement* statement) {m_StatementStackToSendReceiver.push_back(statement);}
        bool isThereAnySendingStatement() {return m_StatementStackToSendReceiver.size() > 0;}
    private:

        Stack<std::vector<Token>> m_ScopeStatementBodyStack;
        std::deque<Statement*> m_StatementStackToSendReceiver;
        Stack<Statement*> m_WaitingReceiverStatementStack;
        Stack<std::string> m_MainCallStack;
        Parser* parserObj;
        Validator* m_syntax_validator;
        int m_ConsumeCountFromMinorCounters {0};
    };

}

