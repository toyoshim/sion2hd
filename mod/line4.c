/* $Id: line4.c,v 1.6 2009-08-08 06:49:44 masamic Exp $ */

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2009/08/05 14:44:33  masamic
 * Some Bug fix, and implemented some instruction
 * Following Modification contributed by TRAP.
 *
 * Fixed Bug: In disassemble.c, shift/rotate as{lr},ls{lr},ro{lr} alway show word size.
 * Modify: enable KEYSNS, register behaiviour of sub ea, Dn.
 * Add: Nbcd, Sbcd.
 *
 * Revision 1.4  2004/12/17 10:29:20  masamic
 * Fixed a if-expression.
 *
 * Revision 1.3  2004/12/17 09:17:27  masamic
 * Delete Miscopied Lines.
 *
 * Revision 1.2  2004/12/17 07:51:06  masamic
 * Support TRAP instraction widely. (but not be tested)
 *
 * Revision 1.1.1.1  2001/05/23 11:22:07  masamic
 * First imported source code and docs
 *
 * Revision 1.8  2000/01/09  06:48:03  yfujii
 * PC relative addressing for movem instruction is fixed.
 *
 * Revision 1.6  1999/12/07  12:44:14  yfujii
 * *** empty log message ***
 *
 * Revision 1.6  1999/11/30  13:27:07  yfujii
 * Wrong operation for 'NOT' instruction is fixed.
 *
 * Revision 1.5  1999/11/22  03:57:08  yfujii
 * Condition code calculations are rewriten.
 *
 * Revision 1.4  1999/10/28  06:34:08  masamichi
 * Modified trace behavior and added abs.w addressing for jump
 *
 * Revision 1.3  1999/10/20  03:55:03  masamichi
 * Added showing more information about errors.
 *
 * Revision 1.2  1999/10/18  03:24:40  yfujii
 * Added RCS keywords and modified for WIN32 a little.
 *
 */

#undef	MAIN

#include <stdio.h>
#include <string.h>
#include "run68.h"

static	int	Lea( char, char );
static	int	Link( char );
static	int	Unlk( char );
static	int	Tas( char );
static	int	Tst( char );
static	int	Pea( char );
static	int	Movem_f( char );
static	int	Movem_t( char );
static	int	Move_f_sr( char );
static	int	Move_t_sr( char );
static	int	Move_f_usp( char );
static	int	Move_t_usp( char );
static	int	Move_t_ccr( char );
static	int	Swap( char );
static	int	Clr( char );
static	int	Ext( char );
static	int	Neg( char );
static	int	Negx( char );
static	int	Not( char );
static	int	Jmp( char, char );
static	int	Jsr( char );
static	int	Trap( char );
static	int	Rte( void );
static	int	Rts( void );
static	int	Nbcd( char );

#if defined(DEBUG_JSR)
static int sub_level = 0;
static int sub_num = 1;
#endif

