//  SC1628D-test.ino
//
//  Arduino test program of the SC1628D LED Driver IC
//
//  (c) 2022/07/13 philippe.corbes@gmail.com
//
//  This project is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This file is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <Arduino.h>
#include <SC1628D.h>

// Module connection pins (Digital Pins)
#define SC1628D_DIO 7
#define SC1628D_STB 6
#define SC1628D_CLK 5

// The amount of time (in milliseconds) between tests
#define TEST_DELAY   2000

SC1628D sc1628d(SC1628D_STB, SC1628D_CLK, SC1628D_DIO);


void setup()
{
  sc1628d.setBrightness(0); // 0-7
  sc1628d.clear();
}

void loop()
{
  // Test digits
  const uint8_t degre[] = {2, 4, DIGIT_DEGRE, DIGIT_C, DIGIT_BLANK};
  sc1628d.displayDigits(degre);
  delay(TEST_DELAY);
 
  // test reverse screen
  sc1628d.setFilter(&SC1628D_InvertedDisplay);
  sc1628d.displayDigits(degre);
  delay(TEST_DELAY);
  sc1628d.setFilter(&SC1628D_NormalDisplay);

  // Test segments
  const uint8_t segment[] = { 
    SEG_A | SEG_F | SEG_E | SEG_D,
    SEG_A | SEG_D | SEG_G,
    SEG_A | SEG_D | SEG_G,
    SEG_A | SEG_B| SEG_C | SEG_D,
    SEG_G
  };
  sc1628d.displaySegments(segment);
  delay(TEST_DELAY/2);

  // Brightness Test
  for(uint8_t k = 0; k < 8; k++) {
    sc1628d.setBrightness(k);
    sc1628d.displaySegments(segment);
    delay(TEST_DELAY/10);
  }
  sc1628d.setBrightness(0);
  
  // Bargraph test
  const uint8_t bars[][2] = {
    {                SEG_C                         | SEG_G },
    {        SEG_B | SEG_C },
    {SEG_A | SEG_B | SEG_C | SEG_D                 | SEG_G },
    {SEG_A | SEG_B | SEG_C | SEG_D | SEG_E },
    {        SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G },
    {                SEG_C | SEG_D | SEG_E | SEG_F },
    {                        SEG_D | SEG_E | SEG_F | SEG_G },
    {        SEG_B | SEG_C | SEG_D | SEG_E | SEG_F },
    {                        SEG_D | SEG_E | SEG_F | SEG_G },
    {0},
    {SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G },
    {0},
    {SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G },
    {0},
  };
  for(uint8_t k = 0; k < 14; k++) {
    sc1628d.displaySegments(bars[k], 4, 1);
    delay(TEST_DELAY/8);
  }

  // Test digits update
  uint8_t keys[] = {DIGIT_BLANK, DIGIT_BLANK, DIGIT_BLANK, DIGIT_BLANK, DIGIT_BLANK};
  for(uint8_t k = 0; k <= DIGIT_QUESTION; k++) {
    sc1628d.displayDigit(k, k%4);
    delay(TEST_DELAY/8);
  }
  delay(TEST_DELAY/2);

  // Test keypad
  while(1) {
    uint32_t key = sc1628d.getButtons();
    keys[0] = (key/256)/16;
    keys[1] = (key/256)%16;
    keys[2] = (key%256)/16;
    keys[3] = (key%256)%16;
    sc1628d.displayDigits(keys);
  }
}
