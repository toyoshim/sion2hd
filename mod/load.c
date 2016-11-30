/* $Id: load.c,v 1.2 2009-08-08 06:49:44 masamic Exp $ */

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2001/05/23 11:22:08  masamic
 * First imported source code and docs
 *
 * Revision 1.5  1999/12/24  04:04:37  yfujii
 * BUGFIX:When .x or .r is ommited and specified drive or path,
 * run68 couldn't find the executable file.
 *
 * Revision 1.4  1999/12/07  12:47:10  yfujii
 * *** empty log message ***
 *
 * Revision 1.4  1999/11/29  06:11:28  yfujii
 * *** empty log message ***
 *
 * Revision 1.3  1999/10/21  13:32:01  yfujii
 * DOS calls are replaced by win32 functions.
 *
 * Revision 1.2  1999/10/18  03:24:40  yfujii
 * Added RCS keywords and modified for WIN32 a little.
 *
 */

#undef	MAIN

#include <stdio.h>
#include <string.h>
#if defined(WIN32)
#include <direct.h>
/* MOD UPSTREAM BEGIN */
#elif !defined(DOSX)
#include <unistd.h>
#include <stdlib.h>
/* MOD UPSTREAM END */
#endif
#include "run68.h"

static	UChar	xhead [ XHEAD_SIZE ];

static	long	xfile_cnv( long *, long, int );
static	int	xrelocate( long, long, long );
static	long	xhead_getl( int );
static	int	set_fname( char *, long );

/* doscall.c */
long Getenv_common(const char *name_p, char *buf_p);

static char *GetAPath(char **path_p, char *buf);

/*
  �@�\�F
    ���s�t�@�C�����I�[�v������B���ϐ���PATH����擾�����p�X��
    ���ԂɒT�����čŏ��Ɍ��t�������t�@�C�����I�[�v������B
    �ŏ��ɃJ�����g�f�B���N�g������������B
  �����F
    char *fname     -- �t�@�C����������
    int  msg_flag   -- 0�łȂ������b�Z�[�W��W���G���[�o�͂ɏo��
  �߂�l�F
    NULL = �I�[�v���ł��Ȃ�
    !NULL = ���s�t�@�C���̃t�@�C���|�C���^
*/
FILE    *prog_open(char *fname, int mes_flag)
{
    char    dir[MAX_PATH], fullname[MAX_PATH], cwd[MAX_PATH];
    FILE    *fp = 0;
    char    *exp = strrchr(fname, '.');
    char    env_p[4096], *p;
/* MOD UPSTREAM BEGIN */
#if defined(WIN32) || defined(DOSX)
    char    sep_chr = '\\';
    char    sep_str[] = "\\";
#else
    char    sep_chr = '/';
    char    sep_str[] = "/";
#endif

    if (strchr(fname, sep_chr) != NULL || strchr(fname, ':') != NULL)
/* MOD UPSTREAM END */
    {
        strcpy(fullname, fname);
        if ((fp=fopen(fullname, "rb")) != NULL)
            goto EndOfFunc;
        // ��������ǉ�(by Yokko��)
        strcat(fullname, ".r");
        if ((fp=fopen(fullname, "rb")) != NULL)
            goto EndOfFunc;
        strcpy(fullname, fname);
        strcat(fullname, ".x");
        if ((fp=fopen(fullname, "rb")) != NULL)
            goto EndOfFunc;
        // �����܂Œǉ�(by Yokko��)
        goto ErrorRet;
    }
    if (exp != NULL && !_stricmp(exp, ".x") && !_stricmp(exp, ".r"))
        goto ErrorRet; /* �g���q���Ⴄ */
#if defined(WIN32)
    GetCurrentDirectory(sizeof(cwd), cwd);
#else
    getcwd(cwd, sizeof(cwd));
#endif
    /* PATH���ϐ����擾���� */
#if defined(WIN32)
    Getenv_common("PATH", env_p);
    p = env_p;
#else
    p = getenv("PATH");
#endif
    for (strcpy(dir, cwd); strlen(dir) != 0; GetAPath(&p, dir))
    {
        if (exp != NULL)
        {
            strcpy(fullname, dir);
/* MOD UPSTREAM BEGIN */
            if (dir[strlen(dir)-1] != sep_chr)
                strcat(fullname, sep_str);
/* MOD UPSTREAM END */
            strcat(fullname, fname);
            if ((fp = fopen(fullname, "rb")) != NULL)
	        	goto EndOfFunc;
        } else
        {
            strcpy(fullname, dir);
/* MOD UPSTREAM BEGIN */
            if (fullname[strlen(fullname)-1] != sep_chr)
                strcat(fullname, sep_str);
/* MOD UPSTREAM END */
	        strcat(fullname, fname);
	        strcat(fullname, ".r");
    	    if ((fp=fopen(fullname, "rb")) != NULL)
	        	goto EndOfFunc;
            strcpy(fullname, dir);
/* MOD UPSTREAM BEGIN */
            if (fullname[strlen(fullname)-1] != sep_chr)
                strcat(fullname, sep_str);
/* MOD UPSTREAM END */
	        strcat(fullname, fname);
            strcat(fullname, ".x");
            if ((fp=fopen(fullname, "rb")) != NULL)
	        	goto EndOfFunc;
        }
    }
EndOfFunc:
    strcpy(fname, fullname);
    return fp;
ErrorRet:
    if (mes_flag == TRUE)
        fprintf(stderr, "�t�@�C�����I�[�v���ł��܂���\n");
    return NULL;
}

