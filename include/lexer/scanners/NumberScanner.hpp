#pragma once



#include <lexer/IScanner.hpp>
#include <lexer/CharClassifier.hpp>
#include <lexer/scanners/HexScanner.hpp>
#include <lexer/scanners/OctalScanner.hpp>
#include <lexer/scanners/DecimalScanner.hpp>
#include <functional>
#include <memory>

// EN: Dispatches number scanning to hex/octal/decimal scanners.
// FR: Dispatch le scan de nombres vers scanners hexa/octal/decimal.
class NumberScanner final : public IScanner {
public:
    using ErrorHandler = std::function<void(const std::string&)>;
    
        // EN: Initializes sub-scanners with a shared error handler.
        // FR: Initialise les sous-scanners avec un handler d erreur commun.
        NumberScanner(const std::string& , ErrorHandler onError)
                : onError_(std::move(onError)),
                    hexScanner_(new HexScanner(onError_)),
                    octalScanner_(new OctalScanner(onError_)),
                    decimalScanner_(new DecimalScanner(onError_)) {}
    
    // EN: Starts when the first character is a digit.
    // FR: Demarre si le premier caractere est un chiffre.
    bool canScan(char c) const override { 
        return CharClassifier::isDigit(c); 
    }
    
    // EN: Chooses the correct numeric scanner based on prefixes and digits.
    // FR: Choisit le scanner numerique selon prefixe et digits.
    Token scan(SourceReader& reader) override {
        if (reader.peek() == '0') {
            char next = reader.peekNext();
            if (next == 'x' || next == 'X')
                return hexScanner_->scan(reader);
            if (CharClassifier::isOctalDigit(next))
                return octalScanner_->scan(reader);
            
            if (CharClassifier::isDigit(next))
                onError_("invalid digit in octal constant");
        }
        return decimalScanner_->scan(reader);
    }

private:
    ErrorHandler onError_;
    std::unique_ptr<HexScanner> hexScanner_;
    std::unique_ptr<OctalScanner> octalScanner_;
    std::unique_ptr<DecimalScanner> decimalScanner_;
};

// TODO(cc1) EN: Support binary literals if the language mode allows it.
// FR: Supporter les litteraux binaires si le mode de langage le permet.
