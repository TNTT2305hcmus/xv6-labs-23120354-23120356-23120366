#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
         fprintf(2, "Usage: xargs command...\n");
         exit(1);
    }

    char buf[512]; //Buffer đọc dữ liệu từ pipe
    char *args[MAXARG]; //Chứa câu lệnh

    int i = 1; // Chỉ số duyệt mảng argv
    int j = 0; // Chỉ số duyệt mảng args

    char *p = buf; // Con trỏ đến từng dòng dữ liệu
    int m = 0; // Con trỏ duyệt từng kí tự dữ liệu
    int n = 1; // Quy định số lượng đối số, 1 là mặc định

    //Kiểm tra nếu có quy định về số lượng tham số
    if (strcmp(argv[1],"-n") == 0) {
        n = atoi(argv[2]);
        i = 3;
    }

    // Đưa vào từng phần của câu lệnh
    for (;i < argc; i++) {
        args[j] = argv[i];
        j++;
    }


    int t = 0; // Đếm số lượng đối số đã đọc

    //Đọc dữ liệu từ pipe
    while (read(0, &buf[m], 1) == 1) {
        if (++t && buf[m] == '\n') {
            buf[m] = 0;

            // Thêm tham số đọc được vào args
            args[j] = p;
            args[j + 1] = 0;

            if (t < n) {
                continue;
            }

            // fork và exec
            if (fork() == 0) {
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

    // Nếu vẫn còn đối số
    if (t > 0) {
        args[j] = 0;
        if (fork() == 0) {
            exec(args[0], args);
            fprintf(2, "xargs: exec %s failed\n", args[0]);
            exit(1);
        }
        wait(0);
    }

    exit(0);
}