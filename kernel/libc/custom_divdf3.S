	.file	"custom_divdf3.s"


.macro mul_ss_64_64_128 OP0_R0, OP0_R1, OP1_R0, OP1_R1
	/* mul_ss_64_64_128 MODIFIED REGISTERS: r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r32, r33 */
	mulsuwd $r4r5 = \OP1_R1, \OP0_R0  /* Acc = A_low x B_high, same sign as A */
	xor $r9 = \OP1_R1, \OP0_R1 /* >= 0 if A and B have the same sign, <0 otherwise */
	make $r7 = 0  /* no-carry value, B positive */
	make $r8 = 1 /* carry value, A >= 0 and B >= 0 */
	;;
	madsuciwd $r4r5 = \OP0_R1, \OP1_R0 /* Acc += A_high x B_low */
	and $r32 = $r9, \OP1_R1 /* b(31) = 1 if A,B opposite signs and B negative, 0 otherwise  */
	cmove.ltz $r7 = \OP1_R1, -1 /* no-carry value, B negative  */
	cmove.ltz $r8 = \OP0_R1, -1 /* carry value, A negative */
	;; 
	copy $r6 = \OP0_R1
	make $r33 = 0
	muluuwd $r0r1 = \OP0_R0, \OP1_R0
	cmove.ltz $r8=$r32, 0 /* A positive, B negative */
	;;
	addc $r33 = $r33, $r33
	mulwd $r2r3 = $r6, \OP1_R1
	;;
	addci $r1 = $r1, $r4
	cmove.nez $r7 = $r33, $r8
	;;
	addcd $r2:$r3 = $r2:$r3, $r5:$r7
	;;
.endm

.macro mul_su_64_64_128 OP0_R0, OP0_R1, OP1_R0, OP1_R1
	/* mul_su_64_64_128 MODIFIED REGISTERS: r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r32, r33, */
	muluuwd $r4r5 = \OP1_R1, \OP0_R0  /* Acc = A_low x B_high, same sign as A */
	extfz $r9 = \OP0_R1, 31, 31 /* extraction of OP0's sign */
	make $r7 = 0  /* no-carry value, OP0 positive */
	make $r8 = 1 /* carry value, OP0 positive */
	;;
	madsuciwd $r4r5 = \OP0_R1, \OP1_R0 /* Acc += A_high x B_low */
	cmove.ltz $r7 = \OP0_R1, -1 /* no-carry value, A negative  */
	cmove.ltz $r8 = \OP0_R1, 0 /* carry value, A negative */
	;; 
	copy $r6 = \OP0_R1
	muluuwd $r0r1 = \OP0_R0, \OP1_R0
	;;
	addc $r33 = $r33, $r33
	mulsuwd $r2r3 = $r6, \OP1_R1
	;;
	addci $r1 = $r1, $r4
	cmove.nez $r7 = $r33, $r8
	;;
	addcd $r2:$r3 = $r2:$r3, $r5:$r7
	;;
.endm

.macro mul_uu_64_64_128 OP0_R0, OP0_R1, OP1_R0, OP1_R1
	/* mul_uu_64_64_128 MODIFIED REGISTERS: r0, r1, r2, r3, r4, r5, r6, r7 */
	muluuwd $r4r5 = \OP1_R1, \OP0_R0  /* Acc = A_low x B_high, same sign as A */
	;;
	maduuciwd $r4r5 = \OP0_R1, \OP1_R0 /* Acc += A_high x B_low */
	;; 
	copy $r6 = \OP0_R1
	muluuwd $r0r1 = \OP0_R0, \OP1_R0
	make $r7 = 0 
	;;
	addc $r7 = $r7, $r7
	mulwd $r2r3 = $r6, \OP1_R1
	;;
	addci $r1 = $r1, $r4
	;;
	addcd $r2:$r3 = $r2:$r3, $r5:$r7
	;;
.endm

