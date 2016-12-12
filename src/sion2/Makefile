# Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

OUT	= ../../out/sion2
RUN68	= ../../third_party/run68/src
MOD68	= ../../mod
XFILE	= ../../www/sion2/sion2.x@sion2.x
SION2	= .
CC	= emcc
DEFS	= -DFNC_TRACE -DENV_FROM_INI
CFLAGS	= $(DEFS) -Oz -include $(MOD68)/preinc.h -I $(RUN68)
EXPORTS	= -s EXPORTED_FUNCTIONS="['_main', '_set_slow_mode']"
LDFLAGS	= -lm -Oz --js-library runtime68.js --preload-file $(XFILE) $(EXPORTS)
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
	$(RUN68)/load.c \
	$(MOD68)/line4.c \
	$(MOD68)/linef.c \
	$(MOD68)/mem.c \
	$(MOD68)/run68.c \
	$(SION2)/doscall.c \
	$(SION2)/iocscall.c \
	$(SION2)/magic2call.c \
	$(SION2)/memop.c \
	$(SION2)/zmusiccall.c

OBJS	= $(addprefix $(OUT)/, $(notdir $(SRCS:.c=.o)))

$(OUT)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

$(OUT)/%.o: $(MOD68)/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(OUT)/%.o: $(RUN68)/%.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: all clean
all: $(OUT) $(OUT)/sion2.html

clean:
	rm -rf $(OUT)

$(OUT)/sion2.html: $(OBJS) runtime68.js
	$(CC) -o $@ $(LDFLAGS) $(OBJS)

$(OUT):
	mkdir -p $(OUT)
