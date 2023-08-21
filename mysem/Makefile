CC = gcc
TARGET = myexe
#SRC = $(wildcard *.c)
#OBJ = $(patsubst %.c, %.o, $(SRC))

CFLAGS+=-pthread
LDFLAGS+=-pthread

$(TARGET): mysem.c main.c
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

#$<表示所有依赖的合集的第一个文件
#$@正在执行的目标文件名
#$^所有的依赖文件


.PHONY:clean
clean:
	rm -rf *.o