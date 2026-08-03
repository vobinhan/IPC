# Roadmap Socket

Mình đề xuất chia thành 10 chương.

```text
Module 1
Socket là gì?

↓

Module 2
Socket trong Kernel hoạt động như thế nào?

↓

Module 3
File Descriptor và Socket

↓

Module 4
Unix Domain Socket (IPC)

↓

Module 5
TCP Socket

↓

Module 6
UDP Socket

↓

Module 7
Socket Buffer

↓

Module 8
Blocking / Non-blocking

↓

Module 9
select / poll / epoll

↓

Module 10
Socket Architecture
```

Hôm nay chúng ta học **Module 1 + Module 2**.

---

# 1. Socket là gì?

Đầu tiên hãy quên API đi.

Socket thực chất là:

> **Một endpoint (điểm cuối) của một kênh giao tiếp.**

Ví dụ:

```text
Process A
     │
 Socket A

==================

 Socket B
     │
Process B
```

Hai process **không giao tiếp trực tiếp**.

Mỗi process chỉ làm việc với socket của mình.

Kernel chịu trách nhiệm chuyển dữ liệu giữa hai socket.

---

# Socket khác Shared Memory thế nào?

Shared Memory

```text
Process A
      │
      ▼
 Shared RAM
      ▲
      │
Process B
```

Hai process cùng nhìn vào một vùng RAM.

---

Socket

```text
Process A

↓

Socket

↓

Kernel

↓

Socket

↓

Process B
```

Kernel luôn đứng giữa.

Đó là sự khác biệt lớn nhất.

---

# Socket khác Message Queue thế nào?

Message Queue

```text
Sender

↓

Kernel Queue

↓

Receiver
```

Socket

```text
Sender

↓

Socket Buffer

↓

Receiver
```

Thoạt nhìn giống nhau.

Nhưng khác ở chỗ:

Message Queue

```text
Queue
```

Socket

```text
Connection
```

Socket tạo ra một **kênh giao tiếp liên tục**.

---

# Tại sao phải có Socket?

Giả sử

```text
GUI

↓

Hardware Process
```

Có thể dùng

```text
Shared Memory
```

được.

---

Nhưng nếu

```text
GUI

↓

Server

↓

Client

↓

Cloud
```

thì sao?

Shared Memory không thể.

Message Queue cũng không.

Socket thì được.

---

Socket được thiết kế để:

* IPC trên cùng máy.
* IPC giữa nhiều máy.

API gần như giống nhau.

---

# 2. Socket nằm ở đâu trong Kernel?

Đây là phần quan trọng nhất.

Nhiều người nghĩ:

```text
socket()

↓

Process A

↓

Process B
```

Không đúng.

Kernel tạo một object.

```text
Process A

↓

File Descriptor

↓

Socket Object

↓

Kernel
```

Socket Object nằm hoàn toàn trong Kernel Space.

---

# Kiến trúc trong Kernel

Có thể hình dung như sau:

```text
+----------------------------------+
|          User Space              |
+----------------------------------+

Process A

socket fd

↓

System Call

------------------------------------

Kernel Space

Socket Object

↓

Socket Buffer

↓

Protocol

↓

Driver

------------------------------------

User Space

Process B
```

---

# Kernel tạo gì khi gọi socket()?

Giả sử:

```cpp
socket(AF_UNIX,
       SOCK_STREAM,
       0);
```

Kernel tạo:

```text
Socket Object
```

Object này chứa:

```text
Domain

Type

Protocol

State

Receive Buffer

Send Buffer

File Operations

Reference Count

Credential

...
```

Có thể hình dung:

```text
+-------------------------------+

Socket

Domain = AF_UNIX

Type = STREAM

State = CREATED

Receive Buffer

Send Buffer

Reference Count

+-------------------------------+
```

---

# Socket Object nằm ở đâu?

Hoàn toàn trong:

```text
Kernel Heap
```

Process chỉ giữ:

```text
File Descriptor
```

Ví dụ:

```text
Process

FD Table

3

↓

Socket Object
```

Giống Shared Memory:

```text
fd

↓

Kernel Object
```

---

# Socket Buffer

Đây là phần cực kỳ quan trọng.

Socket luôn có:

```text
Send Buffer

Receive Buffer
```

Ví dụ:

```text
Process A

↓

write()

↓

Send Buffer

↓

Kernel

↓

Receive Buffer

↓

read()

↓

Process B
```

Đây chính là lý do:

```text
write()
```

không phải lúc nào cũng gửi dữ liệu ngay lập tức.

---

# Socket có những loại nào?

Có hai cách phân loại.

## Theo Domain

### AF_UNIX

Hay còn gọi:

