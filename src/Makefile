objects = main.o daemon.o worker.o passive.o queue.o

chap-proxy: $(objects)
	$(CC) $(LDFLAGS) $^ -o $@

main.o: main.c daemon.h worker.h passive.h global.h
daemon.o: daemon.c daemon.h
worker.o: daemon.c worker.h
passive.o: passive.c passive.h worker.h global.h
queue.o: queue.c queue.h

.PHONY: clean
clean:
	rm -f $(objects) chap-proxy
