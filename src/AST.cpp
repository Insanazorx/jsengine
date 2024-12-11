#include "AST.h"
#include "Parser.h"

#include <optional>
#include <vector>
#include <unordered_map>

namespace JSLib {
    NodeBranchInfo* IfStatement::ParseTokens(ParserContext* context) {
        context->PushCallStack(FUNCTION_NAME());

        auto NewInfo = NodeBranchInfo::Create();

        auto& TestTokens =*new std::optional<std::vector<Token>>(this->GetTestTokens());
        auto& ConsequentTokens = *new std::optional<std::vector<Token>>(this->GetConseqTokens());
        auto& AlternateTokens = *new std::optional<std::vector<Token>> (this->GetAlternateTokens());

        Token EosToken = *new Token();
        EosToken.Type = TokenType::END_OF_STREAM;

        TestTokens.value().push_back(EosToken);
        ConsequentTokens.value().push_back(EosToken);

        if (AlternateTokens)
            AlternateTokens.value().push_back(EosToken);

        bool AnalyzedThisDepth = false;
        ASTNode* TestNode = nullptr;
        ASTNode* ConsequentNode = nullptr;
        ASTNode* AlternateNode = nullptr;

        if (!AnalyzedThisDepth) { //burası multithread olabilir
            TestNode = Parser::Instance()->RecognizeStatementOrRedirectNode(TestTokens, context);

            VERIFY(TestNode,"TestNode returned nullptr");

            ConsequentNode = Parser::Instance()->RecognizeStatementOrRedirectNode(ConsequentTokens, context);

            VERIFY(ConsequentNode, "ConsequentNode returned nullptr");

            if (AlternateTokens) {
                AlternateNode = Parser::Instance()->RecognizeStatementOrRedirectNode(AlternateTokens, context);
                VERIFY(AlternateNode, "AlternateNode returned nullptr");
            }

            AnalyzedThisDepth = true;
        }

        NewInfo->Shape()->Builder().AddChild(TestNode);
        NewInfo->Shape()->Builder().GotoParent();

        NewInfo->Shape()->Builder().AddChild({});

        for (auto node : ConsequentNode->Children()) {
            NewInfo->Shape()->Builder().AddChild(node);
            NewInfo->Shape()->Builder().GotoParent();
        }

        NewInfo->Shape()->Builder().GotoParent();

        auto alternateStatementHolder = ASTNode::Create();
        alternateStatementHolder->SetValue("Alternate Statement Holder");
        NewInfo->Shape()->Builder().AddChild(alternateStatementHolder);

        for (auto node: AlternateNode->Children()) {
            NewInfo->Shape()->Builder().AddChild(node);
            NewInfo->Shape()->Builder().GotoParent();
        }

        NewInfo->Shape()->Builder().GotoParent();

        context->PopCallStack();
        return NewInfo;
    }

    NodeBranchInfo* ForStatement::ParseTokens(ParserContext* context) {


        auto NewInfo = NodeBranchInfo::Create();

        std::vector<Token> InitialConditionTokens = this->GetInitialConditionTokens();
        auto TestTokens = this->GetTestTokens();
        auto IterationTokens = this->GetIterationTokens();
        auto ConsequentTokens = this->GetConseqTokens();

        ASTNode* InitialConditionNode;
        ASTNode* TestNode;
        DummyNode* IterationNode;
        DummyNode* ConsequentNode;


        bool AnalyzedThisDepth = false;

        if (!AnalyzedThisDepth) {



            AnalyzedThisDepth = true;

        }
        /*
        NewInfo->Shape()->Builder().AddChild();
        NewInfo->Shape()->Builder().GotoParent();

        NewInfo->Shape()->Builder().AddChild();
        NewInfo->Shape()->Builder().GotoParent();

        auto IterationStatementHolder = Statement::Create();
        NewInfo->Shape()->Builder().AddChild();

        for (auto& statement: IterationStatements) {
            NewInfo->Shape()->Builder().AddChild();
            NewInfo->Shape()->Builder().GotoParent();
        }

        NewInfo->Shape()->Builder().GotoParent();

        auto ConseqStatementHolder = Statement::Create();
        NewInfo->Shape()->Builder().AddChild();

        for (auto& statement: ConsequentStatements) {
            NewInfo->Shape()->Builder().AddChild(statement);
            NewInfo->Shape()->Builder().GotoParent();
        }

        NewInfo->Shape()->Builder().GotoParent();
*/
        return NewInfo;
    }


