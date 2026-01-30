# cc1

Compilateur C89 → LLVM IR avec driver complet `fcc` (préproc clang -E, compilation cc1, abaissement llc/llc-12, assemblage, lien).

## Build & prérequis

- Dépendances : `clang`, `llc` (ou `llc-12`), `as`, `clang` (lien).
- Build : `make`
- Nettoyer : `make clean`

## Usage rapide

- Compiler et lier : `./fcc prog.c`
- Arrêter à l’IR : `./cc1 prog.c > prog.ll`
- Préprocesser seulement : `./cc1 -E prog.c`
- Conserver les temporaires : `FCC_KEEP_TMP=1 ./fcc prog.c`

Le driver :
- Utilise `llc` si disponible, sinon `llc-12`, sinon erreur.
- Strip les marqueurs de ligne `# 1 "file"` après préproc pour éviter les erreurs sur certaines plateformes.
- Supprime les module flags PIC/PIE dans l’IR avant llc-12.

## Pipelines

- Minimal : `clang -E` → `cc1` → `llc/llc-12` → `as` → `clang` (lien)
- Variantes :
  - `./fcc -S prog.c` produit `prog.s`
  - `./fcc -c prog.c` produit `prog.o`
  - `./fcc -E prog.c` affiche le préprocesseur
- Exemple (pipeline manuel équivalent) :
```/dev/null/pipeline_example.sh#L1-3
clang -E prog.c | ./cc1 > prog.ll
llc prog.ll -o prog.s
as prog.s -o prog.o && clang prog.o -o prog
```

### Driver
- `src/driver/ArgumentParser.cpp` : parse les options CLI (`-E`, `-S`, `-c`, `-o`, `-m32/-m64`, etc.) pour normaliser les arguments et protéger le reste du pipeline des erreurs d’entrée.
- `src/driver/CompilerDriver.cpp` : orchestre la chaîne complète (préproc → cc1 → llc → as → clang) pour offrir une commande unique cohérente.
- `src/driver/CompilerDriverPreprocess.cpp` : exécute uniquement l’étape préprocesseur quand demandée, afin de fournir un mode `-E` isolé sans passer par le reste de la chaîne.
- Exemple (pipeline complet) :
```/dev/null/driver_example.sh#L1-4
./fcc hello.c          # compile et lie
./fcc -S hello.c       # s'arrête à l'ASM
./fcc -E hello.c       # préproc only
FCC_KEEP_TMP=1 ./fcc hello.c  # conserve les temporaires
```

### Lexer
- `src/lexer/CharClassifier.cpp` : classification des caractères (lettres, digits, espace, opérateurs) pour diriger rapidement chaque caractère vers le bon scanner.
- `src/lexer/KeywordMatcher.cpp` : reconnaissance des mots-clés C89 afin de différencier identifiants et mots réservés.
- Exemple (mot réservé vs identifiant) :
```/dev/null/keyword_example.c#L1-3
int main(void) {  /* 'int' et 'void' sont des mots réservés, 'main' est un identifiant */
  return 0;
}
```
- `src/lexer/Lexer.cpp` : boucle principale de lexing et émission des tokens, première barrière pour valider la syntaxe de surface.
- `src/lexer/SourceExtractor.cpp` : extrait les lignes pour les messages d’erreur, pour des diagnostics contextualisés.
- `src/lexer/SourceReader.cpp` : lecture caractère par caractère avec suivi ligne/colonne, nécessaire aux positions précises dans les erreurs.
- `src/lexer/Token.cpp` : définition et helpers sur les tokens, base de communication entre lexer et parser.
- Exemple (tokens attendus : int/ident/assign/int_literal/semicolon/EOF) :
```/dev/null/lexer_example.c#L1-3
int x = 42;
```

