# cc1

Compiler C89/ANSI C vers LLVM IR. Ce repo contient le compilateur `cc1`, un driver `fcc`, des tests, et des echantillons.

## Build et usage

- Build: `make`
- Lancer: `./cc1 <fichier.c>`
- Preprocess only: `./cc1 -E <fichier.c>`
- Driver complet: `./fcc <fichier.c>` (pipeline cc1 -> llc -> as -> clang)


### include/
#### include/driver
- `include/driver/ArgumentParser.hpp` - Interface du parseur d'options CLI.
- `include/driver/CompilerDriver.hpp` - Interface du driver de compilation.

#### include/lexer
- `include/lexer/Lexer.hpp` - Interface principale du lexer.
- `include/lexer/LexerError.hpp` - Exception d'erreur lexicale.
- `include/lexer/KeywordMatcher.hpp` - Mapping identifiant -> mot-cle.
- `include/lexer/IScanner.hpp` - Interface de scanner specialise (id, nombre, etc.).
- `include/lexer/CharClassifier.hpp` - Helpers de classification des caracteres.
- `include/lexer/SourceReader.hpp` - Lecture line/column du source.
- `include/lexer/SourceExtractor.hpp` - Extraction de ligne pour diagnostics.
- `include/lexer/Token.hpp` - Definition des tokens.

#### include/lexer/scanners
- `include/lexer/scanners/IdentifierScanner.hpp` - Scan des identifiants et mots-cles.
- `include/lexer/scanners/NumberScanner.hpp` - Scan des litteraux numeriques.
- `include/lexer/scanners/StringScanner.hpp` - Scan des chaines.
- `include/lexer/scanners/CharScanner.hpp` - Scan des caracteres.
- `include/lexer/scanners/OperatorScanner.hpp` - Scan des operateurs/punctuations.
- `include/lexer/scanners/DecimalScanner.hpp` - Scan des nombres decimaux.
- `include/lexer/scanners/OctalScanner.hpp` - Scan des nombres octaux.
- `include/lexer/scanners/HexScanner.hpp` - Scan des nombres hex.

#### include/parser
- `include/parser/Parser.hpp` - Interface du parser C89.
- `include/parser/ParseError.hpp` - Exception d'erreur de parsing.
- `include/parser/Visitor.hpp` - Interface visitor generique.
- `include/parser/AST.hpp` - AST racine (interface globale).

#### include/parser/ast
- `include/parser/ast/AST.hpp` - Noeud racine AST.
- `include/parser/ast/Node.hpp` - Base commune des noeuds AST.
- `include/parser/ast/Type.hpp` - Types de l'AST.
- `include/parser/ast/Expression.hpp` - Noeuds d'expressions.
- `include/parser/ast/Statement.hpp` - Noeuds de statements.
- `include/parser/ast/Declaration.hpp` - Noeuds de declarations.
- `include/parser/ast/Visitor.hpp` - Interface visitor AST.

#### include/preprocessor
- `include/preprocessor/Preprocessor.hpp` - Interface du preprocessseur.
- `include/preprocessor/PPDirective.hpp` - Definition des directives.
- `include/preprocessor/PPExprEval.hpp` - Evaluation d'expressions #if.
- `include/preprocessor/PPFileHandler.hpp` - Gestion des includes et fichiers.
- `include/preprocessor/PPMacro.hpp` - Definition de macros.
- `include/preprocessor/PPParser.hpp` - Parser des directives de preproc.
- `include/preprocessor/PPUtils.hpp` - Helpers utilitaires pour preproc.

#### include/semantics
- `include/semantics/SemanticAnalyzer.hpp` - Interface de l'analyse semantique.
- `include/semantics/SemanticConstExprVisitors.hpp` - Visitors pour const-expr.
- `include/semantics/SymbolTable.hpp` - Table des symboles.

#### include/codegen
- `include/codegen/IRGenerator.hpp` - Interface generation LLVM IR.
- `include/codegen/IRExprCallHelpers.hpp` - Helpers pour appels de fonctions.

#### include/utils
- `include/utils/Diagnostic.hpp` - Outils de diagnostics/erreurs.
- `include/utils/SourceLocation.hpp` - Structure line/column/source.
- `include/utils/color.hpp` - Helpers de coloration console.

#### include/
- `include/core.hpp` - Definitions centrales (types communs/namespace).

### src/
#### src/main.cpp
- `src/main.cpp` - Point d'entree: parse args, lance le driver.