    ASTNode* BinaryOpStatement::GenerateASTImmediate(ParserContext* context) {

        context->PushBinaryOpGenerateImmediateASTCallStack(new std::string(FUNCTION_NAME()));

        auto InitialNode = ASTNode::Create();
        Parser::Instance()->Builder()->immBuilder()->SetCurrentNode(InitialNode);


        VERIFY(Parser::Instance()
            ->Builder()
            ->immBuilder()
            ->ParseInnerParentheses(
                this->GetTokenChain(),
                nullptr,
                context)
            ,"ParseInnerParentheses returned false in depth " << context->GetDepth() );

        context->ResetDepth();
        context->PopBinaryOpGenerateImmediateASTcallStack();

        return {};
    }


    void ASTBuilder::ImmediateBuilder::GoToNumericChild() {
        auto parent = CurrentNode();
        for (auto child : CurrentNode()->Children()) {
            if (isNumeric(child->Raw()))
                SetCurrentNode(child);
        }
        VERIFY(parent != CurrentNode(), "Could not get to numeric child!");
    }

    std::vector<Token>& ASTBuilder::ImmediateBuilder::EmptyOneExprFromStack() {
        auto retValue = *new std::vector<Token>();
        while (TokenStack().Peek().Type == TokenType::L_PARENTHESES) {
            if (TokenStack().Peek().Type == TokenType::R_PARENTHESES) {
                TokenStack().Pop();
                continue;
            }
            retValue.push_back(TokenStack().Pop());
        }
        return retValue;
    }

    bool ASTBuilder::ImmediateBuilder::ParseInnerParentheses(std::vector<Token>& TokenChain,CacheChain* CurrentChain, ParserContext* context) {

        context->PushBinaryOpGenerateImmediateASTCallStack(new std::string(FUNCTION_NAME()));
        context->IncDepth();

        auto GetPrecedence = [&](BinaryOpSubType subType) -> int {
            return static_cast<int>(subType);
        };

        std::unordered_map<std::string, BinaryOpSubType> OperationTypeMap;
        OperationTypeMap["+"] = BinaryOpSubType::ADD;
        OperationTypeMap["-"] = BinaryOpSubType::SUB;
        OperationTypeMap["*"] = BinaryOpSubType::MUL;
        OperationTypeMap["/"] = BinaryOpSubType::DIV;
        OperationTypeMap["%"] = BinaryOpSubType::MOD;
        OperationTypeMap["^"] = BinaryOpSubType::BITWISE_XOR;
        OperationTypeMap["|"] = BinaryOpSubType::BITWISE_OR;
        OperationTypeMap["&"] = BinaryOpSubType::BITWISE_AND;
        OperationTypeMap[">>"] = BinaryOpSubType::SHIFT_RIGHT;
        OperationTypeMap["<<"] = BinaryOpSubType::SHIFT_LEFT;


    VERIFY_ONCE(!CurrentChain, "CurrentChain should be empty first!");
    if (!CurrentChain) CurrentChain = tlChain();

        if (context->isBinaryOpExprParsingStartedOnce()) {
            CurrentChain->CurrentTLNodeOfBranch = ASTNode::Create();
            CurrentChain->CurrentTLNodeOfBranch->SetValue("FIRST NODE");
            SetCurrentNode(CurrentChain->CurrentTLNodeOfBranch->Children()[0]);
        }

    for (auto token : TokenChain) {
        if (token.Type == TokenType::L_PARENTHESES) {

            TokenStack().Push(token);

            continue;
        }

        if (token.Type == TokenType::R_PARENTHESES) {

            std::vector<Token> InnerTokens;

            if (isStackEmpty())
                return false;

            CurrentChain->AllocInnerChain();

            if (hasRemainedOneExprInStack()) {
                InnerTokens = EmptyOneExprFromStack();

                if (ParseInnerParentheses(InnerTokens,CurrentChain->InnerChain, context))
                    LinkCurrentTLNodeOfBranchToMainBranchNode();
                else return false;

            } else {
                InnerTokens = EmptyOneExprFromStack();

                if (ParseInnerParentheses(InnerTokens,CurrentChain->InnerChain,context))
                    LinkInnerParenthesesNodeToOuterOne(CurrentChain);
                else return false;
            }

            continue;
        }

        auto OpTypeIterator = OperationTypeMap.find(token.Lexeme);


        if (!isStackEmpty()) {
            if (OpTypeIterator != OperationTypeMap.end()) {
                auto OpNode = BinaryOpNode::Create();
                OpNode->SetSubType(OpTypeIterator->second);
                PushToNodeStack(OpNode);
            } else {
                auto NumericNode = ASTNode::Create();
                NumericNode->SetValue(token.Lexeme);
                PushToNodeStack(NumericNode);
            }
            continue;
        }

        if (OpTypeIterator != OperationTypeMap.end()) {

            auto NewNode = BinaryOpNode::Create();
            auto OpType = OpTypeIterator->second;
            NewNode->SetSubType(OpType);

            GoToNumericChild();

            VERIFY(CurrentNode()->hasParent(),"There must be parent node!");
            auto CurrentParent = dynamic_cast<BinaryOpNode*>(CurrentNode()->Parent());
            VERIFY(CurrentParent,"Parent node could not have been casted!");

            auto CurrentParentOpType = CurrentParent->SubType();

            if (GetPrecedence(OpType) > GetPrecedence(CurrentParentOpType)) {
                PushToNodeStack(CurrentNode());
                SetCurrentNode(NewNode);
                AddChild(PopFromNodeStack());
            }else {
                GoTopLevelNodeOfCurrentBranch();
                AddParent(NewNode);
            }
            }

        auto NumericNode = ASTNode::Create();
        NumericNode->SetValue(token.Lexeme);

        AddChild(NumericNode);



    }

    context->PopBinaryOpGenerateImmediateASTcallStack();
        return true;
    }


