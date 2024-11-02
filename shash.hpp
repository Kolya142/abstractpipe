#include <cstdio>
namespace Shash {
    int hash_step1(int a, int b, int c, int d);
    int hash_step2(int a, int b, int c, int d);
    int hash_step3(int a, int b, int c, int d);
    int hash_step4(int a, int b, int c, int d);

    int calculate_shash(int n);
    char* get_hex(int n);
}