#pragma once

#include <string>
#include <vector>
#include <unordered_set>

namespace cc1 {
namespace pp {

// ============================================================================
// File Handler - manages file reading and include paths
// ============================================================================

class FileHandler {
public:
    // Add include search path
    void addIncludePath(const std::string& path);
    
    // Set/get current file being processed
    void setCurrentFile(const std::string& file);
    const std::string& getCurrentFile() const;
    
    // Read file contents
    std::string readFile(const std::string& filename);
    
    // Find include file (returns full path or empty string)
    std::string findInclude(const std::string& filename, bool isSystemInclude);
    
    // Pragma once support
    bool hasIncludedOnce(const std::string& path) const;
    void markIncludedOnce(const std::string& path);
    
private:
    std::vector<std::string> includePaths_;
    std::string currentFile_;
    std::unordered_set<std::string> includedOnce_;
    
    // Check if file exists
    bool fileExists(const std::string& path) const;
    
    // Get directory from path
    std::string getDirectory(const std::string& path) const;
    
    // Join path components
    std::string joinPath(const std::string& dir, const std::string& file) const;
};

} // namespace pp
} // namespace cc1
