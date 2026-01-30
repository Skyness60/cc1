#include <semantics/SemanticConstExprVisitors.hpp>

#include <algorithm>

namespace cc1 {
namespace {

// EN: Aligns an offset up to the requested alignment.
// FR: Aligne un offset vers le haut selon l alignement.
int alignTo(int offset, int alignment) {
    if (alignment <= 1) return offset;
    return (offset + (alignment - 1)) & ~(alignment - 1);
}

// EN: Removes qualifiers/typedefs to reach underlying type.
// FR: Retire qualifiers/typedefs pour atteindre le type de base.
AST::Type* stripQualifiersAndTypedef(AST::Type* type) {
    if (!type) return nullptr;
    while (auto* qual = dynamic_cast<AST::QualifiedType*>(type)) {
        type = qual->baseType.get();
    }
    if (auto* td = dynamic_cast<AST::TypedefType*>(type)) {
        if (td->underlyingType) {
            type = td->underlyingType.get();
            return stripQualifiersAndTypedef(type);
        }
    }
    return type;
}

} 

// EN: Computes byte size for a type, including structs/bitfields.
// FR: Calcule la taille en octets d un type, structs/bitfields compris.
long long SemanticAnalyzer::ConstExprEvalVisitor::getTypeSize(AST::Type* type) {
    if (!type) return 0;
    type = stripQualifiersAndTypedef(type);

    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        return getPrimitiveSize(prim->kind);
    }
    if (dynamic_cast<AST::PointerType*>(type)) {
        return sema.is64bit_ ? 8 : 4;
    }
    if (auto* arr = dynamic_cast<AST::ArrayType*>(type)) {
        long long size = arr->size;
        if (size < 0 && arr->sizeExpr) {
            sema.evaluateConstantExpr(arr->sizeExpr.get(), size);
        }
        if (size > 0) {
            return size * getTypeSize(arr->elementType.get());
        }
        return 0;
    }
    if (auto* st = dynamic_cast<AST::StructType*>(type)) {
        if (st->members.empty() && !st->name.empty() && sema.currentScope_) {
            Symbol* tag = sema.currentScope_->lookupTag(st->name);
            if (tag && tag->structDecl && tag->structDecl->declaredType) {
                return getTypeSize(tag->structDecl->declaredType.get());
            }
        }
        if (st->members.empty()) return 0;

        if (st->isUnion) {
            long long maxSize = 1;
            int maxAlign = 1;
            for (const auto& m : st->members) {
                if (!m.type) continue;
                long long ms = getTypeSize(m.type.get());
                int ma = getTypeAlign(m.type.get());
                maxSize = std::max<long long>(maxSize, ms);
                maxAlign = std::max(maxAlign, ma);
            }
            return alignTo(static_cast<int>(maxSize > 0 ? maxSize : 1), maxAlign);
        }

        int offset = 0;
        int maxAlign = 1;

        int currentUnitSize = 0;
        int currentUnitAlign = 1;
        int currentUnitBitsTotal = 0;
        int currentUnitBitsUsed = 0;

        // EN: Flushes current bitfield storage unit into the size.
        // FR: Vide l unite de stockage des bitfields dans la taille.
        auto flushUnit = [&]() {
            if (currentUnitBitsUsed > 0 && currentUnitSize > 0) {
                offset += currentUnitSize;
            }
            currentUnitSize = 0;
            currentUnitAlign = 1;
            currentUnitBitsTotal = 0;
            currentUnitBitsUsed = 0;
        };

        // EN: Opens or reopens a storage unit for compatible bitfields.
        // FR: Ouvre/reouvre une unite de stockage pour bitfields compatibles.
        auto openStorageUnitIfNeeded = [&](int unitSize, int unitAlign) {
            if (currentUnitBitsTotal == 0 || currentUnitSize != unitSize || currentUnitAlign != unitAlign) {
                flushUnit();
                offset = alignTo(offset, unitAlign);
                currentUnitSize = std::max(1, unitSize);
                currentUnitAlign = std::max(1, unitAlign);
                currentUnitBitsTotal = currentUnitSize * 8;
                currentUnitBitsUsed = 0;
                maxAlign = std::max(maxAlign, currentUnitAlign);
            }
        };

        for (const auto& m : st->members) {
            if (!m.type) continue;

            if (m.isBitfield()) {
                int unitSize = static_cast<int>(getTypeSize(m.type.get()));
                int unitAlign = getTypeAlign(m.type.get());

                if (m.bitWidth == 0) {
                    flushUnit();
                    offset = alignTo(offset, unitAlign);
                    maxAlign = std::max(maxAlign, unitAlign);
                    continue;
                }

                openStorageUnitIfNeeded(unitSize, unitAlign);

                if (currentUnitBitsUsed + m.bitWidth > currentUnitBitsTotal) {
                    flushUnit();
                    openStorageUnitIfNeeded(unitSize, unitAlign);
                }

                currentUnitBitsUsed += m.bitWidth;
                if (currentUnitBitsUsed == currentUnitBitsTotal) {
                    flushUnit();
                }
                continue;
            }

            flushUnit();

            int memberAlign = getTypeAlign(m.type.get());
            offset = alignTo(offset, memberAlign);
            offset += static_cast<int>(getTypeSize(m.type.get()));
            maxAlign = std::max(maxAlign, memberAlign);
        }

        flushUnit();
        int finalSize = alignTo(offset, maxAlign);
        return finalSize > 0 ? finalSize : 1;
    }
    if (dynamic_cast<AST::EnumType*>(type)) {
        return 4;
    }

    return 4;
}

} 
