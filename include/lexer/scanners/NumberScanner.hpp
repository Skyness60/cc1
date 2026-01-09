#pragma once

#include <lexer/IScanner.hpp>
#include <lexer/CharClassifier.hpp>
#include <lexer/scanners/HexScanner.hpp>
#include <lexer/scanners/OctalScanner.hpp>
#include <lexer/scanners/DecimalScanner.hpp>
#include <functional>
#include <memory>

class NumberScanner final : public IScanner {
public:
    using ErrorHandler = std::function<void(const std::string&)>;
    
        NumberScanner(const std::string& /*filename*/, ErrorHandler onError)
                : onError_(std::move(onError)),
                    hexScanner_(new HexScanner(onError_)),
                    octalScanner_(new OctalScanner(onError_)),
                    decimalScanner_(new DecimalScanner(onError_)) {}
    
    bool canScan(char c) const override { 
        return CharClassifier::isDigit(c); 
    }
    
    Token scan(SourceReader& reader) override {
        if (reader.peek() == '0') {
            char next = reader.peekNext();
            if (next == 'x' || next == 'X')
                return hexScanner_->scan(reader);
            if (CharClassifier::isOctalDigit(next))
                return octalScanner_->scan(reader);
            // In C, a leading 0 starts an octal constant; 08/09 are invalid.
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
