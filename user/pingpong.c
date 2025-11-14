#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
pingpong()
{
    int p1[2];
    // p1[0]: Doc tu cha sang con
    // p1[1]: Ghi tu cha sang con
    int p2[2];
    // p2[0]: Doc tu con sang cha
    // p2[1]: Ghi tu con sang cha

    // Tao cap pipe
    if (pipe(p1) < 0 || pipe(p2) < 0)
    {
        printf("pipe failed\n");
        exit(1);
    }

    char byte = 'A';

    int pid = fork();
    if (pid > 0)
    {
        // Doc "ping" tu con sang cha (Dung p2[0])
        // Ghi "pong" tu cha sang con (Dung p1[1])

        // Dong dau ghi cua con (Dong p2[1])
        // Dong dau doc cua cha (Dong p1[0])
        close(p1[0]);
        close(p2[1]);

        // Qua trinh ghi
        write(p1[1], &byte, 1);

        // Qua trinh doc
        char bufChild[1];                                 // 1 byte cho byte dau vao
        int readByteFromChild = read(p2[0], bufChild, 1); // Doc byte tu con
        if (readByteFromChild > 0)
        {
            printf("%d: received pong \n", getpid());
        }

        close(p2[0]);
        close(p1[1]);
        wait(0);
        exit(0);
    }
    else if (pid == 0)
    {
        // Ghi "ping" tu con sang cha (Dung p2[1])
        // Doc "pong" tu cha sang con (Dung p1[0])

        // Dong dau doc cua con (Dong p2[0])
        // Dong dau ghi cua cha (Dong p1[1])
        close(p2[0]);
        close(p1[1]);

        // Qua trinh doc
        char bufParent[1];
        int readByteFromParent = read(p1[0], bufParent, 1); // Doc byte tu cha
        if (readByteFromParent > 0)
        {
            printf("%d: received ping\n", getpid());
        }

        // Qua trinh ghi
        write(p2[1], &byte, 1);

        close(p2[1]);
        close(p1[0]);
        exit(0);
    }
    else
    {
        printf("error fork\n");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        exit(1);
    }
    pingpong();
}