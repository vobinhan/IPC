# Roadmap Socket

```mermaid
graph TD
    M1["Module 1<br/>Socket là gì?"] --> M2["Module 2<br/>Socket trong Kernel hoạt động như thế nào?"]
    M2 --> M3["Module 3<br/>File Descriptor và Socket"]
    M3 --> M4["Module 4<br/>Unix Domain Socket (IPC)"]
    M4 --> M5["Module 5<br/>TCP Socket"]
    M5 --> M6["Module 6<br/>UDP Socket"]
    M6 --> M7["Module 7<br/>Socket Buffer"]
    M7 --> M8["Module 8<br/>Blocking / Non-blocking"]
    M8 --> M9["Module 9<br/>select / poll / epoll"]
    M9 --> M10["Module 10<br/>Socket Architecture"]
```

---

## 1. Socket là gì?

Socket thực chất là:

> **Một endpoint (điểm cuối) của một kênh giao tiếp.**

```mermaid
graph LR
    A["Process A"] --> SA["Socket A"]
    SA -.->|kênh giao tiếp| SB["Socket B"]
    SB --> B["Process B"]
```

Hai process **không giao tiếp trực tiếp**. Mỗi process chỉ làm việc với socket của mình. Kernel chịu trách nhiệm chuyển dữ liệu giữa hai socket.

---

## Socket khác Shared Memory thế nào?

### Shared Memory

```mermaid
graph TD
    A["Process A"] --> RAM["Shared RAM"]
    B["Process B"] --> RAM
```

Hai process cùng nhìn vào một vùng RAM.

### Socket

```mermaid
graph LR
    A["Process A"] --> SA["Socket"] --> K["Kernel"] --> SB["Socket"] --> B["Process B"]
```

Kernel luôn đứng giữa. Đó là sự khác biệt lớn nhất.

---

## Socket khác Message Queue thế nào?

```mermaid
graph LR
    subgraph MQ["Message Queue"]
        S1["Sender"] --> Q1["Kernel Queue"] --> R1["Receiver"]
    end

    subgraph SOCK["Socket"]
        S2["Sender"] --> Q2["Socket Buffer"] --> R2["Receiver"]
    end
```

Thoạt nhìn giống nhau, nhưng khác ở bản chất:

| Message Queue | Socket |
|---|---|
| Queue | Connection |

Socket tạo ra một **kênh giao tiếp liên tục**, trong khi Message Queue chỉ truyền từng message rời rạc.

---

## Tại sao phải có Socket?

Giả sử:

```mermaid
graph TD
    GUI1["GUI"] --> HW["Hardware Process"]
```

Có thể dùng **Shared Memory** được.

Nhưng nếu:

```mermaid
graph TD
    GUI2["GUI"] --> Server["Server"] --> Client["Client"] --> Cloud["Cloud"]
```

thì sao? Shared Memory không thể. Message Queue cũng không. **Socket thì được.**

Socket được thiết kế để:

- IPC trên cùng máy.
- IPC giữa nhiều máy.

API gần như giống nhau.

---

## 2. Socket nằm ở đâu trong Kernel?

Đây là phần quan trọng nhất.

Nhiều người nghĩ socket kết nối trực tiếp giữa hai process — **không đúng**. Thực tế Kernel tạo ra một **object**:

```mermaid
graph TD
    A["Process A"] --> FD["File Descriptor"] --> SO["Socket Object"] --> K["Kernel"]
```

Socket Object nằm hoàn toàn trong Kernel Space.

---

## Kiến trúc trong Kernel

```mermaid
graph TD
    subgraph US1["User Space"]
        PA["Process A<br/>socket fd"]
    end

    PA -->|System Call| SC["System Call Interface"]

    subgraph KS["Kernel Space"]
        SC --> SOBJ["Socket Object"]
        SOBJ --> SB["Socket Buffer"]
        SB --> PROTO["Protocol"]
        PROTO --> DRV["Driver"]
    end

    subgraph US2["User Space"]
        PB["Process B"]
    end

    DRV --> PB
```

---

## Kernel tạo gì khi gọi socket()?

Giả sử:

```cpp
socket(AF_UNIX, SOCK_STREAM, 0);
```

Kernel tạo một **Socket Object** chứa:

- Domain
- Type
- Protocol
- State
- Receive Buffer
- Send Buffer
- File Operations
- Reference Count
- Credential
- ...

```mermaid
graph TD
    SO["Socket Object"]
    SO --> D["Domain = AF_UNIX"]
    SO --> T["Type = STREAM"]
    SO --> S["State = CREATED"]
    SO --> RB["Receive Buffer"]
    SO --> SB["Send Buffer"]
    SO --> RC["Reference Count"]
```

---

## Socket Object nằm ở đâu?

