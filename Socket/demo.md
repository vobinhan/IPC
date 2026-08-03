Server  <------ socket file ------>  Client


Client nhập message từ bàn phím, gửi sang Server. Server nhận, in ra màn hình và echo lại cho Client.

# Kiến trúc hoạt động
```
Client Process
     │
     │ connect()
     ▼
Unix Domain Socket File
/tmp/demo_unix_socket
     ▲
     │ accept()
Server Process
```

# Luồng Server:
``` bash
socket()
   ↓
bind()
   ↓
listen()
   ↓
accept()
   ↓
recv()
   ↓
send()
   ↓
close()
```
# Luồng Client:
``` bash
socket()
   ↓
connect()
   ↓
send()
   ↓
recv()
   ↓
close()
```



