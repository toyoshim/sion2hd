// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* MOD BEGIN */
#include <string.h>
#include <ctype.h>

#define _flushall()	fflush(NULL)
#define _getch		getchar
#define _getche()	putchar(getchar())
#define _fcvt		fcvt
#define _gcvt		gcvt
#define _stricmp	strcasecmp
#define _strlwr(p)	{ char *s; for (s = p; *s; s++) *s = tolower(*s); }

int	magic2_call();
int	zmusic_call();
/* MOD END */
