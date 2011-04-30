ZVER=3.0.0

LDFLAGS+=-L$(HOME)/opt/zeromq-$(ZVER)/lib -lzmq
LDFLAGS+=-Xlinker -rpath=$(HOME)/opt/zeromq-$(ZVER)/lib -lzmq
CFLAGS=-I$(HOME)/opt/zeromq-$(ZVER)/include -ggdb

default: stress server
