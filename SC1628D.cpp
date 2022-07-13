/*
 *  SC1628D.cpp
 *
 *  Arduino Library for the SC1628D LED Driver IC
 *
 *  (c) 2022/07/13 philippe.corbes@gmail.com
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

extern "C" {
	#include <stdlib.h>
	#include <string.h>
	#include <inttypes.h>
}

#include <SC1628D.h>
#include <Arduino.h>

#define SC1628D_DISPLAY_MODE_CMD       0x00
#define SC1628D_DATA_SETTING_CMD_WRITE 0x40
#define SC1628D_DATA_SETTING_CMD_READ  0x42
#define SC1628D_DISPLAY_CONTROL_CMD    0x80
#define SC1628D_ADDRESS_SETTING_CMD    0xC0

/*
 Keyboard pinout connected to the SC1628

 K1 #...              <--->    12 KS5 - 5 K1
 K2 .#..              <--->    13 KS6 - 5 K1
 K3 ..#.              <--->    11 KS4 - 5 K1
 K4 ...#              <--->    10 KS3 - 5 K1
*/

/*
 Display pinout connected to the SC1628
 
 Anode    1 A         <--->    20 GR7
          2 B         <--->    18 GR5
          3 C         <--->    23 GR4
          4 D         <--->    24 GR3
          5 E         <--->    26 GR2
          6 F         <--->    27 GR1
          7 G         <--->    19 GR6
 Cathode  8 2 ..#.    <--->    17 SG10
          9 1 .#..    <--->    16 SG9
         10 3 ...#    <--->    15 SG8
         11 0 #...    <--->     9 SG2
         12 4 Symbols <--->    10 SG3
*/

/*
 Normal segment addressing:
 
    [4F]         [0A]                [1A]                    [2A]                [3A]           [4A]        
  GR7/SG3     +-GR7/SG2-+         +-GR7/SG9-+             +-GR7/SG10+         +-GR7/SG8-+      GR5/SG3
              |         |         |         |             |         |         |         |
             [0F]      [0B]      [1F]      [1B]         [2F]       [2B]      [3F]      [3B]
           GR1/SG2   GR5/SG2   GR1/SG9   GR5/SG9       GR1/SG10  GR5/SG10  GR1/SG8   GR5/SG8 
    [4E]      |  [0G]   |         |  [1G]   |    [4G]     |  [2G]   |         |  [3G]   |       [4B]  
  GR4/SG3     +-GR6/SG2-+         +-GR6/SG9-+   GR1/SG3   +-GR6/SG10+         +-GR6/SG8-+      GR5/SG3
              |         |         |         |             |         |         |         |
             [0E]      [0C]      [1E]      [1C]         [2E]       [2C]      [3E]      [3C]
           GR2/SG2   GR4/SG2   GR2/SG9   GR4/SG9       GR2/SG10  GR4/SG10  GR2/SG8   GR4/SG8 
    [4D]      |  [0D]   |         |  [1D]   |             |  [2D]   |         |  [3D]   |       [4C]  
  GR3/SG3     +-GR3/SG2-+         +-GR3/SG9-+             +-GR3/SG10+         +-GR3/SG8-+      GR2/SG3

*/
void SC1628D_NormalDisplay(uint8_t digit[], uint16_t matrix[])
{
	uint16_t mask;

	// Build GR1
	mask = 0;
	if (digit[0] & SEG_F) mask |= SG2;
	if (digit[4] & SEG_G) mask |= SG3;
	if (digit[3] & SEG_F) mask |= SG8;
	if (digit[1] & SEG_F) mask |= SG9;
	if (digit[2] & SEG_F) mask |= SG10;
	matrix[0] = mask;

	// Build GR2
	mask = 0;
	if (digit[0] & SEG_E) mask |= SG2;
	if (digit[4] & SEG_C) mask |= SG3;
	if (digit[3] & SEG_E) mask |= SG8;
	if (digit[1] & SEG_E) mask |= SG9;
	if (digit[2] & SEG_E) mask |= SG10;
	matrix[1] = mask;

	// Build GR3
	mask = 0;
	if (digit[0] & SEG_D) mask |= SG2;
	if (digit[4] & SEG_D) mask |= SG3;
	if (digit[3] & SEG_D) mask |= SG8;
	if (digit[1] & SEG_D) mask |= SG9;
	if (digit[2] & SEG_D) mask |= SG10;
	matrix[2] = mask;

	// Build GR4
	mask = 0;
	if (digit[0] & SEG_C) mask |= SG2;
	if (digit[4] & SEG_E) mask |= SG3;
	if (digit[3] & SEG_C) mask |= SG8;
	if (digit[1] & SEG_C) mask |= SG9;
	if (digit[2] & SEG_C) mask |= SG10;
	matrix[3] = mask;

	// Build GR5
	mask = 0;
	if (digit[0] & SEG_B) mask |= SG2;
	if (digit[4] & SEG_A) mask |= SG3;
	if (digit[3] & SEG_B) mask |= SG8;
	if (digit[1] & SEG_B) mask |= SG9;
	if (digit[2] & SEG_B) mask |= SG10;
	matrix[4] = mask;

	// Build GR6
	mask = 0;
	if (digit[0] & SEG_G) mask |= SG2;
	if (digit[4] & SEG_B) mask |= SG3;
	if (digit[3] & SEG_G) mask |= SG8;
	if (digit[1] & SEG_G) mask |= SG9;
	if (digit[2] & SEG_G) mask |= SG10;
	matrix[5] = mask;

	// Build GR7
	mask = 0;
	if (digit[0] & SEG_A) mask |= SG2;
	if (digit[4] & SEG_F) mask |= SG3;
	if (digit[3] & SEG_A) mask |= SG8;
	if (digit[1] & SEG_A) mask |= SG9;
	if (digit[2] & SEG_A) mask |= SG10;
	matrix[6] = mask;
}

