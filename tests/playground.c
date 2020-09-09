#include "lscp-parser.h"
#include "stdio.h"

int main()
{
    //const char* source = "int main(int argc, const char** args);";
    //const char* source = "int a; int b;";
    const char* source = "int i; \n#define TEST \n/* test */\n#ifdef TEST\nenum foo { TEST, TEST2 }; \n#endif // test";

    lscp_parse_cstr(source);
}