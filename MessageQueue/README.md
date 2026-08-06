# POSIX Message Queue

POSIX Message Queue là cơ chế IPC (Inter-Process Communication) cho phép các process gửi dữ liệu cho nhau theo dạng **message**.

## Mô hình tổng quan

```mermaid
graph LR
    A["Sender Process"] --> B["Message Queue"]
    B --> C["Receiver Process"]
```

---

## Cơ chế hoạt động chung

Kernel tạo ra queue nằm trong **Kernel Space**. Message Queue thuộc về kernel và không thuộc về bất kỳ process nào — các process chỉ mở (`mq_open`) để truy cập vào cùng một queue.

```mermaid
graph TD
    subgraph Kernel["Kernel Space"]
        MQ["Message Queue<br/>┌─────────────┐<br/>Message 1<br/>Message 2<br/>Message 3<br/>└─────────────┘"]
    end

    Sender["Sender Process"] --> MQ
    Receiver["Receiver Process"] --> MQ
```

> **Lưu ý:** Message Queue thuộc kernel và không thuộc bất kỳ process nào. Vòng đời của queue độc lập với vòng đời của process — queue vẫn tồn tại cho đến khi bị `mq_unlink()` hoặc hệ thống khởi động lại.

---

## Quy trình sử dụng

### Sender

```mermaid
graph TD
    A["mq_open()"] --> B["mq_send()"] --> C["mq_close()"]
```

### Receiver

```mermaid
graph TD
    A["mq_open()"] --> B["mq_receive()"] --> C["mq_close()"] --> D["mq_unlink()"]
```

### Toàn bộ tương tác giữa hai bên

```mermaid
sequenceDiagram
    participant S as Sender Process
    participant K as Kernel (Message Queue)
    participant R as Receiver Process

    S->>K: mq_open()
    R->>K: mq_open()
    S->>K: mq_send(message)
    K-->>K: Lưu message vào queue
    R->>K: mq_receive()
    K-->>R: Trả về message
    S->>K: mq_close()
    R->>K: mq_close()
    R->>K: mq_unlink()
    Note over K: Queue bị xóa khỏi hệ thống
```

---

## Thư viện cần dùng

| API | Header |
|---|---|
| `mq_open()` | `<mqueue.h>` |
| `mq_send()` | `<mqueue.h>` |
| `mq_receive()` | `<mqueue.h>` |
| `mq_close()` | `<mqueue.h>` |
| `mq_unlink()` | `<mqueue.h>` |
| `O_CREAT`, `O_RDWR` | `<fcntl.h>` |
| `perror()` | `<cstdio>` |
| `strlen()` | `<cstring>` |