/*
 Inverted segment addressing:
 
    [4C]         [3D]                [2D]                    [1D]                [0D]           [4D]        
  GR7/SG3     +-GR7/SG2-+         +-GR7/SG9-+             +-GR7/SG10+         +-GR7/SG8-+      GR5/SG3
              |         |         |         |             |         |         |         |
             [3C]      [3E]      [2C]      [2E]         [1C]       [1E]      [0C]      [0E]
           GR1/SG2   GR5/SG2   GR1/SG9   GR5/SG9       GR1/SG10  GR5/SG10  GR1/SG8   GR5/SG8 
    [4B]      |  [3G]   |         |  [2G]   |    [4G]     |  [1G]   |         |  [3G]   |       [4E]  
  GR4/SG3     +-GR6/SG2-+         +-GR6/SG9-+   GR1/SG3   +-GR6/SG10+         +-GR6/SG8-+      GR5/SG3
              |         |         |         |             |         |         |         |
             [3B]      [3F]      [2B]      [2F]         [1B]       [1F]      [0B]      [0F]
           GR2/SG2   GR4/SG2   GR2/SG9   GR4/SG9       GR2/SG10  GR4/SG10  GR2/SG8   GR4/SG8 
    [4A]      |  [3A]   |         |  [2A]   |             |  [1A]   |         |  [0A]   |       [4F]  
  GR3/SG3     +-GR3/SG2-+         +-GR3/SG9-+             +-GR3/SG10+         +-GR3/SG8-+      GR2/SG3
*/

