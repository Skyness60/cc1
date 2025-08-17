#include <stdio.h>
#include <stdarg.h>

struct P { int a; int b; };
struct Q { char c; int d; };

static struct P mkP(int x, int y) { struct P p = {x, y}; return p; }
static struct Q mkQ(char c, int d) { struct Q q; q.c = c; q.d = d; return q; }

static int sumP(struct P p) { return p.a + p.b; }
static int sumQ(struct Q q) { return (int)q.c + q.d; }

int passP(struct P p) { return sumP(p) + 1; }
int passQ(struct Q q) { return sumQ(q) + 2; }

int main(void) {
    struct P p = mkP(10, 20);
    struct Q q = mkQ('A', 5);
    int r1 = passP(p);
    int r2 = passQ(q);
    struct P p2 = mkP(1, 2);
    struct Q q2 = mkQ('\x02', 3);
    /* validate composite ops to stress calling conv */
    int r3 = sumP(p2) * 3 + sumQ(q2);

    /* Encode results in exit code-ish print for shell test */
    printf("%d %d %d\n", r1, r2, r3);
    return 0;
}
