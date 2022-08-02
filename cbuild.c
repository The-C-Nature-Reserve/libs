#define CBUILD
#include "cbuild.h"

#define FLAGS "-g -Wall -pedantic"

#define clean_dir(p) rm(p"/build/*"); rm(p"/out");

void clean(void)
{
    clean_dir("con");
    clean_dir("smap");
}

void compile_smap(void)
{
    compile_object("smap/build/smap.o", FLAGS, "smap/smap.c");
    compile_object("smap/build/test.o", FLAGS, "smap/test.c");
    compile_object_directory("smap/out", FLAGS, "", "smap/build/");
}

void compile_con(void)
{
    compile_object("con/build/con.o", FLAGS, "con/con.c");
    compile_object("con/build/test.o", FLAGS, "con/test.c");
    compile_object_directory("con/out", FLAGS, "", "con/build/");
}

void compile_all(void) {
    compile_smap(); }

int main(int argc, char** argv)
{
    auto_update();
    if (argc == 1) {
        compile_all();
    } else if (strcmp(argv[1], "clean") == 0) {
        clean();
    } else if (strcmp(argv[1], "smap/") == 0) {
        compile_smap();
        system("smap/out");
    } else {
        fprintf(
            stderr, "\033[31mError: \033[39m unknown option: %s\n", argv[1]);
    }
    return 0;
}