.macro mulExtract_s64_s64_s128 OP0_R0, OP0_R1, OP1_R0, OP1_R1, SHIFT
	/* second proposal */
	mulsuwd $r34r35 = \OP1_R1, \OP0_R0  /* Acc = A_low x B_high, same sign as A */
	xor $r9 = \OP1_R1, \OP0_R1 /* >= 0 if A and B have the same sign, <0 otherwise */
	make $r7 = 0  /* no-carry value, B positive */
	make $r8 = 1 /* carry value, A >= 0 and B >= 0 */
	;;
	madsuciwd $r34r35 = \OP0_R1, \OP1_R0 /* Acc += A_high x B_low */
	and $r32 = $r9, \OP1_R1 /* b(31) = 1 if A,B opposite signs and B negative, 0 otherwise  */
	cmove.ltz $r7 = \OP1_R1, -1 /* no-carry value, B negative  */
	cmove.ltz $r8 = \OP0_R1, -1 /* carry value, A negative */
	;; 
	copy $r6 = \OP0_R1
	make $r33 = 0
	muluuwd $r0r1 = \OP0_R0, \OP1_R0
	cmove.ltz $r8=$r32, 0 /* A positive, B negative */
	;;
	addc $r33 = $r33, $r33
	mulwd $r2r3 = $r6, \OP1_R1
	;;
	addci $r1 = $r1, $r34
	cmove.nez $r7 = $r33, $r8
	;;
	/*addcd $r2:$r3 = $r2:$r3, $r35:$r7*/
	addc $r2 = $r2, $r35
	srl $r5 = $r1, 32 - \SHIFT
	;;
	addc $r3 = $r3, $r7
	sll $r0 = $r2, \SHIFT
	;; 
	or $r0 = $r0, $r5
	slld $r5:$r1 = $r2:$r3, \SHIFT
	;;
.endm


.macro mult_64_128_ext OP0_R0, OP0_R1, OP1_R0, OP1_R1, SHIFT
	/* second proposal */
	mulsuwd $r34r35 = \OP1_R1, \OP0_R0  /* Acc = A_low x B_high, same sign as A */
	xor $r9 = \OP1_R1, \OP0_R1 /* >= 0 if A and B have the same sign, <0 otherwise */
	make $r7 = 0  /* no-carry value, B positive */
	make $r8 = 1 /* carry value, A >= 0 and B >= 0 */
	;;
	madsuciwd $r34r35 = \OP0_R1, \OP1_R0 /* Acc += A_high x B_low */
	and $r32 = $r9, \OP1_R1 /* b(31) = 1 if A,B opposite signs and B negative, 0 otherwise  */
	cmove.ltz $r7 = \OP1_R1, -1 /* no-carry value, B negative  */
	cmove.ltz $r8 = \OP0_R1, -1 /* carry value, A negative */
	;; 
	copy $r6 = \OP0_R1
	make $r33 = 0
	muluuwd $r0r1 = \OP0_R0, \OP1_R0
	cmove.ltz $r8=$r32, 0 /* A positive, B negative */
	;;
	addc $r33 = $r33, $r33
	mulwd $r2r3 = $r6, \OP1_R1
	;;
	addci $r1 = $r1, $r34
	cmove.nez $r7 = $r33, $r8
	;;
	addcd $r2:$r3 = $r2:$r3, $r35:$r7
	;;
    slld $r2:$r3 = $r2:$r3, \SHIFT
    ;;
    srl $r4 = $r1, 32 - \SHIFT
	;;
    slld $r0:$r1 = $r0:$r1, \SHIFT
    ;;
    or $r2 = $r2, $r4
    ;;
.endm

.macro mult_64_128_ext2 OP0_R0, OP0_R1, OP1_R0, OP1_R1, R0, R1, R2, R3, SHIFT
	/* second proposal */
	mulsuwd $r34r35 = \OP1_R1, \OP0_R0  /* Acc = A_low x B_high, same sign as A */
	xor $r9 = \OP1_R1, \OP0_R1 /* >= 0 if A and B have the same sign, <0 otherwise */
	make $r7 = 0  /* no-carry value, B positive */
	make $r8 = 1 /* carry value, A >= 0 and B >= 0 */
	;;
	madsuciwd $r34r35 = \OP0_R1, \OP1_R0 /* Acc += A_high x B_low */
	and $r32 = $r9, \OP1_R1 /* b(31) = 1 if A,B opposite signs and B negative, 0 otherwise  */
	cmove.ltz $r7 = \OP1_R1, -1 /* no-carry value, B negative  */
	cmove.ltz $r8 = \OP0_R0, -1 /* carry value, A negative */
	;; 
	copy $r6 = \OP0_R1
	make $r33 = 0
	muluuwd $r0r1 = \OP0_R0, \OP1_R0
	cmove.ltz $r8=$r32, 0 /* A positive, B negative */
	;;
	addc $r33 = $r33, $r33
	mulwd $r2r3 = $r6, \OP1_R1
	;;
	addci $r1 = $r1, $r34
	cmove.nez $r7 = $r33, $r8
	;;
	addcd $r2:$r3 = $r2:$r3, $r35:$r7
	;;
    slld \R2:\R3 = $r2:$r3, \SHIFT
    ;;
    srl $r7 = $r1, 32 - \SHIFT
	;;
    slld \R0:\R1 = $r0:$r1, \SHIFT
    or \R2 = \R2, $r7
    ;;
