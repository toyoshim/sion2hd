/* $Id: linef.c,v 1.3 2009-08-08 06:49:44 masamic Exp $ */

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2009/08/05 14:44:33  masamic
 * Some Bug fix, and implemented some instruction
 * Following Modification contributed by TRAP.
 *
 * Fixed Bug: In disassemble.c, shift/rotate as{lr},ls{lr},ro{lr} alway show word size.
 * Modify: enable KEYSNS, register behaiviour of sub ea, Dn.
 * Add: Nbcd, Sbcd.
 *
 * Revision 1.1.1.1  2001/05/23 11:22:08  masamic
 * First imported source code and docs
 *
 * Revision 1.4  1999/12/07  12:46:15  yfujii
 * *** empty log message ***
 *
 * Revision 1.4  1999/10/22  04:02:00  yfujii
 * 'ltoa()'s are replaced by '_ltoa()'.
 *
 * Revision 1.3  1999/10/20  04:14:48  masamichi
 * Added showing more information about errors.
 *
 * Revision 1.2  1999/10/18  03:24:40  yfujii
 * Added RCS keywords and modified for WIN32 a little.
 *
 */

#undef	MAIN

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include "run68.h"

typedef union {
	double	dbl;
	UChar	c [ 8 ];
} DBL;

typedef union {
	float	flt;
	UChar	c [ 4 ];
} FLT;

static	int	fefunc( UChar );
static	long	Lmul( long, long );
static	long	Ldiv( long, long );
static	long	Lmod( long, long );
static	unsigned long	Umul( unsigned long, unsigned long );
static	unsigned long	Udiv( unsigned long, unsigned long );
static	unsigned long	Umod( unsigned long, unsigned long );
static	long	Dtol( long, long );
static	long	Ltof( long );
static	long	Ftol( long );
static	void	Ftod( long );
static	long	Stol( long );
static	void	Stod( long );
static	void	Ltod( long );
static	void	Dtos( long, long, long );
static	void	Ltos( long, long );
static	void	Htos( long, long );
static	void	Otos( long, long );
static	void	Btos( long, long );
static	void	Val( long );
static	void	Iusing( long, long, long );
static	void	Using( long, long, long, long, long, long );
static	void	Dtst( long, long );
static	void	Dcmp( long, long, long, long );
static	void	Dneg( long, long );
static	void	Dadd( long, long, long, long );
static	void	Dsub( long, long, long, long );
static	void	Dmul( long, long, long, long );
static	void	Ddiv( long, long, long, long );
static	void	Dmod( long, long, long, long );
static	void	Dabs( long, long );
static	void	Dfloor( long, long );
static	void	Fcvt( long, long, long, long );
static	void	Sin( long, long );
static	void	Cos( long, long );
static	void	Tan( long, long );
static	void	Atan( long, long );
static	void	Log( long, long );
static	void	Exp( long, long );
static	void	Sqr( long, long );
static	void	Ftst( long );
static	long	Fmul( long, long );
static	long	Fdiv( long, long );
static	void	Clmul( long );
static	void	Cldiv( long );
static	void	Clmod( long );
static	void	Cumul( unsigned long );
static	void	Cudiv( unsigned long );
static	void	Cumod( unsigned long );
static	void	Cltod( long );
static	void	Cdtol( long );
static	void	Cftod( long );
static	void	Cdtof( long );
static	void	Cdadd( long );
static	void	Cdcmp( long );
static	void	Cdsub( long );
static	void	Cdmul( long );
static	void	Cddiv( long );
static	int	Strl( char *, int );
static	void	From_dbl( DBL *, int );
static	void	To_dbl( DBL *, long, long );

/*
 �@�@�\�FF���C�����߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
int	linef( char *pc_ptr )
{
	char	code;

	code = *(pc_ptr++);
	pc += 2;

	/* DOS�R�[���̏��� */
	if ( code == (char)0xFF )
/* MOD UPSTREAM BEGIN */
                // Reverts to call dos_call().
		return( dos_call( *pc_ptr ) );
/* MOD UPSTREAM END */

	/* FLOAT�R�[���̏��� */
	if ( code == (char)0xFE )
		return( fefunc( *pc_ptr ) );

/* MOD BEGIN */
        // Traps MAGIC2 calls.
	if ( code == (char)0xFD && *pc_ptr == (char)0x13 )
		return( magic2_call() );

/* MOD END */
	err68a( "����`�̂e���C�����߂����s���܂���", __FILE__, __LINE__ );
	return( TRUE );
}

/*
 �@�@�\�FFLOAT CALL�����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	fefunc( UChar code )
{
	long	adr;
	short	save_s;

	/* F�n��̃x�N�^�������������Ă��邩�ǂ������� */
	save_s = SR_S_REF();
	SR_S_ON();
	adr = mem_get( 0x2C, S_LONG );
	if ( adr != HUMAN_WORK ) {
		ra [ 7 ] -= 4;
		mem_set( ra [ 7 ], pc - 2, S_LONG );
		ra [ 7 ] -= 2;
		mem_set( ra [ 7 ], sr, S_WORD );
		pc = adr;
		return( FALSE );
	}
	if ( save_s == 0 )
		SR_S_OFF();

#ifdef	TRACE
	printf( "trace: FEFUNC   0xFE%02X PC=%06lX\n", code, pc );
