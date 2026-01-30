#include <codegen/IRGenerator.hpp>

namespace cc1 {


// EN: Counts flattened members for struct/array initializer expansion.
// FR: Compte les membres flatten pour expansion d init.
size_t IRGenerator::countFlattedMembers(AST::Type* type) {
    if (!type) return 0;

    type = stripQualifiers(type);

    
    if (dynamic_cast<AST::PrimitiveType*>(type)) {
        return 1;
    }
    if (dynamic_cast<AST::PointerType*>(type)) {
        return 1;
    }

    
    
    if (auto* arr = dynamic_cast<AST::ArrayType*>(type)) {
        if (arr->size > 0) {
            return arr->size;
        }
        return 1;  
    }

    
    
    if (auto* st = dynamic_cast<AST::StructType*>(type)) {
        if (st->isUnion) {
            return 1;  
        }
        size_t total = 0;
        size_t i = 0;
        while (i < st->members.size()) {
            const auto& member = st->members[i];

            
            if (member.isBitfield()) {
                total += 1;  
                
                while (i < st->members.size() && st->members[i].isBitfield()) {
                    i++;
                }
            } else {
                total += countFlattedMembers(member.type.get());
                i++;
            }
        }
        return total;
    }

    return 1;
}

} 