Hoàn toàn trong **Kernel Heap**. Process chỉ giữ **File Descriptor**.

```mermaid
graph LR
    P["Process<br/>FD Table"] -->|fd = 3| SO["Socket Object"]
```

Giống Shared Memory: `fd → Kernel Object`.

---

## Socket Buffer

Đây là phần cực kỳ quan trọng. Socket luôn có **Send Buffer** và **Receive Buffer**.

```mermaid
sequenceDiagram
    participant A as Process A
    participant SB as Send Buffer
    participant K as Kernel
    participant RB as Receive Buffer
    participant B as Process B

    A->>SB: write()
    SB->>K: Chuyển dữ liệu
    K->>RB: Đưa vào Receive Buffer
    B->>RB: read()
```

Đây chính là lý do `write()` không phải lúc nào cũng gửi dữ liệu ngay lập tức — dữ liệu được đưa vào Send Buffer trước, kernel xử lý và chuyển đi sau.

---

## Socket có những loại nào?

Có hai cách phân loại: theo **Domain** và theo **Type**.

### Theo Domain

```mermaid
graph TD
    D["Domain"] --> AFU["AF_UNIX<br/>Unix Domain Socket"]
    D --> AFI["AF_INET<br/>IPv4"]
    D --> AFI6["AF_INET6<br/>IPv6"]
    D --> AFP["AF_PACKET<br/>Ethernet Frame"]
    D --> AFN["AF_NETLINK<br/>Giao tiếp với Kernel"]
```

| Domain | Mô tả | Ví dụ sử dụng |
|---|---|---|
| `AF_UNIX` | Chỉ dùng trên cùng máy (GUI ↔ Backend) | IPC nội bộ |
| `AF_INET` | IPv4 (vd: `192.168.1.100`) | Giao tiếp mạng |
| `AF_INET6` | IPv6 | Giao tiếp mạng |
| `AF_PACKET` | Làm việc trực tiếp với Ethernet Frame | Wireshark, tcpdump |
| `AF_NETLINK` | Giao tiếp với Kernel | iproute2, udev, NetworkManager |

### Theo Type

```mermaid
graph TD
    T["Type"] --> STREAM["SOCK_STREAM<br/>TCP / Unix Stream"]
    T --> DGRAM["SOCK_DGRAM<br/>UDP / Unix Datagram"]
    T --> SEQ["SOCK_SEQPACKET<br/>Connection + giữ ranh giới message"]
    T --> RAW["SOCK_RAW<br/>Thao tác trực tiếp với packet"]
```

| Type | Đặc điểm | Ví dụ |
|---|---|---|
| `SOCK_STREAM` | Connection, Reliable, Ordered | SSH, HTTP, HTTPS |
| `SOCK_DGRAM` | Không connection, không đảm bảo, nhanh | DNS, Video Streaming, VoIP |
| `SOCK_SEQPACKET` | Giữ nguyên ranh giới message, có kết nối | Một số hệ thống IPC nội bộ |
| `SOCK_RAW` | Socket thô, thao tác trực tiếp với packet | ping, tcpdump, Wireshark |

---

## Kiến trúc tổng thể

```mermaid
graph TD
    subgraph US1["User Space"]
        PA["Process A<br/>FD = 3"]
    end

    PA -->|socket()| SC["System Call"]

    subgraph KS["Kernel Space"]
        SC --> SOBJ["Socket Object"]
        SOBJ --> SB["Send Buffer"]
        SB --> PROTO["Protocol"]
        PROTO --> RB["Receive Buffer"]
    end

    subgraph US2["User Space"]
        PB["Process B<br/>FD = 5"]
    end

    RB --> PB
```

---

## Trong Embedded Linux người ta dùng loại nào?

Nếu làm **Embedded Linux** sẽ gặp:

| Loại | Mục đích |
|---|---|
| **Unix Domain Socket** (`AF_UNIX + SOCK_STREAM`) | IPC giữa các process trên cùng máy |
| **TCP Socket** (`AF_INET + SOCK_STREAM`) | Giao tiếp với server hoặc thiết bị khác |
| **UDP Socket** (`AF_INET + SOCK_DGRAM`) | Streaming, telemetry, sensor |

Ví dụ trong máy siêu âm:

```mermaid
graph TD
    GUI["GUI"] -->|Unix Domain Socket| IPS["Image Processing Service"]
    IPS -->|Shared Memory| DRV["Ultrasound Driver"]
```

Ở đây:

- **Socket** truyền lệnh như `START_SCAN`, `FREEZE`, `SAVE_IMAGE`.
- **Shared Memory** truyền frame ảnh siêu âm.
- Đây là một kiến trúc rất phổ biến trong các hệ thống nhúng hiệu năng cao.

---

## Lộ trình tiếp theo

