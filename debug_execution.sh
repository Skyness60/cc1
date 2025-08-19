#!/usr/bin/env bash

# Script de diagnostic pour les problèmes d'exécution
echo "🔍 Diagnostic des problèmes d'exécution"
echo "========================================"

# Test simple qui fonctionne
cat > test_debug_simple.c << 'EOF'
int main(void) {
    return 42;
}
EOF

echo "1. Test simple..."
./fcc test_debug_simple.c -o test_debug_simple
if [[ -x test_debug_simple ]]; then
    echo "✅ Génération OK"
    ./test_debug_simple
    echo "Exit code: $?"
else
    echo "❌ Génération échouée"
fi

echo ""
echo "2. Test avec arithmétique..."
cat > test_debug_arith.c << 'EOF'
int main(void) {
    int a, b, result;
    a = 5;
    b = 10;
    result = a + b;
    return result;
}
EOF

./fcc test_debug_arith.c -o test_debug_arith
if [[ -x test_debug_arith ]]; then
    echo "✅ Génération OK"
    ./test_debug_arith
    echo "Exit code: $?"
else
    echo "❌ Génération échouée"
fi

echo ""
echo "3. Test avec condition simple..."
cat > test_debug_cond.c << 'EOF'
int main(void) {
    int x;
    x = 5;
    if (x == 5)
        return 0;
    else
        return 1;
}
EOF

./fcc test_debug_cond.c -o test_debug_cond
if [[ -x test_debug_cond ]]; then
    echo "✅ Génération OK"
    ./test_debug_cond
    echo "Exit code: $?"
else
    echo "❌ Génération échouée"
    echo "Debugging with verbose..."
    ./fcc -v test_debug_cond.c -o test_debug_cond
fi

# Cleanup
rm -f test_debug_*.c test_debug_simple test_debug_arith test_debug_cond
