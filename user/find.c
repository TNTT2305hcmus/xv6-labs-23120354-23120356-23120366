#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
/*
Cach duyet:
    - Kiem tra file hien tai co phai la target hay khong?
        - Neu la target thi in ra.
        - Neu khong la target thi khong in ra.
    - Kiem tra file hien tai:
        - Neu la file thong thuong thi dung.
        - Neu la dir thi tiep tuc duyet ben trong
*/

/*
Tra ve file cuoi cung tu duong dan, giong trong ls
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
    char buf[512], *p; // Dung de thao tac tren path
    int fd;            // file descriptor
    struct dirent de;  // Moi entry trong 1 thu muc la mot dirent
    struct stat st;    // Chua metadata cho file, dir, device

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

    // Da khong la dir thi bo qua
    if (st.type != T_DIR)
    {
        close(fd);
        return;
    }

    // st.type == T_DIR
    // Giong ls
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        printf("find: path too long\n");
        close(fd);
        return;
    }
    // path se bieu dien den trong dir nay
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        // Bo qua cac file khong can thiet
        if (de.inum == 0)
            continue;

        if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;

        // Duyet den ben trong entry nay
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