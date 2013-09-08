#include <stdlib.h>
#include <stdio.h>
#define INIT_VAL(x)    (2 * (x))

/// \brief This structure is garbage
typedef struct pwet
{
    union
    {
        int pwet_a;         /// \brief this field is useless
        float pwet_b;       /// \brief this field is crap
    };
    struct pwet *me;    /// \brief face it, this is stupid
} s_pwet;

static const int my_func(char my_a, int *my_b, s_pwet *p);

typedef enum my_enum
{
    ENUM_START,
    ENUMM_1 = 10,
    ENUM_END
} my_enum; //

/*
    Multi
    ligne
    comment
*/
const int my_func(char my_a, int *my_b, s_pwet *p)
{
    printf("%s %d %f", "This is a string literal", 42, 66.6F);
    return my_a + *my_b;
}

int main(int argc, char const *argv[])
{
    static int my_int = INIT_VAL(0) + sizeof(s_pwet);
    struct pwet my_pwet2;
    const char my_char = 'c';
    s_pwet my_pwet;

    for(my_int = 0; my_int < 30; my_int++)
        my_func(my_char, my_int, &my_pwet);

    while(my_int--)
    {
        my_pwet.pwet_a = my_int;
        my_pwet.pwet_b = my_int * 0.1F;
    }

    goto end;
    end:

    return 0;
}
