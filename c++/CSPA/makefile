RM=rm -rf
CP=cp -rf
CC=g++
CFLAGS= -c -Wall -g -fPIC

INC=
LIB_PATH=
LIB=
C_SRC=$(wildcard *.c)
C_OBJ=$(patsubst %.c, %.o, $(C_SRC))
CPP_SRC=$(wildcard *.cpp)
CPP_OBJ=$(patsubst %.cpp, %.o, $(CPP_SRC))
OBJS=$(C_OBJ) $(CPP_OBJ)

PROJECT=test_cmd
STATIC_LIB=libcts.a
DYNAMIC_LIB=libcts.so
LIB_DEPS=cts.o StringTools.o

INSTALL_HOME=
INSTALL_INC_PATH=/usr/local/include
INSTALL_LIB_PATH=/usr/local/lib
INSTALL_INC_FILE=cts.h StringTools.h
VPATH=./
#---------------------------------------------------------------------------------------------

.PHONY:all
	
all:$(PROJECT) $(STATIC_LIB) $(DYNAMIC_LIB)
	
$(PROJECT):$(OBJS)
	$(CC) -o $@ $(OBJS) $(LIB_PATH) $(LIB)

$(DYNAMIC_LIB):$(LIB_DEPS)
	$(CC) -o $@ -shared $(OBJS) $(LIB_PATH) $(LIB)

$(STATIC_LIB):$(LIB_DEPS)
	ar -rc -o $@ $(OBJS) $(LIB_PATH) $(LIB)

$(CPP_OBJ):%.o:%.cpp
	$(CC) $(CFLAGS) $(INC) $< -o $@
$(C_OBJ):%.o:%.c
	$(CC) $(CFLAGS) $(INC) $< -o $@

install:
	$(CP) $(STATIC_LIB) $(DYNAMIC_LIB) $(INSTALL_LIB_PATH)
	$(CP) $(INSTALL_INC_FILE) $(INSTALL_INC_PATH)
clean:
	$(RM) $(PROJECT) $(OBJS) $(STATIC_LIB) $(DYNAMIC_LIB)
