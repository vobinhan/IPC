# POSIX Message Queue
POSIX Message Queue là cơ chế IPC cho phép các process gửi dữ liệu cho nhau theo dạng message.
Mô hình:
```
Sender Process
      |
      v
+----------------+
| Message Queue  |
+----------------+
      |
      v
Receiver Process
```
# Cơ chế hoạt động chung
Kernel tao ra queue nam trong Kernel Space
```
Kernel Space +----------------------+ | Message Queue | |----------------------| | Message 1 | | Message 2 | | Message 3 | +----------------------+ ▲ ▲ │ │ Sender Process Receiver Process
```

NOTE: 
Message Queue thuoc kernel va khong thuoc bat ki process nao



Sender:
```
mq_open()
   |
mq_send()
   |
mq_close()
```
Receiver:
```
mq_open()
   |
mq_receive()
   |
mq_close()
mq_unlink()
```

# Thư viện cần dùng
| API                 | Header       |
| ------------------- | ------------ |
| `mq_open()`         | `<mqueue.h>` |
| `mq_send()`         | `<mqueue.h>` |
| `mq_receive()`      | `<mqueue.h>` |
| `mq_close()`        | `<mqueue.h>` |
| `mq_unlink()`       | `<mqueue.h>` |
| `O_CREAT`, `O_RDWR` | `<fcntl.h>`  |
| `perror()`          | `<cstdio>`   |
| `strlen()`          | `<cstring>`  |