#### src/driver
- `src/driver/ArgumentParser.cpp` - Implementation du parsing CLI.
- `src/driver/CompilerDriver.cpp` - Orchestration pipeline compilation.
- `src/driver/CompilerDriverPreprocess.cpp` - Etape preprocessing.

#### src/lexer
- `src/lexer/Lexer.cpp` - Implementation du lexer et scan de tokens.
- `src/lexer/Token.cpp` - Implementation des tokens.
- `src/lexer/SourceReader.cpp` - Lecture du source (line/column).
- `src/lexer/SourceExtractor.cpp` - Extraction de lignes pour erreurs.
- `src/lexer/KeywordMatcher.cpp` - Mapping mots-cles.
- `src/lexer/CharClassifier.cpp` - Helpers de classification.

#### src/preprocessor
- `src/preprocessor/Preprocessor.cpp` - Orchestrateur du preprocessseur.
- `src/preprocessor/PreprocessorCore.cpp` - Coeur du preprocessseur.
- `src/preprocessor/PreprocessorDiagnostics.cpp` - Diagnostics preproc.
- `src/preprocessor/PreprocessorDirectivesBasic.cpp` - Directives simples.
- `src/preprocessor/PreprocessorConditionalIf.cpp` - Gestion #if/#elif.
- `src/preprocessor/PreprocessorConditionalElse.cpp` - Gestion #else/#endif.
- `src/preprocessor/PreprocessorBuiltins.cpp` - Macros builtin.
- `src/preprocessor/PPDirective.cpp` - Representations directives.
- `src/preprocessor/PPExprEval.cpp` - Evaluation expressions #if.
- `src/preprocessor/PPExprEvalCore.cpp` - Coeur eval #if.
- `src/preprocessor/PPExprEvalCompare.cpp` - Comparaisons dans #if.
- `src/preprocessor/PPExprEvalBitwise.cpp` - Bitwise dans #if.
- `src/preprocessor/PPExprEvalLogical.cpp` - Logique dans #if.
- `src/preprocessor/PPExprEvalShiftAdd.cpp` - Shift/add/sub dans #if.
- `src/preprocessor/PPExprEvalMulUnaryPrimary.cpp` - Mul/unary/primary #if.
- `src/preprocessor/PPFileHandler.cpp` - IO fichiers/include.
- `src/preprocessor/PPMacro.cpp` - Definition macros.
- `src/preprocessor/PPMacroArgs.cpp` - Parsing des args macro.
- `src/preprocessor/PPMacroTable.cpp` - Table de macros.
- `src/preprocessor/PPMacroExpand.cpp` - Expansion macros.
- `src/preprocessor/PPMacroSubstitute.cpp` - Substitution macros.
- `src/preprocessor/PPParser.cpp` - Parser directives preproc.
- `src/preprocessor/PPParserParse.cpp` - Regles de parsing.
- `src/preprocessor/PPParserUtils.cpp` - Helpers parser preproc.
- `src/preprocessor/PPUtils.cpp` - Utilitaires preproc.

#### src/parser
- `src/parser/ParserCore.cpp` - Coeur du parser (flux principal).
- `src/parser/ParserType.cpp` - Parsing des types.
- `src/parser/ParserDeclarator.cpp` - Parsing des declarators.
- `src/parser/ParserDeclaration.cpp` - Parsing des declarations.
- `src/parser/ParserDeclarationEntry.cpp` - Parsing d'une declaration.
- `src/parser/ParserDeclarationExternal.cpp` - Parsing des declarations externes.
- `src/parser/ParserDeclarationSpecifiers.cpp` - Specifiers de declaration.
- `src/parser/ParserFunctionDefinition.cpp` - Parsing def. de fonction.
- `src/parser/ParserInitializer.cpp` - Parsing des initializers.
- `src/parser/ParserStatement.cpp` - Parsing des statements (dispatcher).
- `src/parser/ParserStatementCore.cpp` - Statements de base.
- `src/parser/ParserStatementControl.cpp` - if/while/for/switch.
- `src/parser/ParserStatementJump.cpp` - return/break/continue/goto.
- `src/parser/ParserExpression.cpp` - Parsing expressions (dispatcher).
- `src/parser/ParserExpressionPrimary.cpp` - Expressions primaires.
- `src/parser/ParserExpressionPrimaryIntFloat.cpp` - Literaux int/float.
- `src/parser/ParserExpressionPrimaryChar.cpp` - Literaux char.
- `src/parser/ParserExpressionPrimaryString.cpp` - Literaux string.
- `src/parser/ParserExpressionPostfix.cpp` - Postfix (call, index, member).
- `src/parser/ParserExpressionCastUnarySizeof.cpp` - cast/unary/sizeof.
- `src/parser/ParserExpressionRelShiftAddMul.cpp` - rel/shift/add/mul.
- `src/parser/ParserExpressionLogicalBitwise.cpp` - logiques/bitwise.
- `src/parser/ParserExpressionOps.cpp` - Operators divers.
- `src/parser/ParserExpressionCommaAssign.cpp` - comma/assign.
- `src/parser/ParserTagSpecifiers.cpp` - struct/union/enum tags.
- `src/parser/Type.cpp` - Definition type AST.
- `src/parser/TypePrimitive.cpp` - Types primitifs.
- `src/parser/TypePointerQualifiedArray.cpp` - Pointeurs/qualifiers/arrays.
- `src/parser/TypeFunction.cpp` - Types fonction.
- `src/parser/TypeRecordEnumTypedef.cpp` - struct/union/enum/typedef.
- `src/parser/AST.cpp` - Construction/gestion AST.
- `src/parser/Declaration.cpp` - Noeuds de declaration.
- `src/parser/Statement.cpp` - Noeuds de statement.
- `src/parser/Expression.cpp` - Noeuds d'expression.

