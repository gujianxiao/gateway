# ndnd/dir.mk
# 
# Part of the NDNx distribution.
#
# Portions Copyright (C) 2013 Regents of the University of California.
# 
# Based on the CCNx C Library by PARC.
# Copyright (C) 2009-2013 Palo Alto Research Center, Inc.
#
# This work is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License version 2 as published by the
# Free Software Foundation.
# This work is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.
#

LDLIBS = -L$(NDNLIBDIR) $(MORE_LDLIBS) -lndn
NDNLIBDIR = ../lib

INSTALLED_PROGRAMS = ndnd ndndsmoketest 
PROGRAMS = $(INSTALLED_PROGRAMS)
DEBRIS = anything.ndnb contentobjecthash.ndnb contentmishash.ndnb \
         contenthash.ndnb

BROKEN_PROGRAMS = 
CSRC = ndnd_main.c ndnd.c ndnd_msg.c ndnd_stats.c ndnd_internal_client.c ndndsmoketest.c
HSRC = ndnd_private.h
SCRIPTSRC = testbasics fortunes.ndnb contentobjecthash.ref anything.ref \
            minsuffix.ref
 
default: $(PROGRAMS)

all: default $(BROKEN_PROGRAMS)

$(PROGRAMS): $(NDNLIBDIR)/libndn.a

NDND_OBJ = ndnd_main.o ndnd.o ndnd_msg.o ndnd_stats.o ndnd_internal_client.o
ndnd: $(NDND_OBJ) ndnd_built.sh
	$(CC) $(CFLAGS) -o $@ $(NDND_OBJ) $(LDLIBS) $(OPENSSL_LIBS) -lcrypto -lpthread
	sh ./ndnd_built.sh

ndnd_built.sh:
	touch ndnd_built.sh

ndndsmoketest: ndndsmoketest.o
	$(CC) $(CFLAGS) -o $@ ndndsmoketest.o $(LDLIBS)

clean:
	rm -f *.o *.a $(PROGRAMS) $(BROKEN_PROGRAMS) depend
	rm -rf *.dSYM $(DEBRIS)

check test: ndnd ndndsmoketest $(SCRIPTSRC)
	./testbasics
	: ---------------------- :
	:  ndnd unit tests pass  :
	: ---------------------- :

###############################
# Dependencies below here are checked by depend target
# but must be updated manually.
###############################
ndnd_main.o: ndnd_main.c ndnd_private.h define.h ../include/ndn/ndn_private.h \
  ../include/ndn/coding.h ../include/ndn/reg_mgmt.h \
  ../include/ndn/charbuf.h ../include/ndn/schedule.h \
  ../include/ndn/seqwriter.h
ndnd.o: ndnd.c ndw.h ../include/ndn/bloom.h ../include/ndn/ndn.h \
  ../include/ndn/coding.h ../include/ndn/charbuf.h \
  ../include/ndn/indexbuf.h ../include/ndn/ndn_private.h \
  ../include/ndn/ndnd.h ../include/ndn/face_mgmt.h \
  ../include/ndn/sockcreate.h ../include/ndn/hashtb.h \
  ../include/ndn/schedule.h ../include/ndn/reg_mgmt.h \
  ../include/ndn/uri.h ndnd_private.h ../include/ndn/seqwriter.h
ndnd_msg.o: ndnd_msg.c ../include/ndn/ndn.h ../include/ndn/coding.h \
  ../include/ndn/charbuf.h ../include/ndn/indexbuf.h \
  ../include/ndn/ndnd.h ../include/ndn/hashtb.h ../include/ndn/uri.h \
  ndnd_private.h ../include/ndn/ndn_private.h ../include/ndn/reg_mgmt.h \
  ../include/ndn/schedule.h ../include/ndn/seqwriter.h
ndnd_stats.o: ndnd_stats.c ../include/ndn/ndn.h ../include/ndn/coding.h \
  ../include/ndn/charbuf.h ../include/ndn/indexbuf.h \
  ../include/ndn/ndnd.h ../include/ndn/schedule.h \
  ../include/ndn/sockaddrutil.h ../include/ndn/hashtb.h \
  ../include/ndn/uri.h ndnd_private.h ../include/ndn/ndn_private.h \
  ../include/ndn/reg_mgmt.h ../include/ndn/seqwriter.h
ndnd_internal_client.o: ndnd_internal_client.c ../include/ndn/ndn.h \
  ../include/ndn/coding.h ../include/ndn/charbuf.h \
  ../include/ndn/indexbuf.h ../include/ndn/ndn_private.h \
  ../include/ndn/hashtb.h ../include/ndn/keystore.h \
  ../include/ndn/schedule.h ../include/ndn/sockaddrutil.h \
  ../include/ndn/uri.h ndnd_private.h ../include/ndn/reg_mgmt.h \
  ../include/ndn/seqwriter.h
ndndsmoketest.o: ndndsmoketest.c ../include/ndn/ndnd.h \
  ../include/ndn/ndn_private.h