### Préprocesseur
- `src/preprocessor/PreprocessorCore.cpp` : pipeline général (lecture, macros, directives, sortie) pour orchestrer toutes les étapes du préproc.
- `src/preprocessor/PreprocessorDiagnostics.cpp` : messages d’erreur/avertissements préproc afin de signaler proprement les directives invalides.
- `src/preprocessor/PreprocessorDirectivesBasic.cpp` : directives simples (#define, #undef, #include…) pour couvrir le socle C89.
- `src/preprocessor/PreprocessorConditionalIf.cpp` : gestion #if/#elif pour activer/désactiver des branches de code.
- `src/preprocessor/PreprocessorConditionalElse.cpp` : gestion #else/#endif pour fermer proprement les conditionnels.
- `src/preprocessor/PreprocessorBuiltins.cpp` : macros prédéfinies (__DATE__, __TIME__, etc.) pour compatibilité avec les toolchains classiques.
- `src/preprocessor/PPDirective.cpp` : représentation des directives (#pragma, #line, #error…) pour uniformiser leur traitement.
- `src/preprocessor/PPExprEval*.cpp` : évaluation d’expressions #if (compare, bitwise, logique, arithmétique) afin de décider des branches actives.
- `src/preprocessor/PPFileHandler.cpp` : ouverture de fichiers et résolution des includes pour gérer correctement les dépendances.
- `src/preprocessor/PPMacro*.cpp` : stockage, parsing, expansion et substitution de macros pour implémenter le cœur de la macro-expansion.
- `src/preprocessor/PPParser*.cpp` : parsing des directives et utilitaires de parsing pour convertir les lignes `#` en structures internes.
- `src/preprocessor/PPUtils.cpp` : fonctions utilitaires communes au préprocesseur pour factoriser le code partagé.
- Exemple (#if + macro) :
```/dev/null/pp_example.c#L1-10
#define FLAG 1
#define ADD(x,y) ((x)+(y))

int main(void) {
#if FLAG
  return ADD(20, 22);  /* 42 */
#else
  return 0;
#endif
}
```

### Parser
- `src/parser/ParserCore.cpp` : boucle principale de parsing, coordination des sous-parsers pour construire l’AST complet.
- `src/parser/ParserType.cpp` : parsing des types de base pour poser les fondations du typage.
- `src/parser/ParserDeclarator.cpp` : parsing des declarators (pointeurs, arrays, fonctions) afin d’associer les identifiants à leur forme déclarative.
- `src/parser/ParserDeclaration*.cpp` : parsing des déclarations (externes, specifiers, entrées) pour enregistrer symboles et propriétés.
- `src/parser/ParserFunctionDefinition.cpp` : parsing des définitions de fonctions pour assembler prototypes, corps et paramètres.
- `src/parser/ParserInitializer.cpp` : parsing des initialiseurs (scalaires et composites) afin de peupler correctement les objets.
- `src/parser/ParserStatement*.cpp` : parsing des statements (if/while/for/switch, jump, blocs) pour structurer le contrôle de flux.
- `src/parser/ParserExpression*.cpp` : parsing des expressions (primaires, postfix, unary, cast, arith, logique, assign) pour produire les arbres d’expressions typables.
- `src/parser/ParserTagSpecifiers.cpp` : parsing des struct/union/enum pour définir les types agrégés.
- `src/parser/Type*.cpp` : structures de type (primitifs, pointeurs/qualifs/arrays, fonctions, struct/union/enum/typedef) pour matérialiser le système de types.
- `src/parser/AST.cpp`, `Declaration.cpp`, `Statement.cpp`, `Expression.cpp` : construction et gestion des nœuds AST pour fournir une représentation intermédiaire cohérente au reste du pipeline.
- Exemple (déclaration, définition, appel) :
```/dev/null/parser_example.c#L1-11
int add(int a, int b);

int add(int a, int b) { return a + b; }

int main(void) {
  return add(20, 22); /* parser : decl -> def -> call */
}
```

### Sémantique
- `src/semantics/SemanticCore.cpp` : orchestration de l’analyse sémantique sur l’AST pour déclencher toutes les vérifications.
- `src/semantics/SemanticScope.cpp` : gestion des portées et des symboles pour garantir la résolution correcte des identifiants.
- `src/semantics/SemanticStmt.cpp` : vérifications des statements afin d’assurer la validité des flux de contrôle.
- `src/semantics/SemanticDecl*.cpp` : vérifications des déclarations (fonctions, tags, variables/paramètres, translation unit) pour valider signatures et liaisons.
- `src/semantics/SemanticExpr*.cpp` : typage et vérifications des expressions (littéraux, opérateurs, conversions) pour produire des expressions bien typées avant IR.
- `src/semantics/SemanticEnum.cpp` : vérifications spécifiques aux enums pour respecter les règles de valeur et de portée.
- `src/semantics/SemanticTypePredicates.cpp` : prédicats et utilitaires de types pour factoriser les règles de compatibilité.
- `src/semantics/SemanticConstExpr*.cpp` : évaluation et vérification des constantes à la compilation (div0, sizeof/alignof, largeur, etc.) pour détecter tôt les erreurs de constantes.
- `src/semantics/SemanticConstExprInternalUtils.cpp` : helpers internes pour l’éval const-expr afin de centraliser les routines partagées.
- Exemple (enum + portée/valeur) :
```/dev/null/sem_enum_example.c#L1-8
enum Color { RED = 1, GREEN, BLUE };

int main(void) {
  enum Color c = BLUE;
  int x = RED + 1;      /* valeurs entières autorisées */
  return c == BLUE ? x : -1;
}
```
- Exemple (compatibilité de types) :
```/dev/null/sem_typepred_example.c#L1-6
int *p;
double *q;

int main(void) {
  p = q; /* devrait être signalé : pointeurs incompatibles */
  return 0;
}
```
- Exemple (const-expr + vérification) :
```/dev/null/sem_example.c#L1-9
int arr[2] = {1, 2};
int sz = sizeof(arr) / sizeof(arr[0]);

int main(void) {
  int x = 1 / 0; /* devrait signaler div0 en const-expr si évalué */
  return sz;
}
```

### Codegen (LLVM IR)
- `src/codegen/IRCore*.cpp` : gestion des blocs, temporaires, symboles, entrées, émission finale pour structurer le module LLVM.
- `src/codegen/IRDecl*.cpp` : génération IR pour déclarations (TU, fonctions, variables locales/globales, protos, floats, divers) pour matérialiser les entités C dans l’IR.
- `src/codegen/IRExpr*.cpp` : génération IR pour expressions (arith, comparaison, logique, assign, appels, identifiants, index, membres, littéraux, unaires, ternaires/init-list) afin de traduire l’AST en instructions LLVM.
- `src/codegen/IRStmt*.cpp` : génération IR pour statements (if/else, loops, switch, labels, jumps, expressions composées) pour construire le contrôle de flux LLVM.
- `src/codegen/IRTypes*.cpp` : mappage type C → type LLVM, tailles/alignements, flatten des structs/arrays, initializers pour assurer la correspondance fidèle des types.
- `src/codegen/IRCoreConstructorDebug.cpp` : infos de debug lors de la construction IR pour aider au diagnostic des issues de génération.
- `src/codegen/IRCoreConstExpr*.cpp` : matérialisation IR pour const-expr int/float pour insérer des constantes évaluées à la compilation.

### Utils
- `src/utils/color.cpp` : coloration des diagnostics pour rendre les messages plus lisibles.
