#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
    // Kiem tra neu so luong doi so khong hop le
    if (argc < 2)
    {
        fprintf(2, "Usage: xargs [options] [command [initial-arguments]]\n");
        exit(1);
    }

    char buf[512];      // Buffer doc du lieu tu pipe
    char *args[MAXARG]; // Chua cau lenh

    int i = 1; // Chi so duyet mang argv
    int j = 0; // Chi so duyet mang args

    char *p = buf; // Con tro den tung dong du lieu
    int m = 0;     // Con tro duyet tung ky tu du lieu
    int n = 1;     // So luong doi so, mac dinh 1

    // Kiem tra neu co quy dinh ve so luong tham so
    if (strcmp(argv[1], "-n") == 0)
    {
        n = atoi(argv[2]);
        i = 3;
    }

    // Dua vao tung phan cua cau lenh
    for (; i < argc; i++)
    {
        args[j] = argv[i];
        // debug
        // printf("%s\n",args[j]);
        j++;
    }

    int t = 0; // Dem so luong doi so da doc

    // Doc du lieu tu pipe
    while (read(0, &buf[m], 1) == 1)
    {
        if (++t && buf[m] == '\n')
        {
            buf[m] = 0;

            // debug
            // printf("%s\n", p);

            // Them tham so doc duoc vao args
            args[j] = p;
            args[j + 1] = 0;

            if (t < n)
            {
                continue;
            }

            // fork va exec
            if (fork() == 0)
            {
                exec(args[0], args);
                fprintf(2, "exec %s failed\n", args[0]);
                exit(1);
            }
            wait(0);

            t = 0;
            p = buf + m + 1;
        }
        m++;
    }

    // Neu van con doi so
    if (t > 0)
    {
        args[j] = 0;
        if (fork() == 0)
        {
            exec(args[0], args);
            fprintf(2, "xargs: exec %s failed\n", args[0]);
            exit(1);
        }
        wait(0);
    }

    exit(0);
}