#endif
	switch( code ) {
		case 0x00:
			rd [ 0 ] = Lmul( rd [ 0 ], rd [ 1 ] );
			break;
		case 0x01:
			rd [ 0 ] = Ldiv( rd [ 0 ], rd [ 1 ] );
			break;
		case 0x02:
			rd [ 0 ] = Lmod( rd [ 0 ], rd [ 1 ] );
			break;
		case 0x04:
			rd [ 0 ] = Umul( (ULong)rd [ 0 ], (ULong)rd [ 1 ] );
			break;
		case 0x05:
			rd [ 0 ] = Udiv( (ULong)rd [ 0 ], (ULong)rd [ 1 ] );
			break;
		case 0x06:
			rd [ 0 ] = Umod( (ULong)rd [ 0 ], (ULong)rd [ 1 ] );
			break;
		case 0x08:	/* _IMUL */
			rd [ 1 ] = (ULong)rd [ 0 ] * (ULong)rd [ 1 ];
			if ( rd [ 1 ] < 0 )
				rd [ 0 ] = -1;	/* �{���͏��4�o�C�g������ */
			else
				rd [ 0 ] = 0;	/* �{���͏��4�o�C�g������ */
			break;
		case 0x09:	/* _IDIV */ /* unsigned int ���Z d0..d1 d0/d1 */
			{
				ULong	d0;
				ULong	d1;

				d0 = (ULong)rd [ 0 ];
				d1 = (ULong)rd [ 1 ];

				rd [ 0 ] = Udiv( d0, d1 );
				rd [ 1 ] = Umod( d0, d1 );
			}
			break;
		case 0x0C:	/* _RANDOMIZE */
			if ( rd [ 0 ] >= -32768 && rd [ 0 ] <= 32767 )
				srand( rd [ 0 ] + 32768 );
			break;
		case 0x0D:	/* _SRAND */
			if ( rd [ 0 ] >= 0 && rd [ 0 ] <= 65535 )
				srand( rd [ 0 ] );
			break;
		case 0x0E:	/* _RAND */
			rd [ 0 ] = ( (unsigned)(rand()) % 32768 );
			break;
		case 0x10:
			rd [ 0 ] = Stol( ra [ 0 ] );
			break;
		case 0x11:
			Ltos( rd [ 0 ], ra [ 0 ] );
			break;
		case 0x13:
			Htos( rd [ 0 ], ra [ 0 ] );
			break;
		case 0x15:
			Otos( rd [ 0 ], ra [ 0 ] );
			break;
		case 0x17:
			Btos( rd [ 0 ], ra [ 0 ] );
			break;
		case 0x18:
			Iusing( rd [ 0 ], rd [ 1 ], ra [ 0 ] );
			break;
		case 0x1A:
			Ltod( rd [ 0 ] );
			break;
		case 0x1B:
			rd [ 0 ] = Dtol( rd [ 0 ], rd [ 1 ] );
			break;
		case 0x1C:
			rd [ 0 ] = Ltof( rd [ 0 ] );
			break;
		case 0x1D:
			rd [ 0 ] = Ftol( rd [ 0 ] );
			break;
		case 0x1E:
			Ftod( rd [ 0 ] );
			break;
		case 0x20:
			Val( ra [ 0 ] );
			break;
		case 0x21:
			Using( rd [ 0 ], rd [ 1 ], rd [ 2 ],
			       rd [ 3 ], rd [ 4 ], ra [ 0 ]  );
			break;
		case 0x22:
			Stod( ra [ 0 ] );
			break;
		case 0x23:
			Dtos( rd [ 0 ], rd [ 1 ], ra [ 0 ] );
			break;
		case 0x25:
			Fcvt( rd [ 0 ], rd [ 1 ], rd [ 2 ], ra [ 0 ] );
			break;
		case 0x28:
			Dtst( rd [ 0 ], rd [ 1 ] );
			break;
		case 0x29:
			Dcmp( rd [ 0 ], rd [ 1 ], rd [ 2 ], rd [ 3 ] );
			break;
		case 0x2A:
			Dneg( rd [ 0 ], rd [ 1 ]  );
			break;
		case 0x2B:
			Dadd( rd [ 0 ], rd [ 1 ], rd [ 2 ], rd [ 3 ] );
			break;
		case 0x2C:
			Dsub( rd [ 0 ], rd [ 1 ], rd [ 2 ], rd [ 3 ] );
			break;
		case 0x2D:
			Dmul( rd [ 0 ], rd [ 1 ], rd [ 2 ], rd [ 3 ] );
			break;
		case 0x2E:
			Ddiv( rd [ 0 ], rd [ 1 ], rd [ 2 ], rd [ 3 ] );
			break;
		case 0x2F:
			Dmod( rd [ 0 ], rd [ 1 ], rd [ 2 ], rd [ 3 ] );
			break;
		case 0x30:
			Dabs( rd [ 0 ], rd [ 1 ]  );
			break;
		case 0x33:
			Dfloor( rd [ 0 ], rd [ 1 ]  );
			break;
		case 0x36:
			Sin( rd [ 0 ], rd [ 1 ] );
			break;
		case 0x37:
			Cos( rd [ 0 ], rd [ 1 ] );
			break;
		case 0x38:
			Tan( rd [ 0 ], rd [ 1 ] );
			break;
		case 0x39:
			Atan( rd [ 0 ], rd [ 1 ] );
			break;
		case 0x3A:
			Log( rd [ 0 ], rd [ 1 ] );
			break;
		case 0x3B:
			Exp( rd [ 0 ], rd [ 1 ]  );
			break;
		case 0x3C:
			Sqr( rd [ 0 ], rd [ 1 ] );
			break;
		case 0x40:	/* _RND */
			rd [ 0 ] = rand() * rand() * 4;
			rd [ 1 ] = rand() * rand() * 4;
			break;
		case 0x58:
			Ftst( rd [ 0 ] );
			break;
		case 0x5D:
			rd [ 0 ] = Fmul( rd [ 0 ], rd [ 1 ] );
			break;
		case 0x5E:
			rd [ 0 ] = Fdiv( rd [ 0 ], rd [ 1 ] );
			break;
/*
		case 0x6C:
			rd [ 0 ] = Fsqr( rd [ 0 ] );
			break;
*/
		case 0xE0:		/* __CLMUL : signed int ��Z */
			Clmul( ra [ 7 ] );
			break;
		case 0xE1:		/* __CLDIV : signed int ���Z */
			Cldiv( ra [ 7 ] );
			break;
		case 0xE2:		/* __CLMOD : signed int ���Z�̏�] */
			Clmod( ra [ 7 ] );
			break;
		case 0xE3:		/* __CUMUL : unsigned int ��Z */
			Cumul( ra [ 7 ] );
			break;
		case 0xE4:		/* __CUDIV : unsigned int ���Z */
			Cudiv( ra [ 7 ] );
			break;
		case 0xE5:		/* __CUMOD : unsigned int ���Z�̏�] */
			Cumod( ra [ 7 ] );
			break;
		case 0xE6:
			Cltod( ra [ 7 ] );
			break;
		case 0xE7:
			Cdtol( ra [ 7 ] );
			break;
		case 0xEA:
			Cftod( ra [ 7 ] );
			break;
		case 0xEB:
			Cdtof( ra [ 7 ] );
			break;
		case 0xEC:
			Cdcmp( ra [ 7 ] );
			break;
		case 0xED:
			Cdadd( ra [ 7 ] );
			break;
		case 0xEE:
			Cdsub( ra [ 7 ] );
			break;
		case 0xEF:
			Cdmul( ra [ 7 ] );
			break;
		case 0xF0:
			Cddiv( ra [ 7 ] );
			break;
		default:
			printf( "0x%X\n", code );
			err68a( "���o�^��FE�t�@���N�V�����R�[�������s���܂���", __FILE__, __LINE__ );
			return( TRUE );
	}

	return( FALSE );
}

