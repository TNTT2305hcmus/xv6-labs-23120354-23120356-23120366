#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
/*
Cách duyệt:
    - Kiểm tra file hiện tại có phải là target hay không?
        - Nếu là target thì in ra.
        - Nếu không là target thì không in ra.
    - Kiểm tra file hiện tại:
        - Nếu là file thông thường thì dừng.
        - Nếu là dir thì tiếp tục duyệt bên trong
*/

/*
Trả về file cuối cùng từ đường dẫn, giống trong ls
*/
char *fmtname(char *path)
{
    char *p;

    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    return p;
}

void find(char *path, char *targetname)
{
    char buf[512], *p; // Dùng để thao tác trên path
    int fd;            // file descriptor
    struct dirent de;  // Mỗi entry trong 1 thư mục là một dirent
    struct stat st;    // Chứa metadata cho file, dir, device

    if (!strcmp(fmtname(path), targetname))
    {
        printf("%s\n", path);
    }

    // Kiểm tra fd và st
    if ((fd = open(path, O_RDONLY)) < 0)
    {
        fprintf(2, "find: cannot open [%s], fd=%d\n", path, fd);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Đã không là dir thì bỏ qua
    if (st.type != T_DIR)
    {
        close(fd);
        return;
    }

    // st.type == T_DIR
    // Giống ls
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        printf("find: path too long\n");
        close(fd);
        return;
    }
    // path sẽ biểu diễn đến trong dir này
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        // Bỏ qua các file không cần thiết
        if (de.inum == 0)
            continue;

        if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;

        // Duyệt đến bên trong entry này
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        find(buf, targetname);
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: find <path> <filename>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}