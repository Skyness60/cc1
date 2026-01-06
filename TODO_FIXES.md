# TODO: Corrections à faire pour cc1

## ✅ DÉJÀ CORRIGÉ (Session actuelle)

1. ✅ **Énumérations avec valeurs calculées**
   - Ajout du champ `computedValue` dans l'AST
   - Pré-calcul pendant l'analyse sémantique
   - Fix: BitwiseNot avec unsigned 32-bit
   - Fix: Modulo avec unsigned arithmetic
   - **Résultat**: Programme 8 enum values corrects (A=1337, G=2)

2. ✅ **Structures anonymes imbriquées**
   - Génération de noms uniques `.anon.X`
   - Enregistrement des layouts
   - Mise en cache par signature
   - **Résultat**: Programme 11 compile et s'exécute

3. ✅ **Indexation d'arrays dans structures**
   - Fix: Ne pas charger les arrays avant IndexExpr
   - Fix: getelementptr avec deux indices pour `[N x T]*`
   - **Résultat**: `array[i].a.b[0]` fonctionne

4. ✅ **Passage d'arrays en argument**
   - Fix: Ne pas charger les arrays dans CallExpr (decay to pointer)
   - **Résultat**: `print_array(array1)` passe un pointeur

5. ✅ **Compilation multi-fichiers**
   - **Résultat**: Programme 7 (7a+7b) fonctionne

---

## 🔴 À CORRIGER - PRIORITÉ CRITIQUE

### 1. Initialisation variables locales float (Programme 3)

**Problème**: 
```c
float a = 10.5f, b = 3.2f, c = -2.4f;
```
Les floats ont des valeurs aléatoires au lieu des valeurs spécifiées.

**Fichier à modifier**: `src/codegen/IRStmt.cpp` (ou `IRDecl.cpp`)

**Ce qui manque**:
```cpp
// Dans visit(VarDecl) pour les variables locales
if (node.initializer) {
    if (auto* floatLit = dynamic_cast<AST::FloatLiteral*>(node.initializer.get())) {
        // Générer: store float <value>, float* %var
        std::string value = floatLit->repr;  // ou std::to_string(floatLit->value)
        emit("store " + llvmType + " " + value + ", " + llvmType + "* " + localName);
    }
    // ... autres types d'initialiseurs
}
```

**Test**: `comparison_output/cc1_3` devrait afficher `a + b: 10.50 + 3.20 = 13.70`

---

### 2. Initialisation structs/arrays globaux (Programme 11)

**Problème**: 
```c
T array1[4] = {
    {{1, {2}, 3}, 4, 5},
    // ...
};
```
Tout est à 0 au lieu des valeurs spécifiées.

**Fichier à modifier**: `src/codegen/IRDecl.cpp`

**Fonction à corriger**: `getDefaultValue(AST::Type*)`

**Ce qui manque**: Évaluer les initialiseurs au lieu de toujours retourner `zeroinitializer`
```cpp
std::string IRGenerator::generateInitializer(AST::Expression* init, AST::Type* type) {
    if (auto* intLit = dynamic_cast<AST::IntegerLiteral*>(init)) {
        return std::to_string(intLit->value);
    }
    if (auto* floatLit = dynamic_cast<AST::FloatLiteral*>(init)) {
        return floatLit->repr;
    }
    if (auto* initList = dynamic_cast<AST::InitializerList*>(init)) {
        // Générer: { val1, val2, ... }
        // Pour arrays: [ val1, val2, ... ]
    }
    return "zeroinitializer";
}
```

**Test**: `array1[0].a.a` devrait être `1.0` pas `0.0`

---

### 3. Boucle infinie (Programme 10)

**Problème**: Le programme ne termine jamais.

**Fichier à investiguer**: 
- `comparison_output/cc1_10.ll` (regarder le LLVM IR généré)
- `src/codegen/IRStmt.cpp` (visit pour ForStmt ou WhileStmt)

**À vérifier**:
1. Conditions de boucle correctes
2. Incréments de boucle générés
3. Branchements conditionnels

**Test**: Le programme devrait terminer rapidement

---

## 🟡 À CORRIGER - PRIORITÉ MOYENNE

### 4. Alignement et padding structs (Programme 8)

**Problème**: 
```
size of struct s: 8   (cc1)
size of struct s: 12  (clang correct)
```

**Fichier à modifier**: `src/codegen/IRTypes.cpp`

**Fonctions à corriger**:
- `computeStructLayout()`
- `getTypeSize()`
- `getTypeAlign()`

**Ce qui manque**: Règles d'alignement C89
```cpp
// Pour chaque membre du struct:
int alignment = getTypeAlign(member.type);
// Ajouter padding si nécessaire
int offset = (currentOffset + alignment - 1) & ~(alignment - 1);
// Ajouter padding final pour aligner sur le plus grand membre
```

**Test**: `sizeof(struct s)` devrait donner 12

---

### 5. Bitfields (Programme 9)

**Problème**: 
```
position: 96  (cc1 - incorrect)
position: 0   (clang - correct)
```

**Fichier à modifier**: `src/codegen/IRExpr.cpp` (accès membres bitfield)

**Ce qui manque**: Extraction de bits
```cpp
// Pour un bitfield à offset X de N bits:
// 1. Charger le conteneur (char/short/int)
// 2. Shifter: value >> offset
// 3. Masquer: value & ((1 << width) - 1)
```

**Test**: Accès à `c.alpha.pos` devrait extraire les bons bits

---

## 📋 CHECKLIST DE TESTS

Après chaque correction, lancer:
```bash
./quick_test.sh
```

Objectif final: **12/12 programmes ✅**

Actuellement: **7/12 programmes ✅** (58%)

---

## 🎯 ORDRE DE CORRECTION RECOMMANDÉ

1. **Programme 3** (floats locaux) - Impact: HIGH, Difficulté: LOW
2. **Programme 10** (boucle infinie) - Impact: BLOCKING, Difficulté: MEDIUM
3. **Programme 11** (structs globaux) - Impact: HIGH, Difficulté: MEDIUM
4. **Programme 8** (alignement) - Impact: LOW, Difficulté: MEDIUM
5. **Programme 9** (bitfields) - Impact: LOW, Difficulté: HIGH

---

## 📝 NOTES

- Programme 2: La différence est juste le nom du programme (argv[0]), c'est normal
- Les énumérations marchent parfaitement maintenant
- La compilation multi-fichiers fonctionne
- Les structures complexes compilent (même si l'initialisation manque)
