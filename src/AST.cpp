#include "AST.h"
#include "Parser.h"

#include <optional>
#include <vector>
#include <unordered_map>

namespace JSLib {

    NodeBranchInfo* ReturnStatement::ParseTokens(ParserContext* context) {
        context->PushCallStack(FUNCTION_NAME());

        auto NewInfo = NodeBranchInfo::Create();

        auto MaybeConsequentTokens = TRY(this->ConsequentImm()->MakeTokenVector());

        Token EosToken = *new Token();
        EosToken.Type = TokenType::END_OF_STREAM;

        MaybeConsequentTokens.value().push_back(EosToken);

        ASTNode* ConsequentNode = nullptr;

        ConsequentNode = Parser::Instance()->RecognizeStatementOrRedirectNode(MaybeConsequentTokens, context);

        auto NewNode = ReturnNode::Create();
        NewNode->SetValue("return");

        NewInfo->SetNode(NewNode);

        NewInfo->Node()->SetConsequentNode(ConsequentNode);

        const auto Dump = NewInfo->Node()->toJson().dump(4);

        std::cout << "-------------------" << "ReturnStatement::ParseTokens" << "-------------------"<< std::endl;
        std::cout << Dump << std::endl;
        std::cout << "-----------------------------------------------------------------------------------------"<< std::endl;
        context->PopCallStack();
        return NewInfo;
    }


    NodeBranchInfo* IfStatement::ParseTokens(ParserContext* context) {
        context->PushCallStack(FUNCTION_NAME());

        std::optional<std::vector<SyntaxError>> MaybeErrors = std::nullopt;

        auto NewInfo = NodeBranchInfo::Create();

        auto MaybeTestTokens = TRY(this->TestExpr()->MakeTokenVector());













        ASTNode* TestNode = nullptr;
        std::vector<ASTNode*> ConsequentNodes;
        std::vector<ASTNode*> AlternateNodes;


        VERIFY(TestNode,"TestNode returned nullptr");




        auto NewNode = IfNode::Create();
        NewNode->SetValue("if");

        NewInfo->SetNode(NewNode);

        NewInfo->Node()->SetTestNode(TestNode);
        NewInfo->Node()->SetConsequentNode(ConsequentNode);

        if (AlternateTokens)
            NewInfo->Node()->SetAlternateNode(AlternateNode);

        auto Dump = NewInfo->Node()->toJson().dump(4);

        std::cout << "-------------------" << "IfStatement::ParseTokens" << "-------------------"<< std::endl;
        std::cout << Dump << std::endl;
        std::cout << "-----------------------------------------------------------------------------------------"<< std::endl;
        context->PopCallStack();
        return NewInfo;
    }

    NodeBranchInfo* ScopeStatement::ParseTokens(ParserContext* context) {

        auto NewInfo = NodeBranchInfo::Create();

        std::optional<std::vector<SyntaxError>> MaybeErrors;

        this->forEach(
                [&](Statement* statement, ParserContext* context_) {
                    auto MaybeConsequentTokens = TRY(statement->MakeTokenVector());
                    if (MaybeConsequentTokens) {
                        context_->PushScopeStatementBody(MaybeConsequentTokens.value());
                    } else {
                        MaybeErrors.value().push_back(MaybeConsequentTokens.error());
                    }

                });


        return NewInfo;
    }