/*
 �@�@�\�FFEFUNC _LMUL�����s����(�G���[�͖��T�|�[�g)
 �߂�l�F���Z����
*/
static	long	Lmul( long d0, long d1 )
{
	return( d0 * d1 );
}

/*
 �@�@�\�FFEFUNC _LDIV�����s����
 �߂�l�F���Z����
*/
static	long	Ldiv( long d0, long d1 )
{
	if ( d1 == 0 ) {
		CCR_C_ON();
		return( 0 );
	}

	CCR_C_OFF();
	return( d0 / d1 );
}

/*
 �@�@�\�FFEFUNC _LMOD�����s����
 �߂�l�F���Z����
*/
static	long	Lmod( long d0, long d1 )
{
	if ( d1 == 0 ) {
		CCR_C_ON();
		return( 0 );
	}

	CCR_C_OFF();
	return( d0 % d1 );
}

/*
 �@�@�\�FFEFUNC _UMUL�����s����(�G���[�͖��T�|�[�g)
 �߂�l�F���Z����
*/
static	unsigned long	Umul( unsigned long d0, unsigned long d1 )
{
	return( d0 * d1 );
}

/*
 �@�@�\�FFEFUNC _UDIV�����s����
 �߂�l�F���Z����
*/
static	unsigned long	Udiv( unsigned long d0, unsigned long d1 )
{
	if ( d1 == 0 ) {
		CCR_C_ON();
		return( 0 );
	}

	CCR_C_OFF();
	return( d0 / d1 );
}

/*
 �@�@�\�FFEFUNC _UMOD�����s����
 �߂�l�F���Z����
*/
static	unsigned long	Umod( unsigned long d0, unsigned long d1 )
{
	if ( d1 == 0 ) {
		CCR_C_ON();
		return( 0 );
	}

	CCR_C_OFF();
	return( d0 % d1 );
}

/*
 �@�@�\�FFEFUNC _DTOL�����s����(�G���[�͖��T�|�[�g)
 �߂�l�F�ϊ����ꂽ����
*/
static	long	Dtol( long d0, long d1 )
{
	DBL	arg1;

	To_dbl( &arg1, d0, d1 );

	return( (long)arg1.dbl );
}

/*
 �@�@�\�FFEFUNC _LTOF�����s����
 �߂�l�F�Ȃ�
*/
static	long	Ltof( long d0 )
{
	FLT	fl;

	fl.flt = (float)d0;

	d0  = (fl.c [ 3 ] << 24);
	d0 |= (fl.c [ 2 ] << 16);
	d0 |= (fl.c [ 1 ] << 8);
	d0 |= fl.c [ 0 ];

	return( d0 );
}

/*
 �@�@�\�FFEFUNC _FTOL�����s����(�G���[�͖��T�|�[�g)
 �߂�l�F�ϊ����ꂽ����
*/
static	long	Ftol( long d0 )
{
	FLT	fl;

	fl.c [ 0 ] = ( d0 & 0xFF );
	fl.c [ 1 ] = ( (d0 >>  8) & 0xFF );
	fl.c [ 2 ] = ( (d0 >> 16) & 0xFF );
	fl.c [ 3 ] = ( (d0 >> 24) & 0xFF );

	return( (long)fl.flt );
}

/*
 �@�@�\�FFEFUNC _FTOD�����s����
 �߂�l�F�Ȃ�
*/
static	void	Ftod( long d0 )
{
	DBL	ret;
	FLT	arg;

	arg.c [ 0 ] = ( d0 & 0xFF );
	arg.c [ 1 ] = ( (d0 >>  8) & 0xFF );
	arg.c [ 2 ] = ( (d0 >> 16) & 0xFF );
	arg.c [ 3 ] = ( (d0 >> 24) & 0xFF );

	ret.dbl = arg.flt;

	From_dbl( &ret, 0 );
}

/*
 �@�@�\�FFEFUNC _STOL�����s����
 �߂�l�F�ϊ����ꂽ����
*/
static	long	Stol( long adr )
{
	char	*p;
	long	ret;

	p = prog_ptr + adr;
	errno = 0;
	ret = strtol( p, NULL, 10 );
	if ( ret == 0 ) {
		if ( errno == EINVAL ) {
			CCR_C_ON();
			CCR_N_ON();
			CCR_V_OFF();
		} else {
			CCR_C_OFF();
			ra [ 0 ] += Strl( p, 10 );
		}
	} else {
		if ( errno == ERANGE ) {
			CCR_C_ON();
			CCR_N_OFF();
			CCR_V_ON();
		} else {
			CCR_C_OFF();
			ra [ 0 ] += Strl( p, 10 );
		}
	}
	return( ret );
}

/*
 �@�@�\�FFEFUNC _STOD�����s����
 �߂�l�F�Ȃ�
*/
static	void	Stod( long adr )
{
	char	*p;
	DBL	ret;

	p = prog_ptr + adr;
	errno = 0;
	ret.dbl = atof( p );
	if ( errno == ERANGE ) {
		CCR_C_ON();
		CCR_N_OFF();
		CCR_V_ON();
	} else {
		CCR_C_OFF();
		ra [ 0 ] += Strl( p, 10 );
	}

	From_dbl( &ret, 0 );

	if ( ret.dbl == (long)ret.dbl ) {
		rd [ 2 ] |= 0xFFFF;
		rd [ 3 ] = (long)ret.dbl;
	} else {
		rd [ 2 ] &= 0xFFFF0000;
	}
}

/*
 �@�@�\�FFEFUNC _LTOD�����s����
 �߂�l�F�Ȃ�
*/
static	void	Ltod( long num )
{
	DBL	arg1;

	arg1.dbl = num;

	From_dbl( &arg1, 0 );
}

/*
 �@�@�\�FFEFUNC _DTOS�����s����
 �߂�l�F�Ȃ�
*/
static	void	Dtos( long d0, long d1, long a0 )
{
	DBL	arg1;
	char	*p;
	int	len;

	To_dbl( &arg1, d0, d1 );

	p = prog_ptr + a0;
	_gcvt( arg1.dbl, 14, p);
	len = strlen( p );
	if ( p [ len - 1 ] == '.' )
		p [ len - 1 ] = '\0';
	ra [ 0 ] += strlen( p );
}

