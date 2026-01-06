#!/bin/bash

echo "🔍 TEST RAPIDE cc1 vs clang"
echo "=============================="
echo ""

# Tests simples sans arguments
test_simple() {
    local n=$1
    local name=$2
    if [ -f "comparison_output/cc1_$n" ] && [ -f "comparison_output/clang_$n" ]; then
        comparison_output/cc1_$n > /tmp/cc1_$n.txt 2>&1
        comparison_output/clang_$n > /tmp/clang_$n.txt 2>&1
        if diff -q /tmp/cc1_$n.txt /tmp/clang_$n.txt > /dev/null 2>&1; then
            echo "✅ Programme $n ($name): OK"
        else
            echo "❌ Programme $n ($name): DIFFÉRENT"
        fi
    else
        echo "⚠️  Programme $n ($name): NON COMPILÉ"
    fi
}

# Tests avec arguments
test_with_args() {
    local n=$1
    local name=$2
    shift 2
    local args="$@"
    if [ -f "comparison_output/cc1_$n" ] && [ -f "comparison_output/clang_$n" ]; then
        comparison_output/cc1_$n $args > /tmp/cc1_$n.txt 2>&1
        comparison_output/clang_$n $args > /tmp/clang_$n.txt 2>&1
        if diff -q /tmp/cc1_$n.txt /tmp/clang_$n.txt > /dev/null 2>&1; then
            echo "✅ Programme $n ($name): OK"
        else
            echo "❌ Programme $n ($name): DIFFÉRENT"
        fi
    else
        echo "⚠️  Programme $n ($name): NON COMPILÉ"
    fi
}

test_simple 0 "return 42"
test_simple 1 "hello world"
test_with_args 2 "print args" arg1 arg2
test_simple 3 "arithmetic"
test_with_args 4 "vowels" Hello
test_simple 5 "test 5"
test_simple 6 "test 6"
test_simple 7 "multi-file"
test_simple 8 "sizeof"
test_with_args 9 "bitfields" a

# Test program 10 avec timeout
if [ -f "comparison_output/cc1_10" ] && [ -f "comparison_output/clang_10" ]; then
    timeout 2 comparison_output/cc1_10 > /tmp/cc1_10.txt 2>&1
    timeout 2 comparison_output/clang_10 > /tmp/clang_10.txt 2>&1
    if diff -q /tmp/cc1_10.txt /tmp/clang_10.txt > /dev/null 2>&1; then
        echo "✅ Programme 10 (array iteration): OK"
    else
        echo "❌ Programme 10 (array iteration): DIFFÉRENT"
    fi
else
    echo "⚠️  Programme 10 (array iteration): NON COMPILÉ"
fi

test_simple 11 "complex struct"

echo ""
echo "📊 Voir RESULTATS_TESTS.md pour les détails complets"