    ASTNode* BinaryOpStatement::GenerateASTImmediate(ParserContext* context) {

        auto InitialNode = ASTNode::Create();
        Parser::Instance()->Builder()->immBuilder()->SetCurrentNode(InitialNode);

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
        OperationTypeMap["=="] = BinaryOpSubType::IS_EQUAL;
        OperationTypeMap["!="] = BinaryOpSubType::IS_NOT_EQUAL;
        OperationTypeMap["==="] = BinaryOpSubType::IS_STRICTLY_EQUAL;
        OperationTypeMap["!=="] = BinaryOpSubType::IS_NOT_STRICTLY_EQUAL;
        OperationTypeMap[">"] = BinaryOpSubType::GREATER_THAN;
        OperationTypeMap["<"] = BinaryOpSubType::LESS_THAN;
        OperationTypeMap[">="] = BinaryOpSubType::GREATER_OR_EQUAL;
        OperationTypeMap["<="] = BinaryOpSubType::LESS_OR_EQUAL;
        OperationTypeMap["("] = BinaryOpSubType::L_BRACKET;
        OperationTypeMap[")"] = BinaryOpSubType::R_BRACKET;


        auto InfixTokens;

        auto PostfixTokens =Parser::Instance()->Builder()->immBuilder()->ConvertInfixToPostfix(InfixTokens,context, OperationTypeMap);

        DEBUG("Press key to print Postfix Tokens...");
        std::cout<<"-------------------" << "BinaryOpStatement::GenerateASTImmediate" << "-------------------"<< std::endl;
        std::cout << "Postfix Tokens: ";
        for (auto& token : *PostfixTokens) {
            std::cout << token.Lexeme<< " ";
        }
        std::cout << std::endl;
        std::cout << "-----------------------------------------------------------------------------------------"<< std::endl;

        auto ASTBranch = Parser::Instance()->Builder()->immBuilder()->GenerateASTFromPostfix(PostfixTokens, context, OperationTypeMap);

        DEBUG ("Press key to print AST...");
        std::cout <<"-------------------" << "BinaryOpStatement::GenerateASTImmediate" << "-------------------"<< std::endl;
        std::cout << "ASTBranch: ";
        const auto dump = ASTBranch->toJson().dump(4);
        std::cout << dump << std::endl;
        std::cout << "-----------------------------------------------------------------------------------------"<< std::endl;


        return ASTBranch;
    }