static char *GetAPath(char **path_p, char *buf)
{
    unsigned int i;

    if (path_p == NULL || *path_p == NULL || strlen(*path_p) == 0)
    {
        *buf = '\0';
        goto ErrorReturn;
    }
    for (i = 0; i < strlen(*path_p) && (*path_p)[i] != ';'; i ++)
    {
        /* 2�o�C�g�R�[�h�̃X�L�b�v */
       ;
    }
    strncpy(buf, *path_p, i);
    buf[i] = '\0';
    if ((*path_p)[i] == '\0')
    {
        *path_p = &((*path_p)[i]);
    } else
    {
        *path_p += i + 1;
    }
    return buf;
ErrorReturn:
    return NULL;
}

/*
 �@�@�\�F�v���O�������������ɓǂݍ���(fp�̓N���[�Y�����)
 �߂�l�F�� = ���s�J�n�A�h���X
 �@�@�@�@�� = �G���[�R�[�h
*/
long	prog_read( FILE *fp, char *fname, long read_top,
		   long *prog_sz, long *prog_sz2, int mes_flag )
		/* prog_sz2�̓��[�h���[�h�{���~�b�g�A�h���X�̖������ʂ��� */
{
	char	*read_ptr;
	long	read_sz;
	long	pc_begin;
	int	x_flag = FALSE;
	int	loadmode;
	int	i;

	loadmode = ((*prog_sz2 >> 24) & 0x03);
	*prog_sz2 &= 0xFFFFFF;

	if ( fseek( fp, 0, SEEK_END ) != 0 ) {
		fclose( fp );
		if ( mes_flag == TRUE )
			fprintf(stderr, "�t�@�C���̃V�[�N�Ɏ��s���܂���\n");
		return( -11 );
	}
	if ( (*prog_sz=ftell( fp )) <= 0 ) {
		fclose( fp );
		if ( mes_flag == TRUE )
			fprintf(stderr, "�t�@�C���T�C�Y���O�ł�\n");
		return( -11 );
	}
	if ( fseek( fp, 0, SEEK_SET ) != 0 ) {
		fclose( fp );
		if ( mes_flag == TRUE )
			fprintf(stderr, "�t�@�C���̃V�[�N�Ɏ��s���܂���\n");
		return( -11 );
	}
	if ( read_top + *prog_sz > *prog_sz2 ) {
		fclose( fp );
		if ( mes_flag == TRUE )
			fprintf(stderr, "�t�@�C���T�C�Y���傫�����܂�\n");
		return( -8 );
	}

	read_sz  = *prog_sz;
	read_ptr = prog_ptr + read_top;
	pc_begin = read_top;

	/* XHEAD_SIZE�o�C�g�ǂݍ��� */
	if ( *prog_sz >= XHEAD_SIZE ) {
		if ( fread( read_ptr, 1, XHEAD_SIZE, fp ) != XHEAD_SIZE ) {
			fclose( fp );
			if ( mes_flag == TRUE )
				fprintf(stderr, "�t�@�C���̓ǂݍ��݂Ɏ��s���܂���\n");
			return( -11 );
		}
		read_sz -= XHEAD_SIZE;
		if ( loadmode == 1 )
			i = 0;		/* R�t�@�C�� */
		else if ( loadmode == 3 )
			i = 1;		/* X�t�@�C�� */
		else
			i = strlen( fname ) - 2;
		if ( mem_get( read_top, S_WORD ) == 0x4855 && i > 0 )
		{
			if ( loadmode == 3 ||
			     strcmp( &(fname [ i ]), ".x" ) == 0 ||
			     strcmp( &(fname [ i ]), ".X" ) == 0 ) {
				x_flag = TRUE;
				memcpy( xhead, read_ptr, XHEAD_SIZE );
				*prog_sz = read_sz;
			}
		}
		if ( x_flag == FALSE )
			read_ptr += XHEAD_SIZE;
	}

	if ( fread( read_ptr, 1, read_sz, fp ) != (size_t)read_sz ) {
		fclose( fp );
		if ( mes_flag == TRUE )
			fprintf(stderr, "�t�@�C���̓ǂݍ��݂Ɏ��s���܂���\n");
		return( -11 );
	}

	/* ���s�t�@�C���̃N���[�Y */
	fclose( fp );

	/* X�t�@�C���̏��� */
	*prog_sz2 = *prog_sz;
	if ( x_flag == TRUE ) {
		if ( (pc_begin=xfile_cnv( prog_sz, read_top, mes_flag )) == 0 )
			return( -11 );
	}

	return( pc_begin );
}

