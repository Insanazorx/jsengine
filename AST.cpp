#include "AST.h"
namespace JSLib {
    NodeBranchInfo& IfStatement::ParseTokens() {
        auto NewInfo = NodeBranchInfo::Create();
        NewInfo->Builder().
        return *NewInfo;
    }
    NodeBranchInfo& BinaryOpStatement::ParseTokens() {
        
    }
    NodeBranchInfo& ForStatement::ParseTokens() {}
    void ASTBuilder::ShapeBuilder::SetMappings() {};
    bool ASTBuilder::ASTVisitorForBuilding::CheckIfThereAreChildren() {};
    bool ASTBuilder::ASTVisitorForBuilding::CheckIfStackContainsElement() {};
    bool ASTBuilder::ASTVisitorForBuilding::CheckIfMoreThanOneChild() {};
    bool ASTBuilder::ASTVisitorForBuilding::ReturnToLastStackElement() {};
    bool ASTBuilder::ASTVisitorForBuilding::SwitchToOtherChild() {};
    void ASTBuilder::ASTVisitorForBuilding::SwapNode(ASTNode* TreeNode, ASTNode* StrayNode) {};
    void ASTBuilder::ASTVisitorForBuilding::PlaceNodeAndIterateToChild(ASTNode* NodeToPlace) {};
    ASTNode* ASTBuilder::GenerateScaffoldingByShape() {};
    void ASTBuilder::SetBuilderParametersByLookingShape() {};
    ASTNode* ASTBuilder::SearchSpecificNode(NodeBranchInfo& info, ASTNode* Current) {};
    ASTNode* ASTBuilder::ExtractTopLevelNode() {};
}