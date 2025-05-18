#include "SourceCode.h"
#include <fstream>
namespace js {
    bool SourceCode::ReadSourceCodeFromPath() {
        std::ifstream file;
        file.open(m_path.value());
        if (!file.is_open()) {
            return false;
        }
        std::string line;
        while (std::getline(file, line)) {
            m_source += line;
            m_source += "\n";
        }
        file.close();
        return true;
    }
    void SourceCode::Tokenize(Lexer& lexer, std::vector<Token>& tokens) {
        lexer.SetExpr(m_source);
        lexer.Scan();
        tokens = lexer.Tokens();
    }
}
