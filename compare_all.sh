#!/bin/bash

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Créer le dossier de sortie
mkdir -p comparison_output

echo "======================================================================"
echo "  COMPILATION DE TOUS LES PROGRAMMES"
echo "======================================================================"
echo ""

# Compiler tous les programmes avec cc1 et clang
compile_program() {
    local src=$1
    local name=$2
    local extra_args=$3
    
    echo "--- Compilation de $name ---"
    
    # cc1
    if ./cc1 "$src" -o "comparison_output/cc1_${name}.ll" 2>/dev/null; then
        if llc -march=x86 -relocation-model=pic -mattr=+sse2 "comparison_output/cc1_${name}.ll" -o "comparison_output/cc1_${name}.s" 2>/dev/null; then
            if gcc -m32 -no-pie "comparison_output/cc1_${name}.s" -o "comparison_output/cc1_${name}" 2>/dev/null; then
                echo -e "${GREEN}✓ cc1 compilé${NC}"
            else
                echo -e "${RED}✗ cc1 échec gcc${NC}"
                return 1
            fi
        else
            echo -e "${RED}✗ cc1 échec llc${NC}"
            return 1
        fi
    else
        echo -e "${RED}✗ cc1 échec compilation${NC}"
        return 1
    fi
    
    # clang
    if clang -m32 -std=c89 "$src" -o "comparison_output/clang_${name}" 2>/dev/null; then
        echo -e "${GREEN}✓ clang compilé${NC}"
    else
        echo -e "${RED}✗ clang échec${NC}"
        return 1
    fi
    
    return 0
}

# Programme 0
compile_program "tests/samples/program_0.c" "0"

# Programme 1
compile_program "tests/samples/program_1.c" "1"

# Programme 2 (besoin d'arguments)
compile_program "tests/samples/program_2.c" "2"

# Programme 3
compile_program "tests/samples/program_3.c" "3"

# Programme 4 (besoin d'un argument)
compile_program "tests/samples/program_4.c" "4"

# Programme 5
compile_program "tests/samples/program_5.c" "5"

# Programme 6
compile_program "tests/samples/program_6.c" "6"

# Programme 7a et 7b (compilation ensemble)
echo "--- Compilation de 7a et 7b ---"
if ./cc1 tests/samples/program_7a.c tests/samples/program_7b.c -o comparison_output/cc1_7.ll 2>/dev/null; then
    if llc -march=x86 -relocation-model=pic -mattr=+sse2 comparison_output/cc1_7.ll -o comparison_output/cc1_7.s 2>/dev/null; then
        if gcc -m32 -no-pie comparison_output/cc1_7.s -o comparison_output/cc1_7 2>/dev/null; then
            echo -e "${GREEN}✓ cc1 compilé (7a+7b)${NC}"
        else
            echo -e "${RED}✗ cc1 échec gcc (7a+7b)${NC}"
        fi
    else
        echo -e "${RED}✗ cc1 échec llc (7a+7b)${NC}"
    fi
else
    echo -e "${RED}✗ cc1 échec compilation (7a+7b)${NC}"
fi

if clang -m32 -std=c89 tests/samples/program_7a.c tests/samples/program_7b.c -o comparison_output/clang_7 2>/dev/null; then
    echo -e "${GREEN}✓ clang compilé (7a+7b)${NC}"
else
    echo -e "${RED}✗ clang échec (7a+7b)${NC}"
fi

# Programme 8
compile_program "tests/samples/program_8.c" "8"

# Programme 9 (besoin d'un argument - 1 caractère)
compile_program "tests/samples/program_9.c" "9"

# Programme 10
compile_program "tests/samples/program_10.c" "10"

# Programme 11
compile_program "tests/samples/program_11.c" "11"

echo ""
echo "======================================================================"
echo "  COMPARAISON DES SORTIES"
echo "======================================================================"
echo ""

# Fonction de comparaison
compare_output() {
    local name=$1
    shift
    local args="$@"
    
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "  PROGRAMME $name"
    if [ -n "$args" ]; then
        echo "  Arguments: $args"
    fi
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    
    if [ ! -f "comparison_output/cc1_${name}" ] || [ ! -f "comparison_output/clang_${name}" ]; then
        echo -e "${RED}Programme non compilé${NC}"
        echo ""
        return
    fi
    
    # Exécuter les deux versions
    comparison_output/cc1_${name} $args > comparison_output/out_cc1_${name}.txt 2>&1
    local exit_cc1=$?
    comparison_output/clang_${name} $args > comparison_output/out_clang_${name}.txt 2>&1
    local exit_clang=$?
    
    # Comparer les codes de sortie
    if [ $exit_cc1 -eq $exit_clang ]; then
        echo -e "${GREEN}✓ Code de sortie identique: $exit_cc1${NC}"
    else
        echo -e "${RED}✗ Code de sortie différent: cc1=$exit_cc1 clang=$exit_clang${NC}"
    fi
    
    # Comparer les sorties
    if diff -q comparison_output/out_cc1_${name}.txt comparison_output/out_clang_${name}.txt > /dev/null 2>&1; then
        echo -e "${GREEN}✓ Sorties identiques${NC}"
    else
        echo -e "${YELLOW}⚠ Sorties différentes${NC}"
        echo ""
        echo "--- Sortie cc1 ---"
        head -30 comparison_output/out_cc1_${name}.txt
        echo ""
        echo "--- Sortie clang ---"
        head -30 comparison_output/out_clang_${name}.txt
        echo ""
        echo "--- Différences ---"
        diff comparison_output/out_cc1_${name}.txt comparison_output/out_clang_${name}.txt | head -20
    fi
    echo ""
}

# Comparer tous les programmes
compare_output "0"
compare_output "1"
compare_output "2" "arg1" "arg2" "arg3"
compare_output "3"
compare_output "4" "Hello"
compare_output "5"
compare_output "6"
compare_output "7"
compare_output "8"
compare_output "9" "a"
compare_output "10"
compare_output "11"

echo ""
echo "======================================================================"
echo "  RÉSUMÉ"
echo "======================================================================"
echo ""

# Compter les succès
success=0
total=0
for name in 0 1 2 3 4 5 6 7 8 9 10 11; do
    if [ -f "comparison_output/out_cc1_${name}.txt" ] && [ -f "comparison_output/out_clang_${name}.txt" ]; then
        total=$((total + 1))
        if diff -q comparison_output/out_cc1_${name}.txt comparison_output/out_clang_${name}.txt > /dev/null 2>&1; then
            success=$((success + 1))
        fi
    fi
done

echo "Programmes identiques: $success/$total"
if [ $success -eq $total ]; then
    echo -e "${GREEN}✓ Tous les programmes produisent la même sortie que clang!${NC}"
else
    echo -e "${YELLOW}⚠ Certains programmes diffèrent de clang${NC}"
fi
