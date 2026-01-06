# Mode Debug - Résumé

## Ajouts effectués

### 1. Mode Verbose pour le débogage

Ajout d'une classe `DebugLogger` pour enregistrer les opérations importantes du code génération:

- **Option CLI**: `-debug` pour activer le mode debug
- **Fichier de sortie**: `cc1_debug.log` dans le répertoire courant
- **Logging automatique**: Enregistre toutes les opérations IndexExpr et MemberExpr

### 2. Fichiers modifiés

#### include/driver/ArgumentParser.hpp
- Ajout du champ `debugMode` dans `CompilerOptions`

#### include/codegen/IRGenerator.hpp
- Ajout de la classe `DebugLogger` avec methods:
  - `setEnabled(bool)`: Active/désactive le logging
  - `setOutputFile(string)`: Défini le fichier de sortie
  - `log(string)`: Enregistre un message
  - `logExpr(exprName, type, name, isPointer, isConstant)`: Enregistre les infos d'une expression

#### src/driver/ArgumentParser.cpp
- Parsing de l'option `-debug`

#### src/driver/CompilerDriver.cpp/hpp
- Initialisation du logger dans le constructeur si debug_mode actif

#### src/codegen/IRExpr.cpp
- Logging détaillé dans `visit(IndexExpr&)`:
  - Type et infos du array
  - Type et infos de l'index
  - Décision si array ou non-array
  - Paramètres du GEP
  - Type de résultat

- Logging détaillé dans `visit(MemberExpr&)`:
  - Type et infos de l'objet
  - Operator (. ou ->)
  - Nom du membre
  - structType et structLLVMType
  - Index du membre et son type
  - Paramètres du GEP
  - Type de résultat

## Problèmes identifiés par le logging

### 1. Double-pointer sur les paramètres array (FIXÉ)

**Problème**: Les paramètres de fonction `void f(T array[N])` étaient générés comme `T**` au lieu de `T*`.

**Impact**: IndexExpr retournait des double-pointers, causant des erreurs de GEP.

**Fix**: Ajout d'un check dans IndexExpr pour réduire les double-pointers.

### 2. Inline struct member access (NON FIXÉ)

**Problème**: Pour les structs inline (sans nom), la fonction `extractFieldTypeFromInlineStruct` cherche par index, mais le `memberIdx` n'est jamais correctement calculé pour les champs nommés. Seul le field 0 est retourné pour tous les appels.

**Symptôme dans le log**:
```
[MemberExpr] member=b1 memberIdx=0 memberType={ float, [2 x i32], { i32 } }
```
Le `memberIdx=0` est incorrect pour `b1` (qui doit être index 1).

**Cause racine**: Le code utilise `layout->memberIndices.find(node.member)` qui échoue pour les structs inline (pas de layout), puis utilise `memberIdx=0` par défaut.

**Impact**: L'accès à `array[i].a.b1` retourne le type du premier field au lieu du second field.

**Solutions possibles**:
1. Créer une StructLayout même pour les structs inline
2. Parser les noms des champs depuis le type inline (impossible - les champs inline n'ont pas de noms)
3. Faire en sorte que le sémantique crée des StructLayouts pour tous les structs

### 3. Union member access (NON FIXÉ)

**Problème**: Les unions sont représentées inline sans noms, donc accéder à `.a.c.a` (où `c` est une union) échoue.

## Recommandations pour les prochaines étapes

1. **Priorité haute**: Fixer le member index lookup pour les inline structs
   - Implémenter un calcul d'index basé sur le parsing du type inline
   - Ou créer des StructLayouts pour tous les structs pendant la sémantique

2. **Priorité moyenne**: Fixer les initializers complexes
   - Implémente proper flattening des initializers plats/structurés en C89
   - Gérer les unions et bitfields dans les initializers

3. **Priorité basse**: Optimiser le logging
   - Ajouter des logs pour d'autres expressions si nécessaire
   - Considérer un mode verbose ultra-détaillé pour le développement

## Utilisation du mode debug

```bash
# Compiler avec debug logging
./cc1 tests/samples/program_11.c -debug

# Voir le log
cat cc1_debug.log

# Filtrer par expression
grep "IndexExpr" cc1_debug.log
grep "MemberExpr" cc1_debug.log
grep "ERROR" cc1_debug.log
```

## Résultats actuels

- Programme 10: ✅ PASSE (fixed by correcting double-pointer handling)
- Programme 11: ❌ ÉCHOUE (inline struct member access et initializers complexes)

Les 9 autres programmes: ✅ PASSENT