/*
 �@�@�\�FFEFUNC _LTOS�����s����
 �߂�l�F�Ȃ�
*/
static	void	Ltos( long num, long adr )
{
	char	*p;

	p = prog_ptr + adr;
	_ltoa( num, p, 10 );
	/*sprintf( p, "%d", num );*/
	ra [ 0 ] += strlen( p );
}

/*
 �@�@�\�FFEFUNC _HTOS�����s����
 �߂�l�F�Ȃ�
*/
static	void	Htos( long num, long adr )
{
	char	*p;

	p = prog_ptr + adr;
	_ltoa( num, p, 16 );
	/*sprintf( p, "%X", num );*/
	ra [ 0 ] += strlen( p );
}

/*
 �@�@�\�FFEFUNC _OTOS�����s����
 �߂�l�F�Ȃ�
*/
static	void	Otos( long num, long adr )
{
	char	*p;

	p = prog_ptr + adr;
	_ltoa( num, p, 8 );
	/*sprintf( p, "%o", num );*/
	ra [ 0 ] += strlen( p );
}

/*
 �@�@�\�FFEFUNC _BTOS�����s����
 �߂�l�F�Ȃ�
*/
static	void	Btos( long num, long adr )
{
	char	*p;

	p = prog_ptr + adr;
	_ltoa( num, p, 2 );
	ra [ 0 ] += strlen( p );
}

/*
 �@�@�\�FFEFUNC _VAL�����s����
 �߂�l�F�Ȃ�
*/
static	void	Val( long str )
{
	char	buf [ 128 ];
	DBL	ret;
	char	*p;
	long	tmp;
	int	base = 10;
	char	c;

	p = prog_ptr + str;
	if ( p [ 0 ] == '&' ) {
		c = toupper( p [ 1 ] );
		if ( c == 'H' )
			base = 16;
		else if ( c == 'O' )
			base = 8;
		else if ( c == 'B' )
			base = 2;
	}
	if ( base != 10 ) {
		tmp = strtol( p + 2, NULL, base );
		_ltoa( tmp, buf, 10 );
		p = buf;
	}

	errno = 0;
	ret.dbl = atof( p );
	if ( errno == ERANGE ) {
		CCR_C_ON();
		CCR_N_OFF();
		CCR_V_ON();
	} else {
		CCR_C_OFF();
		if ( base != 10 )
			ra [ 0 ] += 2 + Strl( p + 2, base );
		else
			ra [ 0 ] += Strl( p, 10 );
	}

	From_dbl( &ret, 0 );

	if ( base == 10 && ret.dbl == (long)ret.dbl ) {
		rd [ 2 ] |= 0xFFFF;
		rd [ 3 ] = (long)ret.dbl;
	} else {
		rd [ 2 ] &= 0xFFFF0000;
	}
}

/*
 �@�@�\�FFEFUNC _IUSING�����s����
 �߂�l�F�Ȃ�
*/
static	void	Iusing( long num, long keta, long adr )
{
	char	form1 [] = { "%1d" };
	char	form2 [] = { "%10d" };
	char	*p;

	p = prog_ptr + adr;
	keta = (ULong)keta % 100;
	if ( keta == 0 )
		return;

	if ( keta < 10 ) {
		form1 [ 1 ] = keta + '0';
		sprintf( p, form1, num );
	} else {
		form2 [ 1 ] = keta / 10 + '0';
		form2 [ 2 ] = keta % 10 + '0';
		sprintf( p, form2, num );
	}
	ra [ 0 ] += strlen( p );
}

/*
 �@�@�\�FFEFUNC _USING�����s����(�A�g���r���[�g�ꕔ���Ή�)
 �߂�l�F�Ȃ�
*/
static	void	Using( long d0, long d1, long isz, long dsz, long atr, long a0 )
{
	char	str [ 128 ];
	DBL	arg1;
	char	form1 [] = { "%1.1f" };
	char	form2 [] = { "%1.10f" };
	char	form3 [] = { "%10.1f" };
	char	form4 [] = { "%10.10f" };
	char	*p;
	char	*p2;

	To_dbl( &arg1, d0, d1 );

	isz = (ULong)isz % 100;
	if ( isz == 0 )
		return;

	if ( dsz <= 0 ) {
		dsz = 0;
	} else {
		dsz = (ULong)dsz % 100;
		isz += dsz + 1;	/* 1 = strlen( "." ) */
		isz = (ULong)isz % 100;
		if ( isz == 0 )
			return;
	}

	p = prog_ptr + a0;
	if ( isz < 10 ) {
		if ( dsz < 10 ) {
			form1 [ 1 ] = isz + '0';
			form1 [ 3 ] = dsz + '0';
			sprintf( p, form1, arg1.dbl );
		} else {
			form2 [ 1 ] = isz + '0';
			form2 [ 3 ] = dsz / 10 + '0';
			form2 [ 4 ] = dsz % 10 + '0';
			sprintf( p, form2, arg1.dbl );
		}
	} else {
		if ( dsz < 10 ) {
			form3 [ 1 ] = isz / 10 + '0';
			form3 [ 2 ] = isz % 10 + '0';
			form3 [ 4 ] = dsz + '0';
			sprintf( p, form3, arg1.dbl );
		} else {
			form4 [ 1 ] = isz / 10 + '0';
			form4 [ 2 ] = isz % 10 + '0';
			form4 [ 4 ] = dsz / 10 + '0';
			form4 [ 5 ] = dsz % 10 + '0';
			sprintf( p, form4, arg1.dbl );
		}
	}

	/* bit5or6�������Ă�����'-'����� */
	if ( (atr & 0x60) != 0 && arg1.dbl < 0 ) {
		if ( *p == '-' && (long)strlen( p ) > isz ) {
			strcpy( str, p + 1 );
			strcpy( p, str );
		} else {
			p2 = p;
			while( *p2 == ' ' )
				p2 ++;
			if ( *p2 == '-' )	/* �O�̂��� */
				*p2 = ' ';
		}
	}

	/* '\'��擪�ɕt�� */
	if ( (atr & 0x02) != 0) {
		p2 = p;
		str [ 0 ] = '\0';
		while( *p2 == ' ' ) {
			if ( p2 != p )
				strcat( str, " " );
			p2 ++;
		}
		if ( *p2 == '-' ) {
			strcat( str, "-" );
			*p2 = '\\';
		} else {
			strcat( str, "\\" );
		}
		strcat( str, p2 );
		strcpy( p, str );
	}

	/* ���̏ꍇ'+'��擪�ɕt�� */
	if ( (atr & 0x10) != 0 && arg1.dbl >= 0) {
		strcpy( str, "+" );
		strcat( str, p );
		strcpy( p, str );
	}

	/* �����𖖔��ɕt�� */
	if ( (atr & 0x20) != 0 ) {
		if ( arg1.dbl < 0 )
			strcat( p, "-" );
		else
			strcat( p, "+" );
	}

	/* ���̏ꍇ'-'���A���̏ꍇ�X�y�[�X�𖖔��ɕt�� */
	if ( (atr & 0x40) != 0 ) {
		if ( arg1.dbl < 0 )
			strcat( p, "-" );
		else
			strcat( p, " " );
	}

	ra [ 0 ] += strlen( p );
}