    void ASTBuilder::ImmediateBuilder::LinkInnerParenthesesNodeToOuterOne(CacheChain* CurrentChain) {
        auto TLNodeOfInnerParentheses = CurrentChain->CurrentTLNodeOfBranch;

        VERIFY(CurrentChain->UpperChain,"UpperChain is nullptr");
        auto TLNodeOfOuterParentheses = CurrentChain->UpperChain->CurrentTLNodeOfBranch;

        this->NodeStack().Push(CurrentNode());
        this->SetCurrentNode(TLNodeOfOuterParentheses);
        this->AddChild(TLNodeOfInnerParentheses);
        this->SetCurrentNode(NodeStack().Pop());
    }
    void ASTBuilder::ImmediateBuilder::LinkCurrentTLNodeOfBranchToMainBranchNode() {
        m_LastUsedMainBranchNode->Children().push_back(m_TopLevelCacheChain.CurrentTLNodeOfBranch);
    }
    bool ASTBuilder::ImmediateBuilder::hasRemainedOneExprInStack() {
        int ParenthesesCount = 0;
        for (auto token: TokenStack().asVector())
            if (token.Type == TokenType::L_PARENTHESES)
                ParenthesesCount++;
        VERIFY(ParenthesesCount > 0 ,"No Parentheses found in Stack");
        return ParenthesesCount > 1 ;
    }


    bool ASTBuilder::ASTSpecializerVisitor::CheckIfStackContainsElement() {};
    bool ASTBuilder::ASTSpecializerVisitor::CheckIfMoreThanOneChild() {};
    bool ASTBuilder::ASTSpecializerVisitor::ReturnToLastStackElement() {};
    bool ASTBuilder::ASTSpecializerVisitor::SwitchToOtherChild() {};
    void ASTBuilder::ASTSpecializerVisitor::SwapNode(ASTNode* TreeNode, ASTNode* StrayNode) {};
    void ASTBuilder::ASTSpecializerVisitor::PlaceNodeAndIterateToChild(ASTNode* NodeToPlace) {};

    ASTNode* ASTBuilder::GenerateScaffoldingByShape() {};
    void ASTBuilder::SetBuilderParametersByLookingShape() {};
    ASTNode* ASTBuilder::SearchSpecificNode(NodeBranchInfo* info, ASTNode* Current) {};
    ASTNode* ASTBuilder::ExtractTopLevelNode() {};
}
