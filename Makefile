#指定编译器
CC = g++

#找出当前目录下，所有的源文件（以.cpp结尾）,所以这里是调用了shell的find命令了
SRCS := $(shell find ./* -type f | grep '\.cpp' | grep -v 'main\.cpp' | grep -v 'cmake')
$(warning SRCS is ${SRCS})

#确定cpp源文件对应的目标文件
OBJS := $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRCS)))
$(warning OBJS is ${OBJS})

#编译选项
CFLAGS = -g -O2 -Wall -Werror -Wno-unused -ldl -lpthread -fPIC -std=c++11
$(warning CFLAGS is ${CFLAGS})

#找出当前目录下所有的头文件
INCLUDE = -I.

SRC_MAIN = main.cpp
OBJ_MAIN = ${SRC_MAIN:%.cpp=%.o}
EXE_MAIN = main

target: ${EXE_MAIN}

$(EXE_MAIN): $(OBJ_MAIN) $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDE)

clean:
	rm -f ${OBJS} ${OBJ_MAIN} ${EXE_MAIN}

%.o: %.cpp
	${CC} ${CFLAGS} -I. -c $< -o $@
