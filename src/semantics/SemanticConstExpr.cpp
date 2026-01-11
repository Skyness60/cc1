#include <semantics/SemanticAnalyzer.hpp>

// Split implementation units (all <200 lines):
// - src/semantics/SemanticConstExprIsConst.cpp
// - src/semantics/SemanticConstExprEvalCtor.cpp
// - src/semantics/SemanticConstExprEvalTypeAlign.cpp
// - src/semantics/SemanticConstExprEvalTypeSize.cpp
// - src/semantics/SemanticConstExprEvalWidth.cpp
// - src/semantics/SemanticConstExprEvalVisitAtoms.cpp
// - src/semantics/SemanticConstExprEvalVisitUnary.cpp
// - src/semantics/SemanticConstExprEvalVisitBinary.cpp
// - src/semantics/SemanticConstExprEvalVisitTernaryCast.cpp
// - src/semantics/SemanticConstExprDivZero.cpp
// - src/semantics/SemanticConstExprAPI.cpp