```text
Unix Domain Socket
```

Chỉ dùng trên cùng máy.

```text
GUI

↓

Backend
```

---

### AF_INET

IPv4.

```text
192.168.1.100
```

---

### AF_INET6

IPv6.

---

### AF_PACKET

Làm việc trực tiếp với Ethernet Frame.

Driver.

Network Analyzer.

Ví dụ:

```text
Wireshark
tcpdump
```

---

### AF_NETLINK

Giao tiếp với Kernel.

Ví dụ:

```text
iproute2
udev
NetworkManager
```

---

# Theo Type

## SOCK_STREAM

```text
TCP

Unix Stream
```

Đặc điểm:

```text
Connection

Reliable

Ordered
```

Ví dụ:

```text
SSH

HTTP

HTTPS
```

---

## SOCK_DGRAM

```text
UDP

Unix Datagram
```

Đặc điểm:

```text
Không connection

Không đảm bảo

Nhanh
```

Ví dụ:

```text
DNS

Video Streaming

VoIP
```

---

## SOCK_SEQPACKET

Giữ nguyên ranh giới message, có kết nối.

Ít phổ biến hơn nhưng được dùng trong một số hệ thống IPC và giao tiếp nội bộ.

---

## SOCK_RAW

Socket thô.

Cho phép thao tác trực tiếp với packet.

Ví dụ:

```text
ping

tcpdump

Wireshark
```

---

# Kiến trúc tổng thể

```text
                  User Space

Process A

↓

FD = 3

↓

socket()

↓

==================================

Kernel Space

Socket Object

↓

Send Buffer

↓

Protocol

↓

Receive Buffer

==================================

↓

FD = 5

↓

Process B
```

---

# Trong Embedded Linux người ta dùng loại nào?

Nếu bạn làm **Embedded Linux**, khoảng **90% trường hợp** bạn sẽ gặp:

| Loại                                             | Mục đích                                |
| ------------------------------------------------ | --------------------------------------- |
| **Unix Domain Socket (`AF_UNIX + SOCK_STREAM`)** | IPC giữa các process trên cùng máy      |
| **TCP Socket (`AF_INET + SOCK_STREAM`)**         | Giao tiếp với server hoặc thiết bị khác |
| **UDP Socket (`AF_INET + SOCK_DGRAM`)**          | Streaming, telemetry, sensor            |

Ví dụ trong máy siêu âm:

```text
GUI
        │
Unix Domain Socket
        │
Image Processing Service
        │
Shared Memory
        │
Ultrasound Driver
```

Ở đây:

* **Socket** truyền lệnh như `START_SCAN`, `FREEZE`, `SAVE_IMAGE`.
* **Shared Memory** truyền frame ảnh siêu âm.
* Đây là một kiến trúc rất phổ biến trong các hệ thống nhúng hiệu năng cao.

---

# Lộ trình tiếp theo

Sau khi nắm vững hai module đầu tiên, nên tiếp tục với:

> **Unix Domain Socket (`AF_UNIX`)**

Đây là IPC thuần túy, rất giống Shared Memory và Message Queue nhưng mạnh hơn nhiều. Sau khi hiểu rõ Unix Domain Socket, việc chuyển sang TCP/UDP sẽ rất tự nhiên vì API gần như giống hệt nhau, chỉ khác `domain` và cách `bind()` địa chỉ.




# Socket - Mindmap tổng quan (Module 1 & Module 2)

