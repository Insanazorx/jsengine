#include "AST.h"
#include "Parser.h"

namespace JSLib {
    NodeBranchInfo* IfStatement::ParseTokens(ParserContext* context) {
        context->PushCallStack(new std::string(FUNCTION_NAME()));
        
        auto NewInfo = NodeBranchInfo::Create();
        
        auto& TestTokens =*new std::optional<std::list<Token*>>(this->GetTestTokens());
        auto& ConsequentTokens = *new std::optional<std::list<Token*>>(this->GetConseqTokens());
        auto& AlternateTokens = *new std::optional<std::list<Token*>> (this->GetAlternateTokens());
        
        bool AnalyzedThisDepth = false;
        ASTNode* TestNode;
        DummyNode* ConsequentNode;
        DummyNode* AlternateNode;

        if (!AnalyzedThisDepth) {
            TestNode = dynamic_cast<ASTNode*>(context->CallAnalyze(TestTokens, context));

            VERIFY(TestNode,"TestNode returned nullptr");

            ConsequentNode = dynamic_cast<DummyNode*>(context->CallAnalyze(ConsequentTokens, context));

            VERIFY(ConsequentNode, "ConsequentNode returned nullptr");

            AlternateNode = dynamic_cast<DummyNode*>(context->CallAnalyze(AlternateTokens, context));

            VERIFY(AlternateNode, "AlternateNode returned nullptr");
            AnalyzedThisDepth = true;
        }
        
        NewInfo->Shape()->Builder().AddChild(this, TestNode);
        NewInfo->Shape()->Builder().GotoParent();
        
        NewInfo->Shape()->Builder().AddChild({}, nullptr);

        for (auto node : ConsequentNode->Children()) {
            NewInfo->Shape()->Builder().AddSpecificChild(dynamic_cast<DummyNode*>(node));
            NewInfo->Shape()->Builder().GotoParent();
        }

        NewInfo->Shape()->Builder().GotoParent();

        auto alternateStatementHolder = Statement::Create();
        NewInfo->Shape()->Builder().AddChild(alternateStatementHolder);

        for (auto node: AlternateNode->Children()) {
            NewInfo->Shape()->Builder().AddSpecificChild(dynamic_cast<DummyNode*>(node));
            NewInfo->Shape()->Builder().GotoParent();
        }

        NewInfo->Shape()->Builder().GotoParent();

        context->PopCallStack();
        return NewInfo;
    }

