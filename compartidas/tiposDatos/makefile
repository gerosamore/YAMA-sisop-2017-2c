H_SRCS=$(shell find . -iname "*.h" | tr '\n' ' ')

install: all
	cp --parents -u $(H_SRCS) /usr/include

uninstall:
	rm -rf /usr/include/tiposDatos

.PHONY: all create-dirs clean install uninstall
