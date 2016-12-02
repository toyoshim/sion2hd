# Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

OUT	= www
OBJS	= out/sion2/sion2.js out/sion2/sion2.data out/sion2/sion2.html.mem

TARGET	= $(addprefix $(OUT)/, $(notdir $(OBJS)))

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJS)
	cp $(OBJS) $(OUT)/

$(OBJS):
	make -C src/sion2

clean:
	make -C src/sion2 clean
	rm -rf out $(TARGET)
