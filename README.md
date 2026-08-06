# Roadmap học IPC (Inter-Process Communication)

---

## Tổng quan

```mermaid
graph TD
    S0["Giai đoạn 0<br/>Nền tảng: Process, Kernel Space, File Descriptor"] --> S1

    subgraph S1["Giai đoạn 1: IPC cùng máy (Local IPC)"]
        M1["Module 1<br/>Pipe / FIFO"]
        M2["Module 2<br/>Signal"]
        M3["Module 3<br/>Message Queue"]
        M4["Module 4<br/>Shared Memory"]
        M5["Module 5<br/>Semaphore"]
        M6["Module 6<br/>Unix Domain Socket"]
        M1 --> M2 --> M3 --> M4 --> M5 --> M6
    end

    S1 --> S2

    subgraph S2["Giai đoạn 2: IPC qua mạng (Network IPC)"]
        M7["Module 7<br/>TCP Socket"]
        M8["Module 8<br/>UDP Socket"]
        M9["Module 9<br/>Blocking / Non-blocking I/O"]
        M10["Module 10<br/>select / poll / epoll"]
        M7 --> M8 --> M9 --> M10
    end

    S2 --> S3

    subgraph S3["Giai đoạn 3: Kiến trúc & Ứng dụng"]
        M11["Module 11<br/>So sánh & lựa chọn cơ chế IPC"]
        M12["Module 12<br/>Kiến trúc IPC thực tế<br/>(Embedded, Server, GUI)"]
        M11 --> M12
    end
```

---

## Giai đoạn 0 — Nền tảng bắt buộc

Trước khi học bất kỳ cơ chế IPC nào, cần nắm chắc:

- Process là gì, Process có những gì (address space, heap, stack).
- Kernel Space vs User Space.
- File Descriptor là gì, vì hầu hết IPC trên Linux đều được truy cập thông qua fd.
- System Call hoạt động như thế nào (`syscall` chuyển từ User Space sang Kernel Space).

```mermaid
graph LR
    P["Process<br/>User Space"] -->|System Call| K["Kernel Space"]
    K --> FD["File Descriptor / Handle"]
    FD --> P
```

---

## Giai đoạn 1 — IPC cùng máy (Local IPC)

### Module 1: Pipe / FIFO

Cơ chế IPC đơn giản nhất — dữ liệu chảy một chiều qua một buffer trong kernel.

```mermaid
graph LR
    A["Process A"] -->|write| Pipe["Pipe Buffer<br/>(Kernel)"] -->|read| B["Process B"]
```

Nội dung cần học:
- `pipe()` — anonymous pipe, chỉ dùng giữa process cha–con.
- `mkfifo()` — named pipe, dùng được giữa các process không liên quan.
- Giới hạn: chỉ truyền byte stream, không giữ ranh giới message, thường chỉ một chiều.

### Module 2: Signal

Cơ chế thông báo sự kiện (không truyền dữ liệu lớn), kernel gửi một "ngắt" tới process.

```mermaid
sequenceDiagram
    participant K as Kernel
    participant P as Process

    K->>P: Gửi signal (SIGINT, SIGTERM, SIGUSR1...)
    P->>P: Signal Handler xử lý
```

Nội dung cần học: `signal()`, `sigaction()`, các signal phổ biến, signal-safe function, sự khác biệt giữa signal và interrupt phần cứng.

### Module 3: Message Queue

Kernel quản lý một hàng đợi message; sender/receiver không cần chạy đồng thời.

```mermaid
graph LR
    S["Sender"] --> Q["Kernel Message Queue"] --> R["Receiver"]
```

Nội dung cần học: `mq_open()`, `mq_send()`, `mq_receive()`, `mq_unlink()`, ưu điểm về priority message so với Pipe.

### Module 4: Shared Memory

Cơ chế IPC nhanh nhất — nhiều process cùng ánh xạ một vùng nhớ vật lý vào Virtual Address Space của mình.

```mermaid
graph TD
    SM["Shared Memory (Kernel)"]
    A["Process A"] -->|mmap| SM
    B["Process B"] -->|mmap| SM
```

Nội dung cần học: `shm_open()`, `ftruncate()`, `mmap()`, `munmap()`, `shm_unlink()`. Lưu ý: Shared Memory tự nó **không đồng bộ hóa** — cần kết hợp với Semaphore hoặc Mutex liên tiến trình.

### Module 5: Semaphore

Cơ chế đồng bộ hóa dùng để bảo vệ Shared Memory hoặc giới hạn số process truy cập tài nguyên.

```mermaid
sequenceDiagram
    participant W as Writer
    participant Sem as Semaphore
    participant R as Reader

    W->>Sem: Ghi xong dữ liệu
    W->>Sem: sem_post()
    R->>Sem: sem_wait()
    Sem-->>R: Cho phép đọc
```

Nội dung cần học: `sem_open()`, `sem_wait()`, `sem_post()`, phân biệt Binary Semaphore vs Counting Semaphore, so sánh với Mutex.

### Module 6: Unix Domain Socket

