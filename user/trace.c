#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char* argv[]){
    int i;
    char *args[MAXARG];

    //Kiểm tra đối số và phải là mask hợp lệ
    if (argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')) {
        fprintf(2, "Usage: %s mask command\n", argv[0]);
        exit(1);
    }

    //Gửi yêu cầu syscall và bắt đầu tracing
    if (trace(atoi(argv[1])) < 0) {
        fprintf(2, "%s: trace failed\n", argv[0]);
        exit(1);
    }

    //Lưu các đối số
    for (i = 2; i < argc && i < MAXARG; i++) {
        args[i-2] = argv[i];
    }

    
    args[argc - 2] = 0;
    exec(args[0], args);
    printf("trace: exec failed\n");

    exit(0);
}