.endm


.macro mult_uu_64_128_ext OP0_R0, OP0_R1, OP1_R0, OP1_R1, SHIFT
	/* second proposal */
	muluuwd $r34r35 = \OP1_R1, \OP0_R0  /* Acc = A_low x B_high, same sign as A */
	;;
	madsuciwd $r34r35 = \OP0_R1, \OP1_R0 /* Acc += A_high x B_low */
	;; 
	copy $r6 = \OP0_R1
	muluuwd $r0r1 = \OP0_R0, \OP1_R0
    make $r7 = 0
	;;
	addc $r7 = $r7, $r7
	mulwd $r2r3 = $r6, \OP1_R1
	;;
	addci $r1 = $r1, $r34
	;;
	addcd $r2:$r3 = $r2:$r3, $r35:$r7
	;;
    slld $r2:$r3 = $r2:$r3, \SHIFT
    ;;
    srl $r4 = $r1, 32 - \SHIFT
	;;
    slld $r0:$r1 = $r0:$r1, \SHIFT
    ;;
    or $r2 = $r2, $r4
    ;;
.endm


	.text
.align 8
.global add_128_128
.type add_128_128, @function
add_128_128:
	/** op0 in $r0r1r2r3
	*  op1 in $r4r5r6r7
	*/
	addcid $r0:$r1 = $r0:$r1, $r4:$r5
	;;
	addcd $r2:$r3 = $r2:$r3, $r6:$r7
	ret
	;;
.size	add_128_128, .-add_128_128
.ident	"GCC: (GNU) 4.7.0 20120311 (prerelease)"

	.text
.align 8
.global sub_128_128
.type sub_128_128, @function
sub_128_128:
	/** op0 in $r0r1r2r3
	 *  op1 in $r4r5r6r7
	 * op0 is substracted from op1
	 */
	sbfcid $r0:$r1 = $r0:$r1, $r4:$r5
	;;
	sbfcd $r2:$r3 = $r2:$r3, $r6:$r7
	ret
	;;
.size	sub_128_128, .-sub_128_128
.ident	"GCC: (GNU) 4.7.0 20120311 (prerelease)"

	.text
.align 8
.global mul_128_128_msb
.type mul_128_128_msb, @function
mul_128_128_msb:
	/* modified register r0-9 r32-45 */
	/** op0 in $r0r1r2r3
	*  op1 in $r4r5r6r7
	* op0 is multiplied by op1, and the result msb
	* are return into $r0r1r2r3
	* tmp acc will be stored in (r42r43)r44r45r38r39
	*/
	/* mul_uu_64_64_128 MODIFIED REGISTERS: r0, r1, r2, r3, r4, r5, r6, r7 */
	/* mul_su_64_64_128 MODIFIED REGISTERS: r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r32, r33 */
	/* mul_ss_64_64_128 MODIFIED REGISTERS: r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r32, r33 */
	copy $r34 = $r0
	copy $r35 = $r1
	copy $r36 = $r2
	copy $r37 = $r3
	;;
	copy $r38 = $r4
	copy $r39 = $r5
	copy $r40 = $r6
	copy $r41 = $r7
	;;
	/* op0_LOW x op1_LOW */
	mul_uu_64_64_128 $r0, $r1, $r4, $r5
	;;
	copy $r42 = $r2
	copy $r43 = $r3
	;;
	/* op0_HIGH x op1_LOW */
	mul_su_64_64_128 $r36, $r37, $r38, $r39
	;;
	addcid $r42:$r43 = $r0:$r1, $r42:$r43
	make $r44 = 0
	make $r45 = 0
	;;
	addcd $r44:$r45 = $r2:$r3, $r44:$r45
	;;
	extfds $r38:$r39 = $r3, 31, 31
	make $r4 = 0
	;;
	addcd $r38:$r39 = $r38:$r39, $r4:$r4 /* internal carry counter */
	;;
	/* op1_HIGH x op0_LOW */
	mul_su_64_64_128 $r40, $r41, $r34, $r35
	;;
	addcid $r42:$r43 = $r0:$r1, $r42:$r43
	;;
	addcd $r44:$r45 = $r2:$r3, $r44:$r45
	;;
	extfds $r0:$r1 = $r45, 31, 31
	;;
	addcd $r38:$r39 = $r38:$r39, $r0:$r1
	;;
	/* op1_HIGH x op0_HIGH */
	mul_ss_64_64_128 $r40, $r41, $r36, $r37
	;;
	addcid $r0:$r1 = $r44:$r45, $r0:$r1
	;;
	addcd $r2:$r3 = $r38:$r39, $r2:$r3 
	ret
	;;
