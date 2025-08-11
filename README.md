# cc1 — Front-end C89 strict

cc1 est un frontend C89 (lexer pour l’instant), pensé pour être strict et simple à intégrer dans une toolchain (ex: piloté par un driver qui prétraite avec clang/gcc).

## Build

Deux options:
- Docker (si Docker Hub accessible): `make cc1`
- Local (sans Docker): `make cc1-local`

Le binaire est disponible en `./cc1` (lien symbolique vers `target/release/cc1`).

## Utilisation

- Lancer sur un fichier:
  - `./cc1 tests/lex/valid_basic.c`

- Suite de tests lexer:
  - `make test-lex`

## Mode strict C89

Le lexer applique des règles strictes C89:
- Interdit `//` (C99) → erreur lexicale
- Interdit les préfixes larges `L"..."` et `L'...'`
- Concaténation de chaînes adjacentes
- Échappements supportés: standard (`\n`, `\t`, …), octal (`\\[0-7]{1,3}`), hex (`\\x` + ≥1 hexa). `\\x` sans chiffre hexa est une erreur
- Littéraux de caractère: mêmes règles d’échappement; quote non fermée → erreur
- Nombres entiers: décimal, octal, hex. Erreurs: `0x` sans chiffres, chiffre 8/9 dans octal, suffixes invalides (`ll`, `f` sur int, `u` sur float, combinaisons non C89)
- Nombres flottants: formes `.5`, `1.`, `1e10`, etc.; l’exposant doit contenir des chiffres; suffixes `f/F` ou `l/L` (pas les deux)
- Opérateurs et ponctuation complets, y compris `...` (ellipsis)
- Caractères inconnus (ex: non‑ASCII isolés) → erreur

## Préprocesseur recommandé

Le C standard impose que certaines transformations (trigraphes, “line splicing” avec `\\` en fin de ligne, directives `#include/#define`, etc.) s’appliquent avant la phase lexicale.

Ce projet considère le préprocesseur hors du scope du lexer. En production, utilisez un préprocesseur C89 avant `cc1`, par exemple:

- `clang -E -std=c89 input.c | ./cc1 -`
- ou via un driver qui gère l’invocation de `-E` et passe le flux prétraité.

Sans préprocesseur, `cc1` analysera le source brut: les trigraphes et la continuation de ligne globale ne sont pas gérés (seules les continuations dans les chaînes via `\\\n` sont acceptées).

## Limites actuelles

- Pas de trigraphes ni de “line splicing” global (préprocesseur requis)
- Pas de support des encodages/large chars C locale‑dépendants
- Lexer uniquement (parser non implémenté)

## Roadmap

- Parser C89 (déclarations, expressions, types, fonctions)
- Intégration avec un driver (prétraitement + pipeline)
- Suite de tests élargie et CI
