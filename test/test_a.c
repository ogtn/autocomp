// #include <../abc.h>

/// \brief This structure is garbage
typedef struct pwet
{
    int pwet_a;     /// \brief this field is useless
    float pwet_b;   /// \brief this field is crap
} s_pwet;


/// \brief This function sucks
int my_func(char my_a, int my_b)
{
    printf("%s %d %f", "This is a string literal", 42, 66.6F);

    return my_a + my_b;
}


int main(int argc, char const *argv[])
{
    char my_char;
    int my_int;

    my_func(my_char, my_int);

    s_pwet my_pwet;
    // my_pwet.

	return 0;
}
