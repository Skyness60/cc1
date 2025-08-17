// Minimal tests for restored gen_expr arms
extern int printf(const char *fmt, ...);
int main() {
    int a = 1, b = 2;
    int arr[3] = {10, 20, 30};
    int *p = arr;
    printf("call: %d\n", printf("x=%d\n", a));
    printf("index: %d\n", arr[1]);
    printf("sizeof(int): %d\n", (int)sizeof(int));
    printf("cast: %d\n", (int)3.14);
    printf("cond: %d\n", a ? b : 42);
    printf("preinc: %d\n", ++a);
    printf("postinc: %d\n", b++);
    printf("assign: %d\n", a = 5);
    printf("addassign: %d\n", a += 2);
    printf("subassign: %d\n", a -= 1);
    return 0;
}
