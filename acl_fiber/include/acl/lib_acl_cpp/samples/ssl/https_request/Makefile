PROG = https_request
util_path = ../..
base_path = ../../..
include ../../Makefile.in
#Path for SunOS
ifeq ($(findstring SunOS, $(UNIXNAME)), SunOS)
	EXTLIBS = -liconv
endif
ifeq ($(findstring FreeBSD, $(UNIXNAME)), FreeBSD)
	EXTLIBS = -L/usr/local/lib -liconv
endif
ifeq ($(findstring Darwin, $(UNIXNAME)), Darwin)
	EXTLIBS += -L/usr/lib -liconv
endif

CFLAGS += -I../..
EXTLIBS += -L/usr/local/lib -lssl -lcrypto -lz -ldl