.size	mul_128_128_msb, .-mul_128_128_msb
.ident	"GCC: (GNU) 4.7.0 20120311 (prerelease)"

	.text
.align 8
.global mul_64_128_msb
.type mul_64_128_msb, @function
mul_64_128_msb:
	/* modified registers : r0-9 r32-41 */
	/** op0 in $r0r1
	*  op1 in $r2r3r4r5
	* op0 is multiplied by op1, and the result msb
	* are return into $r0r1r2r3
	* tmp acc will be stored in (r44r45)r34r35
	*/
	/* mul_su_64_64_128 MODIFIED REGISTERS: r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r32, r33 */
	/* mul_ss_64_64_128 MODIFIED REGISTERS: r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r32, r33 */
	copy $r36 = $r0
	copy $r37 = $r1
	copy $r38 = $r4
	copy $r39 = $r5
	;;
	/* op0_HIGH x op1_LOW */
	mul_su_64_64_128 $r0, $r1, $r2, $r3
	;;
	copy $r40 = $r2
	copy $r41 = $r3
	extfds $r34:$r35 = $r3, 31, 31
	;;
	/* op1_HIGH x op0_HIGH */
	mul_ss_64_64_128 $r38, $r39, $r36, $r37
	;;
	addcid $r0:$r1 = $r40:$r41, $r0:$r1
	;;
	addcd $r2:$r3 = $r34:$r35, $r2:$r3 
	ret
	;;
.size	mul_64_128_msb, .-mul_64_128_msb
.ident	"GCC: (GNU) 4.7.0 20120311 (prerelease)"


	.text
.align 8
.global mul_64_128_shift
.type mul_64_128_shift, @function
mul_64_128_shift:
	/* MODIFIED REGISTERS : r0-9 r32-42 */
	/** op0 in $r0r1
	*  op1 in $r2r3r4r5
	* shift in $r6
	* op0 is multiplied by op1, and the result msb
	* are return into $r0r1r2r3
	* tmp acc will be stored in (r44r45)r46r47r48r49
	*/
	/* mul_su_64_64_128 MODIFIED REGISTERS: r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r32, r33 */
	/* mul_ss_64_64_128 MODIFIED REGISTERS: r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r32, r33 */
	copy $r42 = $r6
	;;
	copy $r36 = $r0
	copy $r37 = $r1
	copy $r38 = $r4
	copy $r39 = $r5
	;;
	/* op0_HIGH x op1_LOW */
	mul_su_64_64_128 $r0, $r1, $r2, $r3
	;;
	copy $r40 = $r2
	copy $r41 = $r3
	extfds $r34:$r35 = $r3, 31, 31
	;;
	/* op1_HIGH x op0_HIGH */
	mul_ss_64_64_128 $r38, $r39, $r36, $r37
	;;
	addcid $r0:$r1 = $r40:$r41, $r0:$r1
	;;
	addcd $r2:$r3 = $r34:$r35, $r2:$r3 
	sbf $r6 = $r42, 32
	;;
	srl $r5 = $r1, $r6
	;;
	slld $r2:$r3 = $r2:$r3, $r42
	;;
	slld $r0:$r1 = $r0:$r1, $r42
	or $r2 = $r2, $r5
	ret
	;;
