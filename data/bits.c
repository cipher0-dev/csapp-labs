/*
 * CS:APP Data Lab
 *
 * c0
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */

#endif
// 1
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */

// ^  0  1
// 0  0  1
// 1  1  0

// x  y  ^ ~y x&y x&~y ~x&y ~x&~y x&~x ~0&~(~x&~y)&~(x&y)
// 0  0  0  1  0   0     0    1    0     0
// 0  1  1  0  0   0     1    0    0     1
// 1  0  1  1  0   1     0    0    0     1
// 1  1  0  0  1   0     0    0    0     0

int bitXor(int x, int y) { return ~0 & ~(~x & ~y) & ~(x & y); }

/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) { return 1 << 31; }
// 2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */

// 0111 1111 -> not zero
// or else -> zero

// 0111 1111 ^
// 0000 0000 -> zero
// 1111 0111
// 1000 1000 -> positive
//
// !0x7f^x but the problem is that I can't have a literal for 32bit Tmax
//
// These are the constant values I can easily obtain:
// - 0-255 -> given by the reqs
// - -1    -> ~0
//
// !(0x7fffffff ^ x)
//
// NOTE: This works but I need to find a way to avoid needing the constant.

// If we can implement this without > then we have it.
//
// x+1 > x
//
// (x+1) ^ ~(x+1)+1
//
// >   &   ^
// 010 000 011
// 001
//
// =
// 010 010 000
// 010
//
// <
// 010 000 110
// 100
//
// <
// 010 010 101
// 111
//
// NOTE: the > comparison gets optimized away because of UB

// int y = x + 1;
// return y ^ (~y + 1);
//
// NOTE: This almost works but -1 is also detected as TMax.

int isTmax(int x) {
  int y = x + 1;
  int z = (y ^ (~y + 1));
  return !z & !!y;
}
/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */

// 1010 1010
//
// 1110 1010
// 1010 1010 &
// 0000 0000 ^
//
// 0110 1110
// 0010 1010 &
// 1000 0000 ^

int allOddBits(int x) {
  int mask = 0xaa;
  mask <<= 8;
  mask |= 0xaa;
  mask <<= 8;
  mask |= 0xaa;
  mask <<= 8;
  mask |= 0xaa;
  return !((mask & x) ^ mask);
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) { return ~x + 1; }
// 3
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0'
 * to '9') Example: isAsciiDigit(0x35) = 1. isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */

int isAsciiDigit(int x) {
  int high_order_bits_set = !((x >> 4) ^ 3);
  int is_0to7 = !(x & 0x8);
  int is_8 = !(x ^ 0x38);
  int is_9 = !(x ^ 0x39);

  return high_order_bits_set & (is_0to7 | is_8 | is_9);
}
/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */

// c     0000 0100
// !!    0000 0001
// -1    1111 0000
// ~     0000 1111

// cond  0000 1111
//
// y     0101 0101
// z     0110 0110
//
// c&y   0000 0101
// ~c&z  0110 0000
//
// |     0110 0101

