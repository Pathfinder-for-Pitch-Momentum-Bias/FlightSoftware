#include "../../src/FCCode/BitStream.h"
#include <unity.h>

/**
 * Check that we can consume 4 bits at a time (nibble)
 */
void test1()
{
  char* myarr = (char*)"\xde\xad\xbe\xef\xab\xcd\xef";
  uint32_t actual [14] = {0xe, 0xd, 0xd, 0xa, 0xe, 0xb, 0xf, 0xe, 0xb, 0xa, 0xd, 0xc, 0xf, 0xe};
  BitStream bs(myarr, 7);
  for (int i = 0; i < 14; ++i)
  {
    uint32_t res = bs.nextN(4);
    TEST_ASSERT_EQUAL(actual[i], res);
  }
}

/**
 * Check that we can consume 8 bits at a time (byte)
 */
void test2()
{
  char* myarr = (char*)"\xde\xad\xbe\xef\xab\xcd\xef";
  BitStream bs(myarr, 7);;
  for (int i = 0; i < 7; ++i)
  {
    uint32_t res = bs.nextN(8);;
    TEST_ASSERT_EQUAL( *(reinterpret_cast<uint8_t*>(myarr) + i), res);
  }
}

/**
 * Check that we can consume 3 bits at a time
 */
void test3()
{
  char* myarr = (char*)"\xde\xad\xbe\xef\xab\xcd\xef";
  uint32_t actual [19] = {6, 3, 7, 6, 2, 5, 7, 5, 7, 5, 7, 5, 2, 3, 3, 6, 7, 5, 3};
  BitStream bs(myarr, 7);
  for (int i = 0; i < 19; ++i)
  {
    uint32_t res = bs.nextN(3);
    TEST_ASSERT_EQUAL(actual[i], res);
  }
}

/**
 * Check that we can consume 5 bits at a time
 */
void test4()
{
  char* myarr = (char*)"\xde\xad\xbe\xef\xab\xcd\xef";
  uint32_t actual [12] = {30, 14, 11, 29, 27, 23, 15, 21, 13, 30, 27, 1};
  BitStream bs(myarr, 7);
  for (int i = 0; i < 12; ++i)
  {
    uint32_t res = bs.nextN(5);
    TEST_ASSERT_EQUAL(actual[i], res);
  }
}

/**
 * Check that we can consume 7 bits at a time
 */
void test5()
{
  char* myarr = (char*)"\xde\xad\xbe\xef\xab\xcd\xef";
  uint32_t actual [8] = {94, 91, 122, 125, 62, 53, 115, 119};
  BitStream bs(myarr, 7);
  for (int i = 0; i < 8; ++i)
  {
    uint32_t res = bs.nextN(7);
    TEST_ASSERT_EQUAL(actual[i], res);
  }
}

/**
 * Check that we can consume an arbitrary number of bits into an array
 */
void test6()
{
  char* myarr = (char*)"\xde\xad\xbe\xef\xab\xcd\xef";
  uint8_t res[64];
  uint32_t actual [7] = {0xde, 0xad, 0xbe, 0xef, 0xab, 0xcd, 0xef};
  BitStream bs(myarr, 7);
  bs.nextN(64, res);
  for (int i = 0; i < 7; ++i)
  {
    TEST_ASSERT_EQUAL(actual[i], (uint32_t)res[i]);
  }
}

/**
 * Test the BitStream constructor which initializes with a bit_array
 */
void test7()
{
  // '1101 1110 1010 1101 1011 1110 1110 1111'
  std::vector<bool> bit_arr (32, 0);
  for (int i = 0; i < 32; ++i)
  {
    bit_arr[i] = (0xdeadbeef >> i) & (0x1);
  }
  char arr [ (bit_arr.size() + 7)/8 ];
  BitStream bs(bit_arr, arr); 
  uint8_t res[4];
  bs.nextN(32, res);

  uint8_t expect[4] = {0xef, 0xbe, 0xad, 0xde};
  for (int i = 0; i < 4; ++i)
  {
    TEST_ASSERT_EQUAL(expect[i], res[i]);
  }
}

