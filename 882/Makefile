
CFLAGS += -Wall

ifneq ($(shell which strace 2>/dev/null),)
TRACE_CONNECT = strace -f -qq -r -e trace=connect
else ifneq ($(shell which ltrace 2>/dev/null),)
TRACE_CONNECT = ltrace -f -C -r -e connect
else
$(warning Could not find strace or ltrace; test targets will likely not produce helpful feedback.)
endif

issue: issue.c
	$(CC) $(CFLAGS) -lzmq -o $@ $^

clean:
	rm -f issue

no-fail: issue
	$(TRACE_CONNECT) ./issue

connect-fail: issue
	$(TRACE_CONNECT) ./issue fail

auth-fail: issue
	$(TRACE_CONNECT) ./issue wrong

PHONY: clean no-fail connect-fail auth-fail