int conditional(int x, int y, int z) {
  int neg_1 = ~0;
  int cond_mask = ~(!!x + neg_1);
  return (cond_mask & y) | (~cond_mask & z);
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */

int isLessOrEqual(int x, int y) {
  int x_sign = !!(x >> 31);
  int y_sign = !!(y >> 31);
  int sign_lt = (x_sign & (!y_sign));
  int sign_gt = ((!x_sign) & y_sign);
  int delta_lt = !!((x + (~y + 1)) >> 31);
  int eq = !(x ^ y);
  return (!sign_gt) & (sign_lt | delta_lt | eq);
}
// 4
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */

// 0110 -> 0000
// 0000 -> 0001
int logicalNeg(int x) {
  int x_pos = (x >> 31) & 1;
  int x_neg = ((~x + 1) >> 31) & 1;
  int ored = x_pos | x_neg;
  int neg = (~ored + 1);
  return neg + 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */

// int bit_count = 1;
// bit_count = bit_count + !!(((x << 1) >> 1) ^ x);
// bit_count = bit_count + !!(((x << 2) >> 2) ^ x);
// bit_count = bit_count + !!(((x << 3) >> 3) ^ x);
// bit_count = bit_count + !!(((x << 4) >> 4) ^ x);
// bit_count = bit_count + !!(((x << 5) >> 5) ^ x);
// bit_count = bit_count + !!(((x << 6) >> 6) ^ x);
// bit_count = bit_count + !!(((x << 7) >> 7) ^ x);
// bit_count = bit_count + !!(((x << 8) >> 8) ^ x);
// bit_count = bit_count + !!(((x << 9) >> 9) ^ x);
// bit_count = bit_count + !!(((x << 10) >> 10) ^ x);
// bit_count = bit_count + !!(((x << 11) >> 11) ^ x);
// bit_count = bit_count + !!(((x << 12) >> 12) ^ x);
// bit_count = bit_count + !!(((x << 13) >> 13) ^ x);
// bit_count = bit_count + !!(((x << 14) >> 14) ^ x);
// bit_count = bit_count + !!(((x << 15) >> 15) ^ x);
// bit_count = bit_count + !!(((x << 16) >> 16) ^ x);
// bit_count = bit_count + !!(((x << 17) >> 17) ^ x);
// bit_count = bit_count + !!(((x << 18) >> 18) ^ x);
// bit_count = bit_count + !!(((x << 19) >> 19) ^ x);
// bit_count = bit_count + !!(((x << 20) >> 20) ^ x);
// bit_count = bit_count + !!(((x << 21) >> 21) ^ x);
// bit_count = bit_count + !!(((x << 22) >> 22) ^ x);
// bit_count = bit_count + !!(((x << 23) >> 23) ^ x);
// bit_count = bit_count + !!(((x << 24) >> 24) ^ x);
// bit_count = bit_count + !!(((x << 25) >> 25) ^ x);
// bit_count = bit_count + !!(((x << 26) >> 26) ^ x);
// bit_count = bit_count + !!(((x << 27) >> 27) ^ x);
// bit_count = bit_count + !!(((x << 28) >> 28) ^ x);
// bit_count = bit_count + !!(((x << 29) >> 29) ^ x);
// bit_count = bit_count + !!(((x << 30) >> 30) ^ x);
// bit_count = bit_count + !!(((x << 31) >> 31) ^ x);
// return bit_count;
// NOTE: this was too many ops

int howManyBits(int x) {
  int b16, b8, b4, b2, b1, b0;

  int sign = x >> 31;
  x = (sign & ~x) | (~sign & x);

  b16 = !!(x >> 16) << 4;
  x >>= b16;

  b8 = !!(x >> 8) << 3;
  x >>= b8;

  b4 = !!(x >> 4) << 2;
  x >>= b4;

  b2 = !!(x >> 2) << 1;
  x >>= b2;

  b1 = !!(x >> 1);
  x >>= b1;

  b0 = x;

  return b16 + b8 + b4 + b2 + b1 + b0 + 1;
}
// float
/*
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */

// 111 -> inf/nan
// 110 -> e = 3
// 101 -> e = 2
// 100 -> e = 1
// 011 -> e = 0
// 010 -> e = -1
// 001 -> e = -2
// 000 -> e = -2

unsigned floatScale2(unsigned uf) {
  // 1 8 23
  // printf("uf: %.8x\n", uf);

  const unsigned sign_mask = 0x80000000u;
  const unsigned exp_mask = 0x7f800000u;
  const unsigned frac_mask = 0x007fffffu;

  unsigned sign_bits = uf & sign_mask;
  unsigned exp_bits = uf & exp_mask;
  unsigned frac_bits = uf & frac_mask;

  // printf("sign_bits: %.8x\n", sign_bits);
  // printf("exp_bits: %.8x\n", exp_bits);
  // printf("frac_bits: %.8x\n", frac_bits);

  unsigned updated_frac_bits = frac_bits;
  unsigned shifted_frac_bits = frac_bits << 1;
  unsigned updated_exp_bits = exp_bits;

  unsigned char exp_biased_value = exp_bits >> 23;

  if (exp_biased_value == 0xff) {
    return uf; // value is +/-inf or NaN, return as is
  }

  if (exp_biased_value == 0) {
    if (frac_bits == 0) {
      return uf; // value is +/-zero, return as is
    }
    // value is denormal
    updated_frac_bits = shifted_frac_bits & frac_mask;
    if (shifted_frac_bits != updated_frac_bits) {
      updated_exp_bits = (exp_biased_value + 1) << 23;
    }
  } else {
    // value is normal
    updated_exp_bits = (exp_biased_value + 1) << 23;
  }

  // printf("updated_exp_bits: %.8x\n", updated_exp_bits);
  // printf("updated_frac_bits: %.8x\n", updated_frac_bits);

  return sign_bits | updated_exp_bits | updated_frac_bits;
}
/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
// 111 -> inf/nan
// 110 -> e = 3
// 101 -> e = 2
// 100 -> e = 1
// 011 -> e = 0
// 010 -> e = -1
// 001 -> e = -2
// 000 -> e = -2
int floatFloat2Int(unsigned uf) {
  // printf("uf: %.8x\n", uf);

  const unsigned sign_mask = 0x80000000u;
  const unsigned exp_mask = 0x7f800000u;
  const unsigned frac_mask = 0x007fffffu;

  unsigned sign_bits = uf & sign_mask;
  unsigned exp_bits = uf & exp_mask;
  unsigned frac_bits = uf & frac_mask;

  // printf("sign_bits: %.8x\n", sign_bits);
  // printf("exp_bits: %.8x\n", exp_bits);
  // printf("frac_bits: %.8x\n", frac_bits);

  // TODO: make sure all the signed/unsigned conversions here work out correctly
  unsigned char exp_biased_value = exp_bits >> 23;
  const signed char bias = 127;
  signed char exp_unbiased_value = exp_biased_value - bias;

  // printf("exp_biased_value: %.2x\n", exp_biased_value);
  // printf("exp_unbiased_value: %d\n", exp_unbiased_value);

  unsigned norm_frac_value = frac_bits | 0x00800000u;

  // printf("norm_frac_value: %d\n", norm_frac_value);

  int sign = 1;
  if (sign_bits) {
    sign = -1;
  }

  if (exp_biased_value == 0xff) {
    // value is +/-inf or NaN, return explicit value
    return 0x80000000u;
  }
  if (exp_biased_value == 0x00) {
    // denorms round to 0
    // puts("denormal");
    return 0;
  }

  // normal
  // puts("normal");
  if (exp_unbiased_value < 0) {
    // puts("-1 < x < 1");
    // -1 < x < 1
    return 0;
  }

  if (exp_unbiased_value > 22) {
    // value is too big, return explicit value
    return 0x80000000u;
  }

  // (2^exp) * (frac + 1)
  return sign * (norm_frac_value >> (23 - exp_unbiased_value));
}
/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  // normal
  const signed char bias = 127;
  unsigned char exp_biased_value = x + bias;
  unsigned exp_bits = exp_biased_value << 23;

  // negative exp
  if (x < 0) {
    return 0;
  }
  // too big
  if (x > 127) {
    return 0x7f800000;
  }

  return exp_bits;
}
