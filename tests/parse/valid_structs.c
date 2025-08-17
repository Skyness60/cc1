struct S { int a, b; };
union U { int x; char c; };
enum E { A, B = 5, C };

int use(struct S *p, union U u, enum E e) {
    struct S s = { 1, 2 };
    int arr[3] = { 1, 2, 3 };
    return p->a + u.x + e + arr[0];
}