.size	mul_64_128_shift, .-mul_64_128_shift
.ident	"GCC: (GNU) 4.7.0 20120311 (prerelease)"

	.text
.align 8
.global shiftL_128
.type shiftL_128, @function
shiftL_128:
	/** op0 in $r0r1r2r3
	 *  shift in $r4 (max 32)
	 */
	slld $r2:$r3 = $r2:$r3, $r4
	sbf $r7 = $r4, 32
	;;
	srl $r5 = $r1, $r7
	;;
	slld $r0:$r1 = $r0:$r1, $r4
	or $r2 = $r2, $r5
	ret
	;;
.size	shiftL_128, .-shiftL_128
.ident	"GCC: (GNU) 4.7.0 20120311 (prerelease)"

	.text
.align 8
.global shiftR_128
.type shiftR_128, @function
shiftR_128:
	/** op0 in $r0r1r2r3
	 *  shift in $r4 (max 32)
	 */
	srld $r0:$r1 = $r0:$r1, $r4
	sbf $r7 = $r4, 32
	;;
	sll $r5 = $r2, $r7
	;;
	srld $r2:$r3 = $r2:$r3, $r4
	;;
	;;
	or $r1 = $r1, $r5
	ret
	;;
.size	shiftR_128, .-shiftR_128
.ident	"GCC: (GNU) 4.7.0 20120311 (prerelease)"



	.text
