# Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

OUT	= ../../out/sion2
RUN68	= ../../third_party/run68/src
MOD68	= ../../mod
SION2	= .
CC	= emcc
DEFS	= -DFNC_TRACE -DENV_FROM_INI
CFLAGS	= $(DEFS) -O2 -include $(MOD68)/preinc.h -I $(RUN68)
LDFLAGS	= -lm -O2 --js-library runtime68.js --preload-file ../../sion2
SRCS	= \
	$(RUN68)/ansicolor-w32.c \
	$(RUN68)/calc.c \
	$(RUN68)/conditions.c \
	$(RUN68)/disassemble.c \
	$(RUN68)/eaaccess.c \
	$(RUN68)/exec.c \
	$(RUN68)/getini.c \
	$(RUN68)/key.c \
	$(RUN68)/line0.c \
	$(RUN68)/line2.c \
	$(RUN68)/line5.c \
	$(RUN68)/line6.c \
	$(RUN68)/line7.c \
	$(RUN68)/line8.c \
	$(RUN68)/line9.c \
	$(RUN68)/lineb.c \
	$(RUN68)/linec.c \
	$(RUN68)/lined.c \
	$(RUN68)/linee.c \
	$(MOD68)/line4.c \
	$(MOD68)/linef.c \
	$(MOD68)/load.c \
	$(MOD68)/mem.c \
	$(MOD68)/run68.c \
	$(SION2)/doscall.c \
	$(SION2)/iocscall.c \
	$(SION2)/magic2call.c \
	$(SION2)/memop.c

OBJS	= $(addprefix $(OUT)/, $(notdir $(SRCS:.c=.o)))

$(OUT)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

$(OUT)/%.o: $(MOD68)/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(OUT)/%.o: $(RUN68)/%.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: all clean
all: $(OUT) $(OUT)/run68.html

clean:
	rm -rf $(OUT)

$(OUT)/run68.html: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^

$(OUT):
	mkdir -p $(OUT)
