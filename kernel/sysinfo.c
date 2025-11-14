#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_sysinfo(void)
{
    uint64 address;
    struct sysinfo info;

    // Ham argaddr truy xuat doi so duoc luu trong thanh ghi khi chuyen tu user sang kernel
    // Lay doi so thu nhat (0) va luu dia chi ao user-space cua doi so vao bien address

    argaddr(0, &address);

    // Gan so luong bo nho trong va so luong tien trinh o trang thai UNUSED vao info
    info.freemem = freemem_count();
    info.nproc = nproc_count();

    // Ham copyout thuc hien sao chep sao cho kernel chi ghi vao mot vung nho hop le thuoc ve user.
    // myproc()->pagetable : Lay bang trang cua tien trinh hien tai thuc hien dich dia chi
    // (char *)&info : Tro toi du lieu nguon trong kernel
    // sizeof(info) : Kich thuoc du lieu can sao chep

    if (copyout(myproc()->pagetable, address, (char *)&info, sizeof(info)) < 0)
    {
        return -1;
    }

    return 0;
}
