# cc1 — Compilateur C89 complet (Version Containerisée)

cc1 est un compilateur C89 complet avec lexer, parser, analyse sémantique, préprocesseur interne et génération LLVM IR, pensé pour être strict et conforme au standard C89. Il s'intègre dans une toolchain complète via le driver `fcc` **entièrement containerisé**.

## ⚠️ Version Containerisée

Cette version utilise **exclusivement Docker** pour toutes les opérations de compilation :
- ✅ **clang** : Préprocesseur et linker dans le conteneur
- ✅ **llc** : Compilateur LLVM IR vers assembleur dans le conteneur  
- ✅ **as** : Assembleur dans le conteneur
- ✅ **cc1** : Compilateur C89 vers LLVM IR dans le conteneur

**Aucune dépendance** sur les outils système de l'hôte requis.

## Synopsis

```
cc1 infile [-o outfile]
fcc [options] <infile.c>
```

## Build

La compilation utilise **uniquement Docker** :
- `make cc1` : Build containerisé (recommandé)
- `make cc1-local` : Build local pour développement uniquement

Le driver `fcc` utilise automatiquement le conteneur Docker pour toutes les opérations.

## Utilisation

- Compiler un fichier C89 vers LLVM IR:
  - `./cc1 main.c -o main.ll`

- Modes de développement:
  - `./cc1 --lex-only file.c` : Analyse lexicale uniquement
  - `./cc1 --parse-expr file.c` : Parser expressions uniquement  
  - `./cc1 --parse-tu file.c` : Parser translation unit uniquement
  - `./cc1 --sem file.c` : Analyse sémantique uniquement

- Driver complet (préprocesseur → compile → assemble → link) **containerisé**:
  - `./fcc main.c -o main` : Compilation complète dans conteneur
  - `./fcc -c main.c` : Compilation vers objet (.o) dans conteneur
  - `./fcc -S main.c` : Compilation vers assembleur (.s) dans conteneur

- Suite de tests containerisés:
  - `make test-compile` : Test de compilation containerisée
  - `make test-all-modes` : Test de tous les modes de compilation
  - `./test-container.sh` : Suite de tests complète
  - `make test-abi` : Tests ABI/runtime (System V i386 : structs, alignement/padding, promotions varargs)

## Driver fcc (Containerisé)

Le compilateur s'utilise via son driver `fcc` qui orchestre la toolchain complète **dans un conteneur Docker** :

```bash
./fcc [options] <infile.c|->
```

- **Pipeline containerisé**: `docker run clang -E` → `docker run cc1` → `docker run llc` → `docker run as` → `docker run clang`
- Modes: `-E` (préprocesseur seulement), `-S` (ASM), `-c` (objet), défaut: link → `a.out`
- Sorties par défaut: `<in>.s`, `<in>.o`, `a.out`; `-o -` pour stdout
- Entrée depuis stdin supportée via l'opérande `-`
- Options POSIX c17 implémentées (sauf `-B -G -R`). Voir `./fcc --help` pour les sections: SYNOPSIS, DESCRIPTION, OPTIONS, OPERANDS, STDIN, INPUT FILES, STDOUT, STDERR, EXIT STATUS
- Pipeline exacte: `docker run cc1` (préprocesseur interne) → `docker run cc1` (LLVM IR) → `docker run llc` → `docker run as` → `docker run clang`
- Option `--use-clang-pp` pour basculer sur `docker run clang -E` (fallback)

## Cibles et standard

- Architecture: Intel 386 (i386) par défaut
- Standard: C89 (ANSI/ISO 9899-1990) strict
- ABI: System V ABI pour i386
- LLVM: versions récentes supportées (≥14, testé avec 14–20)

## Mode strict C89

Le compilateur applique des règles strictes C89:
- Interdit `//` (C99) → erreur lexicale
- Interdit les préfixes larges `L"..."` et `L'...'`
- Concaténation de chaînes adjacentes
- Échappements supportés: standard (`\n`, `\t`, …), octal (`\[0-7]{1,3}`), hex (`\x` + ≥1 hexa). `\x` sans chiffre hexa est une erreur
- Littéraux de caractère: mêmes règles d'échappement; quote non fermée → erreur
- Nombres entiers: décimal, octal, hex. Erreurs: `0x` sans chiffres, chiffre 8/9 dans octal, suffixes invalides (`ll`, `f` sur int, `u` sur float, combinaisons non C89)
- Nombres flottants: formes `.5`, `1.`, `1e10`, etc.; l'exposant doit contenir des chiffres; suffixes `f/F` ou `l/L` (pas les deux)
- Opérateurs et ponctuation complets, y compris `...` (ellipsis)
- Caractères inconnus (ex: non‑ASCII isolés) → erreur

## Fonctionnalités avancées

### Préprocesseur C89 complet
- Phases 1-4 du standard C89 complètement implémentées
- Support complet des macros (#define, #undef, #ifdef, #ifndef, #if, #else, #endif)
- Directives #include avec recherche de fichiers  
- Évaluation d'expressions constantes dans les directives conditionnelles
- Gestion de la récursion infinie et des dépendances circulaires

### Informations de debug DWARF complètes
- Génération complète de métadonnées DWARF pour gdb
- Support des types de base C89 (int, char, float, double, pointeurs)
- Informations de fonctions avec noms et types
- Variables locales avec noms, types et emplacements
- Compatible avec gdb pour le debugging step-by-step

### Cross-compilation i386/x86_64
- Support complet des architectures 32-bit et 64-bit
- Cohérence des tailles de types et alignements selon l'architecture
- Évaluation correcte des expressions constantes par architecture
- Génération d'ABI approprié (System V i386/x86_64)

## Tests

Suites de tests complètes:
- Lexer : validation stricte C89 + détection erreurs
- Parser : grammaire C89 complète + validation erreurs
- Sémantique : type checking + const-eval + varargs
- Génération : LLVM IR + System V ABI

## Exemple

```c
#include <stdio.h>
int main() {
    printf("hello, world\n");
    return 0;
}
```

```
$ ./fcc hello.c -o hello
$ ./hello
hello, world
```
