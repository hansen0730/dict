#include <stdio.h>
#include <stdlib.h>

extern int engine_init();
extern char *query(const char *);

int main(int argc, char **argv)
{
    int wc;
    char *info;

    if(engine_init())
        return 0;
    info = query(argv[1]);
    if(info)
    {
        printf("%s\n", info);
        free(info);
    }
    else
        printf("Not found~\n");

    return 0;
}