.align 8
.globl __divdf3
.type	__divdf3, @function
__divdf3:
	/* $r0r1 contains a (dividend)
	   $r2r3 contains b (d) */
	extfz $r5 = $r1, 19, 0
	extfz $r7 = $r3, 19, 0
	copy $r4 = $r0
	copy $r6 = $r2
	;;
	extfz $r62 = $r1, 30, 20 /* exp_a */
	extfz $r63 = $r3, 30, 20 /* exp_b */
	xor $r59 = $r1, $r3 /* sign of result */
	;;
	or $r5 = $r5, 1 << 20
	or $r7 = $r7, 1 << 20
	/* $r4r5 contains mant_a
	 * $r6r7 contains mant_b */
	slldm $r8 = $r2:$r3, 3 /* $r8 will contains float approx of 1/d */
	;;
	and $r8 = $r8, 0x7fffff
	slld $r2:$r3 = $r6:$r7, 9 /* $r2r3 contains d in Q3.61 */
	cb.eqz $r62, aissubnormal
	;;
	or $r8 = $r8, 0x3f800000
	slld $r4:$r5 = $r4:$r5, 9 /* $r4r5 contains dividend in Q3.61 */
	cb.eqz $r63, bissubnormal
	and $r59 = $r59, 0x80000000
	;;
	comp.eq $r0 = $r62, 0x7ff
	comp.eq $r61 = $r63, 0x7ff
	fsinvn $r8 = $r8
	;;
	extfz $r1 = $r8, 22, 0
	sbf $r60 = $r63, 1023
	;;
	add $r60 = $r60, $r62             /* exp_inv is store in $r60 */
	or $r1 = $r1, 1 << 23
	cb.nez $r61, bisnan
	;;
	cb.nez $r0, aisnan
	sll $r1 = $r1, 6
	make $r0 = 1 /* final bit for nr */
	/* $r0r1 contains Q2.62 approx of 1/d (8 bits ) */
	;;
    /* $r0r1 contains an approximation to 1/d in Q2.62
       $r2r3 contains d  in Q3.61 saved in $r50r51
       $r4r5 contains dividend in Q3.61 saved in $r52r53 */
    /* first newton raphson iteration */
    mulwd $r6r7 = $r1, $r3 /* Q5.59 */
    copy $r52 = $r4
    copy $r53 = $r5
    ;;
    /* 2.0 in Q5.69 */
    make $r8 = 0
    make $r9 = 0x10000000 
    copy $r50 = $r2
    copy $r51 = $r3
    ;;
    sbfd $r6:$r7 = $r6:$r7, $r8:$r9 /* sub in Q5.59 */
	cb.lez $r60, specialcases     /* branch if exp_inv <= 0 */
	sbf $r61 = $r60, 2045         /* testing exp_inv >= 2045 */
    ;;
	cb.lez $r61, specialcases     /* branching if exp_inv >= 2045 */
    slldm $r6 = $r6:$r7, 2 /* sub in Q3.61 */
    ;;
    mulwd $r0r1 = $r1, $r6 /* new approx in Q5.59 */
    ;;
    slld $r0:$r1 = $r0:$r1, 3 /* new approx in Q2.62 */
    ;;
    /* second newton iteration */
    msbwd $r8r9 = $r1, $r3 /* prod in Q5.59 */
    ;;
    /* STALL */
    slldm $r9 = $r8:$r9, 2  /* prod in Q3.61 */
    ;;
    mulwd $r0r1 = $r9, $r1 
    ;;
    /* STALL */
    make $r54 = 0
    make $r55 = 0x40000000 
    ;;
    slld $r0:$r1 = $r0:$r1, 3 /* new approx in Q2.62 */
    ;;
    copy $r56 = $r0
    copy $r57 = $r1
    ;;
    /* third newton raphson iteration */
    mulExtract_s64_s64_s128 $r0, $r1, $r2, $r3, 2
    ;;
    sbfd $r0:$r1 = $r0:$r1, $r54:$r55
    ;;
    /*mult_64_128_ext*/
	mulExtract_s64_s64_s128 $r0, $r1, $r56, $r57, 3
	;;
    copy $r56 = $r0
    copy $r57 = $r1
    ;;
	/* fourth newton raphson iteration */
    mulExtract_s64_s64_s128 $r0, $r1, $r50, $r51, 2
    ;;
    sbfd $r0:$r1 = $r0:$r1, $r54:$r55
    ;;
	/* placing approx in $r2r3r4r5 */
    mult_64_128_ext2 $r0, $r1, $r56, $r57, $r2, $r3, $r4, $r5, 3
	;;
	/* starting effective division computation by multiplicating with dividend */	
	/* copying dividend */
	copy $r0 = $r52
	copy $r1 = $r53
	/** op0 in $r0r1
	*  op1 in $r2r3r4r5
	* shift is 2
	* op0 is multiplied by op1, and the result msb
	* are return into $r0r1r2r3
	* tmp acc will be stored in (r44r45)r46r47r48r49
	*/
	copy $r40 = $r4
	copy $r41 = $r5
	;;
	/* op0_HIGH x op1_LOW */
	mul_su_64_64_128 $r0, $r1, $r2, $r3
	;;
	copy $r44 = $r2
	copy $r45 = $r3
	extfds $r46:$r47 = $r3, 31, 31
	;;
	/* op1_HIGH x op0_HIGH */
	mul_ss_64_64_128 $r40, $r41, $r52, $r53
	;;
	addcid $r0:$r1 = $r44:$r45, $r0:$r1
	;;
	addcd $r2:$r3 = $r46:$r47, $r2:$r3 
	;;
	srl $r5 = $r1, 30
	;;
	slld $r2:$r3 = $r2:$r3, 2
	make $r8 = 0
	make $r9 = 32
	;;
	slld $r0:$r1 = $r0:$r1, 2
	or $r2 = $r2, $r5
	and $r7 = $r3, 0xe0000000 /* test if there is 3 zeros at the begining of approx */
	/* $r0r1r2r3 contains a Q3.125 approximation of div */
	;;
