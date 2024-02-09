LOGGER_IP ?= "" # put log server IP in string
FTP_IP ?= 192.168.1.136 # put console IP here
.PHONY: all clean config

all:
	cmake --toolchain=cmake/toolchain.cmake -S . -B build && $(MAKE) -C build subsdk9_meta

clean:
	rm -r build || true

send:
	cmake --toolchain=cmake/toolchain.cmake -DFTP_IP=$(FTP_IP) -S . -B build && $(MAKE) -C build subsdk9_meta

config:
	cmake --toolchain=cmake/toolchain.cmake -DFTP_IP=$(FTP_IP) -DLOGGER_IP=$(LOGGER_IP) -S . -B build

clean:
	rm -r build || true

log: all
	python3.8 scripts/tcpServer.py 0.0.0.0
