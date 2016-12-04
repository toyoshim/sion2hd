# Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

OUT	= www
OBJS	= out/sion2/sion2.js out/sion2/sion2.data out/sion2/sion2.html.mem \
	  out/zmusic/zmusic.js out/zmusic/zmusic.data out/zmusic/zmusic.html.mem

TARGET	= $(addprefix $(OUT)/, $(notdir $(OBJS)))

.PHONY: all clean
all:
	make -C src/sion2
	make -C src/zmusic
	cp $(OBJS) $(OUT)/

clean:
	make -C src/sion2 clean
	make -C src/zmusic clean
	rm -rf out $(TARGET)