/**
 * Test the BitStream constructor which initializes with a bit_array and 
 * consume 18 bits into an uint8_t array
 */
void test8()
{
  // '1101 1110 1010 1101 1011 1110 1110 1111'
  std::vector<bool> bit_arr (32, 0);
  for (int i = 0; i < 32; ++i)
  {
    bit_arr[i] = (0xdeadbeef >> i) & (0x1);
  }

  char arr [ (bit_arr.size() + 7)/8 ];
  BitStream bs(bit_arr, arr); 
  uint8_t res[4];
  bs.nextN(18, res);

  uint8_t expect[4] = {0xef, 0xbe, 0x1};
  for (int i = 0; i < 3; ++i)
  {
    TEST_ASSERT_EQUAL(expect[i], res[i]);
  }
}

/**
 * Test the BitStream constructor which initializes with a char*
 */
void test9()
{
  char* myarr = (char*)"\xde\xad\xbe\xef\xab\xcd\xef";
  uint32_t actual [7] = {222, 173, 190, 239, 171, 205, 239};
  BitStream bs(myarr, 7);
  for (int i = 0; i < 7; ++i)
  {
    uint32_t res = bs.nextN(8);
    TEST_ASSERT_EQUAL((uint32_t)actual[i], res);
  }
}

/**
 * Test consuming an arbitrary number of bits into a bit_array
 */
void test10()
{
  char* myarr = (char*)"\xde\xad\xbe\xef\xab\xcd\xef";
  // '0b 1101 1110 1010 1101 1011 1110 1110 1111 1010 1011 1100 1101 1110 1111'
  // 0xdeadbeefabcdef
  BitStream bs(myarr, 7);
  std::vector<bool> my_ba = std::vector<bool>(56, 0); 
  bs.nextN(42, my_ba);
  uint8_t expect;
  for (int i = 0; i < 6; ++i ) 
  {
    expect = 0;
    for (int j = 0; j < 8; ++j)
    {
      if (i*8 + j >= 42) // since i only asked for 42, there should only be at most 42
        break;
      expect = (myarr[i] >> j) & 1;
      //cout << "my_ba[" << i*8+j << "]" << my_ba[i*8+j] << " expected " << (uint16_t)expect<< endl;
      TEST_ASSERT_EQUAL( expect, my_ba[i*8 + j] );
    }
  }

  for (int i = 42; i < 64; ++i) // everyone else should be 0
  {
    TEST_ASSERT_EQUAL( 0, my_ba[i] );
  }
}

/**
 * Test the >> operators into uint32, uint16, and uint8
 */
void test11()
{
  char* myarr = (char*)"\xef\xbe\xad\xde\xb0\x01\x69\x08";
  BitStream bs(myarr, 7);
  uint32_t u32 = 0;
  bs >> u32;
  TEST_ASSERT_EQUAL(0xdeadbeef, u32); // little endian
  uint16_t u16 = 0;
  bs >> u16;
  TEST_ASSERT_EQUAL(0x01b0, u16);
  uint8_t u8 = 0;
  bs >> u8;
  TEST_ASSERT_EQUAL(0x69, u8);
}

/**
 * Test the >> operator into bit_arrays
'0b 1 0001 0010 0010 0000 0011 0000 0100 1010 1010 1011 1011 1100 1101 0100 0010'
0x11220304aabbcd42
*/
void test12()
{
  char* myarr = (char*)"\x11\x22\x03\x04\xaa\xbb\xcd\x42\x00\x13";
  BitStream bs(myarr, 10);
  vector<bool> b_arr = vector<bool>(69, 0);
  TEST_ASSERT_EQUAL(69, b_arr.size());
  bs >> b_arr;
  for (int i = 0; i < 10; ++i)
  {
    for (int j = 0; j < 8; ++j)
    {
      if (i*8 + j >= 69) 
        return;
      bool expected = (*(myarr +i) >> j) & 1;
      //cout << "b_arr[" << i*8+j <<"] " << b_arr[i*8 + j] << " " << expected << endl;
      TEST_ASSERT_EQUAL(expected, b_arr[i*8 + j]);
    }
  }
}

/**
 * Test that we can seek in both directions
 */
