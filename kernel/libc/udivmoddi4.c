/*
 *  Copyright (C) 2013-2016 Kalray SA.
 *  All rights reserved.
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
 *		LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTEtions and the following
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
 *		FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NORRUPTION) HOWEVER
 *		CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *		LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *		ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *		POSSIBILITY OF SUCH DAMAGE.
 */


static unsigned long long
udivmoddi4(unsigned long long num, unsigned long long den, int modwanted)
{
	unsigned long long r = num, q = 0;

	if(den <= r) {
		unsigned k = __builtin_k1_clzdl (den) - __builtin_k1_clzdl (r);
		den = den << k;
		if(r >= den) {
			r = r - den;
			q = 1LL << k;
		}
		if(k != 0) {
			unsigned i = k;
			den = den >> 1;
			do {
				r = __builtin_k1_stsud (den, r);
				i--;
			} while (i!= 0);
			q = q + r;
			r = r >> k;
			q = q - (r << k);
		}
	}

	return modwanted ? r : q;
}

unsigned long long
__udivdi3 (unsigned long long a, unsigned long long b)
{
	return udivmoddi4 (a, b, 0);
}

unsigned long long
__umoddi3 (unsigned long long a, unsigned long long b)
{
	return udivmoddi4 (a, b, 1);
}

long long
__divdi3 (long long a, long long b)
{
	int neg = 0;
	long long res;

	if (a < 0)
	{
		a = -a;
		neg = !neg;
	}

	if (b < 0)
	{
		b = -b;
		neg = !neg;
	}

	res = udivmoddi4 (a, b, 0);

	if (neg)
		res = -res;

	return res;
}

long long
__moddi3 (long long a, long long b)
{
	int neg = 0;
	long long res;

	if (a < 0)
	{
		a = -a;
		neg = 1;
	}

	if (b < 0)
		b = -b;

	res = udivmoddi4 (a, b, 1);

	if (neg)
		res = -res;

	return res;
}
