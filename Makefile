# dict app
# need zlib support

APP := trans
OUT_DIR := /usr/local

all:
	gcc *.c -lz -o ${APP}

install:
	cp -f ${APP} ${OUT_DIR}/bin
	cp -rf dict ${OUT_DIR}/share

uninstall:
	rm -f ${OUT_DIR}/bin/${APP}
	rm -rf dict ${OUT_DIR}/share/dict

clean:
	rm -f ${APP}