void test13()
{
  // '0b 1 0010 0011 | 0100 0101 0110 0111 1000 1001 1010 1011'
  char* arr = (char*)"\x01\x23\x45\x67\x89\xab";
  BitStream bs(arr, 8);
  uint8_t u8 = 0;
  bs >> u8;
  TEST_ASSERT_EQUAL(0x01, u8);
  bs >> u8;

  TEST_ASSERT_EQUAL(0x23, u8);
  bs.seekG(8, bs_end); // go 8 bits forward
  bs >> u8;
  TEST_ASSERT_EQUAL(0x67, u8);

  // 1100 0 --> 1001 01 0 0010 --> 10 1001  
  bs.seekG(19, bs_beg); // move back 19 bits // 10 1001
  bs >> u8;
  TEST_ASSERT_EQUAL(41, u8);

}

/**
 * Test the << operator from uint32, uint16, and uint8 
 */
void test14()
{
  const char* arr = "\xab\xcd\xef\x12\x34\x56\x78\x90";
  char non_const[9];
  memcpy(non_const, arr, 8);

  BitStream bs(non_const, 8);

  uint32_t u32 = 0;
  bs >> u32;
  TEST_ASSERT_EQUAL(0x12efcdab, u32); // little endian
  bs.seekG(32, bs_beg); // go back 32 bits
  u32 = 0x9;
  u32 << bs;
  bs.seekG(32, bs_beg);
  bs >> u32;
  TEST_ASSERT_EQUAL(0x9, u32);

  // x34\x56\x78\x90
  uint16_t u16 = 0x9988;
  // 4    3    6    5     8    7    0    9
  // 001|0 1100 0110 1010 0001 1110 0000 1001
  bs.seekG(3, bs_end); 
  u16 << bs;
  bs.seekG(16, bs_beg);

  // 4    3    6    5     8    7    0    9
  // 001|0 1100 0110 1010 0001 1110 0000 1001 
  //     0 0010 0011 0011 001
  // 4     4    c    c    c    7
  u16 = 0;
  bs >> u16;
  TEST_ASSERT_EQUAL(0x9988, u16);
  
  bs.seekG(16 + 3, bs_beg);
  uint8_t u8 = 0;
  bs >> u8;
  TEST_ASSERT_EQUAL(0x44, u8); 
  bs >> u16;
  TEST_ASSERT_EQUAL(0x7ccc, u16);

  // 0000 1001 
  // Now try t run off the edge
  // c    d    c    a
  // 0011 1011 0011 0101
  u16 = 0xacdc;
  bs.seekG(1, bs_end);
  // 000 1001 -->  
  // 001 1|101
  u16 << bs;
  bs.seekG(7, bs_beg);
  u16 = 0;
  bs >> u16;
  TEST_ASSERT_EQUAL(0x5c, u16);
}

/**
 * Test the << operator from bit_arrays and other BitStreams
 */
void test15()
{
  const char* arr1 = "\x12\x34";
  const char* arr2 =  "\xab\xcd";
  char nonconst[2];
  char nonconst2[2];
  memcpy(nonconst, arr1, 2);
  memcpy(nonconst2, arr2, 2);
  BitStream bs1(nonconst, 2);
  BitStream bs2(nonconst2, 2);
  bs2 << bs1;
  bs1.seekG(16, bs_beg);
  uint16_t u16;
  bs1 >> u16;
  TEST_ASSERT_EQUAL(0xcdab, u16);

  const char* arr3 = "\x56\x78";
  char nonconst3[2];
  memcpy(nonconst3, arr3, 2);
  BitStream bs3(nonconst3, 2);

  vector<bool> b_arr = vector<bool>(16, 0);
  TEST_ASSERT_EQUAL(16, b_arr.size());
  bs3 >> b_arr;
  bs1.seekG(16, bs_beg);

  b_arr << bs1;
  bs1.seekG(16, bs_beg);
  bs1 >> u16;
  TEST_ASSERT_EQUAL(0x7856, u16); // stored in order: b a d c
}

/**
 *  Test copy on boundary
 */
