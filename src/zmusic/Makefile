# Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

OUT	= ../../out/zmusic
RUN68	= ../../third_party/run68/src
OPM	= ../../third_party/YM2151
MOD68	= ../../mod
SION2	= ../../www/sion2
ZMSC2	= .
CC	= emcc
DEFS	= -DFNC_TRACE -DENV_FROM_INI -DEMSCRIPTEN_KEEPR
CFLAGS	= $(DEFS) -O2 -include $(MOD68)/preinc.h -I $(RUN68) -I $(OPM)
ZMFUNCS	= '_zmusic_init', '_zmusic_update', '_zmusic_trap'
EXPORTS	= -s EXPORTED_FUNCTIONS="['_main', $(ZMFUNCS)]"
LDFLAGS	= -lm -O2 --js-library runtime68.js --preload-file $(SION2) $(EXPORTS)
SRCS	= \
	$(OPM)/ym2151.c \
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
	$(ZMSC2)/doscall.c \
	$(ZMSC2)/iocscall.c \
	$(ZMSC2)/memop.c \
	$(ZMSC2)/zmusic.c

OBJS	= $(addprefix $(OUT)/, $(notdir $(SRCS:.c=.o)))

$(OUT)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

$(OUT)/%.o: $(OPM)/%.c
	$(CC) $(CFLAGS) -o $@ $< -include compat.h

$(OUT)/%.o: $(MOD68)/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(OUT)/%.o: $(RUN68)/%.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: all clean
all: $(OUT) $(OUT)/zmusic.html

clean:
	rm -rf $(OUT)

$(OUT)/zmusic.html: $(OBJS) runtime68.js
	$(CC) -o $@ $(LDFLAGS) $(OBJS)

$(OUT):
	mkdir -p $(OUT)
