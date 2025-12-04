#include <lexer/SourceReader.hpp>

SourceReader::SourceReader(const std::string& source)
    : source_(source) {}

char SourceReader::peek() const {
    return isAtEnd() ? '\0' : source_[pos_];
}

char SourceReader::peekNext() const {
    return (pos_ + 1 >= source_.length()) ? '\0' : source_[pos_ + 1];
}

char SourceReader::advance() {
    char c = source_[pos_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

bool SourceReader::match(char expected) {
    if (isAtEnd() || source_[pos_] != expected) return false;
    advance();
    return true;
}

bool SourceReader::isAtEnd() const {
    return pos_ >= source_.length();
}

void SourceReader::retreat() {
    if (pos_ > 0) {
        pos_--;
        char c = source_[pos_];
        if (c == '\n') {
            line_--;
            // Recalculer column_ en comptant depuis le début de la ligne
            column_ = 1;
            for (size_t i = pos_; i > 0 && source_[i - 1] != '\n'; --i)
                column_++;
        } else {
            column_--;
        }
    }
}