IPC mạnh nhất trên cùng máy — hỗ trợ cả Stream (TCP-like) và Datagram (UDP-like), full-duplex, có thể truyền cả file descriptor.

```mermaid
graph LR
    A["Process A"] --> SA["Socket"] --> K["Kernel"] --> SB["Socket"] --> B["Process B"]
```

Nội dung cần học: `socket(AF_UNIX, ...)`, `bind()`, `listen()`, `accept()`, `connect()`, `send()`, `recv()`.

---

## Giai đoạn 2 — IPC qua mạng (Network IPC)

### Module 7: TCP Socket

Mở rộng Unix Domain Socket sang `AF_INET` — connection-oriented, reliable, ordered. API gần như giống hệt Unix Domain Socket.

### Module 8: UDP Socket

Connectionless, không đảm bảo thứ tự, nhanh — dùng cho streaming, telemetry, sensor data.

### Module 9: Blocking / Non-blocking I/O

Hiểu sự khác biệt giữa gọi hàm I/O bị chặn (blocking) và không chặn (non-blocking, trả về ngay kèm mã lỗi `EAGAIN`/`EWOULDBLOCK`).

### Module 10: select / poll / epoll

Cơ chế giám sát nhiều file descriptor cùng lúc — nền tảng để xây dựng server xử lý hàng nghìn kết nối đồng thời.

```mermaid
graph TD
    FD1["fd 1"] --> M["select / poll / epoll"]
    FD2["fd 2"] --> M
    FD3["fd 3"] --> M
    M --> R["Trả về fd nào đã sẵn sàng đọc/ghi"]
```

---

## Giai đoạn 3 — Kiến trúc & Ứng dụng thực tế

### Module 11: So sánh & lựa chọn cơ chế IPC

| Cơ chế | Tốc độ | Đồng bộ tự động? | Phạm vi | Phù hợp cho |
|---|---|---|---|---|
| Pipe / FIFO | Trung bình | Có (blocking read/write) | Cùng máy | Luồng dữ liệu đơn giản, một chiều |
| Signal | Rất nhanh | Không | Cùng máy | Thông báo sự kiện, không truyền data lớn |
| Message Queue | Trung bình | Có (queue) | Cùng máy | Command/event có cấu trúc, có priority |
| Shared Memory | Rất nhanh | Không (cần Semaphore) | Cùng máy | Dữ liệu lớn, tần suất cao (frame ảnh, buffer) |
| Semaphore | — | — (là công cụ đồng bộ) | Cùng máy | Bảo vệ Shared Memory, giới hạn tài nguyên |
| Unix Domain Socket | Nhanh | Có (connection) | Cùng máy | IPC command/control, truyền fd |
| TCP Socket | Chậm hơn (qua mạng) | Có (connection) | Cùng máy hoặc nhiều máy | Giao tiếp client-server, cần độ tin cậy |
| UDP Socket | Nhanh (qua mạng) | Không | Cùng máy hoặc nhiều máy | Streaming, real-time, chấp nhận mất gói |

### Module 12: Kiến trúc IPC thực tế

Ví dụ kiến trúc trong một hệ thống nhúng (máy siêu âm):

```mermaid
graph TD
    GUI["GUI"] -->|Unix Domain Socket<br/>lệnh: START_SCAN, FREEZE| IPS["Image Processing Service"]
    IPS -->|Shared Memory + Semaphore<br/>frame ảnh| DRV["Ultrasound Driver"]
    IPS -->|Message Queue<br/>trạng thái, log| LOG["Logging Service"]
```

Nguyên tắc thiết kế:
- **Signal/Message Queue** cho lệnh điều khiển ngắn.
- **Shared Memory + Semaphore** cho dữ liệu lớn, tần suất cao (ảnh, audio buffer).
- **Socket** khi cần giao tiếp qua mạng hoặc muốn kiến trúc client-server rõ ràng, dễ mở rộng.

---

## Mindmap tổng kết

```mermaid
mindmap
  root((IPC))
    Local IPC
      Pipe FIFO
      Signal
      Message Queue
      Shared Memory
      Semaphore
      Unix Domain Socket
    Network IPC
      TCP Socket
      UDP Socket
      Blocking Non-blocking
      select poll epoll
    Kien truc thuc te
      So sanh co che
      Embedded Linux
      Client Server
      GUI Backend Driver
```

---

## Gợi ý cách học

1. Học từng module theo thứ tự — mỗi module đều nên có **code demo giữa 2 process thật** (không chỉ đọc lý thuyết).
2. Sau mỗi cơ chế, tự trả lời: *"Dữ liệu đi qua đâu? Ai giữ buffer? Cần đồng bộ hóa không?"*
3. Làm một dự án nhỏ tổng hợp nhiều IPC (ví dụ: GUI ↔ Backend qua Socket, Backend ↔ Worker qua Shared Memory) để thấy cách chúng phối hợp trong kiến trúc thực tế.
4. Khi phỏng vấn, chuẩn bị so sánh được các cơ chế theo: tốc độ, độ phức tạp, có cần đồng bộ hóa không, và trường hợp sử dụng phù hợp.