/*
 *  SC1628D.h
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

#ifndef __SC1628D__
#define __SC1628D__

#include <inttypes.h>

#define SG1     0x001
#define SG2     0x002
#define SG3     0x004
#define SG4     0x008
#define SG5     0x010
#define SG6     0x020
#define SG7     0x040
#define SG8     0x080
#define SG9     0x100
#define SG10    0x200
#define SG11    0x400
#define SG12    0x800

#define SEG_A   0b00000001
#define SEG_B   0b00000010
#define SEG_C   0b00000100
#define SEG_D   0b00001000
#define SEG_E   0b00010000
#define SEG_F   0b00100000
#define SEG_G   0b01000000
  
#define DIGIT_BLANK             16
#define DIGIT_MINUS             17
#define DIGIT_DEGRE             18
#define DIGIT_C                 19
#define DIGIT_QUESTION          20

#define SC1628D_BIT_DELAY        5

#define SC1628D_6GRID_12SEG      2
#define SC1628D_7GRID_11SEG      3
#define SC1628D_2_FIXED_ADDR     4
#define SC1628D_2_INCREMENT_ADDR 0


// Default charaters definition
extern const uint8_t SC1628D_NORMAL_FONT[];

// Default charaters definition
extern const uint8_t SC1628D_INVERTED_FONT[];

// Default function used to build the message3's datas from the segment mask array
void SC1628D_NormalDisplay(uint8_t digit[], uint16_t matrix[]);

// Default function used to build the message3's datas from the segment mask array on a reversed screen
void SC1628D_InvertedDisplay(uint8_t digit[], uint16_t matrix[]);


class SC1628D {

public:
	// Initialize a SC1628D object, setting the clock and
	// data pins.
	//
	// @param pinSTB - The number of the digital pin connected to the STB pin of the module
	// @param pinCLK - The number of the digital pin connected to the clock pin of the module
	// @param pinDIO - The number of the digital pin connected to the DIO pin of the module
	// @param bitDelay - The delay, in microseconds, between bit transition on the serial bus
	//                   connected to the display
	//                  
	SC1628D(uint8_t pinSTB, uint8_t pinCLK, uint8_t pinDIO, unsigned int bitDelay = SC1628D_BIT_DELAY);

	// Clear the display
	//                  
	void clear();

	// Sets the brightness of the display.
	//
	// The setting takes effect when a command is given to change the data being
	// displayed. This take effect on the next display refresh.
	//
	// @param brightness A number from 0 (lowes brightness) to 7 (highest brightness)
	// @param on Turn display on or off
	//                  
	void setBrightness(uint8_t brightness, bool on = true);

	// Set the digits table to character to a segments set
	//
	// @param font An array of digits segments definition
	//                  
	void setFont(uint8_t font[] = SC1628D_NORMAL_FONT);

	// Set the function to transform the 7 segment mask to the SC1628D memory map
	//
	// @param aFilterFunction A function address to transform the digit array to a matrixdata array
	//                  
	void setFilter(void (*aFilterFunction)(uint8_t digit[], uint16_t matrix[]));

	// Update a digit and refresh the screen
	//
	// This function receives a digit as input and update displays. 
	//
	// @param digit A digit to display
	// @param pos The position from which to display this digitn (0 - leftmost, 4 - rightmost)
	//                  
	void displayDigit(const uint8_t digit, uint8_t pos = 0);

	// Display a text on the screen
	//
	// This function receives an array of 5 digits values as input and displays them. 
	//
	// @param digits An array of digits to display
	// @param shape  The grid font for digits. This is a segments mask for each digits.
	//                  
	void displayDigits(const uint8_t digits[], uint8_t pos = 0, uint8_t length = 5);

	// Update a digit's segments on the module
	//
	// This function receives a segment mask as input and refresh display. 
	//
	// @param segment A segment mask to display
	// @param pos The position from which to start the modification (0 - leftmost, 4 - rightmost)
	//                  
	void displaySegment(const uint8_t segment, uint8_t pos = 0);

	// Display segments on the module
	//
	// This function receives an array of 5 segments mask as input and displays them. 
	//
	// @param segments An array of segments to display
	// @param pos The position from which to start the modification (0 - leftmost, 4 - rightmost)
	// @param length The number of digits to be modified from 0 to 5
	//                  
	void displaySegments(const uint8_t segments[], uint8_t pos = 0, uint8_t length = 5);

	// Get pressed buttons code
	//
	// @return a mask of pressed buttons, K1 from bit 0 to bit 9, K2 from bit 16 to bit 25.
	//                  
    uint32_t getButtons();


protected:
	void bitDelay();
	void start();
	void stop();
	void writeCommand(uint8_t b);
	void writeData(uint16_t b);
	void writeMatrix(const uint16_t matrix[]);
	uint8_t receiveData();

private:
	void (*m_filter)(uint8_t digits[], uint16_t matrix[]);
	uint8_t m_pinSTB;
	uint8_t m_pinCLK;
	uint8_t m_pinDIO;
	uint8_t m_brightness;
	unsigned int m_bitDelay;
	uint8_t m_mode;
	uint8_t *m_font;
	uint8_t m_segments[7];
};

#endif // __SC1628D__