/*
 �@�@�\�FFEFUNC _DTST�����s����
 �߂�l�F�Ȃ�
*/
static	void	Dtst( long d0, long d1 )
{
	DBL	arg;

	To_dbl( &arg, d0, d1 );

	if ( arg.dbl == 0 ) {
		CCR_Z_ON();
		CCR_N_OFF();
	}
	else if ( arg.dbl < 0 ) {
		CCR_Z_OFF();
		CCR_N_ON();
	}
	else {
		CCR_Z_OFF();
		CCR_N_OFF();
	}
}

/*
 �@�@�\�FFEFUNC _DCMP�����s����
 �߂�l�F�Ȃ�
*/
static	void	Dcmp( long d0, long d1, long d2, long d3 )
{
	DBL	arg1;
	DBL	arg2;

	To_dbl( &arg1, d0, d1 );
	To_dbl( &arg2, d2, d3 );

	arg1.dbl = arg1.dbl - arg2.dbl;

	if ( arg1.dbl < 0 ) {
		CCR_C_ON();
		CCR_Z_OFF();
		CCR_N_ON();
	}
	else if ( arg1.dbl > 0 ) {
		CCR_C_OFF();
		CCR_Z_OFF();
		CCR_N_OFF();
	}
	else {
		CCR_C_OFF();
		CCR_Z_ON();
		CCR_N_OFF();
	}
}

/*
 �@�@�\�FFEFUNC _DNEG�����s����
 �߂�l�F�Ȃ�
*/
static	void	Dneg( long d0, long d1 )
{
	DBL	arg1;

	To_dbl( &arg1, d0, d1 );

	arg1.dbl = -arg1.dbl;

	From_dbl( &arg1, 0 );
}

/*
 �@�@�\�FFEFUNC _DADD�����s����
 �߂�l�F�Ȃ�
*/
static	void	Dadd( long d0, long d1, long d2, long d3 )
{
	DBL	arg1;
	DBL	arg2;

	To_dbl( &arg1, d0, d1 );
	To_dbl( &arg2, d2, d3 );

	CCR_C_OFF();
	arg1.dbl = arg1.dbl + arg2.dbl;

	From_dbl( &arg1, 0 );
}

/*
 �@�@�\�FFEFUNC _DSUB�����s����
 �߂�l�F�Ȃ�
*/
static	void	Dsub( long d0, long d1, long d2, long d3 )
{
	DBL	arg1;
	DBL	arg2;

	To_dbl( &arg1, d0, d1 );
	To_dbl( &arg2, d2, d3 );

	CCR_C_OFF();
	arg1.dbl = arg1.dbl - arg2.dbl;

	From_dbl( &arg1, 0 );
}

/*
 �@�@�\�FFEFUNC _DMUL�����s����
 �߂�l�F�Ȃ�
*/
static	void	Dmul( long d0, long d1, long d2, long d3 )
{
	DBL	arg1;
	DBL	arg2;

	To_dbl( &arg1, d0, d1 );
	To_dbl( &arg2, d2, d3 );

	CCR_C_OFF();
	arg1.dbl = arg1.dbl * arg2.dbl;

	From_dbl( &arg1, 0 );
}

/*
 �@�@�\�FFEFUNC _DDIV�����s����
 �߂�l�F�Ȃ�
*/
static	void	Ddiv( long d0, long d1, long d2, long d3 )
{
	DBL	arg1;
	DBL	arg2;

	To_dbl( &arg1, d0, d1 );
	To_dbl( &arg2, d2, d3 );

	if ( arg2.dbl == 0 ) {
		CCR_C_ON();
		CCR_Z_ON();
		return;
	}

	CCR_C_OFF();
	arg1.dbl = arg1.dbl / arg2.dbl;

	From_dbl( &arg1, 0 );
}

/*
 �@�@�\�FFEFUNC _DMOD�����s����
 �߂�l�F�Ȃ�
*/
static	void	Dmod( long d0, long d1, long d2, long d3 )
{
	DBL	arg1;
	DBL	arg2;

	To_dbl( &arg1, d0, d1 );
	To_dbl( &arg2, d2, d3 );

	if ( arg2.dbl == 0 ) {
		CCR_C_ON();
		CCR_Z_ON();
		return;
	}

	CCR_C_OFF();
	arg1.dbl = fmod( arg1.dbl, arg2.dbl );

	From_dbl( &arg1, 0 );
}

/*
 �@�@�\�FFEFUNC _DABS�����s����
 �߂�l�F�Ȃ�
*/
static	void	Dabs( long d0, long d1 )
{
	DBL	arg1;

	To_dbl( &arg1, d0, d1 );

	arg1.dbl = fabs( arg1.dbl );

	From_dbl( &arg1, 0 );
}

/*
 �@�@�\�FFEFUNC _DFLOOR�����s����
 �߂�l�F�Ȃ�
*/
static	void	Dfloor( long d0, long d1 )
{
	DBL	arg1;

	To_dbl( &arg1, d0, d1 );

	arg1.dbl = floor( arg1.dbl );

	From_dbl( &arg1, 0 );
}

