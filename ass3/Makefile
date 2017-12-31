all: sender receiver file

sender: sender.c
	gcc -Wall sender.c -o sender

receiver: receiver.c
	gcc -Wall receiver.c -o receiver

file:
	dd if=/dev/zero of=sharedfile count=10 bs=10

clean:
	rm -f sender receiver sharedfile

tar:
	tar -cvf assign3.tar sender.c receiver.c Makefile