rounding:
	cmove.eqz $r8 = $r7, 1
	cmove.eqz $r9 = $r7, 31
	;;
	slld $r2:$r3 = $r2:$r3, $r8
	sbf $r7 = $r8, $r60                    /* exp_inv -= offset */
	;;
	srl $r5 = $r1, $r9
	sll $r7 = $r7, 20                     /* preparing exponent */
	make $r38 = 2
	make $r39 = 0
	;;
	slld $r0:$r1 = $r0:$r1, $r8
	or $r2 = $r2, $r5
	copy $r4 = $r8
	;;
	# addd $r34:$r35 = $r2:$r3,  $r38:$r39        /* bdiv + ERR */
	addci $r34 = $r2, $r38
	;;
	sbfd $r36:$r37 = $r38:$r39, $r2:$r3       /* bdiv - ERR */
	;;
	/*srld $r34:$r35 = $r34:$r35, 8
	;;
	srld $r36:$r37 = $r36:$r37, 8
	;;
	srld $r38:$r39 = $r2:$r3, 8
	;;
	compdl.eq $r34 = $r34:$r35, $r38:$r39 
	;;
	compdl.eq $r36 = $r36:$r37, $r38:$r39 
	;;*/
	extfz $r34 = $r34, 31, 8
	extfz $r38 = $r2, 31, 8
	;;
	addc $r35 = $r3, $r39
	extfz $r36 = $r36, 31, 8
	;;
	compdl.eq $r34 = $r34:$r35, $r38:$r3
	;;
	compdl.eq $r36 = $r36:$r37, $r38:$r3 
	/* rounding constant */
	make $r32 = 1 << 8 
	make $r33 = 0
	;;
	addd $r32:$r33 = $r32:$r33, $r2:$r3     /* rounding */
	or $r7 = $r7, $r59                    /* pasting sign with exponent */
	;;
	srld $r0:$r1 = $r32:$r33, 9           /* Q3.61 -> Q12.52 */
	cb.eqz $r36, precise
	;;
	extfz $r1 = $r1, 19, 0                /* cutting implicit 1 */
	cb.eqz $r34, precise
	/* cmove.eqz $r4 = $r34, -1 */
	;;
	/* $r4/offset is -1 when a rounding error could have occured */
	or $r1 = $r1, $r7                    /* inserting exponent into result MSB */
	/* cmove.eqz $r4 = $r36, -1 */
	make $r4 = 0
    ret
	;;
precise:
	/* d        was saved in $r50r51 */
    /* dividend was saved in $r52r53 */
	/* approx   was saved in $r56r57 */
	/* ra       will be saved in $r49 */
	mult_64_128_ext $r56, $r57, $r50, $r51, 2 /* prod = approx * d*/
	;;
	/* setting $r32r32r32r33 to 2.0 in Q3.61 representation */
	make $r32 = 0
	make $r33 = 0x40000000
	make $r6 = 3
	;;
	/* 2.0 - prod */
	sbfcid $r34:$r35 = $r0:$r1, $r32:$r32
	copy $r0 = $r56
	copy $r1 = $r57
	;;
	sbfcd $r4:$r5 = $r2:$r3, $r32:$r33
	copy $r3 = $r35
	copy $r2 = $r34
	;;
	/* saving RA */
	get $r49 = $ra
	make $r6 = 3
	;;
	/** approx * (2.0 - prod) */
	call mul_64_128_shift
	;;
	/* placing approx into $r2r3r4r5 */
	copy $r5 = $r3
	copy $r4 = $r2
	copy $r3 = $r1
	copy $r2 = $r0
	;;
	/* saving bapprox in $r44r45r46r47 */
	copy $r44 = $r2
	copy $r45 = $r3
	copy $r46 = $r4
	copy $r47 = $r5
	;;
	/* setting r0r1 to d */
	copy $r0 = $r50
	copy $r1 = $r51
	make $r6 = 2
	;;
	/* d * new_approx  << 2 */
	call mul_64_128_shift
	;;
	make $r4 = 0
	make $r5 = 0x40000000
	;;
	/* 2.0 - approx */
	sbfcid $r0:$r1 = $r0:$r1, $r4:$r4
	/* saving */ 
	copy $r6 = $r46
	copy $r7 = $r47 
	;;
	sbfcd $r2:$r3 = $r2:$r3, $r4:$r5
	copy $r4 = $r44
	copy $r5 = $r45
	;;
	call mul_128_128_msb
	;;
	make $r4 = 3
	call shiftL_128
	;;
	/* a new precise approx has been obtained */
	/* starting multiplication by dividend */
	copy $r5 = $r3
	copy $r4 = $r2
	;;
	copy $r3 = $r1
	copy $r2 = $r0
	;;
	/* setting $r0r1 to dividend */
	copy $r0 = $r52
	copy $r1 = $r53
	make $r6 = 2 /* setting shift to 2 */
	;;
	call mul_64_128_shift
	;;
	/* restoring ra */
	set $ra = $r49
	make $r8 = 0
	and $r7 = $r3, 0xe0000000 /* test if there is 3 zeros at the begining of approx */
	;;
	cmove.eqz $r8 = $r7, 1
	;;
	slld $r2:$r3 = $r2:$r3, $r8
	sbf $r9 = $r8, 32
	sbf $r60 = $r8, $r60                    /* exp_inv -= offset */
	;;
	srl $r5 = $r1, $r9
	sll $r60 = $r60, 20                     /* preparing exponent */
	/* rounding constant */
	make $r32 = 1 << 8 
	make $r33 = 0
	;;
	slld $r0:$r1 = $r0:$r1, $r8
	or $r2 = $r2, $r5
	copy $r4 = $r8
	;;
	addd $r34:$r35 = $r2:$r3,  0x02        /* bdiv + ERR */
	;;
	addd $r36:$r37 = $r2:$r3, -0x02       /* bdiv - ERR */
	;;
	/*# srld $r34:$r35 = $r34:$r35, 8
	;;
	# srld $r36:$r37 = $r36:$r37, 8
	;;
	#srld $r38:$r39 = $r2:$r3, 8
	;;
	compdl.eq $r34 = $r34:$r35, $r38:$r39 
	;;
	compdl.eq $r36 = $r36:$r37, $r38:$r39 
	;;*/
	extfz $r34 = $r34, 31, 8
	extfz $r38 = $r2, 31, 8
	;;
	extfz $r36 = $r36, 31, 8
	;;
	compdl.eq $r34 = $r34:$r35, $r38:$r3
	;;
	compdl.eq $r36 = $r36:$r37, $r38:$r3 
	;;
	addd $r32:$r33 = $r32:$r33, $r2:$r3     /* rounding */
	or $r60 = $r60, $r59                    /* pasting sign with exponent */
	;;
	srld $r0:$r1 = $r32:$r33, 9           /* Q3.61 -> Q12.52 */
	;;
	extfz $r1 = $r1, 19, 0                /* cutting implicit 1 */
	/* cmove.eqz $r4 = $r34, -1 */
	;;
	/* $r4/offset is -1 when a rounding error could have occured */
	or $r1 = $r1, $r60                    /* inserting exponent into result MSB */
	/* cmove.eqz $r4 = $r36, -1 */
	make $r4 = 3
    ret
	;;