/*
 �@�@�\�FFEFUNC _FCVT�����s����
 �߂�l�F�Ȃ�
*/
static	void	Fcvt( long d0, long d1, long keta, long adr )
{
	DBL	arg;
	char	*p;
	int	loc;
	int	sign;

	To_dbl( &arg, d0, d1 );
	p = prog_ptr + adr;
	keta &= 0xFF;

	strcpy( p, (char *)_fcvt( arg.dbl, keta, &loc, &sign ) );

	rd [ 0 ] = loc;
	if ( sign == 0 )
		rd [ 1 ] = 0;
	else
		rd [ 1 ] = 1;
}

/*
 �@�@�\�FFEFUNC _SIN�����s����
 �߂�l�F�Ȃ�
*/
static	void	Sin( long d0, long d1 )
{
	DBL	arg;
	DBL	ans;

	To_dbl( &arg, d0, d1 );

	ans.dbl = sin( arg.dbl );

	From_dbl( &ans, 0 );
}

/*
 �@�@�\�FFEFUNC _COS�����s����
 �߂�l�F�Ȃ�
*/
static	void	Cos( long d0, long d1 )
{
	DBL	arg;
	DBL	ans;

	To_dbl( &arg, d0, d1 );

	ans.dbl = cos( arg.dbl );

	From_dbl( &ans, 0 );
}

/*
 �@�@�\�FFEFUNC _TAN�����s����
 �߂�l�F�Ȃ�
*/
static	void	Tan( long d0, long d1 )
{
	DBL	arg;
	DBL	ans;

	To_dbl( &arg, d0, d1 );

	ans.dbl = tan( arg.dbl );
	CCR_C_OFF();

	From_dbl( &ans, 0 );
}

/*
 �@�@�\�FFEFUNC _ATAN�����s����
 �߂�l�F�Ȃ�
*/
static	void	Atan( long d0, long d1 )
{
	DBL	arg;
	DBL	ans;

	To_dbl( &arg, d0, d1 );

	ans.dbl = atan( arg.dbl );

	From_dbl( &ans, 0 );
}

/*
 �@�@�\�FFEFUNC _LOG�����s����
 �߂�l�F�Ȃ�
*/
static	void	Log( long d0, long d1 )
{
	DBL	arg;
	DBL	ans;

	To_dbl( &arg, d0, d1 );

	if ( ans.dbl == 0 ) {
		CCR_C_ON();
		CCR_Z_ON();
		return;
	}

	ans.dbl = log( arg.dbl );
	CCR_C_OFF();

	From_dbl( &ans, 0 );
}

/*
 �@�@�\�FFEFUNC _EXP�����s����
 �߂�l�F�Ȃ�
*/
static	void	Exp( long d0, long d1 )
{
	DBL	arg;
	DBL	ans;

	To_dbl( &arg, d0, d1 );

	if ( arg.dbl > 709.782712893 ) {
		CCR_C_ON();
		CCR_Z_OFF();
		CCR_V_ON();
		return;
	}

	errno = 0;
	ans.dbl = exp( arg.dbl );
	CCR_C_OFF();

	From_dbl( &ans, 0 );
}

/*
 �@�@�\�FFEFUNC _SQR�����s����
 �߂�l�F�Ȃ�
*/
static	void	Sqr( long d0, long d1 )
{
	DBL	arg;
	DBL	ans;

	To_dbl( &arg, d0, d1 );

	if ( arg.dbl < 0 ) {
		CCR_C_ON();
		return;
	}
	ans.dbl = sqrt( arg.dbl );
	CCR_C_OFF();

	From_dbl( &ans, 0 );
}

/*
 �@�@�\�FFEFUNC _FTST�����s����
 �߂�l�F�Ȃ�
*/
static	void	Ftst( long d0 )
{
	FLT	arg;

	arg.c [ 0 ] = ( d0 & 0xFF );
	arg.c [ 1 ] = ( (d0 >>  8) & 0xFF );
	arg.c [ 2 ] = ( (d0 >> 16) & 0xFF );
	arg.c [ 3 ] = ( (d0 >> 24) & 0xFF );

	if ( arg.flt == 0 ) {
		CCR_Z_ON();
		CCR_N_OFF();
	}
	else if ( arg.flt < 0 ) {
		CCR_Z_OFF();
		CCR_N_ON();
	}
	else {
		CCR_Z_OFF();
		CCR_N_OFF();
	}
}

/*
 �@�@�\�FFEFUNC _FMUL�����s���遃�G���[�͖��T�|�[�g��
 �߂�l�F���Z����
*/
static	long	Fmul( long d0, long d1 )
{
	FLT	arg1;
	FLT	arg2;

	arg1.c [ 0 ] = ( d0 & 0xFF );
	arg1.c [ 1 ] = ( (d0 >>  8) & 0xFF );
	arg1.c [ 2 ] = ( (d0 >> 16) & 0xFF );
	arg1.c [ 3 ] = ( (d0 >> 24) & 0xFF );

	arg2.c [ 0 ] = ( d1 & 0xFF );
	arg2.c [ 1 ] = ( (d1 >>  8) & 0xFF );
	arg2.c [ 2 ] = ( (d1 >> 16) & 0xFF );
	arg2.c [ 3 ] = ( (d1 >> 24) & 0xFF );

	CCR_C_OFF();
	arg1.flt = arg1.flt * arg2.flt;

	d0  = (arg1.c [ 3 ] << 24);
	d0 |= (arg1.c [ 2 ] << 16);
	d0 |= (arg1.c [ 1 ] << 8);
	d0 |= arg1.c [ 0 ];

	return( d0 );
}

/*
 �@�@�\�FFEFUNC _FDIV�����s����
 �߂�l�F�Ȃ�
*/
static	long	Fdiv( long d0, long d1 )
{
	FLT	arg1;
	FLT	arg2;

	arg1.c [ 0 ] = ( d0 & 0xFF );
	arg1.c [ 1 ] = ( (d0 >>  8) & 0xFF );
	arg1.c [ 2 ] = ( (d0 >> 16) & 0xFF );
	arg1.c [ 3 ] = ( (d0 >> 24) & 0xFF );

	arg2.c [ 0 ] = ( d1 & 0xFF );
	arg2.c [ 1 ] = ( (d1 >>  8) & 0xFF );
	arg2.c [ 2 ] = ( (d1 >> 16) & 0xFF );
	arg2.c [ 3 ] = ( (d1 >> 24) & 0xFF );

	if ( arg2.flt == 0 ) {
		CCR_C_ON();
		CCR_Z_ON();
		return( 0 );
	}

	CCR_C_OFF();
	arg1.flt = arg1.flt / arg2.flt;

	d0  = (arg1.c [ 3 ] << 24);
	d0 |= (arg1.c [ 2 ] << 16);
	d0 |= (arg1.c [ 1 ] << 8);
	d0 |= arg1.c [ 0 ];

	return( d0 );
}

