.PHONY: clean run test

WRAP   := valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all 
CFLAGS += -fno-builtin -I/usr/include/libircclient/
ifeq ($(DEBUG), 1)
  CFLAGS += -Wall -Wextra -Wpedantic 
  CFLAGS += -DDEBUG -O0 -ggdb -fno-inline	
else
  CFLAGS += -O3 -fno-stack-protector -fno-rtti
endif

LDLIBS := -lircclient

INSTALL_TARGET := /etc/init.d/hibot
OUT := hibot

SOURCE.d := source/
SOURCE   := main.c
SOURCE   := $(addprefix ${SOURCE.d}, ${SOURCE})
HEADER   := config.inc version.inc log.h bot.h syntax.h
HEADER   := $(addprefix ${SOURCE.d}, ${HEADER})

${OUT}: ${SOURCE} ${HEADER}
	${CC} ${CFLAGS} -o $@ ${SOURCE} ${LDLIBS}

run: ${OUT}
	./${OUT} irc.rizon.net:6665 "#/g/test"

test: ${OUT}
	${WRAP} ${OUT} irc.rizon.net:6665 "#/g/test"

install:
	m4 script/hibot.m4 > ${INSTALL_TARGET}
	chmod 755 ${INSTALL_TARGET}

clean:
	-rm ${OUT}
	-rm ${INSTALL_TARGET}
