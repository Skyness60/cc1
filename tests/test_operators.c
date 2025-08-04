int main() {
    int a = 5, b = 3;
    int result;
    
    result = a + b * 2;
    result = (a == b) ? a : b;
    result = a && b || !a;
    result = a & b | a ^ b;
    result = ~a;
    
    a++;
    --b;
    
    return 0;
}
