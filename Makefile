OUT=tmp
SRC=		\
    algebra.c	\
    camera.c	\
    color.c	\
    image.c	\
    light.c	\
    mainIUP.c	\
    material.c	\
    object.c	\
    raytracing.c\
    scene.c

# Configs
CC=gcc
RM=rm
MV=mv
CFLAGS=-O0 -Wall `pkg-config gl --cflags` -I /usr/include/iup -ggdb
LIBS=-liup -liupgl -liupimglib


MAKEFILE=Makefile
OBJ=$(SRC:.c=.o)

.c.o:
	$(CC) -c $(CFLAGS) $<

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

clean:
	$(RM) $(OBJ) $(OUT)

depend:
	if grep '^# DO NOT DELETE' $(MAKEFILE) >/dev/null; \
	then \
		sed -e '/^# DO NOT DELETE/,$$d' $(MAKEFILE) > \
			$(MAKEFILE).$$$$ && \
		$(MV) $(MAKEFILE).$$$$ $(MAKEFILE); \
	fi
	echo '# DO NOT DELETE THIS LINE -- make depend depends on it.' \
		>> $(MAKEFILE); \
	$(CC) -M $(SRC) >> $(MAKEFILE)