    std::shared_ptr<std::vector<Token>> ASTBuilder::ImmediateBuilder::ConvertInfixToPostfix(
        const std::vector<Token> &TokensToConvert, ParserContext *context,
        std::unordered_map<std::string, BinaryOpSubType>& OpTypeMap) {

        std::shared_ptr<std::vector<Token>> PostfixTokens = std::make_shared<std::vector<Token>>();

        for (auto token : TokensToConvert) {

            if (token.Type == TokenType::NUMERIC || token.Type == TokenType::IDENTIFIER) {
                PostfixTokens->push_back(token);
                continue;
            }

            if (token.Type == TokenType::BINARY_OP) {

                if (OperatorStack().IsEmpty()) {
                    OperatorStack().Push(token);
                    continue;
                }

                auto BinaryOpIteratorType = OpTypeMap.find(token.Lexeme);

                if (BinaryOpIteratorType == OpTypeMap.end()) {
                    DEBUG("BinaryOpIteratorType could not be found in OpTypeMap");
                    VERIFY_NOT_REACHED();
                }

                auto TopOfStackOpTypeIterator = OpTypeMap.find(OperatorStack().Peek().Lexeme);

                if (TopOfStackOpTypeIterator == OpTypeMap.end()) {
                    DEBUG("TopOfStackOpTypeIterator could not be found in OpTypeMap");
                    VERIFY_NOT_REACHED();
                }

                BinaryOpSubType OpType = BinaryOpIteratorType->second;
                BinaryOpSubType TopOfStackOpType = TopOfStackOpTypeIterator->second;

                if (OperatorStack().Peek().Type == TokenType::BINARY_OP) {
                    if (GetPrecedence(OpType) > GetPrecedence(TopOfStackOpType)) {
                        OperatorStack().Push(token);
                        continue;
                    }
                    PostfixTokens->push_back(OperatorStack().Pop());
                    OperatorStack().Push(token);
                    continue;
                }
                OperatorStack().Push(token);
                continue;
            }
            if (token.Type == TokenType::L_BRACKET) {
                OperatorStack().Push(token);
                continue;
            }

            if (token.Type == TokenType::R_BRACKET) {
                while (OperatorStack().Peek().Type != TokenType::L_BRACKET) {
                    PostfixTokens->push_back(OperatorStack().Pop());
                }
                OperatorStack().Pop(); //pop the left parentheses
                continue;
            }

        }
        while (!OperatorStack().IsEmpty()) {
            PostfixTokens->push_back(OperatorStack().Pop());
        };
        return PostfixTokens;
    }
    ASTNode* ASTBuilder::ImmediateBuilder::GenerateASTFromPostfix(std::shared_ptr<std::vector<Token>> PostfixToASTTokens,
                                                                  ParserContext* context,
                                                                  std::unordered_map<std::string,BinaryOpSubType>& OpTypeMap) {
        int TokenIndex = 0;
        bool isRunOnce = false;
        for (auto& token : *PostfixToASTTokens) {
            if (token.Type == TokenType::NUMERIC) {
                TokenIndex++;
                continue;
            }
            if (token.Type == TokenType::IDENTIFIER) {
                TokenIndex++;
                continue;
            }
            if (token.Type == TokenType::BINARY_OP) {
                auto BinaryOpIteratorType = OpTypeMap.find(token.Lexeme);
                if (BinaryOpIteratorType == OpTypeMap.end()) {
                    VERIFY_NOT_REACHED();
                }
                ASTNode* NumericOrVariable = nullptr;
                auto OpType = BinaryOpIteratorType->second;
                auto OpNode = BinaryOpNode::Create();
                OpNode->SetValue(token.Lexeme);
                OpNode->SetSubType(OpType);

                if (!isRunOnce) {
                    auto& Rhs = PostfixToASTTokens->at(TokenIndex - 1);
                    auto& Lhs = PostfixToASTTokens->at(TokenIndex - 2);

                    if (Rhs.Type == TokenType::NUMERIC) {
                        NumericOrVariable = NumericNode::Create();
                        NumericOrVariable->SetValue(Rhs.Lexeme);
                        NumericOrVariable->SetNumericValueFromRaw();
                        OpNode->SetRhs(NumericOrVariable);
                    }
                    else if (Rhs.Type == TokenType::IDENTIFIER) {
                        NumericOrVariable = VariableNode::Create();
                        NumericOrVariable->SetValue(Rhs.Lexeme);
                        OpNode->SetRhs(NumericOrVariable);
                    }



                    if (Lhs.Type == TokenType::NUMERIC) {
                        auto LhsNumeric = NumericNode::Create();
                        LhsNumeric->SetValue(Lhs.Lexeme);
                        LhsNumeric->SetNumericValueFromRaw();
                        OpNode->SetLhs(LhsNumeric);
                    }
                    else if (Lhs.Type == TokenType::IDENTIFIER) {
                        auto LhsVariable = VariableNode::Create();
                        LhsVariable->SetValue(Lhs.Lexeme);
                        OpNode->SetLhs(LhsVariable);
                    }

                    Rhs.Analyzed = true;
                    Lhs.Analyzed = true;
                    token.Analyzed = true;
                    isRunOnce = true;
                    TokenIndex++;

                    SetCurrentNode(OpNode);

                    continue;
                }

                int i = TokenIndex - 1;
                bool isLhs = false;
                while (PostfixToASTTokens->at(i).Analyzed) {
                    isLhs = true;
                    i--;
                }

                auto& CurrentToken = PostfixToASTTokens->at(i);
                if (CurrentToken.Type == TokenType::NUMERIC) {
                    NumericOrVariable = NumericNode::Create();
                    NumericOrVariable->SetValue(CurrentToken.Lexeme);
                    NumericOrVariable->SetNumericValueFromRaw();
                    if (isLhs) {
                        AddLhs(NumericOrVariable, OpNode);
                    } else {
                        AddRhs(NumericOrVariable, OpNode);
                    }
                    CurrentToken.Analyzed = true;
                    token.Analyzed = true;
                }
                else if (CurrentToken.Type == TokenType::IDENTIFIER) {
                    NumericOrVariable = VariableNode::Create();
                    NumericOrVariable->SetValue(CurrentToken.Lexeme);
                    if (isLhs) {
                        AddLhs(NumericOrVariable, OpNode);
                    } else {
                        AddRhs(NumericOrVariable, OpNode);
                    }

                }

                if (isLhs) {
                    AddRhs(CurrentNode(), OpNode);
                } else {
                    AddLhs(CurrentNode(), OpNode);
                }


                CurrentToken.Analyzed=true;
                token.Analyzed = true;

                GoTopLevelNodeOfCurrentBranch();

                continue;
            }
        }
        return CurrentNode();
    }
    bool ASTBuilder::SpecializerVisitor::CheckIfStackContainsElement() {};
    bool ASTBuilder::SpecializerVisitor::CheckIfMoreThanOneChild() {};
    bool ASTBuilder::SpecializerVisitor::ReturnToLastStackElement() {};
    bool ASTBuilder::SpecializerVisitor::SwitchToOtherChild() {};
    void ASTBuilder::SpecializerVisitor::SwapNode(ASTNode* TreeNode, ASTNode* StrayNode) {};
    void ASTBuilder::SpecializerVisitor::PlaceNodeAndIterateToChild(ASTNode* NodeToPlace) {};

    ASTNode* ASTBuilder::GenerateScaffoldingByShape() {};
    void ASTBuilder::SetBuilderParametersByLookingShape() {};
    ASTNode* ASTBuilder::SearchSpecificNode(NodeBranchInfo* info, ASTNode* Current) {};
    ASTNode* ASTBuilder::ExtractTopLevelNode() {};
}
