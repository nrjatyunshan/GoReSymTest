```bash
# 编译
gcc -ldl main.c

# 测试获取函数地址和函数长度
./a.out test-go-bin runtime.casgstatus

# 测试获取符号地址
./a.out test-go-bin go.itab.syscall.Errno,error
```