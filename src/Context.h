#pragma once
#include "Util.h"
#include <optional>
#include "Forward.h"
#include "Validator.h"

namespace JSLib{

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
            if (m_StatementStackToSendReceiver.Size() > 0) {
                return m_StatementStackToSendReceiver.Pop();
            }
            return nullptr;
        }

        Validator* SyntacticValidator() const {return m_syntax_validator;}


        void PushSendingStack(Statement* statement) {m_StatementStackToSendReceiver.Push(statement);}
        bool isThereAnySendingStatement() {return m_StatementStackToSendReceiver.Size() > 0;}
    private:

        Stack<Statement*> m_StatementStackToSendReceiver;
        Stack<Statement*> m_WaitingReceiverStatementStack;
        Stack<std::string> m_MainCallStack;
        Parser* parserObj;
        Validator* m_syntax_validator;
        int m_ConsumeCountFromMinorCounters {0};
    };

}

