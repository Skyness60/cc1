struct Point {
    int x;
    int y;
};

typedef struct {
    char name[50];
    int age;
} Person;

int main() {
    struct Point p;
    p.x = 10;
    p.y = 20;
    
    Person person;
    person.age = 25;
    
    return 0;
}
