#!/bin/bash

# Script de comparaison cc1 vs clang

cd /home/sperron/Work/cc1

echo "==============================================="
echo "Comparaison des sorties: cc1 vs clang -m32 -std=c89"
echo "==============================================="
echo ""

for i in {0..11}; do
    # Ignorer program_7 qui n'existe pas
    if [ ! -f "tests/samples/program_${i}.c" ]; then
        continue
    fi
    
    echo "════════════════════════════════════════════════"
    echo "  PROGRAM ${i}: tests/samples/program_${i}.c"
    echo "════════════════════════════════════════════════"
    
    # Compilation avec cc1
    echo ""
    echo "--- Compilation avec cc1 ---"
    if ./cc1 tests/samples/program_${i}.c -o comparison_output/cc1_${i}.ll 2>&1 > /dev/null; then
        # Convertir LLVM IR en assembleur x86 avec les bons flags
        if llc -march=x86 -relocation-model=pic -mattr=+sse2 comparison_output/cc1_${i}.ll -o comparison_output/cc1_${i}.s 2>&1 > /dev/null; then
            # Assembler et lier avec -no-pie
            if gcc -m32 -no-pie comparison_output/cc1_${i}.s -o comparison_output/cc1_${i}.bin 2>&1 > /dev/null; then
                echo "✓ cc1: Compilation réussie"
                # Exécution
                echo "Sortie cc1:"
                timeout 2 comparison_output/cc1_${i}.bin 2>&1 | head -50
                CC1_EXIT=$?
                echo "Code de sortie cc1: $CC1_EXIT"
            else
                echo "✗ cc1: Échec de l'assemblage"
                CC1_EXIT="N/A"
            fi
        else
            echo "✗ cc1: Échec de la conversion LLVM IR"
            CC1_EXIT="N/A"
        fi
    else
        echo "✗ cc1: Échec de la compilation"
        CC1_EXIT="N/A"
    fi
    
    # Compilation avec clang
    echo ""
    echo "--- Compilation avec clang -m32 -std=c89 ---"
    if clang -m32 -std=c89 tests/samples/program_${i}.c -o comparison_output/clang_${i}.bin 2>&1 > /dev/null; then
        echo "✓ clang: Compilation réussie"
        # Exécution
        echo "Sortie clang:"
        timeout 2 comparison_output/clang_${i}.bin 2>&1 | head -50
        CLANG_EXIT=$?
        echo "Code de sortie clang: $CLANG_EXIT"
    else
        echo "✗ clang: Échec de la compilation"
        CLANG_EXIT="N/A"
    fi
    
    # Comparaison
    echo ""
    if [ "$CC1_EXIT" = "$CLANG_EXIT" ]; then
        echo "✓ Codes de sortie identiques: $CC1_EXIT"
    else
        echo "✗ Codes de sortie différents: cc1=$CC1_EXIT, clang=$CLANG_EXIT"
    fi
    
    echo ""
    echo ""
done

echo "==============================================="
echo "Comparaison terminée!"
echo "==============================================="