/*
 �@�@�\�F4���C�����߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
int	line4( char *pc_ptr )
{
	char	code1, code2;
	code1 = *(pc_ptr++);
	code2 = *pc_ptr;
	pc += 2;

	/* lea */
	if ( (code1 & 0x01) == 0x01 && (code2 & 0xC0) == 0xC0 )
		return( Lea( code1, code2 ) );

	switch( code1 ) {
		case 0x40:
			if ( (code2 & 0xC0) != 0xC0 )
				return( Negx( code2 ) );
			else
				return( Move_f_sr( code2 ) );
			break;
		case 0x42:
			if ( (code2 & 0xC0) != 0xC0 )
				return( Clr( code2 ) );
			break;
		case 0x44:
			if ( (code2 & 0xC0) != 0xC0 )
				return( Neg( code2 ) );
			else
				return( Move_t_ccr( code2 ) );
			break;
		case 0x46:
			if ( (code2 & 0xC0) == 0xC0 )
				return( Move_t_sr( code2 ) );
			else
				return( Not( code2 ) );
			break;
		case 0x48:	/* movem_f_reg / swap / pea / ext / nbcd */
			if ((code2 & 0xC0) == 0x40 ) {
				if ( (code2 & 0xF8) == 0x40 )
					return( Swap( code2 ) );
				else
					return( Pea( code2 ) );
			} else {
				if ((code2 & 0xC0) == 0 ) {
					if ( ((code2 & 0x38) >> 3) == 0x01 ) {
						/* link.l am,$12345678 �� ������ */ 
						;
					}else{
						/* nbcd */
						return( Nbcd( code2 ) );
					}
				}
				if ((code2 & 0x38) == 0 )
					return( Ext( code2 ) );
				if ((code2 & 0x80) != 0 )
					return( Movem_f( code2 ) );
			}
			break;
		case 0x4A:	/* tas / tst */
			if ( (code2 & 0xC0) == 0xC0 )
				return( Tas( code2 ) );
			else
				return( Tst( code2 ) );
		case 0x4C:	/* movem_t_reg */
			if ( (code2 & 0x80) != 0 )
				return( Movem_t( code2 ) );
			break;
		case 0x4E:
			if ( (code2 & 0xF0) == 0x40 )
				return( Trap( code2 ) );
			if ( code2 == 0x71 )	/* nop */
				return( FALSE );
			if ( code2 == 0x73 )
				return( Rte() );
			if ( code2 == 0x75 )
				return( Rts() );
			if ( code2 == 0x76 ) {
				err68a( "TRAPV���߂����s���܂���", __FILE__, __LINE__ );
				return( TRUE );
			}
			if ( code2 == 0x77 )
				;	/* rtr */
			if ( (code2 & 0xF8) == 0x50 )
				return( Link( code2 ) );
			if ( (code2 & 0xF8) == 0x58 )
				return( Unlk( code2 ) );
			if ( (code2 & 0xF8) == 0x60 )
				return( Move_t_usp( code2 ) );
			if ( (code2 & 0xF8) == 0x68 )
				return( Move_f_usp( code2 ) );
			if ( (code2 & 0xC0) == 0xC0 )
				return( Jmp( code1, code2 ) );
			if ( (code2 & 0xC0) == 0x80 )
				return( Jsr( code2 ) );
			break;
	}

	err68a( "����`���߂����s���܂���", __FILE__, __LINE__ );
	return( TRUE );
}

/*
 �@�@�\�Flea���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Lea( char code1, char code2 )
{
	char	mode;
	char	src_reg;
	char	dst_reg;
	long	save_pc;

	save_pc = pc;
	mode = ((code2 & 0x38) >> 3);
	src_reg = (code2 & 0x07);
	dst_reg = ((code1 & 0x0E) >> 1);

	/* �\�[�X�̃A�h���b�V���O���[�h�ɉ��������� */
	if (get_ea(save_pc, EA_Control, mode, src_reg, &(ra[dst_reg]))) {
		return(TRUE);
	}

#ifdef	TRACE
	printf( "trace: lea      src=%d PC=%06lX\n", ra [ dst_reg ], save_pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Flink���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Link( char code )
{
	char	reg;
	short	len;

	reg = (code & 0x07);
	len = (short)imi_get( S_WORD );

	ra [ 7 ] -= 4;
	mem_set( ra [ 7 ], ra [ reg ], S_LONG );
	ra [ reg ] = ra [ 7 ];
	ra [ 7 ] += len;

#ifdef	TRACE
	printf( "trace: link     len=%d PC=%06lX\n", len, pc - 2 );
#endif

	return( FALSE );
}

/*
 �@�@�\�Funlk���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Unlk( char code )
{
	char	reg;

	reg = (code & 0x07);

	ra [ 7 ] = ra [ reg ];
	ra [ reg ] = mem_get( ra [ 7 ], S_LONG );
	ra [ 7 ] += 4;

#ifdef	TRACE
	printf( "trace: unlk     PC=%06lX\n", pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Ftas���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Tas( char code )
{
	char	mode;
	char	reg;
	long	data;
	int	work_mode;

	mode = ((code & 0x38) >> 3);
	reg = (code & 0x07);

	/* �A�h���b�V���O���[�h���|�X�g�C���N�������g�Ԑڂ̏ꍇ�͊ԐڂŃf�[�^�̎擾 */
	if (mode == EA_AIPI) {
		work_mode = EA_AI;
	} else {
		work_mode = mode;
	}

	if (get_data_at_ea_noinc(EA_VariableData, work_mode, reg, S_BYTE, &data)) {
		return(TRUE);
	}

	/* �t���O�̕ω� */
	general_conditions(data, S_BYTE);

	/* OR���Z */
	data |= 0x80;

	/* �A�h���b�V���O���[�h���v���f�N�������g�Ԑڂ̏ꍇ�͊ԐڂŃf�[�^�̐ݒ� */
	if (mode == EA_AIPD) {
		work_mode = EA_AI;
	} else {
		work_mode = mode;
	}

	if (set_data_at_ea(EA_VariableData, work_mode, reg, S_BYTE, data)) {
		return(TRUE);
	}

	return( FALSE );
}

