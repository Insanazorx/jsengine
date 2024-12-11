#pragma once
#include "Util.h"
#include <optional>

namespace JSLib{
class Parser;
class Node;
class Context{};
    class ParserContext : public Context {
        friend class Parser;
    private:
        ParserContext(Parser* obj): parserObj(obj) {}
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
        Parser* ParserObj() {return parserObj;};
        bool isBinaryOpExprParsingStartedOnce() {return m_isBinaryOpExprParsingStartedOnce;}
        void ResetBinaryOpExprParsingState() {m_isBinaryOpExprParsingStartedOnce = false;}
        void SetBinaryOpExprParsingState() {m_isBinaryOpExprParsingStartedOnce = true;}

        void PushBinaryOpGenerateImmediateASTCallStack(std::string* funcName) {
            m_BinaryOpGenerateImmediateASTcallStack.Push(std::move(*funcName));
        };
        void PopBinaryOpGenerateImmediateASTcallStack() {
            m_BinaryOpGenerateImmediateASTcallStack.Pop();
        }
        std::string& PeekForParentFunctionName() {
            auto name = m_BinaryOpGenerateImmediateASTcallStack.Peek();
            return name;
        }

        void IncDepth(){InWhichDepthGenerateImmediateASTInnerLoopReturnedFalse++;}
        void ResetDepth(){InWhichDepthGenerateImmediateASTInnerLoopReturnedFalse=0;}
        int GetDepth(){return InWhichDepthGenerateImmediateASTInnerLoopReturnedFalse;}

    private:
        Stack<std::string> m_MainCallStack;
        int InWhichDepthGenerateImmediateASTInnerLoopReturnedFalse;
        Stack<std::string> m_BinaryOpGenerateImmediateASTcallStack;
        Parser* parserObj;
        int m_ConsumeCountFromMinorCounters {0};
        bool m_isBinaryOpExprParsingStartedOnce {false};
    };

}

