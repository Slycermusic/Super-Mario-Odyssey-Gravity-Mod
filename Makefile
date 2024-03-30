LOGGER_IP ?= "192.168.226.136" # put log server IP in string
FTP_IP ?= 192.168.226.248 # put console IP here
FTP_USER ?= "crafty"
FTP_PASS ?= "boss"
.PHONY: all clean config

all:
	cmake --toolchain=cmake/toolchain.cmake -S . -B build && $(MAKE) -C build subsdk9_meta

clean:
	rm -r build || true

send:
	cmake --toolchain=cmake/toolchain.cmake -DFTP_IP=$(FTP_IP) -S . -B build && $(MAKE) -C build subsdk9_meta

config:
	cmake --toolchain=cmake/toolchain.cmake -DFTP_IP=$(FTP_IP) -DFTP_USER=$(FTP_USER) -DFTP_PASS=$(FTP_PASS) -DLOGGER_IP=$(LOGGER_IP) -S . -B build

clean:
	rm -r build || true

log: all
	python3.8 scripts/tcpServer.py 0.0.0.0

ida:
	box64 wine64 /root/IDA-Pro-7.7/ida64.exe build/subsdk9