```text
SOCKET
│
├── 1. Socket là gì?
│   │
│   ├── Endpoint của một kênh giao tiếp
│   ├── Là cầu nối giữa hai tiến trình hoặc hai máy
│   ├── Kernel chịu trách nhiệm truyền dữ liệu
│   └── Process chỉ thao tác với File Descriptor
│
├── 2. Socket hoạt động ở đâu?
│   │
│   ├── User Space
│   │      │
│   │      ├── Process A
│   │      └── Process B
│   │
│   └── Kernel Space
│          │
│          ├── Socket Object
│          ├── Send Buffer
│          ├── Receive Buffer
│          ├── Protocol Stack
│          └── Driver
│
├── 3. Kernel tạo gì khi socket()?
│   │
│   ├── Socket Object
│   │      │
│   │      ├── Domain
│   │      ├── Type
│   │      ├── Protocol
│   │      ├── State
│   │      ├── Send Buffer
│   │      ├── Receive Buffer
│   │      ├── File Operations
│   │      └── Reference Count
│   │
│   └── Trả về File Descriptor
│
├── 4. Kiến trúc truyền dữ liệu
│   │
│   ├── write(fd)
│   │      │
│   │      ▼
│   │  Send Buffer
│   │      │
│   │      ▼
│   │ Protocol Stack
│   │      │
│   │      ▼
│   │ Receive Buffer
│   │      │
│   │      ▼
│   └── read(fd)
│
├── 5. Socket Buffer
│   │
│   ├── Send Buffer
│   ├── Receive Buffer
│   ├── Kernel quản lý
│   ├── Có thể Blocking
│   └── Có thể Non-blocking
│
├── 6. File Descriptor
│   │
│   ├── socket() → fd
│   ├── fd chỉ là Handle
│   ├── fd → Socket Object
│   └── Mỗi Process có FD Table riêng
│
├── 7. Domain (Address Family)
│   │
│   ├── AF_UNIX
│   │      └── IPC trên cùng máy
│   │
│   ├── AF_INET
│   │      └── IPv4
│   │
│   ├── AF_INET6
│   │      └── IPv6
│   │
│   ├── AF_PACKET
│   │      └── Ethernet Frame
│   │
│   └── AF_NETLINK
│          └── User Space ↔ Kernel
│
├── 8. Type
│   │
│   ├── SOCK_STREAM
│   │      │
│   │      ├── Connection-oriented
│   │      ├── Reliable
│   │      ├── Ordered
│   │      └── TCP / Unix Stream
│   │
│   ├── SOCK_DGRAM
│   │      │
│   │      ├── Connectionless
│   │      ├── Unreliable
│   │      └── UDP / Unix Datagram
│   │
│   ├── SOCK_SEQPACKET
│   │      └── Connection + giữ nguyên Message
│   │
│   └── SOCK_RAW
│          └── Raw Packet
│
├── 9. So sánh IPC
│   │
│   ├── Shared Memory
│   │      ├── Chia sẻ RAM
│   │      ├── Không copy
│   │      ├── Rất nhanh
│   │      └── Cần Semaphore
│   │
│   ├── Message Queue
│   │      ├── Kernel Queue
│   │      ├── Copy User→Kernel→User
│   │      ├── Event / Command
│   │      └── Priority
│   │
│   └── Socket
│          ├── Kernel Socket Object
│          ├── Full Duplex
│          ├── IPC hoặc Network
│          └── Stream hoặc Datagram
│
└── 10. Embedded Linux Architecture
    │
    ├── GUI
    │      │
    │      ▼
    ├── Unix Domain Socket
    │      │
    │      ▼
    ├── Image Processing Service
    │      │
    │      ▼
    ├── Shared Memory
    │      │
    │      ▼
    └── Driver / Hardware
```

---

# Kiến thức cốt lõi cần nhớ

### Socket = Endpoint

```
Process A
     │
 Socket A

===========

 Socket B
     │
Process B
```

---

### Socket Object nằm trong Kernel

```
Process
   │
 File Descriptor
   │
   ▼
Socket Object
```

---

### Dữ liệu luôn đi qua Kernel

```
write()

↓

Send Buffer

↓

Protocol

↓

Receive Buffer

↓

read()
```

---

### Socket khác Shared Memory

```
Shared Memory

Process A

↓

RAM

↑

Process B
```

```
Socket

Process A

↓

Kernel

↓

Process B
```

---

### Socket khác Message Queue

```
Message Queue

Sender

↓

Kernel Queue

↓

Receiver
```

```
Socket

Process A

↓

Connection

↓

Process B
```

Message Queue truyền từng **Message**.

Socket tạo một **kênh giao tiếp liên tục (Connection)**.

---

# Những API sẽ học ở Module tiếp theo (Unix Domain Socket)

```
socket()

bind()

listen()

accept()

connect()

send()

recv()

close()
```

Đây là bộ API nền tảng của Socket. Khi đã hiểu chúng với **Unix Domain Socket**, bạn gần như đã nắm được khoảng **90% API của TCP Socket**, vì sự khác biệt chủ yếu nằm ở **Address Family (`AF_UNIX` ↔ `AF_INET`)** và cách biểu diễn địa chỉ.



# Ý nghĩa từng API
| API         | Vai trò                                                             |
| ----------- | ------------------------------------------------------------------- |
| `socket()`  | Tạo socket object trong kernel, trả về file descriptor              |
| `bind()`    | Gắn socket server với một địa chỉ, ở đây là `/tmp/demo_unix_socket` |
| `listen()`  | Chuyển socket sang trạng thái chờ kết nối                           |
| `accept()`  | Chấp nhận một client mới                                            |
| `connect()` | Client kết nối tới server                                           |
| `send()`    | Gửi dữ liệu                                                         |
| `recv()`    | Nhận dữ liệu                                                        |
| `close()`   | Đóng file descriptor                                                |
| `unlink()`  | Xóa socket file trong filesystem                                    |
