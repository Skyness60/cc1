# cc1 — Compilateur C89 strict

cc1 est un compilateur C89 complet avec lexer, parser, analyse sémantique et génération LLVM IR, pensé pour être strict et conforme au standard. Il s'intègre dans une toolchain via le driver `fcc`.

## Synopsis

```
cc1 infile [-o outfile]
fcc [options] <infile.c>
```

## Build

Deux options:
- Docker (si Docker Hub accessible): `make cc1`
- Local (sans Docker): `make cc1-local`

Le binaire est disponible en `./cc1` (lien symbolique vers `target/release/cc1`).

## Utilisation

- Compiler un fichier C89 vers LLVM IR:
  - `./cc1 main.c -o main.ll`

- Modes de développement:
  - `./cc1 --lex-only file.c` : Analyse lexicale uniquement
  - `./cc1 --parse-expr file.c` : Parser expressions uniquement  
  - `./cc1 --parse-tu file.c` : Parser translation unit uniquement
  - `./cc1 --sem file.c` : Analyse sémantique uniquement

- Driver complet (préprocesseur → compile → assemble → link):
  - `./fcc main.c -o main` : Compilation complète
  - `./fcc -c main.c` : Compilation vers objet (.o)
  - `./fcc -S main.c` : Compilation vers assembleur (.s)

- Suite de tests:
  - `make test-lex` : Tests lexer
  - `make test-parse` : Tests parser  
  - `make test-sem` : Tests sémantique
  - `make test-abi` : Tests ABI/runtime (System V i386 : structs, alignement/padding, promotions varargs)

## Driver fcc

Le compilateur s'utilise principalement via son driver `fcc` qui orchestre la toolchain complète :

```bash
./fcc [options] <infile.c|->
```

- Modes: `-E` (préprocesseur seulement), `-S` (ASM), `-c` (objet), défaut: link → `a.out`
- Sorties par défaut: `<in>.s`, `<in>.o`, `a.out`; `-o -` pour stdout
- Entrée depuis stdin supportée via l'opérande `-`
- Options POSIX c17 implémentées (sauf `-B -G -R`). Voir `./fcc --help` pour les sections: SYNOPSIS, DESCRIPTION, OPTIONS, OPERANDS, STDIN, INPUT FILES, STDOUT, STDERR, EXIT STATUS
- Pipeline exacte: `cc1` (préprocesseur interne) → `cc1` (LLVM IR) → `llc` → `as -c` → `clang -m32 -no-pie`
- Option `--use-clang-pp` pour utiliser `clang -E` au lieu du préprocesseur interne

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

## Limitations actuelles

- Génération LLVM IR basique (optimisations à faire côté LLVM)
- Pas de support des encodages/large chars C locale‑dépendants

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