void SC1628D_InvertedDisplay(uint8_t digit[], uint16_t matrix[])
{
	uint16_t mask;

	// Build GR1
	mask = 0;
	if (digit[3] & SEG_C) mask |= SG2;
	if (digit[4] & SEG_G) mask |= SG3;
	if (digit[0] & SEG_C) mask |= SG8;
	if (digit[2] & SEG_C) mask |= SG9;
	if (digit[1] & SEG_C) mask |= SG10;
	matrix[0] = mask;

	// Build GR2
	mask = 0;
	if (digit[3] & SEG_B) mask |= SG2;
	if (digit[4] & SEG_F) mask |= SG3;
	if (digit[0] & SEG_B) mask |= SG8;
	if (digit[2] & SEG_B) mask |= SG9;
	if (digit[1] & SEG_B) mask |= SG10;
	matrix[1] = mask;

	// Build GR3
	mask = 0;
	if (digit[3] & SEG_A) mask |= SG2;
	if (digit[4] & SEG_A) mask |= SG3;
	if (digit[0] & SEG_A) mask |= SG8;
	if (digit[2] & SEG_A) mask |= SG9;
	if (digit[1] & SEG_A) mask |= SG10;
	matrix[2] = mask;

	// Build GR4
	mask = 0;
	if (digit[3] & SEG_F) mask |= SG2;
	if (digit[4] & SEG_B) mask |= SG3;
	if (digit[0] & SEG_F) mask |= SG8;
	if (digit[2] & SEG_F) mask |= SG9;
	if (digit[1] & SEG_F) mask |= SG10;
	matrix[3] = mask;

	// Build GR5
	mask = 0;
	if (digit[3] & SEG_E) mask |= SG2;
	if (digit[4] & SEG_D) mask |= SG3;
	if (digit[0] & SEG_E) mask |= SG8;
	if (digit[2] & SEG_E) mask |= SG9;
	if (digit[1] & SEG_E) mask |= SG10;
	matrix[4] = mask;

	// Build GR6
	mask = 0;
	if (digit[3] & SEG_G) mask |= SG2;
	if (digit[4] & SEG_E) mask |= SG3;
	if (digit[0] & SEG_G) mask |= SG8;
	if (digit[2] & SEG_G) mask |= SG9;
	if (digit[1] & SEG_G) mask |= SG10;
	matrix[5] = mask;

	// Build GR7
	mask = 0;
	if (digit[3] & SEG_D) mask |= SG2;
	if (digit[4] & SEG_C) mask |= SG3;
	if (digit[0] & SEG_D) mask |= SG8;
	if (digit[2] & SEG_D) mask |= SG9;
	if (digit[1] & SEG_D) mask |= SG10;
	matrix[6] = mask;
}

// Segment names
//     <-A->
//    ^     ^
//    F     B
//    v     v
//     <-G->
//    ^     ^
//    E     C
//    v     v
//     <-D->
const uint8_t SC1628D_NORMAL_FONT[] = {
	/* 0 */  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
	/* 1 */  SEG_B | SEG_C,
	/* 2 */  SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,
	/* 3 */  SEG_A | SEG_B | SEG_C | SEG_D | SEG_G,
	/* 4 */  SEG_F | SEG_G | SEG_B | SEG_C,
	/* 5 */  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
	/* 6 */  SEG_A | SEG_F | SEG_E | SEG_D | SEG_C | SEG_G,
	/* 7 *//*SEG_F |*/ SEG_A | SEG_B | SEG_C,
	/* 8 */  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
	/* 9 */  SEG_G | SEG_F | SEG_A | SEG_B | SEG_C | SEG_D,
	/* A */  SEG_E | SEG_F | SEG_A | SEG_B | SEG_C | SEG_G,
	/* b */  SEG_F | SEG_E | SEG_D | SEG_C | SEG_G,
	/* c */  SEG_G | SEG_E | SEG_D,
	/* d */  SEG_G | SEG_E | SEG_D | SEG_C | SEG_B,
	/* E */  SEG_A | SEG_F | SEG_E | SEG_D | SEG_G,
	/* F */  SEG_A | SEG_F | SEG_E | SEG_G,
	/*   */  0,								// DIGIT_BLANK
	/* - */  SEG_G,							// DIGIT_MINUS
	/* ° */  SEG_A | SEG_B | SEG_G | SEG_F,	// DIGIT_DEGRE
	/* C */  SEG_A | SEG_F | SEG_E | SEG_D,	// DIGIT_C
	/* ? */  SEG_A | SEG_B | SEG_G | SEG_E,	// DIGIT_QUESTION
};

