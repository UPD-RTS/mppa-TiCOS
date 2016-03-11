/**
 * Copyright (C) 2013-2014 Kalray SA.
 *
 * All rights reserved.
 *
 * Author: Pierre Guironnet de Massas, pgmassas@kalray.eu
 *
 */

#include <stdint.h>
#include <string.h>
#include "HAL/hal/hal_ext.h"

/**********************************************************************************************
 ***              Common, generic, low-level utilitary functions                            ***
 *** provides usual libc/compiler provided  functions as locked_memset, locked_assert, etc. ***
 **********************************************************************************************/
void*
locked_memset(void *s, int c, size_t n) __attribute__((section(".locked_text")));
void*
locked_memset(void *s, int c, size_t n)
{
	char *ptr = s;

	while (n--) {
		*ptr++ = c;
	}

	return ptr;
}

size_t
locked_strlen(const char *s) __attribute__((section(".locked_text")));
size_t
locked_strlen(const char *s)
{
	int a           = 0;
	int max_size    = 256;
	do {
		a++;
	} while ((*s++) && (a < max_size));
 #if 1
	if (a >= max_size) {
		__k1_club_syscall1(__NR_exit, -2);
	}
 #endif
	return a;
}

char*
locked_strcpy(char *dest, const char *src) __attribute__((section(".locked_text")));
char*
locked_strcpy(char *dest, const char *src)
{
	char *save_dest = dest;
	do {
		*dest++ = *src;
	} while (*src++);
	return save_dest;
}

int
locked_puts(const char *str) __attribute__((section(".locked_text")));
int
locked_puts(const char *str)
{
	int size = locked_strlen(str);
	char formated_str[size + 2];
	char *s;
	formated_str[size]      = '\n';
	formated_str[size + 1]  = '0';
	s                       = locked_strcpy(formated_str, str);
/* Cant rely on OS services here, it might trap */
	__k1_club_syscall2(__NR_iss_printf, (volatile int)s, (size + 1));
	return 0;
}

int
__vbsp_get_router_id(unsigned int node_id) {
	return (node_id % 16) /* offset of RM for I/O or compute cluster id. */
	  + ((node_id / 128) * 16) /* offset for I/O. */
	  + (((node_id % 128) / 32) * 4); /* I/O number 0, 1, 2, 3. */
}

int
__vbsp_get_cluster_offset_within_bsp_platform(unsigned int router_id) {
	return  (((~(router_id / 16)) & 1) * router_id) /* First part is 1 if < 16, 0 otherwise. */
	  + ((router_id / 16) * 16) /* 16 if router_id >= 16. */
	  + ((router_id / 16) * ((router_id % 16)) / 4) /* 19->16; 20->17; ...; 24->18... */;
}

#ifndef NDEBUG
static void
locked_itoa(int i, char a[16]) __attribute__((section(".locked_text")));
static void
locked_itoa(int i, char a[16])
{
	int size, idx = 0;
	char b[16];
	b[0] = 0;
	do {
		char c = (char) ((i % 10) + 0x30);
		b[++idx]    = c;
		i           = i / 10;
	} while (i);
	// idx : prochain vide
	size = idx;
	do {
		a[size - idx] = b[idx];
	} while (idx--);
}



char*
locked_strncpy(char *dest, const char *src, size_t n) __attribute__((section(".locked_text")));
char*
locked_strncpy(char *dest, const char *src, size_t n)
{
	char *save_dest = dest;
	do {
		*dest++ = *src;
	} while ((*src++) && (--n));
	return save_dest;
}



void
__assert_func(const char *file, int line, const char *func,
	const char *cond) __attribute__((alias("locked_assert_func"), weak));
void
locked_assert_func(const char *file, int line, const char *func,
	const char *cond) __attribute__((section(".locked_text")));
void
locked_assert_func(const char *file, int line, const char *func, const char *cond)
{
	char a_i[16];
	// locked_puts( a_i );
	char output_str[128];
	char *next = output_str;

	output_str[127] = '\0';
	locked_strncpy(output_str, file, sizeof(output_str) - 1);
	next        = next + locked_strlen(file);
	*(next - 1) = ':';

	locked_itoa(line, a_i);
	locked_strcpy(next, a_i);
	next        = next + locked_strlen(a_i);
	*(next - 1) = ' ';

	locked_strcpy(next, func);
	next        = next + locked_strlen(func);
	*(next - 1) = ' ';
	locked_strcpy(next, cond);
	locked_puts(output_str);
	// __k1_break( 0x1ffff);
#if 0
		__asm__ __volatile__ ("break 0x1ffff\n\t;;" :::);
		while(1){;}
#endif
	__k1_club_syscall1(4095, 1);
}
#endif
