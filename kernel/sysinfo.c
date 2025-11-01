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

    // Hàm argaddr truy xuất đối số được lưu trong thanh ghi khi chuyển từ user sang kernel
    // Lấy đối số thứ nhất (0) và lưu địa chỉ ảo user-space của đối số vào biến address
    argaddr(0, &address);

    // Gán số lượng bộ nhớ trống và số lượng tiến trình ở trạng thái UNUSED vào info
    info.freemem = freemem_count();
    info.nproc = nproc_count();

    // Hàm copyout thực hiện sao chép sao cho kernel chỉ ghi vào một vùng nhớ hợp lệ thuộc về user.
    // myproc()->pagetable : Lấy bảng trang của tiến trình hiện tại thực hiện dịch địa chỉ
    // (char *)&info : Trỏ tới dữ liệu nguồn trong kernel
    // sizeof(info) : Kích thước dữ liệu cần sao chép
    if(copyout(myproc()->pagetable, address, (char *)&info, sizeof(info)) < 0){
        return -1;
    }

    return 0;
}
