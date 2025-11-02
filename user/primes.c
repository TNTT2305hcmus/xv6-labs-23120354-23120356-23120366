#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define START 2
#define END 280

/*
Thuật toán:
    - Từ dãy: 2,3,4,5,6,7,8,9,10...,280 ta tìm ra dãy prime bằng cách:
    - Xét bỏ số chia hết cho 2: 3,5,7,9,11,...,280
    - Xét bỏ số chia hết cho 3: 5,7,11,...
    -...
pipe:
    - Tạo đầu vào đầu ra của một process
    - pipefd[0] đầu vào
    - pipefd[1] đầu ra

fork(): tạo tiến trình con

pipe + fork: dùng để giao tiếp giữa tiến trình cha và tiến trình con.
*/

/*
Hàm thực hiện:
    - Trả về đầu vào của process cha
    - Liên tục đẩy các số từ start đến end vào đầu ra của process con
*/
int generate_number()
{
    // Khai báo pipe
    int pipefd[2];
    pipe(pipefd);

    // Sinh process con
    int pid = fork();
    if (!pid)
    {
        // Đẩy các số từ START đến END vào đầu ra của process con
        for (int i = START; i <= END; i++)
        {
            write(pipefd[1], &i, sizeof(int));
        }
        // Dùng xong đầu ra thì đóng
        close(pipefd[1]);
        exit(0);
    }

    // Tiến trình cha không đẩy ra gì nên đóng
    close(pipefd[1]);
    return pipefd[0];
}

/*
Hàm thực hiện:
    - Chọn ra các số không chia hết cho prime và đẩy vào đầu ra của con
    - fd_in là đầu vào của danh sách trước, nó chứa danh sách các số chưa được lọc bởi prime hiện tại
*/
int filter(int fd_in, int prime)
{
    // number là các số không chia hết
    int number;
    int pipefd[2];
    pipe(pipefd);

    int pid = fork();
    if (!pid)
    {
        // Đọc từng số từ danh sách trước và chọn ra những số không chia hết cho prime, đẩy ra đầu ra của process con
        while (read(fd_in, &number, sizeof(int)))
        {
            if (number % prime)
            {
                write(pipefd[1], &number, sizeof(int));
            }
        }
        // Đóng đầu vào của cha, không dùng nữa do ta đã có danh sách đã lọc
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
    // Lấy danh sách ban đầu
    int fd_in = generate_number();

    while (read(fd_in, &prime, sizeof(int)))
    {
        printf("prime %d\n", prime);
        // Cập nhật lại đầu vào của process cha mới
        fd_in = filter(fd_in, prime);
    }
    // Chờ tất cả process con kết thúc
    while (wait(0) > 0)
    {
    }

    exit(0);
}