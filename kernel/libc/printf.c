/*  
 * Copyright (c) 2012  University of Padua, Department of Mathematics  
 *  
 *  This file is free software: you may copy, redistribute and/or modify it  
 *  under the terms of the GNU General Public License as published by the  
 *  Free Software Foundation, either version 2 of the License, or (at your  
 *  option) any later version.  
 *  
 *  This file is distributed in the hope that it will be useful, but  
 *  WITHOUT ANY WARRANTY; without even the implied warranty of  
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  
 *  General Public License for more details.  
 *  
 *  You should have received a copy of the GNU General Public License  
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  
 *  
 * This file incorporates work covered by the following copyright and  
 * permission notice:  
 * 
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *         Maintainers: nipo
 * 
 * Copyright (c) Barcelona Supercomputing Center (BSC)
 *         Leonidas Kosmidi, 2011
 */

#include "dependencies.h"

#if defined (POK_NEEDS_CONSOLE) || defined (POK_NEEDS_DEBUG) 

 #ifdef POK_NEEDS_QEMU_SETUP 

#include <types.h>
#include <libc.h>
#include <stdarg.h>
#include <bsp.h>

static const char digits[] = "0123456789abcdef";

#define INT_UNSIGNED    1
#define INT_SIGNED      2

#define BASE_HEX        16
#define BASE_OCT        8

#define MY_BUF_SIZE     8192

/*
 * some types
 */

struct		s_file
{
   char		buff[MY_BUF_SIZE];
   size_t	pos;
};

union		u_arg
{
   unsigned long	value;
   unsigned int	uint;
   int            sint;
   void*          ptr;
};

typedef int (*t_fmtfun)(union u_arg *arg, struct s_file *file, int flags);

struct		s_format
{
   char     ch;
   t_fmtfun	fun;
   int      flags;
};

/*
 * buffered I/O
 */

static void	my_fflush(struct s_file *file)
{
   pok_cons_write (file->buff, file->pos);
   file->pos = 0;
}

static struct s_file*	init_buffered_output(void)
{
   static struct s_file	res;

   res.pos = 0;
   return &res;
}

static void	my_putc(char c, struct s_file *file)
{
   file->buff[file->pos++] = c;

   if (file->pos == MY_BUF_SIZE)
   {
      my_fflush(file); // invoked only when the buffer is full or when the buffer has being closed
   }
}

static void	close_buffered_output(struct s_file *file)
{
   my_fflush(file);
}

/*
 * formatting functions
 */

static int	conv(unsigned int n, int base, int dig[])
{
   int i;

   i = 0;

   while (n)
   {
      dig[i] = n % base;
      ++i;
      n /= base;
   }
   return i - 1;
}

static int	my_printnbr_base (unsigned int   n,
                              const char     base[],
                              int            card,
                              struct s_file  *file)
{
   int   digits[96];
   int   i;
   int   count;

   if (n == 0)
   {
      my_putc ('0', file);
      return 1;
   }

   count = i = conv (n, card, digits);

   for (; i >= 0; --i)
   {
      my_putc(base[digits[i]], file);
   }

   return count;
}

static int	print_int (union u_arg* value,
                       struct s_file* file,
                       int flags)
{
   int sh;

   sh = 0;

   if (value->sint == 0)
   {
      my_putc('0', file);
      return 1;
   }

   if (flags == INT_SIGNED)
   {
      if (value->sint < 0)
      {
         my_putc('-', file);
         value->uint = -value->sint;
         sh = 1;
      }
      else
      {
         value->uint = value->sint;
      }
   }

   return my_printnbr_base (value->uint, digits, 10, file) + sh;
}

static int	print_str (union u_arg* value, struct s_file* file, int flags)
{
   int   count;
   char*	s;

   count = 0;
   s     = value->ptr;

   flags = flags;

   for (; *s; ++count, ++s)
   {
      my_putc(*s, file);
   }

   return count;
}

static int	print_char(union u_arg* value, struct s_file* file, int flags)
{
   char	c;

   flags = flags;
   c     = value->sint;

   my_putc (c, file);

   return 1;
}

static int	print_base(union u_arg* value, struct s_file* file, int flags)
{
   return my_printnbr_base (value->uint, digits, flags, file);
}

