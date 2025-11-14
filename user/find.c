#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
/*
Traversal Method:
    - Check if the current file is the target:
        - If it is the target, print it.
        - If it is not the target, do not print it.
    - Examine the current file type:
        - If it is a regular file, stop (for this path).
        - If it is a directory (dir), continue traversal inside it.
*/

/*
Returns the final file or directory name from a given path, similar to the `ls` command.
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
    char buf[512], *p; // Used for operations/manipulation on the path string.
    int fd;            // file descriptor
    struct dirent de;  // Each entry in a directory is a dirent
    struct stat st;    // Contains metadata for file, dir, device

    if (!strcmp(fmtname(path), targetname))
    {
        printf("%s\n", path);
    }

    // Check fd and st
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

    // If not a directory, skip
    if (st.type != T_DIR)
    {
        close(fd);
        return;
    }

    // st.type == T_DIR
    // Similar to ls
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        printf("find: path too long\n");
        close(fd);
        return;
    }
    // path will represent inside this directory
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        // Skip unnecessary files
        if (de.inum == 0)
            continue;

        if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;

        // Traverse inside this entry
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