Sau khi nắm vững hai module đầu tiên, nên tiếp tục với:

> **Unix Domain Socket (`AF_UNIX`)**

Đây là IPC thuần túy, rất giống Shared Memory và Message Queue nhưng mạnh hơn nhiều. Sau khi hiểu rõ Unix Domain Socket, việc chuyển sang TCP/UDP sẽ rất tự nhiên vì API gần như giống hệt nhau, chỉ khác `domain` và cách `bind()` địa chỉ.

---

## Socket - Mindmap tổng quan (Module 1 & Module 2)

```mermaid
mindmap
  root((SOCKET))
    1. Socket là gì?
      Endpoint của kênh giao tiếp
      Cầu nối giữa hai process/hai máy
      Kernel chịu trách nhiệm truyền dữ liệu
      Process chỉ thao tác với FD
    2. Socket hoạt động ở đâu?
      User Space
        Process A
        Process B
      Kernel Space
        Socket Object
        Send Buffer
        Receive Buffer
        Protocol Stack
        Driver
    3. Kernel tạo gì khi socket?
      Socket Object
        Domain
        Type
        Protocol
        State
        Send/Receive Buffer
        File Operations
        Reference Count
      Trả về File Descriptor
    4. Kiến trúc truyền dữ liệu
      write fd
      Send Buffer
      Protocol Stack
      Receive Buffer
      read fd
    5. Socket Buffer
      Send Buffer
      Receive Buffer
      Kernel quản lý
      Blocking
      Non-blocking
    6. File Descriptor
      socket returns fd
      fd la Handle
      fd tro Socket Object
      Moi Process co FD Table rieng
    7. Domain
      AF_UNIX
      AF_INET
      AF_INET6
      AF_PACKET
      AF_NETLINK
    8. Type
      SOCK_STREAM
      SOCK_DGRAM
      SOCK_SEQPACKET
      SOCK_RAW
    9. So sanh IPC
      Shared Memory
      Message Queue
      Socket
    10. Embedded Linux Architecture
      GUI
      Unix Domain Socket
      Image Processing Service
      Shared Memory
      Driver Hardware
```

---

## Kiến thức cốt lõi cần nhớ

### Socket = Endpoint

```mermaid
graph LR
    A["Process A"] --> SA["Socket A"]
    SA -.-> SB["Socket B"]
    SB --> B["Process B"]
```

### Socket Object nằm trong Kernel

```mermaid
graph LR
    P["Process"] --> FD["File Descriptor"] --> SO["Socket Object"]
```

### Dữ liệu luôn đi qua Kernel

```mermaid
graph LR
    W["write()"] --> SB["Send Buffer"] --> PR["Protocol"] --> RB["Receive Buffer"] --> R["read()"]
```

### Socket khác Shared Memory

```mermaid
graph TD
    subgraph SM["Shared Memory"]
        A1["Process A"] --> RAM["RAM"]
        B1["Process B"] --> RAM
    end

    subgraph SK["Socket"]
        A2["Process A"] --> K["Kernel"] --> B2["Process B"]
    end
```

### Socket khác Message Queue

```mermaid
graph LR
    subgraph MQ["Message Queue"]
        S1["Sender"] --> Q1["Kernel Queue"] --> R1["Receiver"]
    end

    subgraph SC["Socket"]
        S2["Process A"] --> C2["Connection"] --> R2["Process B"]
    end
```

Message Queue truyền từng **Message**. Socket tạo một **kênh giao tiếp liên tục (Connection)**.

---

## Những API sẽ học ở Module tiếp theo (Unix Domain Socket)

```mermaid
graph LR
    A["socket()"] --> B["bind()"] --> C["listen()"] --> D["accept()"]
    E["connect()"] --> F["send()"] --> G["recv()"] --> H["close()"]
```

Đây là bộ API nền tảng của Socket. Khi đã hiểu chúng với **Unix Domain Socket**, bạn gần như đã nắm được khoảng **90% API của TCP Socket**, vì sự khác biệt chủ yếu nằm ở **Address Family** (`AF_UNIX` ↔ `AF_INET`) và cách biểu diễn địa chỉ.

### Ý nghĩa từng API

| API | Vai trò |
|---|---|
| `socket()` | Tạo socket object trong kernel, trả về file descriptor |
| `bind()` | Gắn socket server với một địa chỉ, ví dụ `/tmp/demo_unix_socket` |
| `listen()` | Chuyển socket sang trạng thái chờ kết nối |
| `accept()` | Chấp nhận một client mới |
| `connect()` | Client kết nối tới server |
| `send()` | Gửi dữ liệu |
| `recv()` | Nhận dữ liệu |
| `close()` | Đóng file descriptor |
| `unlink()` | Xóa socket file trong filesystem |