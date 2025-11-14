#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define START 2
#define END 280

/*
Thuat toan:
    - Tu day: 2,3,4,5,6,7,8,9,10...,280 ta tim ra day prime bang cach:
    - Xet bo so chia het cho 2: 3,5,7,9,11,...,280
    - Xet bo so chia het cho 3: 5,7,11,...
    -...
pipe:
    - Tao dau vao dau ra cua mot process
    - pipefd[0] dau vao
    - pipefd[1] dau ra

fork(): tao tien trinh con

pipe + fork: dung de giao tiep giua tien trinh cha va tien trinh con.
*/

/*
Ham thuc hien:
    - Tra ve dau vao cua process cha
    - Lien tuc day cac so tu start den end vao dau ra cua process con
*/
int generate_number()
{
    // Khai bao pipe
    int pipefd[2];
    pipe(pipefd);

    // Sinh process con
    int pid = fork();
    if (!pid)
    {
        // Day cac so tu START den END vao dau ra cua process con
        for (int i = START; i <= END; i++)
        {
            write(pipefd[1], &i, sizeof(int));
        }
        // Dung xong dau ra thi dong
        close(pipefd[1]);
        exit(0);
    }

    // Tien trinh cha khong day ra gi nen dong
    close(pipefd[1]);
    return pipefd[0];
}

/*
Ham thuc hien:
    - Chon ra cac so khong chia het cho prime va day vao dau ra cua con
    - fd_in la dau vao cua danh sach truoc, chua danh sach cac so chua duoc loc boi prime hien tai
*/
int filter(int fd_in, int prime)
{
    // number la cac so khong chia het
    int number;
    int pipefd[2];
    pipe(pipefd);

    int pid = fork();
    if (!pid)
    {
        // Doc tung so tu danh sach truoc va chon ra nhung so khong chia het cho prime, day ra dau ra cua process con
        while (read(fd_in, &number, sizeof(int)))
        {
            if (number % prime)
            {
                write(pipefd[1], &number, sizeof(int));
            }
        }
        // Dong dau vao cua cha, khong dung nua do da co danh sach da loc
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
    // Lay danh sach ban dau
    int fd_in = generate_number();

    while (read(fd_in, &prime, sizeof(int)))
    {
        printf("prime %d\n", prime);
        // Cap nhat lai dau vao cua process cha moi
        fd_in = filter(fd_in, prime);
    }
    // Cho tat ca process con ket thuc
    while (wait(0) > 0)
    {
    }

    exit(0);
}
