/*
 *  Copyright (c) 2012  University of Padua, Department of Mathematics
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
 *  This file incorporates work covered by the following copyright and
 *  permission notice:
 *
 * 		Copyright (c) 2007-2009 POK team
 *
 *		Redistribution and use in source and binary forms, with or without
 *		modification, are permitted provided that the following conditions
 *		are met:
 *
 *			* Redistributions of source code must retain the above copyright
 *			  notice, this list of conditions and the following disclaimer.
 *			* Redistributions in binary form must reproduce the above
 *			  copyright notice, this list of conditions and the following
 *			  disclaimer in the documentation and/or other materials
 *			  provided with the distribution.
 *			* Neither the name of the POK Team nor the names of its main
 *			  author (Julien Delange) or its contributors may be used to
 *			  endorse or promote products derived from this software
 *			  without specific prior written permission.
 *
 *		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *		AS IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *		LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *		FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *		COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *		INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *		BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *		LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *		CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *		LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *		ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *		POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef ELF_H_
# define ELF_H_

typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Addr;

#define EI_NIDENT (16)

typedef struct
{
	unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
	Elf32_Half	e_type;			/* Object file type */
	Elf32_Half	e_machine;		/* Architecture */
	Elf32_Word	e_version;		/* Object file version */
	Elf32_Addr	e_entry;		/* Entry point virtual address */
	Elf32_Off	e_phoff;		/* Program header table file offset */
	Elf32_Off	e_shoff;		/* Section header table file offset */
	Elf32_Word	e_flags;		/* Processor-specific flags */
	Elf32_Half	e_ehsize;		/* ELF header size in bytes */
	Elf32_Half	e_phentsize;	/* Program header table entry size */
	Elf32_Half	e_phnum;		/* Program header table entry count */
	Elf32_Half	e_shentsize;	/* Section header table entry size */
	Elf32_Half	e_shnum;		/* Section header table entry count */
	Elf32_Half	e_shstrndx;		/* Section header string table index */
} Elf32_Ehdr;

/* Program segment header.  */

typedef struct
{
	Elf32_Word	p_type;			/* Segment type */
	Elf32_Off	p_offset;		/* Segment file offset */
	Elf32_Addr	p_vaddr;		/* Segment virtual address */
	Elf32_Addr	p_paddr;		/* Segment physical address */
	Elf32_Word	p_filesz;		/* Segment size in file */
	Elf32_Word	p_memsz;		/* Segment size in memory */
	Elf32_Word	p_flags;		/* Segment flags */
	Elf32_Word	p_align;		/* Segment alignment */
} Elf32_Phdr;

#endif /* !ELF_H_ */
