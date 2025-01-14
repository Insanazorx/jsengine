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

        static bool AnalyzedThisDepth = false;
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
        ASTNode* IterationNode;
        ASTNode* ConsequentNode;


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
        OperationTypeMap["("] = BinaryOpSubType::L_PARENTHESES;
        OperationTypeMap[")"] = BinaryOpSubType::R_PARENTHESES;


        auto& InfixTokens = this->GetTokenChain();

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
            if (token.Type == TokenType::L_PARENTHESES) {
                OperatorStack().Push(token);
                continue;
            }

            if (token.Type == TokenType::R_PARENTHESES) {
                while (OperatorStack().Peek().Type != TokenType::L_PARENTHESES) {
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
        bool IsOnceActivated = false;
        int TokenIndex = 0;
        for (auto& token : *PostfixToASTTokens) {

            DEBUG(LOGGER_BANNER(ImmediateBuilder)<< "token: " << token.Lexeme);

            if (token.Type == TokenType::NUMERIC || token.Type == TokenType::IDENTIFIER) {
                TokenIndex++;
                continue;
            }

            if (token.Type == TokenType::BINARY_OP) {

                auto it = OpTypeMap.find(token.Lexeme);
                if (it == OpTypeMap.end()) {
                    DEBUG("Could not find the operator in OpTypeMap");
                    VERIFY_NOT_REACHED();
                }
                auto OpType = it->second;

                auto NewNode = BinaryOpNode::Create();
                NewNode->SetSubType(OpType);
                NewNode->SetValue(token.Lexeme);

                token.Analyzed = true;

                if (!IsOnceActivated) {
                    NumericNode* LhsNodeIfNumeric;
                    NumericNode* RhsNodeIfNumeric;
                    VariableNode* LhsNodeIfVariable;
                    VariableNode* RhsNodeIfVariable;

                    SetCurrentNode(NewNode);
                    auto& RhsToken = PostfixToASTTokens->at(TokenIndex-1);
                    auto& LhsToken = PostfixToASTTokens->at(TokenIndex-2);
                    if (RhsToken.Type == TokenType::NUMERIC) {
                        RhsNodeIfNumeric = NumericNode::Create();
                        RhsNodeIfNumeric->SetValue(RhsToken.Lexeme);
                        RhsNodeIfNumeric->SetNumericValueFromRaw();
                    } else if (RhsToken.Type == TokenType::IDENTIFIER) {
                        RhsNodeIfVariable = VariableNode::Create();
                        RhsNodeIfVariable->SetValue(RhsToken.Lexeme);
                    }
                    RhsToken.Analyzed = true;

                    if (LhsToken.Type == TokenType::NUMERIC) {
                        LhsNode = NumericNode::Create();
                        LhsNode->SetValue(LhsToken.Lexeme);
                        LhsNode->SetNumericValueFromRaw();
                    } else if (LhsToken.Type == TokenType::IDENTIFIER) {
                        LhsNode = VariableNode::Create();
                        LhsNode->SetValue(LhsToken.Lexeme);
                    }
                    LhsToken.Analyzed = true;

                    VERIFY(LhsNode,"lhs node could not get created");
                    VERIFY(RhsNode,"rhs node could not get created");

                    AddLhs(LhsNode);
                    AddRhs(RhsNode);

                    IsOnceActivated = true;
                    continue;
                }

                AddParent(NewNode);
                GoTopLevelNodeOfCurrentBranch();

                int i = 0;
                while (TokenIndex != i && PostfixToASTTokens->at(TokenIndex - i++).Analyzed);

                if (PostfixToASTTokens->at(TokenIndex - i).Type == TokenType::NUMERIC) {
                    auto Node = NumericNode::Create();
                    Node->SetValue(PostfixToASTTokens->at(TokenIndex - i).Lexeme);
                    Node->SetNumericValueFromRaw();
                    NewNode->SetRhs(Node);
                } else if (PostfixToASTTokens->at(TokenIndex - i).Type == TokenType::IDENTIFIER) {
                    auto Node = VariableNode::Create();
                    Node->SetValue(PostfixToASTTokens->at(TokenIndex - i).Lexeme);
                    NewNode->SetRhs(Node);
                }


                TokenIndex++;
                continue;
            }
        }
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
