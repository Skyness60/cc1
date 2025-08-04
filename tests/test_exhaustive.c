/* Test exhaustif C89 - Tous les tokens possibles */
#include <stdio.h>
#include <stdlib.h>

/* Types de base et modifiers */
char c = 'X';
signed char sc = '\n';
unsigned char uc = '\t';
short s = -32768;
unsigned short us = 65535U;
int i = 42;
signed int si = -2147483648L;
unsigned int ui = 4294967295UL;
long l = 9223372036854775807LL;
unsigned long ul = 18446744073709551615ULL;
float f = 3.14159f;
double d = 2.71828;
long double ld = 1.41421356L;

/* Nombres dans toutes les bases */
int decimal = 123;
int octal = 0777;
int hex_lower = 0xabcdef;
int hex_upper = 0XABCDEF;
float sci_notation = 1.23e-45f;
double exp_notation = 6.022E23;

/* Opérateurs arithmétiques */
void arithmetic_ops() {
    int a = 10, b = 3, result;
    result = a + b;     /* addition */
    result = a - b;     /* soustraction */
    result = a * b;     /* multiplication */
    result = a / b;     /* division */
    result = a % b;     /* modulo */
    
    /* Assignations composées */
    a += b;
    a -= b;
    a *= b;
    a /= b;
    a %= b;
    
    /* Incrémentation/décrémentation */
    ++a; a++; --b; b--;
}

/* Opérateurs de comparaison */
void comparison_ops() {
    int x = 5, y = 10;
    int cmp;
    cmp = (x == y);     /* égalité */
    cmp = (x != y);     /* inégalité */
    cmp = (x < y);      /* inférieur */
    cmp = (x > y);      /* supérieur */
    cmp = (x <= y);     /* inférieur ou égal */
    cmp = (x >= y);     /* supérieur ou égal */
}

/* Opérateurs logiques */
void logical_ops() {
    int a = 1, b = 0, result;
    result = a && b;    /* ET logique */
    result = a || b;    /* OU logique */
    result = !a;        /* NON logique */
}

/* Opérateurs bitwise */
void bitwise_ops() {
    unsigned int x = 0xFF, y = 0xF0, result;
    result = x & y;     /* ET bitwise */
    result = x | y;     /* OU bitwise */
    result = x ^ y;     /* XOR bitwise */
    result = ~x;        /* NON bitwise */
    result = x << 2;    /* décalage gauche */
    result = x >> 2;    /* décalage droite */
    
    /* Assignations bitwise */
    x &= y;
    x |= y;
    x ^= y;
    x <<= 2;
    x >>= 2;
}

/* Pointeurs et adresses */
void pointer_ops() {
    int value = 42;
    int *ptr = &value;  /* adresse de */
    int deref = *ptr;   /* déréférencement */
    
    int array[10];
    ptr = array;        /* pointeur vers tableau */
    deref = ptr[0];     /* accès par index */
}

/* Structures et unions */
struct Point {
    int x, y;
};

union Data {
    int i;
    float f;
    char str[20];
};

typedef struct {
    char name[50];
    int age;
} Person;

/* Énumérations */
enum Color {
    RED,
    GREEN = 5,
    BLUE
};

/* Structures de contrôle */
void control_structures() {
    int i, j;
    
    /* Conditions */
    if (i > 0) {
        /* ... */
    } else if (i < 0) {
        /* ... */
    } else {
        /* ... */
    }
    
    /* Opérateur ternaire */
    int max = (i > j) ? i : j;
    
    /* Switch */
    switch (i) {
        case 1:
            break;
        case 2:
        case 3:
            continue;
        default:
            goto end;
    }
    
    /* Boucles */
    while (i > 0) {
        i--;
    }
    
    do {
        i++;
    } while (i < 10);
    
    for (j = 0; j < 10; j++) {
        if (j == 5) break;
        if (j == 3) continue;
    }
    
    end:
        return;
}

/* Classes de stockage */
static int static_var = 100;
extern int extern_var;
register int fast_var;
auto int local_var;

/* Qualifieurs */
const int constant = 42;
volatile int hardware_reg;

/* Fonction avec tous les types de paramètres */
double complex_function(
    const char *str,
    int count,
    float values[],
    struct Point *points,
    enum Color color,
    ...  /* paramètres variables */
) {
    /* Accès aux membres */
    struct Point p;
    p.x = 10;
    p.y = 20;
    
    struct Point *ptr = &p;
    ptr->x = 30;
    ptr->y = 40;
    
    /* Taille d'un type */
    int size = sizeof(int);
    size = sizeof(struct Point);
    
    return 3.14159;
}

/* Point d'entrée principal */
int main(void) {
    /* Déclarations avec initialisations complexes */
    int numbers[] = {1, 2, 3, 4, 5};
    char string[] = "Hello, World!\n\t\"Quotes\"\\Backslash";
    char escape_chars = '\a\b\f\n\r\t\v\\\?\'\"\0';
    
    struct Point origin = {0, 0};
    Person person = {"John Doe", 30};
    
    /* Appels de fonctions */
    printf("Testing lexer with: %d %f %s\n", 42, 3.14, "success");
    
    /* Retour */
    return 0;
}
