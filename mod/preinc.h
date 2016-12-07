/* MOD BEGIN */
// Copyright 2016 Takashi Toyoshima <toyoshim@gmail.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define _flushall()	fflush(NULL)
#define _getch		getchar
#define _getche()	putchar(getchar())
#define _fcvt		fcvt
#define _gcvt		gcvt
#define _stricmp	strcasecmp
#define _strlwr(p)	{ char *s; for (s = p; *s; s++) *s = tolower(*s); }

extern char *fcvt(double number, int ndigits, int *decpt, int *sign);
extern char *gcvt(double number, int ndigits, char *buf);

int	magic2_call();
int	pcm8_call();
int	zmusic_call();

#ifdef __cplusplus
}
#endif  // __cplusplus

/* MOD END */
