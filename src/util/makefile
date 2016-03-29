
include ../../../make.inc

all:
	$(CC) ${CPPFLAGS} $(CFLAGS) -o doy doy.c ${LDFLAGS}
	$(CC) ${CPPFLAGS} $(CFLAGS) -o month_day month_day.c ${LDFLAGS}

delete:
	rm -f doy month_day

clean:
	rm -f *.o