void test16()
{
  // 2     1    4    3    6    5    8    7
  // 0100 1000 0010 1100 0110 1010 0001 1110
  const char* arr = "\x12\x34\x56\x78";
  char non_const[4];
  memcpy(non_const, arr, 4);
  BitStream bs(non_const, 4);

  // 0100 1
  uint8_t u8 = u8 = bs.nextN(5);
  TEST_ASSERT_EQUAL(5, bs.bit_offset);
  TEST_ASSERT_EQUAL(0, bs.byte_offset);
  TEST_ASSERT_EQUAL(0x12, u8);

  // 8    7
  // 0001 1110
  uint8_t uChange = 0x87;
  // 2    1    4    3    6    5    8    7
  //       111 0000 1
  // 0100 1000 0010 1100 0110 1010 0001 1110
  //       7
  uint8_t res0[2];
  bs.peekN(16, res0);
  TEST_ASSERT_EQUAL(0xb1a0, *reinterpret_cast<uint16_t*>(res0));
  uChange << bs;
  bs.seekG(8, bs_beg);
  uint16_t res;
  bs >> res;
  // 111 0|000 1|100 0|110 1|010
  // 7     8     1     b
  TEST_ASSERT_EQUAL(0xb187, res);
  bs.seekG(5 + 16, bs_beg); // go back in time
  bs >> res;
  TEST_ASSERT_EQUAL(0x30f2,res);

  // Test that bits that fall off the end are not written to
  //     c    3
  // 001 1|110
  bs.seekG(9, bs_end);
  TEST_ASSERT_EQUAL(3, bs.byte_offset);
  TEST_ASSERT_EQUAL(1, bs.bit_offset);
  memset(res0, 0, 2);
  bs.peekN(8, res0);
  TEST_ASSERT_EQUAL(0x3c, *reinterpret_cast<uint16_t*>(res0));
  // attempt to write 8 bits
  // b    a
  // 1101 010|1 
  uChange = 0xab;
  uChange << bs;
  bs.seekG(7, bs_beg);
  // Should become
  memset(res0, 0, 2);
  bs.peekN(8, res0);
  TEST_ASSERT_EQUAL(0x2b, *reinterpret_cast<uint16_t*>(res0));
}

// Test >> operator on BitStreams
void test17()
{
  char mydata[8];
  // 2    1    4    3    6    5    2    1    9    0    5    0    8
  // 0100 1000 0010 1100 0110 1010 0100 1000 1001 0000 1010 0000 0001
  memcpy(mydata, "\x12\x34\x56\x67\x12\x09\x05\x80", 8);
  char mydata2[2];
  memset(mydata2, 0, 2);

  BitStream bs1(mydata, 8);
  BitStream bs2(mydata2, 2);

  // Test write fewer than 8 bits 

  // Test write (destination no more bits)

  // Test write (source no more bits)

  // Test write < 8 bits to source with bit offset

  // Test write < 8 bits from dest with bit offset

  // Test write > 8 bits to source with bit offset

  // Test write > 8 bits from dest with bit offset

  // Test write transform and transform back

}

/**
 * Recursive helper function to print bits into little endian in order to
 * paste into python to evaluate binary numbers
 */
std::string dumbrecursive(BitStream& bs, int stop, std::string ms)
{
  if (stop == 0)
    return ms;
  std::string bad =  std::to_string(bs.nextN(1));
  return dumbrecursive(bs, --stop, bad + ms);
}

int test_bitstream()
{
    UNITY_BEGIN();
    RUN_TEST(test1);
    RUN_TEST(test2);
    RUN_TEST(test3);
    RUN_TEST(test4);
    RUN_TEST(test5);
    RUN_TEST(test6);
    RUN_TEST(test7);
    RUN_TEST(test8);
    RUN_TEST(test9);
    RUN_TEST(test10);
    RUN_TEST(test11);
    RUN_TEST(test12);
    RUN_TEST(test13);
    RUN_TEST(test14);
    RUN_TEST(test15);
    RUN_TEST(test16);
    return UNITY_END();
}

//cat test/test_uplink_consumer//test_bitstream.cpp | grep "void test_" | sed 's/^void \(.*\)$/\1/' | sed 's/()/);/g'| sed -e 's/^/RUN_TEST(/'