specialcases:
	/* $r62 contains exp_a,
	 * $r63 contains exp_b 
	 */
	/* $r4r5 contains mant_a
	 * $r6r7 contains mant_b */
aissubnormal:
	extfz $r6 = $r2, 30, 20 /**/
	;;
	# compdl $r6 = $r0:$r6, 0
	;;


bissubnormal:

	make $r4 = -1 
	ret
	;;
aiszero:
	/* test wheter b is zero */
	extfz $r4 = $r3, 30, 0
	;;
	lor $r4 = $r4, $r2
	extfz $r7 = $r3, 19, 0
	comp.eq $r6 = $r62, 0x7ff 
	;;
	cb.eqz $r4, returnnan /* if b is zero return nan */
	/* test wheter b is nan */
	lor $r5 = $r7, $r2 /* is there a 1 in mant_a */
	xor $r7 = $r1, $r3
	;; 
	land $r5 = $r5, $r6
	and  $r7 = $r7, 0x80000000
	;;
	cb.nez $r5, returnnan /* $r5 != 0 => b is NaN */
	;;
	goto returnzero
	;;
bsizero:
	/* test wheter a is zero */
	extfz $r4 = $r1, 30, 0
	extfz $r6 = $r1, 30, 20
	;;
	lor $r4 = $r4, $r0
	extfz $r7 = $r1, 19, 0
	comp.eq $r6 = $r62, 0x7ff 
	;;
	cb.eqz $r4, returnnan /* if a is zero return nan */
	/* test wheter a is nan */
	lor $r5 = $r7, $r0 /* is there a 1 in mant_a */
	xor $r7 = $r1, $r3
	;; 
	land $r5 = $r5, $r6
	and  $r7 = $r7, 0x80000000
	;;
	cb.nez $r5, returnnan /* $r5 != 0 => a is NaN */
	;;
returninf:
	/* return inf */
	make $r0 = 0
	or   $r1 = $r7, 0x7ff00000
	make $r4 = -1
	ret
	;;
returnzero:
	/* return zero */
	make $r0 = 0
	or   $r1 = $r7, 0x7ff00000
	make $r4 = -1
	ret
	;;


aisnan:
bisnan:
returnnan:
	make $r0 = 0xffffffff
	make $r1 = 0xffffffff
	make $r4 = -1
	ret
	;;
	.size	__divdf3, .-__divdf3
	.ident	"GCC: (GNU) 4.7.0 20120311 (prerelease)"