static const struct s_format formats[] =
{
   { 'd', print_int, INT_SIGNED },
   { 'i', print_int, INT_SIGNED },
   { 'u', print_int, INT_UNSIGNED },
   { 's', print_str, 0 },
   { 'c', print_char, 0 },
   { 'o', print_base, BASE_OCT },
   { 'x', print_base, BASE_HEX },
   { 0, NULL, 0 }
};

static int special_char (char fmt,
                         union u_arg* value,
                         struct s_file* file)
{
   int i;

   for (i = 0; formats[i].fun; ++i)
   {
      if (formats[i].ch == fmt)
      {
         break;
      }
   }

   if (formats[i].fun)
   {
      return formats[i].fun(value, file, formats[i].flags);
   }
   else
   {
      if (fmt != '%')
      {
         my_putc('%', file);
      }

      my_putc(fmt, file);

      return 1 + (fmt != '%');
   }
}

/*
 * finally, printf
 */

int vprintf (const char* format, va_list args)
{
   struct s_file* file;
   union u_arg	   arg;
   int            count;

   count = 0;

   for (file = init_buffered_output();
         *format;
         format += (*format == '%' ? 2 : 1))
   {
      if (*format == '%')
      {
         if (!*(format + 1))
         {
            break;
         }

         if (*(format + 1) != '%')
         {
            arg.value = va_arg(args, unsigned long);
         }

         count += special_char(*(format + 1), &arg, file);
      }
      else
      {
         my_putc(*format, file);
         ++count;
      }
   }

   close_buffered_output(file);
   return count;
}

int printf (const char *format, ...)
{
   int      res;
   va_list  args;

   va_start (args, format);
   res = vprintf (format, args);
   va_end (args);

   return res;
}

 #else // POK_NEEDS_QEMU_SETUP

#include <stdarg.h>
#include <soclib.h>

int printf(const char *format, ...);

char *strcpy( char *dst, const char *src )
{
        while ( (*dst++ = *src++) )
                ;
        return dst;
}

/*
   prints integer num in string str
   returns the number of printed characters
 */
int sprintfinteger(char * str, int num)
{
	char * tmp = str;
	int negative = (num < 0)? 1 : 0;
	char *p;
	int len;

	/*print the sign*/
	if (negative) {
		num = -num;
		*tmp++ = '-';
	}
	/*generate number's digits in reverse order*/
	do {
		*tmp++ = num % 10 + '0';
		num /= 10;
	} while (num);
	*tmp = '\0';
	len= tmp - str;
	/*reverse digits*/
	for(p = str + negative, tmp--; 
		p<=tmp;
		p++, tmp--)
	{	/*swap*/
		char c=*tmp;
		*tmp=*p;
		*p=c;
	}
	return len;
}

/*
	prints double number num in string str with fp_digits after radix point (.)
	returns the number of characters printed
	if integral part has more than 10 digits or more, number is printed in e notation
	if -1 < |number| < 1, number is printed in e notattion, too

	The algorithm is not very accurate, but very simple and built for debug
	Integral part is obtained with number's conversion to integer
	The decimal digits are obtained by substructing the previous integer value,
	multiplying with 10, and converting to integer
 */

int sprintfdouble(char * str, double num, int fp_digits) 
{
	/*integral part*/
	int decimal=0;
	/*temporary pointer to the beggining of the buffer*/
	char * p = str;
	/*exponent in case of e notation*/
	int exp=0;
	double num2=num;

	/*check if nan
	  we do it before checking for the sign, since in this case is meaningless
	 */
	if( Is_NaNd(*((uint64_t*) &num2) ))
	{
		printf("it is nan %p\n", *((uint64_t*) &num2));
		strcpy(p, "nan");
		p+=3;
		goto end;
	}

	/* print the sign if it needed*/
	/* the second condition is needed when a single precision variable is given as
	 * argument in printf
	 * We cannot use only the second condition because it doesn't work in the
	 * case of Infs and NaNs
	 */
	/*maybe all the checks must be done for both single and double like in
	 * ppc_instructions*/
	if(Is_Negatived(*((uint64_t*) &num2) ) )
	{
		*p++='-';
		num2 = -num2;
	}

	/*check if inf*/
	if( Is_INFd(*((uint64_t*) &num2) ))
	{
		printf("it is inf %p\n", *((uint64_t*) &num2));
		strcpy(p, "inf");
		p+=3;
		goto end;
	}

	/*check if 0.0 to avoid endless loop later*/
	if( num == 0.0)
	{
		strcpy(p, "0.0");
		p+=3;
		goto end;
	}

	/*take the integer part*/
	decimal = (int) num2;

	if(decimal)
	{
		/*check if we need e notation because integral part >= INT32_MAX*/
		while( (int) (num2 - decimal) )
		{
			/*scale the num2ber and increase exponent*/
			num2 /= 10.0;
			decimal = (int) num2;
			exp++;
		}
	}
	else
	{
		/*check if we need e notation because integral part 0*/
		while( decimal == 0)
		{
			/*scale the num2ber and descrease exponent*/
			num2 *= 10.0;
			decimal = (int) num2;
			exp--;
		}
	}

	p += sprintfinteger(p, decimal);

	/*print the digits after .*/
	*p++='.';
	while((num2 != 0.0)  && fp_digits--)
	{
		num2 -= decimal;
		num2 *= 10.0;
		decimal = (int) num2;
		*p++ = '0' + decimal;
	}
	/*print exponent if exists*/
	if(exp)
	{
		*p++='e';
		p += sprintfinteger(p, exp);
	}

end:
	*p='\0';
	/*compute length of "printed" characters*/
	return (int)(p-str);
}


