

#ifndef SOURCECODE_H
#define SOURCECODE_H
#include <string>
#include <optional>

#include "Lexer.h"
#include "LexerTypes.h"

namespace JSLib {
    class SourceCode {
    private:
        explicit SourceCode(std::string path) : m_path(path) {};
        explicit SourceCode(std::string&& source) : m_source(source) {};
    public:
        static SourceCode* Create(std::string&& path) {return new SourceCode(path);}
        static SourceCode* Create(const std::string source) {return new SourceCode(move(source));}
        ~SourceCode() = default;
        bool ReadSourceCodeFromPath();
        void Tokenize(Lexer &lexer, std::vector<Token>& tokens);
        void print() {std::cout << m_source;}
    private:
        std::string m_source;
        std::optional<std::string> m_path;
        int m_line {0};
        int m_pos {0};
    };
}
#endif //SOURCECODE_H
