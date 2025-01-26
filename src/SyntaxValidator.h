#ifndef SYNTAXVALIDATOR_H
#define SYNTAXVALIDATOR_H
namespace JSLib {
    class SyntaxValidator {
    public:
        SyntaxValidator() = default;
        ~SyntaxValidator() = default;
        static SyntaxValidator* Create() {return new SyntaxValidator();}
        void Validate();
    };
}
#endif //SYNTAXVALIDATOR_H