const uint8_t SC1628D_INVERTED_FONT[] = {
	/* 0 */  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
	/* 1 */  SEG_E | SEG_F,
	/* 2 */  SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,
	/* 3 */  SEG_A | SEG_F | SEG_E | SEG_D | SEG_G,
	/* 4 */  SEG_C | SEG_G | SEG_F | SEG_E,
	/* 5 */  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
	/* 6 */  SEG_D | SEG_E | SEG_F | SEG_A | SEG_B | SEG_G,
	/* 7 */  SEG_D | SEG_E | SEG_F,
	/* 8 */  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
	/* 9 */  SEG_G | SEG_C | SEG_D | SEG_E | SEG_F | SEG_A,
	/* A */  SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
	/* b */  SEG_C | SEG_B | SEG_A | SEG_F | SEG_G,
	/* c */  SEG_G | SEG_B | SEG_A,
	/* d */  SEG_G | SEG_B | SEG_A | SEG_F | SEG_D,
	/* E */  SEG_D | SEG_C | SEG_B | SEG_A | SEG_G,
	/* F */  SEG_D | SEG_D | SEG_B | SEG_G,
	/*   */  0,								// DIGIT_BLANK
	/* - */  SEG_G,							// DIGIT_MINUS
	/* ° */  SEG_D | SEG_C | SEG_G | SEG_E,	// DIGIT_DEGRE
	/* C */  SEG_D | SEG_C | SEG_B | SEG_A,	// DIGIT_C
	/* ? */  SEG_D | SEG_E | SEG_G | SEG_B,	// DIGIT_QUESTION
};


//-----------------------------------------------------------------


SC1628D::SC1628D(uint8_t pinSTB, uint8_t pinCLK, uint8_t pinDIO, unsigned int bitDelay)
{
	m_font = SC1628D_NORMAL_FONT;
	m_filter = &SC1628D_NormalDisplay;

	// Copy the pin numbers
	m_pinSTB = pinSTB;
	m_pinCLK = pinCLK;
	m_pinDIO = pinDIO;
	m_bitDelay = bitDelay;

	// Set the pin direction and default value.
	// Both pins are set as inputs, allowing the pull-up resistors to pull them up
    pinMode(m_pinSTB, OUTPUT);
    pinMode(m_pinCLK, OUTPUT);
    pinMode(m_pinDIO, OUTPUT);
	digitalWrite(m_pinSTB, HIGH);
	digitalWrite(m_pinCLK, HIGH);
	digitalWrite(m_pinDIO, HIGH);
}


void SC1628D::clear()
{
    uint8_t data[] = { 0, 0, 0, 0, 0};
	displayDigits(data);
}

void SC1628D::setBrightness(uint8_t brightness, bool on)
{
	m_brightness = (brightness & 0x7) | (on? 0x08 : 0x00);
}

void SC1628D::setFont(uint8_t font[])
{
	m_font = font;
}

void SC1628D::setFilter(void (*aFilterFunction)(uint8_t digit[], uint16_t matrix[]))
{
	m_filter = aFilterFunction;
}

void SC1628D::displayDigit(const uint8_t digit, uint8_t pos)
{
	m_segments[pos] = m_font[digit];
	displaySegments(m_segments);
}

void SC1628D::displayDigits(const uint8_t digits[], uint8_t pos, uint8_t length)
{
	for (uint8_t i = 0; i < length; i++) {
		m_segments[pos++] = m_font[digits[i]];
	}
	displaySegments(m_segments);
}

void SC1628D::displaySegment(const uint8_t segment, uint8_t pos)
{
	m_segments[pos] = segment;
    displaySegments(m_segments);
}

void SC1628D::displaySegments(const uint8_t segments[], uint8_t pos, uint8_t length)
{
	uint16_t matrix[7];

	if (segments != m_segments)
		for (uint8_t i = 0; i < length; i++) 
			m_segments[pos++] = segments[i];
	m_filter(m_segments, matrix);
	writeMatrix(matrix);

	// Command 1: Set display mode (default: 7 grids - 11 segments)
	start();
	writeCommand(SC1628D_DISPLAY_MODE_CMD + SC1628D_7GRID_11SEG);
	stop();

	// Command 4: Set Display on/off + Brightness
	start();
	writeCommand(SC1628D_DISPLAY_CONTROL_CMD + (m_brightness & 0x0f));
	stop();
}

