#       Ideally, should also include the dependency on header files.
#
#       Remember to
#       source ../../../env.sh

include ../make.inc

select_version=v3.0

TARGET = albers.toa.tile.ofutm.proc.v3.0
  OBJS = $(TARGET).o \
		 error.o \
		 weld.util.o\
		 weld.comp.util.o\
		 weld.projection.o\
		 weld.setup.sds.o \
		 L457.comp.tile.handler.o \
		 L457.toa.acq.handler.o \
		 weld.hdf.eos.o 

#		 select_obs.$(select_version).o


$(TARGET): $(OBJS)
	$(CC) ${CFLAGS} -o $(TARGET) $(OBJS) $(OPT) ${LDFLAGS}


$(TARGET).o: $(TARGET).c
	$(CC) ${CFLAGS} -c $(TARGET).c $(OPT) ${CPPFLAGS}

error.o: $(WELDCOMM)/error.c
	$(CC) ${CFLAGS} -c $(WELDCOMM)/error.c $(OPT) ${CPPFLAGS}

weld.util.o: $(WELDCOMM)/weld.util.c
	$(CC) ${CFLAGS} -c $(WELDCOMM)/weld.util.c $(OPT) ${CPPFLAGS}
	
weld.comp.util.o: $(WELDCOMM)/weld.comp.util.c
	$(CC) ${CFLAGS} -c $(WELDCOMM)/weld.comp.util.c $(OPT) ${CPPFLAGS}

weld.projection.o: $(WELDCOMM)/weld.projection.c
	$(CC) ${CFLAGS} -c $(WELDCOMM)/weld.projection.c $(OPT) ${CPPFLAGS}

weld.setup.sds.o: $(WELDCOMM)/weld.setup.sds.c
	$(CC) ${CFLAGS} -c $(WELDCOMM)/weld.setup.sds.c $(OPT) ${CPPFLAGS}

L457.toa.acq.handler.o: $(WELDCOMM)/L457.toa.acq.handler.c
	$(CC) ${CFLAGS} -c $(WELDCOMM)/L457.toa.acq.handler.c  $(OPT) ${CPPFLAGS}

L457.comp.tile.handler.o: $(WELDCOMM)/L457.comp.tile.handler.c
	$(CC) ${CFLAGS} -c $(WELDCOMM)/L457.comp.tile.handler.c $(OPT) ${CPPFLAGS}

#select_obs.$(select_version).o: $(WELDCOMM)/select_obs.$(select_version).c
	#$(CC) ${CFLAGS} -c $(WELDCOMM)/select_obs.$(select_version).c  $(OPT) ${CPPFLAGS}

weld.hdf.eos.o: $(WELDCOMM)/weld.hdf.eos.c
	$(CC) ${CFLAGS} -c $(WELDCOMM)/weld.hdf.eos.c $(OPT) ${CPPFLAGS}



clean:
	rm -f *.o

delete:
	rm -f $(TARGET)