/*
 �@�@�\�FX�t�@�C�����R���o�[�g����
 �߂�l�F 0 = �G���[
 �@�@�@�@!0 = �v���O�����J�n�A�h���X
*/
static	long	xfile_cnv( long *prog_size, long read_top, int mes_flag )
{
	long	pc_begin;
	long	code_size;
	long	data_size;
	long	bss_size;
	long	reloc_size;

	if ( xhead_getl( 0x3C ) != 0 ) {
		if ( mes_flag == TRUE )
			fprintf(stderr, "BIND����Ă���t�@�C���ł�\n");
		return( 0 );
	}
	pc_begin   = xhead_getl( 0x08 );
	code_size  = xhead_getl( 0x0C );
	data_size  = xhead_getl( 0x10 );
	bss_size   = xhead_getl( 0x14 );
	reloc_size = xhead_getl( 0x18 );

	if ( reloc_size != 0 ) {
		if ( xrelocate( code_size + data_size, reloc_size, read_top )
		     == FALSE ) {
			if ( mes_flag == TRUE )
				fprintf(stderr, "���Ή��̃����P�[�g��񂪂���܂�\n");
			return( 0 );
		}
	}

	memset( prog_ptr + read_top + code_size + data_size, 0, bss_size );
	*prog_size += bss_size;

	return( read_top + pc_begin );
}

/*
 �@�@�\�FX�t�@�C���������P�[�g����
 �߂�l�F TRUE = ����I��
 �@�@�@�@FALSE = �ُ�I��
*/
static	int	xrelocate( long reloc_adr, long reloc_size, long read_top )
{
	long	prog_adr;
	long	data;
	UShort	disp;

	prog_adr = read_top;
	for(; reloc_size > 0; reloc_size -= 2, reloc_adr += 2 ) {
		disp = (UShort)mem_get( read_top + reloc_adr, S_WORD );
		if ( disp == 1 )
			return ( FALSE );
		prog_adr += disp;
		data = mem_get( prog_adr, S_LONG ) + read_top;
		mem_set( prog_adr, data, S_LONG );
	}

	return( TRUE );
}

/*
 �@�@�\�Fxhead���烍���O�f�[�^���Q�b�g����
 �߂�l�F�f�[�^�̒l
*/
static	long	xhead_getl( int adr )
{
	UChar	*p;
	long	d;

	p = &( xhead [ adr ] );

	d = *(p++);
	d = ((d << 8) | *(p++));
	d = ((d << 8) | *(p++));
	d = ((d << 8) | *p);
	return( d );
}

