#include "lscparser.h"

int main()
{
    const char* source = "int main() {}";
    lscp_parse_cstr(source);
}