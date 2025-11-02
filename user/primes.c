#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define START 2
#define END 280

/*
pipe + fork: dùng để giao tiếp giữa tiến trình cha và tiến trình con.
*/
int generate_number()
{
    int pipefd[2];
    pipe(pipefd);

    int pid = fork();
    if (!pid)
    {
        for (int i = START; i <= END; i++)
        {
            write(pipefd[1], &i, sizeof(int));
        }
        close(pipefd[1]);
        exit(0);
    }

    close(pipefd[1]);
    return pipefd[0];
}

int filter(int fd_in, int prime)
{
    int number;
    int pipefd[2];
    pipe(pipefd);

    int pid = fork();
    if (!pid)
    {
        while (read(fd_in, &number, sizeof(int)))
        {
            if (number % prime)
            {
                write(pipefd[1], &number, sizeof(int));
            }
        }
        close(fd_in);
        close(pipefd[1]);
        exit(0);
    }

    close(fd_in);
    close(pipefd[1]);
    return pipefd[0];
}

int main(int argc, int *argv[])
{
    int prime;
    int fd_in = generate_number();
    int pid;

    while (read(fd_in, &prime, sizeof(int)))
    {
        printf("prime %d\n", prime);
        fd_in = filter(fd_in, prime);
    }
    while ((pid = wait(0)) > 0)
    {
    }

    exit(0);
}