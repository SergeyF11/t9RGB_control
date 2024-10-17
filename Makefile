.PHONY: default build install uninstall

APP=t9rgb_control
T9=t9-server.local
INSTALL_PATH=/usr/local/bin

default: build

build:
	gcc -o ${APP} ${APP}.c

install:
	mv ${APP} ${INSTALL_PATH}/${APP}

uninstall:
	rm ${INSTALL_PATH}/${APP}

