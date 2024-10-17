.PHONY: default build install

APP=t9rgb_control
T9=t9-server.local

default: build

build:
	gcc -o ${APP} ${APP}.c

install:
	mv ${APP} /usr/local/bin/${APP}