uint32_t SC1628D::getButtons(void)
{
	// Keyscan data on the SC1628 is 2x10 keys, received as an array of 5 bytes (same as TM1668).
	// Of each byte the bits B0/B3 and B1/B4 represent status of the connection of K1 and K2 to KS1-KS10
	// Byte1[0-1]: KS1xK1, KS1xK2
	// The return value is a 32 bit value containing button scans for both K1 and K2, the high word is for K2 and the low word for K1.
	uint8_t keys;
	uint32_t buttons = 0;

	start();
	writeCommand(SC1628D_DATA_SETTING_CMD_READ);		// send read buttons command
	for (int i = 0; i < 5; i++) {
		keys = receiveData();
		buttons |= (( (keys & 0x01)       | ((keys & 0x08) >> 2)) << (2*i));       // bit 0 for K1/KS1 and bit 3 for K1/KS2
		buttons |= ((((keys & 0x02) >> 1) | ((keys & 0x10) >> 3)) << (2*i + 16));  // bit 1 for K2/KS1 and bit 4 for K2/KS2
	}
	stop();
	return buttons;
}


//-----------------------------------------------------------------


void SC1628D::bitDelay()
{
	delayMicroseconds(m_bitDelay);
}

void SC1628D::start()
{
	digitalWrite(m_pinSTB, LOW);
	bitDelay();
}

void SC1628D::stop()
{
	digitalWrite(m_pinSTB, HIGH);
	bitDelay();
}

void SC1628D::writeCommand(uint8_t b)
{
	uint8_t data = b;

	// 8 Data Bits
	for(uint8_t i = 0; i < 8; i++) 
	{
		// CLK low
		digitalWrite(m_pinCLK, LOW);

		// Set data bit
		if (data & 0x01)
			digitalWrite(m_pinDIO, HIGH);
		else
			digitalWrite(m_pinDIO, LOW);

		bitDelay();

		// CLK high
		digitalWrite(m_pinCLK, HIGH);
		bitDelay();

		data = data >> 1;
	}
	bitDelay();
}

void SC1628D::writeData(uint16_t b)
{
	uint16_t data = b;

	// 8 Data Bits
	for(uint8_t i = 0; i < 16; i++) 
	{
		// CLK low
		digitalWrite(m_pinCLK, LOW);

		// Set data bit
		if (data & 0x01)
			digitalWrite(m_pinDIO, HIGH);
		else
			digitalWrite(m_pinDIO, LOW);

		bitDelay();

		// CLK high
		digitalWrite(m_pinCLK, HIGH);
		bitDelay();

		data = data >> 1;
	}
	bitDelay();
}

void SC1628D::writeMatrix(const uint16_t matrix[])
{
    // Command 2: Set Write data to display, in incremental mode
	start();
	writeCommand(SC1628D_DATA_SETTING_CMD_WRITE);
	stop();

	// Command 3: Set write address + digits data
	start();
	writeCommand(SC1628D_ADDRESS_SETTING_CMD);

	// Write the data bytes
	for (uint8_t k = 0; k < 7; k++)
    {
		writeData(matrix[k]);
    }
	stop();
}

uint8_t SC1628D::receiveData()
{
	uint8_t temp = 0;

	// Pull-up on
//    pinMode(m_pinDIO, INPUT);
//	digitalWrite(m_pinDIO, HIGH);
    pinMode(m_pinDIO, INPUT_PULLUP);

	for (int i = 0; i < 8; i++) {
		temp >>= 1;

		// CLK low
		digitalWrite(m_pinCLK, LOW);

		bitDelay();

		if (digitalRead(m_pinDIO)) {
			temp |= 0x80;
		}

		// CLK high
		digitalWrite(m_pinCLK, HIGH);
		bitDelay();
	}

	// Pull-up off
	pinMode(m_pinDIO, OUTPUT);
//	digitalWrite(m_pinDIO, LOW);

	return temp;
}

