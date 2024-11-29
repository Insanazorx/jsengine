#pragma once
#include "Stack.h"

namespace JSLib{
class Parser;
class Node;
class Context{};
    class ParserContext : public Context {;
        friend class Parser;
    private:
        ParserContext(Parser* obj, Node* (Parser::*func)(std::optional<std::list<Token*>>& ,ParserContext*) ): parserObj(obj) , LoopStartingPoint((func)) {}
    public:
        static ParserContext *Create(Parser* obj, Node* (Parser::*func)(std::optional<std::list<Token*>>&, ParserContext*)) {
            return new ParserContext(obj, func);
        }
        bool isAnalyzingLoopDone() {return m_MainCallStack.Size() <= 2 && m_MainCallStack.Peek() == "StatementParser";}
        void PushCallStack(std::string* funcName) {m_MainCallStack.Push(std::move(*funcName));}
        void PopCallStack() {m_MainCallStack.Pop();}
        int CallStackCount() {return m_MainCallStack.Size();}
        Node* CallAnalyze(std::optional<std::list<Token*>>& arg, ParserContext* context) {return (parserObj->*LoopStartingPoint)(arg,context);}
        Parser* ParserObj() {return parserObj;};


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
        Util::Stack<std::string> m_MainCallStack;
        int InWhichDepthGenerateImmediateASTInnerLoopReturnedFalse;
        Util::Stack<std::string> m_BinaryOpGenerateImmediateASTcallStack;
        Parser* parserObj;
        Node* (Parser::*LoopStartingPoint)(std::optional<std::list<Token*>>& ,ParserContext*); //bunu Function class ile yaz

    };

}

