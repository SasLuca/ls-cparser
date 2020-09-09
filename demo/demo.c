#include "lscp.h"
#include "stdio.h"
#include "malloc.h"
#include "string.h"

int main(int argc, const char** args)
{
    char* source = 0;
    {
        FILE* file = fopen("./lscparser-c-code.temp", "rb");
        if (file != NULL)
        {
            fseek(file, 0L, SEEK_END);
            int file_size = ftell(file);
            fseek(file, 0L, SEEK_SET);

            source = malloc(file_size + 1);
            memset(source, 0, file_size + 1);
            if (source)
            {
                int read_size = fread(source, 1, file_size, file);
                if (ferror(file) != 0 || read_size != file_size)
                {
                    return -1;
                }
            }
        }

        fclose(file);
    }

    //source = "enum foo { TEST, TEST2 };";
    //source = "struct foo { int a; int b; }; \n int c; enum bar { TEST1 };";
    //source = "int main(int argc) {}";
    //source = "int i; \n#define TEST \n/* test */\n#ifdef TEST\nenum foo { TEST, TEST2 }; \nint main(int argc) {} \n#endif // test";
    lscp_parse_result r = lscp_parse_cstr(source);
    lscp_ast_to_json(r);
}