/*
 �@�@�\�FFEFUNC _CLMUL�����s����(�G���[�͖��T�|�[�g)
 �߂�l�F�Ȃ�
*/
static	void	Clmul( long adr )
{
	long	a;
	long	b;

	a = mem_get( adr, S_LONG );
	b = mem_get( adr + 4, S_LONG );

	a = a * b;
	CCR_C_OFF();

	mem_set( adr, a, S_LONG );
}

/*
 �@�@�\�FFEFUNC _CLDIV�����s����
 �߂�l�F�Ȃ�
*/
static	void	Cldiv( long adr )
{
	long	a;
	long	b;

	a = mem_get( adr, S_LONG );
	b = mem_get( adr + 4, S_LONG );

	if ( b == 0 ) {
		CCR_C_ON();
		return;
	}

	a = a / b;
	CCR_C_OFF();

	mem_set( adr, a, S_LONG );
}

/*
 �@�@�\�FFEFUNC _CLMOD�����s����
 �߂�l�F�Ȃ�
*/
static	void	Clmod( long adr )
{
	long	a;
	long	b;

	a = mem_get( adr, S_LONG );
	b = mem_get( adr + 4, S_LONG );

	if ( b == 0 ) {
		CCR_C_ON();
		return;
	}

	a = a % b;
	CCR_C_OFF();

	mem_set( adr, a, S_LONG );
}

/*
 �@�@�\�FFEFUNC _CUMUL�����s����(�G���[�͖��T�|�[�g)
 �߂�l�F�Ȃ�
*/
static	void	Cumul( unsigned long adr )
{
	unsigned long	a;
	unsigned long	b;

	a = mem_get( adr, S_LONG );
	b = mem_get( adr + 4, S_LONG );

	a = a * b;
	CCR_C_OFF();

	mem_set( adr, a, S_LONG );
}

/*
 �@�@�\�FFEFUNC _CUDIV�����s����
 �߂�l�F�Ȃ�
*/
static	void	Cudiv( unsigned long adr )
{
	unsigned long	a;
	unsigned long	b;

	a = mem_get( adr, S_LONG );
	b = mem_get( adr + 4, S_LONG );

	if ( b == 0 ) {
		CCR_C_ON();
		return;
	}

	a = a / b;
	CCR_C_OFF();

	mem_set( adr, a, S_LONG );
}

/*
 �@�@�\�FFEFUNC _CUMOD�����s����
 �߂�l�F�Ȃ�
*/
static	void	Cumod( unsigned long adr )
{
	unsigned long	a;
	unsigned long	b;

	a = mem_get( adr, S_LONG );
	b = mem_get( adr + 4, S_LONG );

	if ( b == 0 ) {
		CCR_C_ON();
		return;
	}

	a = a % b;
	CCR_C_OFF();

	mem_set( adr, a, S_LONG );
}

/*
 �@�@�\�FFEFUNC _CLTOD�����s����
 �߂�l�F�Ȃ�
*/
static	void	Cltod( long adr )
{
	DBL	arg1;
	long	num;
	long	d0;
	long	d1;

	num = mem_get( adr, S_LONG );
	arg1.dbl = num;

	d0 = rd [ 0 ];
	d1 = rd [ 1 ];
	From_dbl( &arg1, 0 );
	mem_set( adr, rd [ 0 ], S_LONG );
	mem_set( adr + 4, rd [ 1 ], S_LONG );
	rd [ 0 ] = d0;
	rd [ 1 ] = d1;
}

/*
 �@�@�\�FFEFUNC _CDTOL�����s����(�G���[�͖��T�|�[�g)
 �߂�l�F�Ȃ�
*/
static	void	Cdtol( long adr )
{
	DBL	arg1;
	long	d0;
	long	d1;

	d0 = mem_get( adr, S_LONG );
	d1 = mem_get( adr + 4, S_LONG );
	To_dbl( &arg1, d0, d1 );

	d0 = (long)arg1.dbl;

	mem_set( adr, d0, S_LONG );
}

/*
 �@�@�\�FFEFUNC _CFTOD�����s����
 �߂�l�F�Ȃ�
*/
static	void	Cftod( long adr )
{
	DBL	db;
	FLT	fl;
	long	d0;
	long	d1;

	d0 = mem_get( adr, S_LONG );
	fl.c [ 0 ] = ( d0 & 0xFF );
	fl.c [ 1 ] = ( (d0 >>  8) & 0xFF );
	fl.c [ 2 ] = ( (d0 >> 16) & 0xFF );
	fl.c [ 3 ] = ( (d0 >> 24) & 0xFF );

	db.dbl = fl.flt;

	d0 = rd [ 0 ];
	d1 = rd [ 1 ];
	From_dbl( &db, 0 );
	mem_set( adr, rd [ 0 ], S_LONG );
	mem_set( adr + 4, rd [ 1 ], S_LONG );
	rd [ 0 ] = d0;
	rd [ 1 ] = d1;
}

/*
 �@�@�\�FFEFUNC _CDTOF�����s����
 �߂�l�F�Ȃ�
*/
static	void	Cdtof( long adr )
{
	DBL	arg;
	FLT	fl;
	long	d0;
	long	d1;

	d0 = rd [ 0 ];
	d1 = rd [ 1 ];
	rd [ 0 ] = mem_get( adr, S_LONG );
	rd [ 1 ] = mem_get( adr + 4, S_LONG );
	To_dbl( &arg, d0, d1 );
	rd [ 0 ] = d0;
	rd [ 1 ] = d1;

	fl.flt = (float)arg.dbl;
	CCR_C_OFF();

	d0  = (fl.c [ 3 ] << 24);
	d0 |= (fl.c [ 2 ] << 16);
	d0 |= (fl.c [ 1 ] << 8);
	d0 |= fl.c [ 0 ];
	mem_set( adr, d0, S_LONG );
}