#### src/semantics
- `src/semantics/SemanticCore.cpp` - Orchestrateur semantique.
- `src/semantics/SemanticScope.cpp` - Gestion des scopes.
- `src/semantics/SemanticStmt.cpp` - Verification des statements.
- `src/semantics/SemanticDecl.cpp` - Verification des declarations.
- `src/semantics/SemanticDeclFunction.cpp` - Verification des fonctions.
- `src/semantics/SemanticDeclTranslationUnit.cpp` - Verification TU.
- `src/semantics/SemanticDeclTag.cpp` - Verification struct/union/enum tags.
- `src/semantics/SemanticDeclVarParam.cpp` - Verification vars/params.
- `src/semantics/SemanticExpr.cpp` - Verification expressions.
- `src/semantics/SemanticExprOperators.cpp` - Types/ops.
- `src/semantics/SemanticExprLiterals.cpp` - Literaux.
- `src/semantics/SemanticExprType.cpp` - Typage expressions.
- `src/semantics/SemanticEnum.cpp` - Verification enum.
- `src/semantics/SemanticTypeUtils.cpp` - Utils de types.
- `src/semantics/SemanticTypePredicates.cpp` - Predicats de types.
- `src/semantics/SemanticConstExpr.cpp` - Const-expr: facade.
- `src/semantics/SemanticConstExprAPI.cpp` - API const-expr.
- `src/semantics/SemanticConstExprDivZero.cpp` - Check division par zero.
- `src/semantics/SemanticConstExprInternal.hpp` - Helpers internes.
- `src/semantics/SemanticConstExprInternalUtils.cpp` - Utils internes.
- `src/semantics/SemanticConstExprEval.cpp` - Eval const-expr.
- `src/semantics/SemanticConstExprEvalExpr.cpp` - Eval expr.
- `src/semantics/SemanticConstExprEvalCtor.cpp` - Eval des constructeurs/initializers.
- `src/semantics/SemanticConstExprEvalVisitAtoms.cpp` - Eval atomes.
- `src/semantics/SemanticConstExprEvalVisitBinary.cpp` - Eval binaire.
- `src/semantics/SemanticConstExprEvalVisitUnary.cpp` - Eval unaire.
- `src/semantics/SemanticConstExprEvalVisitTernaryCast.cpp` - Eval ternary/cast.
- `src/semantics/SemanticConstExprEvalTypeSize.cpp` - sizeof constant.
- `src/semantics/SemanticConstExprEvalTypeAlign.cpp` - alignof constant.
- `src/semantics/SemanticConstExprEvalWidth.cpp` - Width/range.
- `src/semantics/SemanticConstExprEvalHelpers.cpp` - Helpers eval.
- `src/semantics/SemanticConstExprTypeSizerBasic.cpp` - Size calc types simples.
- `src/semantics/SemanticConstExprTypeSizerStruct.cpp` - Size calc struct/union.
- `src/semantics/SemanticConstExprIsConst.cpp` - Check constness.

