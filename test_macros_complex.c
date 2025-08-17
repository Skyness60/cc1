#define SQUARE(x) ((x) * (x))
#define ADD(a, b) ((a) + (b))
int main() {
    int result = SQUARE(5) + ADD(3, 4);
    return result;
}
