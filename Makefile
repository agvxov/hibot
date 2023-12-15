.PHONY: clean run test

WRAP   := valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all 
CFLAGS += -fno-builtin -I/usr/include/libircclient/
ifeq ($(DEBUG), 1)
  CFLAGS += -Wall -Wextra -Wpedantic 
  CFLAGS += -DDEBUG -O0 -ggdb -fno-inline	
else
  CFLAGS += -O3 -fno-stack-protector -fno-exceptions -fno-rtti
endif

LDLIBS := -lircclient

OUT := hibot

SOURCE.d := source/
SOURCE   := main.c
SOURCE   := $(addprefix ${SOURCE.d}, ${SOURCE})
HEADER   := config.inc log.h bot.h
HEADER   := $(addprefix ${SOURCE.d}, ${HEADER})

${OUT}: ${SOURCE} ${HEADER}
	${CC} ${CFLAGS} -o $@ ${SOURCE} ${LDLIBS}

run:
	${OUT} irc.rizon.net:6665 "#/g/test"

test: ${OUT}
	${WRAP} ${OUT} irc.rizon.net:6665 "#/g/test"

clean:
	-rm ${OUT}

docs:
	for i in documentation/*.md; do \
		kramdown-man "$$i" -o documentation/manual/$$(basename -s .md $$i) ; \
	done