/*
 �@�@�\�F�v���Z�X�Ǘ��e�[�u�����쐬����
 �߂�l�F TRUE = ����I��
 �@�@�@�@FALSE = �ُ�I��
*/
int	make_psp( char *fname, long prev_adr, long end_adr, long process_id,
		  long prog_size2 )
{
	char	*mem_ptr;

	mem_ptr = prog_ptr + ra [ 0 ];
	memset( mem_ptr, 0, PSP_SIZE );
	mem_set( ra [ 0 ],        prev_adr,   S_LONG );		/* �O */
	mem_set( ra [ 0 ] + 0x04, process_id, S_LONG );		/* �m�ۃv���Z�X */
	mem_set( ra [ 0 ] + 0x08, end_adr,    S_LONG );		/* �I���+1 */
	mem_set( ra [ 0 ] + 0x0c, 0,          S_LONG );		/* �� */

	mem_set( ra [ 0 ] + 0x10, ra [ 3 ], S_LONG );
	mem_set( ra [ 0 ] + 0x20, ra [ 2 ], S_LONG );
	mem_set( ra [ 0 ] + 0x30, ra [ 0 ] + PSP_SIZE + prog_size2, S_LONG );
	mem_set( ra [ 0 ] + 0x34, ra [ 0 ] + PSP_SIZE + prog_size2, S_LONG );
	mem_set( ra [ 0 ] + 0x38, ra [ 1 ], S_LONG );
	mem_set( ra [ 0 ] + 0x44, sr, S_WORD );	/* �e��SR�̒l */
	mem_set( ra [ 0 ] + 0x60, 0, S_LONG );		/* �e���� */
	if ( set_fname( fname, ra [ 0 ] ) == FALSE )
		return( FALSE );

	psp [ nest_cnt ] = ra [ 0 ];
	return( TRUE );
}

/*
 �@�@�\�F�v���Z�X�Ǘ��e�[�u���Ƀt�@�C�������Z�b�g����
 �߂�l�F TRUE = ����I��
 �@�@�@�@FALSE = �ُ�I��
*/
static	int	set_fname( char *p, long psp_adr )
{
	char	 cud [ 67 ];
	char	 *mem_ptr;
	int	 i;

	for( i = strlen( p ) - 1; i >= 0; i-- ) {
		if ( p [ i ] == '\\' || p [ i ] == '/' || p [ i ] == ':' )
			break;
	}
	i ++;
	if ( strlen( &(p [ i ]) ) > 22 )
		return( FALSE );
	mem_ptr = prog_ptr + psp_adr + 0xC4;
	strcpy( mem_ptr, &(p [ i ]) );

	mem_ptr = prog_ptr + psp_adr + 0x82;
	if ( i == 0 ) {
		/* �J�����g�f�B���N�g�����Z�b�g */
#if defined(WIN32)
        {
        BOOL b;
        b = GetCurrentDirectoryA(sizeof(cud), cud);
        cud[sizeof(cud)-1] = '\0';
        }
        if (FALSE) {
#else
		if ( getcwd( cud, 66 ) == NULL ) {
#endif
            strcpy( mem_ptr, ".\\" );
		} else {
			mem_ptr -= 2;
			strcpy( mem_ptr, cud );
			if ( cud [ strlen( cud ) - 1 ] != '\\' )
				strcat( mem_ptr, "\\" );
			return( TRUE );
		}
	} else {
		p [ i ] = '\0';
		for( i--; i >= 0; i-- ) {
			if ( p [ i ] == ':' )
				break;
		}
		i ++;
		if ( strlen( &(p [ i ]) ) > 64 )
			return( FALSE );
		strcpy( mem_ptr, &(p [ i ]) );
	}

	mem_ptr = prog_ptr + psp_adr + 0x80;
	if ( i == 0 ) {
		/* �J�����g�h���C�u���Z�b�g */
#if defined(WIN32)
        {
        char cpath[MAX_PATH];
        BOOL b;
        b = GetCurrentDirectoryA(sizeof(cpath), cpath);
        mem_ptr[0] = cpath[0];
        }
#elif defined(DOSX)
        dos_getdrive( &drv );
		mem_ptr [ 0 ] = drv - 1 + 'A';
#else
	mem_ptr [ 0 ] = 'A';
#endif
        mem_ptr [ 1 ] = ':';
	} else {
		memcpy( mem_ptr, p, 2 );
	}

	return( TRUE );
}
