#include <preprocessor/PPFileHandler.hpp>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <libgen.h>
#include <cstring>
#include <cstdlib>
#include <climits>

namespace cc1 {
namespace pp {

void FileHandler::addIncludePath(const std::string& path) {
    includePaths_.push_back(path);
}

void FileHandler::setCurrentFile(const std::string& file) {
    currentFile_ = file;
}

const std::string& FileHandler::getCurrentFile() const {
    return currentFile_;
}

std::string FileHandler::readFile(const std::string& filename) {
    // Check pragma once
    if (hasIncludedOnce(filename)) {
        return "";
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string FileHandler::findInclude(const std::string& filename, bool isSystemInclude) {
    // For local includes, first search relative to current file
    if (!isSystemInclude && !currentFile_.empty()) {
        std::string dir = getDirectory(currentFile_);
        std::string path = joinPath(dir, filename);
        if (fileExists(path)) {
            return path;
        }
    }
    
    // Search in include paths
    for (size_t i = 0; i < includePaths_.size(); i++) {
        std::string path = joinPath(includePaths_[i], filename);
        if (fileExists(path)) {
            return path;
        }
    }
    
    // Try standard system paths for system includes
    if (isSystemInclude) {
        static const char* systemPaths[] = {
            "/usr/include",
            "/usr/local/include",
            "/usr/include/x86_64-linux-gnu"
        };
        
        for (size_t i = 0; i < sizeof(systemPaths) / sizeof(systemPaths[0]); i++) {
            std::string path = joinPath(systemPaths[i], filename);
            if (fileExists(path)) {
                return path;
            }
        }
    }
    
    // Try current directory as last resort
    if (fileExists(filename)) {
        return filename;
    }
    
    return "";
}

bool FileHandler::hasIncludedOnce(const std::string& path) const {
    return includedOnce_.find(path) != includedOnce_.end();
}

void FileHandler::markIncludedOnce(const std::string& path) {
    includedOnce_.insert(path);
}

bool FileHandler::fileExists(const std::string& path) const {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string FileHandler::getDirectory(const std::string& path) const {
    if (path.empty()) return ".";
    
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        return ".";
    }
    
    return path.substr(0, pos);
}

std::string FileHandler::joinPath(const std::string& dir, const std::string& file) const {
    if (dir.empty() || dir == ".") {
        return file;
    }
    
    if (dir.back() == '/') {
        return dir + file;
    }
    
    return dir + "/" + file;
}

} // namespace pp
} // namespace cc1
