objects = main.o daemon.o worker.o

chap-proxy: $(objects)
	$(CC) $(LDFLAGS) $^ -o $@

main.o: main.c daemon.h worker.h
daemon.o: daemon.c daemon.h
worker.o: daemon.c worker.h

.PHONY: clean
clean:
	rm -f $(objects) chap-proxy
