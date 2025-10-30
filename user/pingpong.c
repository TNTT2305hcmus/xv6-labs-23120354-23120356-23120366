#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
pingpong (){
    int p1[2];
    // p1[0]: Đọc từ cha sang con
    // p1[1]: Ghi từ cha sang con
    int p2[2];
    // p2[0]: Đọc từ con sang cha
    // p2[1]: Ghi từ con sang cha

    // Tạo cặp pipe
    if (pipe(p1) < 0 || pipe(p2) < 0) {
        printf("pipe failed\n");
        exit(1);
    }

    char byte = 'A';

    int pid = fork();
    if(pid > 0){
        // Đọc "ping" từ con sang cha (Dùng p2[0])
        // Ghi "pong" từ cha sang con (Dùng p1[1])

        // Đóng đầu ghi của con (Đóng p2[1])
        // Đóng đầu đọc của cha (Đóng p1[0])
        close(p1[0]);
        close(p2[1]);


        // Quá trình ghi
        write(p1[1], &byte, 1);

        // Quá trình đọc
        char bufChild[1]; // 1 byte cho byte đầu vào
        int readByteFromChild = read(p2[0], bufChild, 1); // Đọc byte từ con
        if(readByteFromChild > 0){
            printf("%d: received pong \n", getpid());
        }

        close(p2[0]);
        close(p1[1]);
        wait(0);
        exit(0);

    } else if(pid == 0){
        // Ghi "ping" từ con sang cha (Dùng p2[1])
        // Đọc "pong" từ cha sang con (Dùng p1[0])

        // Đóng đầu đọc của con (Đóng p2[0])
        // Đóng đầu ghi của cha (Đóng p1[1])
        close(p2[0]);
        close(p1[1]);

        // Quá trình đọc
        char bufParent[1];
        int readByteFromParent = read(p1[0], bufParent, 1); // Đọc byte từ cha
        if(readByteFromParent > 0){
            printf("%d: received ping\n", getpid());
        }

        // Quá trình ghi
        write(p2[1], &byte, 1);

        close(p2[1]);
        close(p1[0]);
        exit(0);

    } else {
        printf("error fork\n");
        exit(1);
    }
}

int main(int argc, char* argv[]){
    if(argc != 1){
        exit(1);
    }
    pingpong();
}