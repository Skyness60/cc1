#include <stdio.h>

struct S1 { char a; int b; };
struct S2 { int a; char b; };
struct S3 { char a; char b; int c; };

/* return and param passing with padding/alignment differences */
static struct S1 mkS1(char a, int b) { struct S1 s; s.a = a; s.b = b; return s; }
static struct S2 mkS2(int a, char b) { struct S2 s; s.a = a; s.b = b; return s; }
static struct S3 mkS3(char a, char b, int c) { struct S3 s; s.a = a; s.b = b; s.c = c; return s; }

static int chkS1(struct S1 s) { return (int)s.a + s.b; }
static int chkS2(struct S2 s) { return s.a + (int)s.b; }
static int chkS3(struct S3 s) { return (int)s.a + (int)s.b + s.c; }

int main(void) {
    struct S1 s1 = mkS1('A', 1000);
    struct S2 s2 = mkS2(2000, 'B');
    struct S3 s3 = mkS3('C', 'D', 3000);

    int r1 = chkS1(s1);
    int r2 = chkS2(s2);
    int r3 = chkS3(s3);

    printf("%d %d %d\n", r1, r2, r3);
    return 0;
}
