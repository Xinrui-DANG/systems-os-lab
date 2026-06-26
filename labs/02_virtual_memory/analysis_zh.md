# 02 Virtual Memory 中文分析

本模块将围绕虚拟地址、页表、TLB、缺页异常、mmap、copy-on-write 和 backing store 展开。

关键主线：

```text
Virtual Address
  -> Page Table Lookup
  -> TLB
  -> Physical Page Frame
  -> DRAM
```

当页面不在内存中时：

```text
Virtual Address
  -> Page Table Lookup
  -> Present Bit = 0
  -> Page Fault
  -> OS finds backing store location
  -> OS loads page into DRAM page frame
  -> OS updates PTE with PFN
  -> CPU retries instruction
```

通常 CPU 不是直接把外部 flash 的 PFN 填进页表让程序访问，而是由 OS 把缺失页面加载到 DRAM 物理页框中，再更新页表。
