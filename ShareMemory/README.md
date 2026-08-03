Shared Memory hoạt động như thế nào?
Kernel tạo ra một vùng nhớ vật lý dùng chung.
Sau đó Kernel ánh xạ vùng nhớ này vào Virtual Address Space của từng process.

Luồng hoạt động
Bước 1: Process A yêu cầu Kernel tạo Shared Memory.
Bước 2: Kernel trả về một handlemotjot

```
               Kernel
        +------------------+
        |  Shared Memory   |
        +------------------+
          ▲              ▲
          │              │
      mmap()         mmap()
          │              │
    Writer Process   Reader Process
```
## POSIX Shared Memory giữa 2 process
Mục tiêu:
Writer Process tạo Shared Memory và ghi dữ liệu.
Reader Process mở cùng Shared Memory và đọc dữ liệu.
Chưa sử dụng Semaphore để bạn thấy rõ cơ chế hoạt động của Shared Memory trước. Sau đó mới bổ sung Semaphore ở bài tiếp theo.