int printf(const char *format, ...)
{
	register char *tmp;
	int val, i, count = 0;
	double d_val=0.0;
	char buf[SIZE_OF_BUF];
    va_list ap;
    va_start(ap, format);

	while (*format)
	{
		while ((*format != '%') && (*format)) 
		{
			putchar(*format++);
			count++;
		}
		if (*format)
		{
        again:
			format++;
			switch (*format)
			{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case '.':
				case ' ':
					goto again;
				case '%':
					putchar('%');
					count++;
					goto next;
				case 'c':
					putchar(va_arg(ap, int));
					count++;
					goto next;
				case 'd':
				case 'i':
					val = va_arg(ap, int);
					if (val < 0) {
						val = -val;
						putchar('-');
						count++;
					}
					tmp = buf + SIZE_OF_BUF;
					*--tmp = '\0';
					do {
						*--tmp = val % 10 + '0';
						val /= 10;
					} while (val);
					break;
				case 'u':
					val = va_arg(ap, unsigned int);
					tmp = buf + SIZE_OF_BUF;
					*--tmp = '\0';
					do {
						*--tmp = val % 10 + '0';
						val /= 10;
					} while (val);
					break;
				case 'o':
					val = va_arg(ap, int);
					tmp = buf + SIZE_OF_BUF;
					*--tmp = '\0';
					do {
						*--tmp = (val & 7) + '0';
						val = (unsigned int) val >> 3;
					} while (val);
					break;
				case 's':
					tmp = va_arg(ap, char *);
					if (!tmp)
						tmp = "(null)";
					break;
				case 'p':
				case 'x':
				case 'X':
					val = va_arg(ap, int);
					tmp = buf + SIZE_OF_BUF;
					*--tmp = '\0';
					i = 0;
					do {
						char t = '0'+(val & 0xf);
						if (t > '9')
							t += 'a'-'9'-1;
						*--tmp = t;
						val = ((unsigned int) val) >> 4;
						i++;
					} while (val);
					if (*format == 'p') {
						while (i < 8) {
							*--tmp = '0';
							i++;
						}
						*--tmp = 'x';
						*--tmp = '0';
					}
					break;
				case 'f':
					d_val = va_arg(ap, double);
					tmp = buf;
					sprintfdouble(tmp, d_val, FP_DIGITS);
					break;
				default:
					putchar(*format);
					count++;
					goto next;
				}//end switch
				while (*tmp) {
					putchar(*tmp++);
					count++;
				}
		next:
			format++;
		}
	}
	va_end(ap);
	return count;
}

 #endif // else POK_NEEDS_QEMU_SETUP

#if defined POK_NEEDS_DEBUG || POK_NEEDS_DEBUG_O1

void print_long(long long a)
{
  int i  , k , mask;
  int tot;
  tot=0;

  for( i =63 ; i >= 0 ; i--)
  {
     mask = 1 << i;
     k = a & mask;
     if( k == 0)
	printf("");
     else
	{
	int tmp,j;
	tmp=1;
	for(j=i; j>0; j--)
	  tmp *= 2;
	tot += tmp;
	}
  }
  printf("%d",tot);
}
#endif
#endif // debug, console