#### src/codegen
- `src/codegen/IRCore.cpp` - Coeur generation IR.
- `src/codegen/IRCoreEntry.cpp` - Entrees/initialisation IR.
- `src/codegen/IRCoreEmit.cpp` - Emission finale IR.
- `src/codegen/IRCoreTemps.cpp` - Gestion temporaires IR.
- `src/codegen/IRCoreSymbols.cpp` - Symboles et scopes IR.
- `src/codegen/IRCoreValueLoad.cpp` - Load d'adresses/valeurs.
- `src/codegen/IRCoreValueStore.cpp` - Store d'adresses/valeurs.
- `src/codegen/IRCoreConstExprInt.cpp` - Const expr int en IR.
- `src/codegen/IRCoreConstExprFloat.cpp` - Const expr float en IR.
- `src/codegen/IRCoreConstructorDebug.cpp` - Debug info/constructors IR.
- `src/codegen/IRDecl.cpp` - Declarations IR (dispatcher).
- `src/codegen/IRDeclTranslationUnit.cpp` - TU IR.
- `src/codegen/IRDeclFunction.cpp` - Fonctions IR.
- `src/codegen/IRDeclVar.cpp` - Variables IR (dispatcher).
- `src/codegen/IRDeclVarGlobal.cpp` - Vars globales.
- `src/codegen/IRDeclVarLocal.cpp` - Vars locales.
- `src/codegen/IRDeclVarFunctionProto.cpp` - Protos de fonction.
- `src/codegen/IRDeclFloat.cpp` - Litteraux float.
- `src/codegen/IRDeclMisc.cpp` - Decls diverses.
- `src/codegen/IRExpr.cpp` - Expressions IR (dispatcher).
- `src/codegen/IRExprBinaryEntry.cpp` - Entrypoint binaire.
- `src/codegen/IRExprBinaryArithmetic.cpp` - Arithmetique.
- `src/codegen/IRExprBinaryComparison.cpp` - Comparaison.
- `src/codegen/IRExprBinaryLogical.cpp` - Logique.
- `src/codegen/IRExprBinaryAssign.cpp` - Assign.
- `src/codegen/IRExprBinaryCompoundAssign.cpp` - Assign compose.
- `src/codegen/IRExprBinaryRegular.cpp` - Cas binaire generique.
- `src/codegen/IRExprUnary.cpp` - Unaire.
- `src/codegen/IRExprCastSizeof.cpp` - Cast/sizeof.
- `src/codegen/IRExprIdentifier.cpp` - Identifiants.
- `src/codegen/IRExprLiterals.cpp` - Literaux.
- `src/codegen/IRExprMember.cpp` - Acces membre.
- `src/codegen/IRExprIndex.cpp` - Indexation tableaux.
- `src/codegen/IRExprCall.cpp` - Appels de fonctions.
- `src/codegen/IRExprTernaryInitList.cpp` - Ternary / init list.
- `src/codegen/IRStmt.cpp` - Statements IR (dispatcher).
- `src/codegen/IRStmtCompoundExpr.cpp` - Compound expressions.
- `src/codegen/IRStmtIf.cpp` - If/else.
- `src/codegen/IRStmtLoops.cpp` - Boucles.
- `src/codegen/IRStmtSwitch.cpp` - Switch.
- `src/codegen/IRStmtSwitchLabels.cpp` - Labels de switch.
- `src/codegen/IRStmtJump.cpp` - return/break/continue/goto.
- `src/codegen/IRTypes.cpp` - Types IR (dispatcher).
- `src/codegen/IRTypesDefaults.cpp` - Defaults types.
- `src/codegen/IRTypesSizeAlign.cpp` - Size/align.
- `src/codegen/IRTypesTypeMapping.cpp` - Mapping type C -> IR.
- `src/codegen/IRTypesQualifiersTypedef.cpp` - Qualifiers/typedef.
- `src/codegen/IRTypesStructRegistry.cpp` - Registre des structs.
- `src/codegen/IRTypesStructLayoutType.cpp` - Layout types.
- `src/codegen/IRTypesStructLayoutTypeStruct.cpp` - Layout struct.
- `src/codegen/IRTypesStructLayoutTypeUnion.cpp` - Layout union.
- `src/codegen/IRTypesStructLayoutDecl.cpp` - Layout decl.
- `src/codegen/IRTypesFlatCount.cpp` - Comptage flatten.
- `src/codegen/IRTypesFlattenedArray.cpp` - Flatten arrays.
- `src/codegen/IRTypesFlattenedStructFromInit.cpp` - Flatten struct init.
- `src/codegen/IRTypesFlatStructInit.cpp` - Init struct flatten.
- `src/codegen/IRTypesInitializerEntry.cpp` - Init entry.
- `src/codegen/IRTypesInitializerArray.cpp` - Init array.
- `src/codegen/IRTypesInitializerStruct.cpp` - Init struct.

#### src/utils
- `src/utils/color.cpp` - Implementation des couleurs console.
