#include <stdio.h>

struct s{
    int a;
    int b;
};

struct s get_s(void)
{
    struct s fuga;
    fuga.a = 1;
    fuga.b = 2;

    return fuga;
}

int main(void)
{
    struct s hoge;

    hoge = get_s();

    printf("%d, %d\n", hoge.a, hoge.b);

    return 0;
}