/*
 �@�@�\�FFEFUNC _CDCMP�����s����
 �߂�l�F�Ȃ�
*/
static	void	Cdcmp( long adr )
{
	DBL	arg1;
	DBL	arg2;
	long	d0;
	long	d1;

	d0 = rd [ 0 ];
	d1 = rd [ 1 ];
	rd [ 0 ] = mem_get( adr, S_LONG );
	rd [ 1 ] = mem_get( adr + 4, S_LONG );
	To_dbl( &arg1, d0, d1 );
	rd [ 0 ] = mem_get( adr + 8, S_LONG );
	rd [ 1 ] = mem_get( adr + 12, S_LONG );
	To_dbl( &arg2, d0, d1 );
	rd [ 0 ] = d0;
	rd [ 1 ] = d1;

	arg1.dbl = arg1.dbl - arg2.dbl;

	if ( arg1.dbl < 0 ) {
		CCR_C_ON();
		CCR_Z_OFF();
		CCR_N_ON();
	}
	else if ( arg1.dbl > 0 ) {
		CCR_C_OFF();
		CCR_Z_OFF();
		CCR_N_OFF();
	}
	else {
		CCR_C_OFF();
		CCR_Z_ON();
		CCR_N_OFF();
	}
}

/*
 �@�@�\�FFEFUNC _CDADD�����s����
 �߂�l�F�Ȃ�
*/
static	void	Cdadd( long adr )
{
	DBL	a;
	DBL	b;
	int	i;

	for ( i = 0; i < 8; i ++ ) {
		a.c [ i ] = (unsigned char)mem_get( adr +  7 - i, S_BYTE );
		b.c [ i ] = (unsigned char)mem_get( adr + 15 - i, S_BYTE );
	}

	a.dbl = a.dbl + b.dbl;

	for ( i = 0; i < 8; i ++ )
		mem_set( adr + 7 - i, a.c [ i ], S_BYTE );
}

/*
 �@�@�\�FFEFUNC _CDSUB�����s����
 �߂�l�F�Ȃ�
*/
static	void	Cdsub( long adr )
{
	DBL	a;
	DBL	b;
	int	i;

	for ( i = 0; i < 8; i ++ ) {
		a.c [ i ] = (unsigned char)mem_get( adr +  7 - i, S_BYTE );
		b.c [ i ] = (unsigned char)mem_get( adr + 15 - i, S_BYTE );
	}

	a.dbl = a.dbl - b.dbl;

	for ( i = 0; i < 8; i ++ )
		mem_set( adr + 7 - i, a.c [ i ], S_BYTE );
}

/*
 �@�@�\�FFEFUNC _CDMUL�����s����
 �߂�l�F�Ȃ�
*/
static	void	Cdmul( long adr )
{
	DBL	a;
	DBL	b;
	int	i;

	for ( i = 0; i < 8; i ++ ) {
		a.c [ i ] = (unsigned char)mem_get( adr +  7 - i, S_BYTE );
		b.c [ i ] = (unsigned char)mem_get( adr + 15 - i, S_BYTE );
	}

	a.dbl = a.dbl * b.dbl;

	for ( i = 0; i < 8; i ++ )
		mem_set( adr + 7 - i, a.c [ i ], S_BYTE );
}

/*
 �@�@�\�FFEFUNC _CDDIV�����s����
 �߂�l�F�Ȃ�
*/
static	void	Cddiv( long adr )
{
	DBL	a;
	DBL	b;
	int	i;

	for ( i = 0; i < 8; i ++ ) {
		a.c [ i ] = (unsigned char)mem_get( adr +  7 - i, S_BYTE );
		b.c [ i ] = (unsigned char)mem_get( adr + 15 - i, S_BYTE );
	}

	if ( b.dbl == 0 ) {
		CCR_C_ON();
		CCR_Z_ON();
		return;
	}

	CCR_C_OFF();
	a.dbl = a.dbl / b.dbl;

	for ( i = 0; i < 8; i ++ )
		mem_set( adr + 7 - i, a.c [ i ], S_BYTE );
}

/*
 �@�@�\�F����������̐����ȊO�̕����܂ł̒��������߂�
 �߂�l�F����
*/
static	int	Strl( char *p, int base )
{
	int	l;

	for ( l = 0; p [ l ] == ' '; l++ )
		;
	switch( base ) {
		case 10 :
			for (; p [ l ] != '\0'; l++ ) {
				if ( p [ l ] >= '0' && p [ l ] <= '9' )
					continue;
				if ( p [ l ] == '.' )
					continue;
				break;
			}
			break;
		case 2:
			for (; p [ l ] != '\0'; l++ ) {
				if ( p [ l ] != '0' && p [ l ] != '1' )
					break;
			}
			break;
		case 8:
			for (; p [ l ] != '\0'; l++ ) {
				if ( p [ l ] < '0' || p [ l ] > '7' )
					break;
			}
			break;
		case 16:
			for (; p [ l ] != '\0'; l++ ) {
				if ( p [ l ] >= '0' && p [ l ] <= '9' )
					continue;
				if ( p [ l ] >= 'A' && p [ l ] <= 'F' )
					continue;
				break;
			}
			break;
	}
	return( l );
}

/*
 �@�@�\�F�{���x���������_�������W�X�^2�Ɉړ�����
 �߂�l�F�Ȃ�
*/
static	void	From_dbl( DBL *p, int reg )
{
	rd [ reg     ]  = (p -> c [ 7 ] << 24);
	rd [ reg     ] |= (p -> c [ 6 ] << 16);
	rd [ reg     ] |= (p -> c [ 5 ] << 8);
	rd [ reg     ] |= p -> c [ 4 ];
	rd [ reg + 1 ]  = (p -> c [ 3 ] << 24);
	rd [ reg + 1 ] |= (p -> c [ 2 ] << 16);
	rd [ reg + 1 ] |= (p -> c [ 1 ] << 8);
	rd [ reg + 1 ] |= p -> c [ 0 ];
}

/*
 �@�@�\�F4�o�C�g����2�ɓ������{���x���������_�����G���f�B�A���ϊ�����
 �߂�l�F�Ȃ�
*/
static	void	To_dbl( DBL *p, long d0, long d1 )
{
	p -> c [ 0 ] = ( d1 & 0xFF );
	p -> c [ 1 ] = ( (d1 >>  8) & 0xFF );
	p -> c [ 2 ] = ( (d1 >> 16) & 0xFF );
	p -> c [ 3 ] = ( (d1 >> 24) & 0xFF );
	p -> c [ 4 ] = ( d0 & 0xFF );
	p -> c [ 5 ] = ( (d0 >>  8) & 0xFF );
	p -> c [ 6 ] = ( (d0 >> 16) & 0xFF );
	p -> c [ 7 ] = ( (d0 >> 24) & 0xFF );
}