    NodeBranchInfo* ForStatement::ParseTokens(ParserContext* context) {


        auto NewInfo = NodeBranchInfo::Create();
        
        std::list<Token*> InitialConditionTokens = this->GetInitialConditionTokens();
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
        Parser::Instance()->Builder()->immBuilder().SetCurrentNode(InitialNode);


        VERIFY(Parser::Instance()
            ->Builder()
            ->immBuilder()
            .ParseInnerParentheses(
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
            auto childToASTNode = dynamic_cast<ASTNode*>(child);

            VERIFY(child,"Child could not casted to ASTNode");

            if (childToASTNode->Raw().isNumeric())
                SetCurrentNode(childToASTNode);
        }
        VERIFY(parent != CurrentNode(), "Could not get to numeric child!");
    }

    std::list<Token*>& ASTBuilder::ImmediateBuilder::EmptyOneExprFromStack() {
        auto retValue = *new std::list<Token*>();
        while (TokenStack().Peek()->Type == TokenType::L_PARENTHESES) {
            if (TokenStack().Peek()->Type == TokenType::R_PARENTHESES) {
                TokenStack().Pop();
                continue;
            }
            retValue.push_back(TokenStack().Pop());
        }
        return retValue;
    }

    bool ASTBuilder::ImmediateBuilder::ParseInnerParentheses(std::list<Token*>& TokenChain,ASTBuilder::CacheChain* CurrentChain, ParserContext* context) {

        context->PushBinaryOpGenerateImmediateASTCallStack(new std::string(FUNCTION_NAME()));
        context->IncDepth();

        auto SwapAndPlaceNode = [&](BinaryOpStatementNode* node) ->void {
            GoToNumericChild();
            PushToNodeStack(CurrentNode());
            SetCurrentNode(node);
            AddChild(PopFromNodeStack());
        };

        UnorderedMap<std::string, BinaryOpSubType> OperationTypeMap;
        OperationTypeMap.insert("+",BinaryOpSubType::ADD)
                        ->insert("-",BinaryOpSubType::SUB)
                        ->insert("*",BinaryOpSubType::MUL)
                        ->insert("/",BinaryOpSubType::DIV)
                        ->insert("%",BinaryOpSubType::MOD)
                        ->insert("^", BinaryOpSubType::BITWISE_XOR)
                        ->insert("|", BinaryOpSubType::BITWISE_OR)
                        ->insert("&",BinaryOpSubType::BITWISE_AND)
                        ->insert(">>", BinaryOpSubType::SHIFT_RIGHT)
                        ->insert("<<", BinaryOpSubType::SHIFT_LEFT);



    if (!CurrentChain) {
        CurrentChain->currentNode = ASTNode::Create();
        SetCurrentNode(CurrentChain->currentNode);
    }

    for (auto token : TokenChain) {
        if (token->Type == TokenType::L_PARENTHESES) {

            TokenStack().Push(token);

            continue;
        }

        if (token->Type == TokenType::R_PARENTHESES) {

            std::list<Token*> InnerTokens;

            if (isStackEmpty())
                return false;

            CurrentChain->AllocInnerChain();

            if (hasRemainedOneExprInStack()) {
                InnerTokens = EmptyOneExprFromStack();

                if (ParseInnerParentheses(InnerTokens,CurrentChain->InnerChain, context))
                    LinkcurrentNodeToMainBranchNode();
                else return false;

            } else {
                InnerTokens = EmptyOneExprFromStack();

                if (ParseInnerParentheses(InnerTokens,CurrentChain->InnerChain,context))
                    LinkInnerParenthesesNodeToOuterOne(CurrentChain);
                else return false;
            }

            continue;
        }

        auto OpType = OperationTypeMap.find(token->Lexeme);
        const auto CurrentParentOpType = dynamic_cast<BinaryOpStatementNode*>(CurrentNode()->Parent())->SubType();

        if (!isStackEmpty()) {
            if (OpType.has_value()) {
                auto OpNode = BinaryOpStatementNode::Create();
                OpNode->SetSubType(OpType.get());
                PushToNodeStack(OpNode);
            } else {
                auto NumericNode = ASTNode::Create();
                NumericNode->SetValue(token->Lexeme);
                PushToNodeStack(NumericNode);
            }
            continue;
        }

        if (OpType.has_value()) {

            auto NewNode = BinaryOpStatementNode::Create();

            switch (OpType.get()) {
                case BinaryOpSubType::POW:

                    NewNode->SetSubType(BinaryOpSubType::POW);

                switch (CurrentParentOpType) {
                    case BinaryOpSubType::ADD:
                        case BinaryOpSubType::SUB:
                    case BinaryOpSubType::MUL:
                    case BinaryOpSubType::DIV:
                    case BinaryOpSubType::MOD:
                    case BinaryOpSubType::BITWISE_XOR:
                    case BinaryOpSubType::BITWISE_OR:
                    case BinaryOpSubType::BITWISE_AND:
                    case BinaryOpSubType::SHIFT_RIGHT:
                    case BinaryOpSubType::SHIFT_LEFT:
                        SwapAndPlaceNode(NewNode);
                        break;
                    case BinaryOpSubType::POW:
                        AddParent(NewNode);
                        break;
                    default:
                        VERIFY_NOT_REACHED();
                    break;
                }
                break;


                case BinaryOpSubType::MUL:

                    NewNode->SetSubType(BinaryOpSubType::MUL);

                switch (CurrentParentOpType) {
                    case BinaryOpSubType::MUL:
                        case BinaryOpSubType::DIV:
                    case BinaryOpSubType::MOD:
                    case BinaryOpSubType::POW:
                        AddParent(NewNode);
                    break;
                    case BinaryOpSubType::ADD:
                        case BinaryOpSubType::SUB:
                    case BinaryOpSubType::BITWISE_OR:
                    case BinaryOpSubType::BITWISE_XOR:
                    case BinaryOpSubType::BITWISE_AND:
                    case BinaryOpSubType::SHIFT_RIGHT:
                    case BinaryOpSubType::SHIFT_LEFT:
                        SwapAndPlaceNode(NewNode);
                    break;
                    default:
                        VERIFY_NOT_REACHED();
                }

                break;


                case BinaryOpSubType::DIV:

                    NewNode->SetSubType(BinaryOpSubType::DIV);

                switch (CurrentParentOpType) {
                    case BinaryOpSubType::MUL:
                        case BinaryOpSubType::DIV:
                    case BinaryOpSubType::MOD:
                    case BinaryOpSubType::POW:
                        AddParent(NewNode);
                    break;
                    case BinaryOpSubType::ADD:
                        case BinaryOpSubType::SUB:
                    case BinaryOpSubType::BITWISE_OR:
                    case BinaryOpSubType::BITWISE_XOR:
                    case BinaryOpSubType::BITWISE_AND:
                    case BinaryOpSubType::SHIFT_RIGHT:
                    case BinaryOpSubType::SHIFT_LEFT:
                        SwapAndPlaceNode(NewNode);
                    break;
                    default:
                        VERIFY_NOT_REACHED();
                }

                break;


                case BinaryOpSubType::MOD:

                    NewNode->SetSubType(BinaryOpSubType::MOD);

                switch (CurrentParentOpType) {
                    case BinaryOpSubType::MUL:
                        case BinaryOpSubType::DIV:
                    case BinaryOpSubType::MOD:
                    case BinaryOpSubType::POW:
                        AddParent(NewNode);
                    break;
                    case BinaryOpSubType::ADD:
                        case BinaryOpSubType::SUB:
                    case BinaryOpSubType::BITWISE_OR:
                    case BinaryOpSubType::BITWISE_XOR:
                    case BinaryOpSubType::BITWISE_AND:
                    case BinaryOpSubType::SHIFT_RIGHT:
                    case BinaryOpSubType::SHIFT_LEFT:
                        SwapAndPlaceNode(NewNode);
                    break;
                    default:
                        VERIFY_NOT_REACHED();
                    break;
                }


                break;

                case BinaryOpSubType::ADD:

                    NewNode->SetSubType(BinaryOpSubType::ADD);

                switch (CurrentParentOpType) {
                    case BinaryOpSubType::POW:
                        case BinaryOpSubType::MUL:
                    case BinaryOpSubType::DIV:
                    case BinaryOpSubType::MOD:
                    case BinaryOpSubType::ADD:
                    case BinaryOpSubType::SUB:
                        AddParent(NewNode);
                    break;
                    case BinaryOpSubType::BITWISE_OR:
                        case BinaryOpSubType::BITWISE_XOR:
                    case BinaryOpSubType::BITWISE_AND:
                    case BinaryOpSubType::SHIFT_RIGHT:
                    case BinaryOpSubType::SHIFT_LEFT:
                        SwapAndPlaceNode(NewNode);
                    break;
                    default:
                        VERIFY_NOT_REACHED();
                    break;
                }

                break;
                case BinaryOpSubType::SUB:

                    NewNode->SetSubType(BinaryOpSubType::SUB);

                switch (CurrentParentOpType) {
                    case BinaryOpSubType::POW:
                        case BinaryOpSubType::MUL:
                    case BinaryOpSubType::DIV:
                    case BinaryOpSubType::MOD:
                    case BinaryOpSubType::ADD:
                    case BinaryOpSubType::SUB:
                        AddParent(NewNode);
                    break;
                    case BinaryOpSubType::BITWISE_OR:
                        case BinaryOpSubType::BITWISE_XOR:
                    case BinaryOpSubType::BITWISE_AND:
                    case BinaryOpSubType::SHIFT_RIGHT:
                    case BinaryOpSubType::SHIFT_LEFT:
                        SwapAndPlaceNode(NewNode);
                    break;
                    default:
                        VERIFY_NOT_REACHED();
                    break;
                }
                break;


                case BinaryOpSubType::SHIFT_RIGHT:

                    NewNode->SetSubType(BinaryOpSubType::SHIFT_RIGHT);

                switch (CurrentParentOpType) {
                    case BinaryOpSubType::POW:
                        case BinaryOpSubType::MUL:
                    case BinaryOpSubType::DIV:
                    case BinaryOpSubType::MOD:
                    case BinaryOpSubType::ADD:
                    case BinaryOpSubType::SUB:
                    case BinaryOpSubType::SHIFT_LEFT:
                    case BinaryOpSubType::SHIFT_RIGHT:
                        AddParent(NewNode);
                    break;
                    case BinaryOpSubType::BITWISE_OR:
                        case BinaryOpSubType::BITWISE_XOR:
                    case BinaryOpSubType::BITWISE_AND:
                        SwapAndPlaceNode(NewNode);
                    break;
                    default:
                        VERIFY_NOT_REACHED();
                    break;
                }
                break;

                case BinaryOpSubType::SHIFT_LEFT:

                    NewNode->SetSubType(BinaryOpSubType::SHIFT_LEFT);

                switch (CurrentParentOpType) {
                    case BinaryOpSubType::POW:
                        case BinaryOpSubType::MUL:
                    case BinaryOpSubType::DIV:
                    case BinaryOpSubType::MOD:
                    case BinaryOpSubType::ADD:
                    case BinaryOpSubType::SUB:
                    case BinaryOpSubType::SHIFT_LEFT:
                    case BinaryOpSubType::SHIFT_RIGHT:
                        AddParent(NewNode);
                    break;
                    case BinaryOpSubType::BITWISE_OR:
                        case BinaryOpSubType::BITWISE_XOR:
                    case BinaryOpSubType::BITWISE_AND:
                        SwapAndPlaceNode(NewNode);
                    break;
                    default:
                        VERIFY_NOT_REACHED();
                    break;
                }
                break;


                case BinaryOpSubType::BITWISE_XOR:

                    NewNode->SetSubType(BinaryOpSubType::BITWISE_XOR);

                switch (CurrentParentOpType) {
                    case BinaryOpSubType::POW:
                        case BinaryOpSubType::MUL:
                    case BinaryOpSubType::DIV:
                    case BinaryOpSubType::MOD:
                    case BinaryOpSubType::ADD:
                    case BinaryOpSubType::SUB:
                    case BinaryOpSubType::SHIFT_LEFT:
                    case BinaryOpSubType::SHIFT_RIGHT:
                    case BinaryOpSubType::BITWISE_XOR:
                    case BinaryOpSubType::BITWISE_AND:
                    case BinaryOpSubType::BITWISE_OR:
                        AddParent(NewNode);
                    break;
                    default:
                        VERIFY_NOT_REACHED();
                    break;

                }

                break;

                case BinaryOpSubType::BITWISE_OR:

                    NewNode->SetSubType(BinaryOpSubType::BITWISE_OR);

                switch (CurrentParentOpType) {
                    case BinaryOpSubType::POW:
                        case BinaryOpSubType::MUL:
                    case BinaryOpSubType::DIV:
                    case BinaryOpSubType::MOD:
                    case BinaryOpSubType::ADD:
                    case BinaryOpSubType::SUB:
                    case BinaryOpSubType::SHIFT_LEFT:
                    case BinaryOpSubType::SHIFT_RIGHT:
                    case BinaryOpSubType::BITWISE_OR:
                    case BinaryOpSubType::BITWISE_XOR:
                    case BinaryOpSubType::BITWISE_AND:
                        AddParent(NewNode);
                    break;
                    default:
                        VERIFY_NOT_REACHED();
                    break;

                }

                break;

                case BinaryOpSubType::BITWISE_AND:

                    NewNode->SetSubType(BinaryOpSubType::BITWISE_AND);

                switch (CurrentParentOpType) {
                    case BinaryOpSubType::POW:
                        case BinaryOpSubType::MUL:
                    case BinaryOpSubType::DIV:
                    case BinaryOpSubType::MOD:
                    case BinaryOpSubType::ADD:
                    case BinaryOpSubType::SUB:
                    case BinaryOpSubType::SHIFT_LEFT:
                    case BinaryOpSubType::SHIFT_RIGHT:
                    case BinaryOpSubType::BITWISE_AND:
                    case BinaryOpSubType::BITWISE_XOR:
                    case BinaryOpSubType::BITWISE_OR:
                        AddParent(NewNode);
                    break;
                    default:
                        VERIFY_NOT_REACHED();
                    break;
                }
                break;
                default:
                    break;

            }

            continue;
        }

        auto NumericNode = ASTNode::Create();
        NumericNode->SetValue(token->Lexeme);

        AddChild(NumericNode);



    }

    context->PopBinaryOpGenerateImmediateASTcallStack();
        return true;
    }


    void ASTBuilder::ImmediateBuilder::LinkInnerParenthesesNodeToOuterOne(ASTBuilder::CacheChain* CurrentChain) {
        auto TLNodeOfInnerParentheses = CurrentChain->currentNode;

        VERIFY(CurrentChain->UpperChain,"UpperChain is nullptr");
        auto TLNodeOfOuterParentheses = CurrentChain->UpperChain->currentNode;

        this->NodeStack().Push(CurrentNode());
        this->SetCurrentNode(TLNodeOfOuterParentheses);
        this->AddChild(TLNodeOfInnerParentheses);
        this->SetCurrentNode(NodeStack().Pop());
    }
    void ASTBuilder::ImmediateBuilder::LinkcurrentNodeToMainBranchNode() {
        m_LastUsedMainBranchNode->Children().push_back(m_TopLevelCacheChain.currentNode);
    }
    bool ASTBuilder::ImmediateBuilder::hasRemainedOneExprInStack() {
        auto StackAsstd::list = TokenStack().ConvertTostd::list();
        int ParenthesesCount = 0;
        for (auto token: StackAsstd::list)
            if (token->Type == TokenType::L_PARENTHESES)
                ParenthesesCount++;
        VERIFY(ParenthesesCount > 0 ,"No Parentheses found in Stack");
        return ParenthesesCount > 1 ;
    }


    bool ASTBuilder::ASTVisitorForBuilding::CheckIfStackContainsElement() {};
    bool ASTBuilder::ASTVisitorForBuilding::CheckIfMoreThanOneChild() {};
    bool ASTBuilder::ASTVisitorForBuilding::ReturnToLastStackElement() {};
    bool ASTBuilder::ASTVisitorForBuilding::SwitchToOtherChild() {};
    void ASTBuilder::ASTVisitorForBuilding::SwapNode(ASTNode* TreeNode, ASTNode* StrayNode) {};
    void ASTBuilder::ASTVisitorForBuilding::PlaceNodeAndIterateToChild(ASTNode* NodeToPlace) {};

    ASTNode* ASTBuilder::GenerateScaffoldingByShape() {};
    void ASTBuilder::SetBuilderParametersByLookingShape() {};
    ASTNode* ASTBuilder::SearchSpecificNode(NodeBranchInfo* info, ASTNode* Current) {};
    ASTNode* ASTBuilder::ExtractTopLevelNode() {};
}