/*
 �@�@�\�Ftst���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Tst( char code )
{
	char	size;
	char	mode;
	char	reg;
	long	data;
	long	save_pc;

	save_pc = pc;
	size = ((code >> 6) & 0x03);
	mode = ((code & 0x38) >> 3);
	reg = (code & 0x07);

	if (get_data_at_ea(EA_VariableData, mode, reg, size, &data)) {
		return(TRUE);
	}

	/* �t���O�̕ω� */
	general_conditions(data, size);

#ifdef	TRACE
	printf( "trace: tst.%c    dst=%d PC=%06lX\n",
		size_char [ size ], data, save_pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Fpea���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Pea( char code )
{
	char	mode;
	char	reg;
	long	data;
	long	save_pc;

	save_pc = pc;
	mode = ((code & 0x38) >> 3);
	reg = (code & 0x07);

	if (get_ea(save_pc, EA_Control, mode, reg, &data)) {
		return(TRUE);
	}

	ra [ 7 ] -= 4;
	mem_set( ra [ 7 ], data, S_LONG );

#ifdef	TRACE
	printf( "trace: pea      src=%d PC=%06lX\n", data, save_pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Fmovem from reg���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Movem_f( char code )
{
	long	mem_adr;
	char	mode;
	char	reg;
	char	size;
	char	size2;
	short	rlist;
	short	mask = 1;
	short	disp = 0;
	long	save_pc;
	int		i;
	int		work_mode;

	save_pc = pc;
	if ( (code & 0x40) != 0 ) {
		size = S_LONG;
		size2 = 4;
	} else {
		size = S_WORD;
		size2 = 2;
	}
	mode = (code & 0x38) >> 3;
	reg = (code & 0x07);
	rlist = (short)imi_get( S_WORD );

	// �A�h���b�V���O���[�h�� MD_AIPD �̏ꍇ�́A
	// MD_AI�Ƃ��Ď����A�h���X���擾����B
	if (mode == MD_AIPD) {
		work_mode = MD_AI;
	} else {
		work_mode = mode;
	}

	/* �A�h���b�V���O���[�h�ɉ��������� */
	if (get_ea(save_pc, EA_PreDecriment, work_mode, reg, &mem_adr)) {
		return(TRUE);
	}

	if (mode == MD_AIPD) {

		// �A�h���X���W�X�^�̑ޔ�
		for ( i = 7; i >= 0; i--, mask <<= 1 ) {
			if ( (rlist & mask) != 0 ) {
				ra [ reg ] -= size2;
				mem_adr -= size2;
				mem_set( mem_adr, ra [ i ] , size );
			}
		}

		// �f�[�^���W�X�^�̑ޔ�
		for ( i = 7; i >= 0; i--, mask <<= 1 ) {
			if ( (rlist & mask) != 0 ) {
				ra [ reg ] -= size2;
				mem_adr -= size2;
				mem_set( mem_adr, rd [ i ] , size );
			}
		}

	} else {

		// �f�[�^���W�X�^�̑ޔ�
		for ( i = 0; i <= 7; i++, mask <<= 1 ) {
			if ( (rlist & mask) != 0 ) {
				mem_set( mem_adr, rd [ i ] , size );
				mem_adr += size2;
			}
		}

		// �A�h���X���W�X�^�̑ޔ�
		for ( i = 0; i <= 7; i++, mask <<= 1 ) {
			if ( (rlist & mask) != 0 ) {
				mem_set( mem_adr, ra [ i ] , size );
				mem_adr += size2;
			}
		}

	}

#ifdef	TRACE
	printf( "trace: movemf.%c PC=%06lX\n", size_char [ size ], save_pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Fmovem to reg���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Movem_t( char code )
{
	long	mem_adr;
	char	mode;
	char	reg;
	char	size;
	char	size2;
	short	rlist;
	short	mask = 1;
	long	save_pc;
	int		i;
	int		work_mode;

	//save_pc = pc;
	if ( (code & 0x40) != 0 ) {
		size = S_LONG;
		size2 = 4;
	} else {
		size = S_WORD;
		size2 = 2;
	}
	mode = (code & 0x38) >> 3;
	reg = (code & 0x07);
	rlist = (short)imi_get( S_WORD );

	// PC���Ύ��s�A�h���X�pPC�Z�[�u
	save_pc = pc;

	// �A�h���b�V���O���[�h�� MD_AIPI �̏ꍇ�́A
	// MD_AI�Ƃ��Ď����A�h���X���擾����B
	if (mode == MD_AIPI) {
		work_mode = MD_AI;
	} else {
		work_mode = mode;
	}

	/* �A�h���b�V���O���[�h�ɉ��������� */
	if (get_ea(save_pc, EA_PostIncrement, work_mode, reg, &mem_adr)) {
		return(TRUE);
	}

	// �f�[�^���W�X�^�̕��A
	for ( i = 0; i <= 7; i++, mask <<= 1 ) {
		if ( (rlist & mask) != 0 ) {
			if (size == S_WORD) {
				rd [ i ] = mem_get( mem_adr, S_WORD );
				if (rd[i] & 0x8000) {
					rd [ i ] |= 0xFFFF0000;
				} else {
					rd [ i ] &= 0xFFFF;
				}
			} else {
				rd [ i ] = mem_get( mem_adr, S_LONG );
			}
			if ( mode == MD_AIPI )
				ra [ reg ] += size2;
			mem_adr += size2;
		}
	}

	// �A�h���X���W�X�^�̕��A
	for ( i = 0; i <= 7; i++, mask <<= 1 ) {
		if ( (rlist & mask) != 0 ) {
			if (size == S_WORD) {
				ra [ i ] = mem_get( mem_adr, S_WORD );
				if (ra[i] & 0x8000) {
					ra [ i ] |= 0xFFFF0000;
				} else {
					ra [ i ] &= 0xFFFF;
				}
			} else {
				ra [ i ] = mem_get( mem_adr, S_LONG );
			}
			if ( mode == MD_AIPI )
				ra [ reg ] += size2;
			mem_adr += size2;
		}
	}

#ifdef	TRACE
	printf( "trace: movemt.%c PC=%06lX\n", size_char [ size ], save_pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Fmove from SR���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Move_f_sr( char code )
{
	char	mode;
	char	reg;
	long	save_pc;

	save_pc = pc;
	mode = ((code & 0x38) >> 3);
	reg = (code & 0x07);

	/* �f�B�X�e�B�l�[�V�����̃A�h���b�V���O���[�h�ɉ��������� */
	// ���A�N�Z�X������EA_ALL�ɂȂ��Ă��邪�A����͌�Ń`�F�b�N�̕K�v������
	if (set_data_at_ea(EA_All, mode, reg, S_WORD, (long)sr)) {
		return(TRUE);
	}

#ifdef	TRACE
	printf( "trace: move_f_sr PC=%06lX\n", save_pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Fmove to SR���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Move_t_sr( char code )
{
	char	mode;
	char	reg;
	long	save_pc;
	long	data;

	save_pc = pc;
	mode = ((code & 0x38) >> 3);
	reg = (code & 0x07);

	if ( SR_S_REF() == 0 ) {
		err68a( "�������߂����s���܂���", __FILE__, __LINE__ );
		return( TRUE );
	}

	/* �\�[�X�̃A�h���b�V���O���[�h�ɉ��������� */
	// ���A�N�Z�X������EA_ALL�ɂȂ��Ă��邪�A����͌�Ń`�F�b�N�̕K�v������
	if (get_data_at_ea(EA_All, mode, reg, S_WORD, &data)) {
		return(TRUE);
	}

	sr = (short)data;

#ifdef	TRACE
	printf( "trace: move_t_sr PC=%06lX\n", save_pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Fmove from USP���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Move_f_usp( char code )
{
	char	reg;

	if ( SR_S_REF() == 0 ) {
		err68a( "�������߂����s���܂���", __FILE__, __LINE__ );
		return( TRUE );
	}

	reg = (code & 0x07);

#ifdef	TRACE
	printf( "trace: move_f_usp PC=%06lX\n", pc );
#endif

	if ( usp == 0 ) {
		err68( "MOVE FROM USP���߂����s���܂���" );
		return( TRUE );
	}

	ra [ reg ] = usp;

	return( FALSE );
}

/*
 �@�@�\�Fmove to USP���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Move_t_usp( char code )
{
	char	reg;

	if ( SR_S_REF() == 0 ) {
		err68a( "�������߂����s���܂���", __FILE__, __LINE__ );
		return( TRUE );
	}

	reg = (code & 0x07);

#ifdef	TRACE
	printf( "trace: move_t_usp PC=%06lX\n", pc );
#endif
/* MOD BEGIN */
        // Allows to use MOVE TO USP.
        /*
	err68( "MOVE TO USP���߂����s���܂���" );
	return( TRUE );
        */
	return( FALSE );
/* MOD END */
}

/*
 �@�@�\�Fmove to CCR���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Move_t_ccr( char code )
{
	char	mode;
	char	reg;
	long	save_pc;
	long	data;

	save_pc = pc;
	mode = ((code & 0x38) >> 3);
	reg = (code & 0x07);

	/* �\�[�X�̃A�h���b�V���O���[�h�ɉ��������� */
	// ���A�N�Z�X������EA_ALL�ɂȂ��Ă��邪�A����͌�Ń`�F�b�N�̕K�v������
	if (get_data_at_ea(EA_All, mode, reg, S_WORD, &data)) {
		return(TRUE);
	}

	sr = (( sr & 0xFF00 ) | ( (short)data & 0xFF ));

#ifdef	TRACE
	printf( "trace: move_t_ccr PC=%06lX\n", save_pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Fswap���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Swap( char code )
{
	char	reg;
	long	data;
	long	data2;

	reg = (code & 0x07);
	data = ((rd [ reg ] >> 16) & 0xFFFF);
	data2 = ((rd [ reg ] & 0xFFFF) << 16);
	data |= data2;
	rd [ reg ] = data;

#ifdef	TRACE
	printf( "trace: swap     PC=%06lX\n", pc );
#endif

	/* �t���O�̕ω� */
	general_conditions(data, S_LONG);

	return( FALSE );
}

/*
 �@�@�\�Fclr���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Clr( char code )
{
	char	size;
	char	mode;
	char	reg;
	long	save_pc;
	long	data;
	int	work_mode;

	save_pc = pc;
	size = ((code >> 6) & 0x03);
	mode = ((code & 0x38) >> 3);
	reg = (code & 0x07);

	/* �����ł킴�킴�g�������Ȃ��l�����[�h���Ă���̂� */
	/* 68000�̎d�l�������Ȃ��Ă��邽�߁B */

	/* �A�h���b�V���O���[�h���|�X�g�C���N�������g�Ԑڂ̏ꍇ�͊ԐڂŃf�[�^�̎擾 */
	if (mode == EA_AIPI) {
		work_mode = EA_AI;
	} else {
		work_mode = mode;
	}

	if (get_data_at_ea_noinc(EA_VariableData, work_mode, reg, size, &data)) {
		return(TRUE);
	}

	/* CLEAR */
	data = 0;

	/* �A�h���b�V���O���[�h���v���f�N�������g�Ԑڂ̏ꍇ�͊ԐڂŃf�[�^�̐ݒ� */
	if (mode == EA_AIPD) {
		work_mode = EA_AI;
	} else {
		work_mode = mode;
	}

	if (set_data_at_ea(EA_VariableData, work_mode, reg, size, data)) {
		return(TRUE);
	}

	/* �t���O�̕ω� */
	general_conditions(data, size);

#ifdef	TRACE
	printf( "trace: clr.%c    PC=%06lX\n", size_char [ size ], save_pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Fext���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Ext( char code )
{
	char	size;
	char	reg;

	reg = (code & 0x07);
	if ( (code & 0x40) != 0 )
		size = S_LONG;
	else
		size = S_WORD;

	if ( size == S_WORD ) {
		if ( (rd [ reg ] & 0x80) != 0 )
			rd [ reg ] |= 0xFF00;
		else
			rd [ reg ] &= 0xFFFF00FF;
	} else {
		if ( (rd [ reg ] & 0x8000) != 0 )
			rd [ reg ] |= 0xFFFF0000;
		else
			rd [ reg ] &= 0x0000FFFF;
	}

	/* �t���O�̕ω� */
	general_conditions(rd[reg], size);

#ifdef	TRACE
	printf( "trace: ext.%c    PC=%06lX\n", size_char [ size ], pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Fneg���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Neg( char code )
{
	char	size;
	char	mode;
	char	reg;
	long	data;
	long	save_pc;
	long	dest_data;
	int	work_mode;

	save_pc = pc;
	size = ((code >> 6) & 0x03);
	mode = ((code & 0x38) >> 3);
	reg = (code & 0x07);

	/* �A�h���b�V���O���[�h���|�X�g�C���N�������g�Ԑڂ̏ꍇ�͊ԐڂŃf�[�^�̎擾 */
	if (mode == EA_AIPI) {
		work_mode = EA_AI;
	} else {
		work_mode = mode;
	}

	if (get_data_at_ea_noinc(EA_VariableData, work_mode, reg, size, &data)) {
		return(TRUE);
	}

	/* NEG���Z */
	dest_data = sub_long(data, 0, size);

	/* �A�h���b�V���O���[�h���v���f�N�������g�Ԑڂ̏ꍇ�͊ԐڂŃf�[�^�̐ݒ� */
	if (mode == EA_AIPD) {
		work_mode = EA_AI;
	} else {
		work_mode = mode;
	}

	if (set_data_at_ea(EA_VariableData, work_mode, reg, size, dest_data)) {
		return(TRUE);
	}

	/* �t���O�̕ω� */
	neg_conditions(data, dest_data,  size, 1);

#ifdef	TRACE
	printf( "trace: neg.%c    PC=%06lX\n", size_char [ size ], save_pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Fnegx���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Negx( char code )
{
	char	size;
	char	mode;
	char	reg;
	long	data;
	long	save_pc;
	short	save_z;
	short	save_x;
	long	dest_data;
	int	work_mode;

	save_pc = pc;
	size = ((code >> 6) & 0x03);
	mode = ((code & 0x38) >> 3);
	reg = (code & 0x07);

	/* �A�h���b�V���O���[�h���|�X�g�C���N�������g�Ԑڂ̏ꍇ�͊ԐڂŃf�[�^�̎擾 */
	if (mode == EA_AIPI) {
		work_mode = EA_AI;
	} else {
		work_mode = mode;
	}

	if (get_data_at_ea_noinc(EA_VariableData, work_mode, reg, size, &data)) {
		return(TRUE);
	}

	save_x = CCR_X_REF() != 0 ? 1 : 0;
	save_z = CCR_Z_REF() != 0 ? 1 : 0;

	/* NEG���Z */
	dest_data = sub_long(data + save_x, 0, size);

	/* �A�h���b�V���O���[�h���v���f�N�������g�Ԑڂ̏ꍇ�͊ԐڂŃf�[�^�̐ݒ� */
	if (mode == EA_AIPD) {
		work_mode = EA_AI;
	} else {
		work_mode = mode;
	}

	if (set_data_at_ea(EA_VariableData, work_mode, reg, size, dest_data)) {
		return(TRUE);
	}

	/* �t���O�̕ω� */
	neg_conditions(data, dest_data,  size, save_z);

#ifdef	TRACE
	printf( "trace: negx.%c   PC=%06lX\n", size_char [ size ], save_pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Fnot���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Not( char code )
{
	char	size;
	char	mode;
	char	reg;
	long	data;
	long	save_pc;
	int	work_mode;

	save_pc = pc;
	size = ((code >> 6) & 0x03);
	mode = ((code & 0x38) >> 3);
	reg = (code & 0x07);

	/* �A�h���b�V���O���[�h���|�X�g�C���N�������g�Ԑڂ̏ꍇ�͊ԐڂŃf�[�^�̎擾 */
	if (mode == EA_AIPI) {
		work_mode = EA_AI;
	} else {
		work_mode = mode;
	}

	if (get_data_at_ea_noinc(EA_VariableData, work_mode, reg, size, &data)) {
		return(TRUE);
	}

	/* NOT���Z */
    data = data ^ 0xffffffff;

	/* �A�h���b�V���O���[�h���v���f�N�������g�Ԑڂ̏ꍇ�͊ԐڂŃf�[�^�̐ݒ� */
	if (mode == EA_AIPD) {
		work_mode = EA_AI;
	} else {
		work_mode = mode;
	}

	if (set_data_at_ea(EA_VariableData, work_mode, reg, size, data)) {
		return(TRUE);
	}

	/* �t���O�̕ω� */
	general_conditions(data,  size);

#ifdef	TRACE
	printf( "trace: not.%c    PC=%06lX\n", size_char [ size ], save_pc );
#endif

	return( FALSE );
}

/*
 �@�@�\�Fjmp���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Jmp( char code1, char code2 )
{
	char	mode;
	char	reg;
	long	save_pc;

	save_pc = pc;

	mode = ((code2 & 0x38) >> 3);
	reg = (code2 & 0x07);

#ifdef TRACE
    /* �j�[���j�b�N�̃g���[�X�o�� */
    fprintf(stderr, "0x%08x: %s\n", pc, mnemonic);
#endif

	/* �A�h���b�V���O���[�h�ɉ��������� */
	// ���A�N�Z�X������EA_ALL�ɂȂ��Ă��邪�A����͌�Ń`�F�b�N�̕K�v������
	if (get_ea(save_pc, EA_All, mode, reg, &pc)) {
		return(TRUE);
	}

	return( FALSE );
}

/*
 �@�@�\�Fjsr���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Jsr( char code )
{
	char	mode;
	char	reg;
	long	data;
	long	save_pc;

	save_pc = pc;
	mode = ((code & 0x38) >> 3);
	reg = (code & 0x07);

#ifdef	TRACE
	printf( "trace: jsr      PC=%06lX\n", pc );
#endif

	/* �A�h���b�V���O���[�h�ɉ��������� */
	// ���A�N�Z�X������EA_ALL�ɂȂ��Ă��邪�A����͌�Ń`�F�b�N�̕K�v������
	if (get_ea(save_pc, EA_All, mode, reg, &data)) {
		return(TRUE);
	}

	ra [ 7 ] -= 4;
	mem_set( ra [ 7 ], pc, S_LONG );
	pc = data;

#if defined(DEBUG_JSR)
			printf("%8d: %8d: $%06x JSR    TO $%06x, TOS = $%06x\n", sub_num++, sub_level++, save_pc - 2, pc, mem_get(ra[7], S_LONG));
#endif

	return( FALSE );
}

/*
 �@�@�\�Ftrap���߂����s����
 �߂�l�F TRUE = ���s�I��
         FALSE = ���s�p��
*/
static	int	Trap( char code )
{
	int vector;

	if ( (code & 0x0F) == 15 ) {
		return( iocs_call() );
/* MOD BEGIN */
        // Traps PCM8 calls.
	} else if ( (code & 0x0F) == 2 ) {
		return( pcm8_call() );
        // Traps ZMUSIC calls.
	} else if ( (code & 0x0F) == 3 ) {
		return( zmusic_call() );
/* MOD END */
	} else if (((code & 0x0f) >= 0x0) && ((code & 0x0f) <= 0x8)) {

		ra [ 7 ] -= 4;
		mem_set( ra [ 7 ], pc, S_LONG );

		ra [ 7 ] -= 2;
		mem_set( ra [ 7 ], sr, S_WORD );

		vector = mem_get((0x80 + ((code & 0x0f) << 2)), S_LONG );
		
		pc = vector;
		return( FALSE );
	} else {
		err68a( "����`�̗�O���������s���܂���", __FILE__, __LINE__ );
		return( TRUE );
	}
}

/*
 �@�@�\�Frte���߂����s����
 �߂�l�F TRUE = ���s�I��
 �߂�l�FFALSE = ���s�p��
*/
static	int	Rte()
{
#ifdef	TRACE
	printf( "trace: rte      PC=%06lX\n", pc );
#endif

	if ( SR_S_REF() == 0 ) {
		err68a( "�������߂����s���܂���", __FILE__, __LINE__ );
		return( TRUE );
	}
	sr = (short)mem_get( ra [ 7 ], S_WORD );
	ra [ 7 ] += 2;
	pc = mem_get( ra [ 7 ], S_LONG );
	ra [ 7 ] += 4;
	trap_count = RAS_INTERVAL;

	return( FALSE );
}

/*
 �@�@�\�Frts���߂����s����
 �߂�l�FFALSE = ���s�p��
*/
static	int	Rts()
{
#if defined(DEBUG_JSR)
	long	save_pc;
	save_pc = pc - 2;
#endif

#ifdef	TRACE
	printf( "trace: rts      PC=%06lX\n", pc );
#endif

	pc = mem_get( ra [ 7 ], S_LONG );
	ra [ 7 ] += 4;

#if defined(DEBUG_JSR)
			printf("%8d: %8d: $%06x RETURN TO $%06x\n", sub_num++, --sub_level, save_pc, pc - 2);
#endif

	return( FALSE );
}

/*
	Nbcd

	4805		0100_1000_00 00_0 mmm	nbcd dm
	4808 1234 5678	0100_1000_00 00_1 000	link.l a0,$12345678
	4813		0100_1000_00 01_0 mmm	nbcd (am)
	481c		0100_1000_00 01_1 mmm	nbcd (am)+
	4824		0100_1000_00 10_0 mmm	nbcd -(am)
	482c 000a	0100_1000_00 10_1 mmm	nbcd 10(am)
	4834 3005	0100_1000_00 11_0 mmm	nbcd 5(am,d3.w)
	4834 3805	0100_1000_00 11_0 mmm	nbcd 5(am,d3.l)
	4839 1234 5678	0100_1000_00 11_1 001	nbcd $12345678
*/
static	int	Nbcd( char code2 )
{
	/* nbcd */
	char	src_reg  = (code2 & 0x7);
	char	mode = (code2 & 0x38) >> 3;
	char	work_mode;
	char	size = 0;	/* S_BYTE �Œ� */
	long	src_data;
	long	dst_data;
	long	kekka;
	long	X;

/*
	0: 2byte: dm
	1: 6byte: Link����
	2: 2byte: (am)
	3: 2byte: (am)+
	4: 2byte: -(am)
	5: 4byte: 10(am)
	6: 4byte: 5(am,d3.w)  5(am,d3.l)
	7: 6byte: ��΃A�h���X�����O
*/

	/* �A�h���b�V���O���[�h���|�X�g�C���N�������g�Ԑڂ̏ꍇ�͊ԐڂŃf�[�^�̎擾 */
	if (mode == EA_AIPI) {
		work_mode = EA_AI;
	} else {
		work_mode = mode;
	}

	/* �\�[�X�̃A�h���b�V���O���[�h�ɉ��������� */
	if ( work_mode == EA_AD ) {
		err68a( "nbcd �ɂ� �A�h���X���W�X�^���ڂ͂���܂���B", __FILE__, __LINE__ );
		return(TRUE);
	} else if (get_data_at_ea(EA_All, work_mode, src_reg, size, &src_data)) {
		return(TRUE);
	}

#ifdef	TRACE
	printf( "trace: nbcd     src=%d PC=%06lX\n", src_data, save_pc );
#endif

	/* 0 - <ea> - X  */
	dst_data = 0;
	X = (CCR_X_REF() != 0) ? 1 : 0;
	kekka = dst_data - src_data - X;

	if ( (dst_data & 0xff) < ((src_data & 0xff) + X) )
		kekka -= 0x60;

	if ( (dst_data & 0x0f) < ((src_data & 0x0f) + X) )
		kekka -= 0x06;

	if ( (dst_data ^ kekka) & 0x100 ) {
		CCR_X_ON();
		CCR_C_ON();
	}else{
		CCR_X_OFF();
		CCR_C_OFF();
	}

	dst_data = kekka & 0xff;

	/* 0 �ȊO�̒l�ɂȂ������̂݁AZ �t���O�����Z�b�g���� */
	if ( dst_data != 0 ) {
		CCR_Z_OFF();
	}

	/* N�t���O�͌��ʂɉ����ė��Ă� */
	if ( dst_data & 0x80 ) {
		CCR_N_ON();
	}else{
		CCR_N_OFF();
	}

	/* V�t���O */
	if ( (dst_data ^ src_data) & 0x80 ) {
		CCR_V_OFF();
	}else{
		CCR_V_ON();
	}

	/* �A�h���b�V���O���[�h���v���f�N�������g�Ԑڂ̏ꍇ�͊ԐڂŃf�[�^�̐ݒ� */
	if (mode == EA_AIPD) {
		work_mode = EA_AI;
	} else {
		work_mode = mode;
	}

	if ( set_data_at_ea(EA_All, work_mode, src_reg, size, dst_data) ) {
		return( TRUE );
	}

	return( FALSE );
}

