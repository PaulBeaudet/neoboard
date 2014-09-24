/*
  neoboard.cpp - fork of PS2Keyboard library for experimentation purposes
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

  ** Mostly rewritten Paul Stoffregen <paul@pjrc.com> 2010, 2011
  ** Modified for use beginning with Arduino 13 by L. Abraham Smith, 
     <n3bah@microcompdesign.com> ** 
  ** Modified for easy interrup pin assignement on method begin(datapin,irq_pin) 
     Cuningan <cuninganreset@gmail.com> **
  ** Reperposed for PS2 --> bluefruit conversions 
     Paul Beaudet <inof8or@gmail.com> fall 2014 **

  for more information you can read the original wiki in arduino.cc
  at http://www.arduino.cc/playground/Main/PS2Keyboard
  or http://www.pjrc.com/teensy/td_libs_PS2Keyboard.html

  Version 2.4 (March 2013)
  - Support Teensy 3.0, Arduino Due, Arduino Leonardo & other boards
  - French keyboard layout, David Chochoi, tchoyyfr at yahoo dot fr

  Version 2.3 (October 2011)
  - Minor bugs fixed

  Version 2.2 (August 2011)
  - Support non-US keyboards - thanks to Rainer Bruch for a German keyboard :)

  Version 2.1 (May 2011)
  - timeout to recover from misaligned input
  - compatibility with Arduino "new-extension" branch
  - TODO: send function, proposed by Scott Penrose, scooterda at me dot com

  Version 2.0 (June 2010)
  - Buffering added, many scan codes can be captured without data loss
    if your sketch is busy doing other work
  - Shift keys supported, completely rewritten scan code to ascii
  - Slow linear search replaced with fast indexed table lookups
  - Support for Teensy, Arduino Mega, and Sanguino added

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "neoboard.h"

#define BT_OUTPUT Serial.write // this can be adjusted per arduino in future

// whoa watch out here is some global shit
#define BUFFER_SIZE 45 // why the big buffer? just drop em if they didn't get
// there fast enough, users will be confused if there is lag anyhow
static volatile uint8_t buffer[BUFFER_SIZE];// how the buffer is used?
static volatile uint8_t head, tail; 
static uint8_t DataPin;
static uint8_t CharBuffer=0;
static uint8_t UTF8next=0;
static const PS2Keymap_t *keymap=NULL;
static uint8_t GLOBAL_MOD = 0; // Modifier shared between getCode() and out() 
// makes shift alt and control possible
static bool mapShift = 0; // toggle for switching keymap 

// The ISR for the external interrupt
void ps2interrupt(void)
{
	static uint8_t bitcount=0;
	static uint8_t incoming=0;
	static uint32_t prev_ms=0;
	uint32_t now_ms;
	uint8_t n, val;

	val = digitalRead(DataPin);
	now_ms = millis();
	if (now_ms - prev_ms > 250) 
	{// timing exlaination? //if 250ms elapsed set bit and incoming back
		bitcount = 0;
		incoming = 0;
	}
	prev_ms = now_ms;// update stored time
	n = bitcount - 1; // ?
	if (n <= 7) {incoming |= (val << n);}//
	bitcount++;
	if (bitcount == 11) 
	{// explain this condition
		uint8_t i = head + 1;
		if (i >= BUFFER_SIZE) {i = 0;}
		if (i != tail) 
		{
			buffer[i] = incoming;
			head = i;
		}
		bitcount = 0;
		incoming = 0;
	}
}

static inline uint8_t get_scan_code(void)
{
	uint8_t c, i;

	i = tail;
	if (i == head) {return 0;}
	i++;
	if (i >= BUFFER_SIZE) {i = 0;}
	c = buffer[i];
	tail = i;
	return c;
}

// http://www.quadibloc.com/comp/scan.htm
// http://www.computer-engineering.org/ps2keyboard/scancodes2.html

// These arrays provide a simple key map, to turn scan codes into ISO8859
// output.  If a non-US keyboard is used, these may need to be modified
// for the desired output.
// interation of the array in hex corrisponds to represented scan code
const PROGMEM PS2Keymap_t PS2Keymap_US = 
{// 8 by 16 plus a row of 4  
	{ // without shift //where are the gui keys?
	0, KEY_F9, 0, KEY_F5, KEY_F3, KEY_F1, KEY_F2, KEY_F12,
	0, KEY_F10, KEY_F8, KEY_F6, KEY_F4, KEY_TAB, KEY_GRAVE, 0,
	0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, KEY_Q, KEY_1, 0,
	0, 0, KEY_Z, KEY_S, KEY_A, KEY_W, KEY_2, 0,
	0, KEY_C, KEY_X, KEY_D, KEY_E, KEY_4, KEY_3, 0,
	0, KEY_SPACE, KEY_V, KEY_F, KEY_T, KEY_R, KEY_5, 0,
	0, KEY_N, KEY_B, KEY_H, KEY_G, KEY_Y, KEY_6, 0,
	0, 0, KEY_M, KEY_J, KEY_U, KEY_7, KEY_8, 0,
	0, KEY_COMMA, KEY_K, KEY_I, KEY_O, KEY_0, KEY_9, 0,
	0, KEY_PERIOD, KEY_SLASH /*'/'*/, KEY_L, KEY_SEMICOLON, KEY_P, KEY_MINUS, 0,
	0, 0, KEY_APOSTROPHE, 0, KEY_BRACKET_LEFT, KEY_EQUAL, 0, 0,
	KEY_BACKSPACE /*Caps*/, 0 /*Rshift*/, KEY_RETURN /*Enter*/, KEY_BRACKET_RIGHT, 0, KEY_BACKSLASH, 0, 0,
	0, 0, 0, 0, 0, 0, KEY_BACKSPACE, 0,
	0, KEY_KEYPAD_1, 0, KEY_KEYPAD_4, KEY_KEYPAD_7, 0, 0, 0,
	KEY_KEYPAD_0, KEY_KEYPAD_DECIMAL, KEY_KEYPAD_2, KEY_KEYPAD_5, KEY_KEYPAD_6, KEY_KEYPAD_8, KEY_ESCAPE, KEY_NUM_LOCK /*NumLock*/,
	KEY_F11, KEY_KEYPAD_ADD, KEY_KEYPAD_3, KEY_KEYPAD_SUBTRACT, KEY_KEYPAD_MULTIPLY, KEY_KEYPAD_9, KEY_SCROLL_LOCK, 0,
	0, 0, 0, KEY_F7 
	},
	{  // alternitive layout
	0, KEY_F9, 0, KEY_F5, KEY_F3, KEY_F1, KEY_F2, KEY_F12,
	0, KEY_F10, KEY_F8, KEY_F6, KEY_F4, KEY_TAB, ALT_KEY_GRAVE, 0,
	0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, ALT_KEY_Q, KEY_1, 0,
	0, 0, ALT_KEY_Z, ALT_KEY_S, ALT_KEY_A, ALT_KEY_W, KEY_2, 0,
	0, ALT_KEY_C, ALT_KEY_X, ALT_KEY_D, ALT_KEY_E, KEY_4, KEY_3, 0,
	0, KEY_SPACE, ALT_KEY_V, ALT_KEY_F, ALT_KEY_T, ALT_KEY_R, KEY_5, 0,
	0, ALT_KEY_N, ALT_KEY_B, ALT_KEY_H, ALT_KEY_G, ALT_KEY_Y, KEY_6, 0,
	0, 0, ALT_KEY_M, ALT_KEY_J, ALT_KEY_U, KEY_7, KEY_8, 0,
	0, ALT_KEY_COMMA, ALT_KEY_K, ALT_KEY_I, ALT_KEY_O, KEY_0, KEY_9, 0,
	0, ALT_KEY_PERIOD, ALT_KEY_SLASH /*'/'*/, ALT_KEY_L, ALT_KEY_SEMICOLON, ALT_KEY_P, ALT_KEY_MINUS, 0,
	0, 0, ALT_KEY_APOSTROPHE, 0, ALT_KEY_BRACKET_LEFT, ALT_KEY_EQUAL, 0, 0,
	KEY_BACKSPACE /*Caps*/, 0 /*Rshift*/, KEY_RETURN /*Enter*/, ALT_KEY_BRACKET_RIGHT, 0, ALT_KEY_BACKSLASH, 0, 0,
	0, 0, 0, 0, 0, 0, KEY_BACKSPACE, 0,
	0, KEY_KEYPAD_1, 0, KEY_KEYPAD_4, KEY_KEYPAD_7, 0, 0, 0,
	KEY_KEYPAD_0, KEY_KEYPAD_DECIMAL, KEY_KEYPAD_2, KEY_KEYPAD_5, KEY_KEYPAD_6, KEY_KEYPAD_8, KEY_ESCAPE, KEY_NUM_LOCK /*NumLock*/,
	KEY_F11, KEY_KEYPAD_ADD, KEY_KEYPAD_3, KEY_KEYPAD_SUBTRACT, KEY_KEYPAD_MULTIPLY, KEY_KEYPAD_9, KEY_SCROLL_LOCK, 0,
	0, 0, 0, KEY_F7 
	}
};

#define BREAK     0x01 // 1  
#define MODIFIER  0x02 // 2
#define SHIFT_L   0x04 // 4
#define SHIFT_R   0x08 // 8
#define ALTGR     0x10 // 16
#define CTRL      0x20 // 32
#define ALT_L     0x40 // 64

static char getCode(void)
{
	static uint8_t state=0;
	uint8_t s;
	char c;

	while (1) // stay in this loop until a char is returned
	{// this really has implications for anthing else you might want to do
		s = get_scan_code(); //read the code at the "moment"
		if      (!s)        {return 0;} // If you get nothing no keys were pressed
		//this should now exit the loop making more sketch level code possible
		if      (s == 0xF0) {state |= BREAK;} // release key state (bit switch)
		else if (s == 0xE0) {state |= MODIFIER;} // Modification state (bit switch)
		else // Cases of actuation 
		{
			if (state & BREAK) //detect if last state was broken
			{//negate modified states given their release
				if(s == 0x12) 
				{
				  state &= ~SHIFT_L;
				  GLOBAL_MOD &= ~MODIFIER_SHIFT_LEFT;
				} 
				else if (s == 0x59) 
				{
				  state &= ~SHIFT_R;
				  GLOBAL_MOD &= ~MODIFIER_SHIFT_RIGHT;
				} 
				else if (s == 0x14) 
				{
				   state &= ~CTRL;
				   GLOBAL_MOD &= ~MODIFIER_CONTROL_LEFT;    
				} 
				else if (s == 0x11 && (state & MODIFIER))//right alt key 
				{
				  state &= ~ALTGR;
				  GLOBAL_MOD &= ~MODIFIER_ALT_RIGHT;
				}
				else if (s== 0x11)
				{
				  state &= ~ALT_L;
				  GLOBAL_MOD &= ~MODIFIER_ALT_LEFT;
				}
				state &= ~(BREAK | MODIFIER);// break is done, modifiers now irrelevent
				continue; 
				// in all break cases restart the loop to avoid returning anything
			}
			if (s == 0x12) // Left shift modifier detected
			{
				state |= SHIFT_L;
				GLOBAL_MOD |= MODIFIER_SHIFT_LEFT;
				continue;
			} 
			else if (s == 0x59) // right shift modifier detected
			{
				state |= SHIFT_R;
				GLOBAL_MOD |= MODIFIER_SHIFT_RIGHT;
				continue;
			}
			else if (s == 0x14) //control modifier detected (is this just one?)
			{
				state |= CTRL;
				GLOBAL_MOD |= MODIFIER_CONTROL_LEFT;
				continue;
			}  
			else if (s == 0x11 && (state & MODIFIER)) 
			{
			  state |= ALTGR;
			  GLOBAL_MOD |= MODIFIER_ALT_RIGHT;
			  continue;
			}
			else if (s == 0x11)
			{
			  state |= ALT_L;
			  GLOBAL_MOD |= MODIFIER_ALT_LEFT;
			  continue;
			}
			c = 0; // why was c intialized here as opposed to when it was istantiated?
			if (state & MODIFIER) // two make char cases * OxE0 *
			{
				switch (s) 
				{
				  case 0x70: c = KEY_INSERT;      break;
				  case 0x6C: c = KEY_HOME;        break;
				  case 0x7D: c = KEY_PAGE_UP;     break;
				  case 0x71: c = KEY_DELETE;      break;
				  case 0x69: c = KEY_END;         break;
				  case 0x7A: c = KEY_PAGE_DOWN;   break;
				  case 0x75: c = KEY_ARROW_UP;    break;
				  case 0x6B: c = KEY_ARROW_LEFT;  break;
				  case 0x72: c = KEY_ARROW_DOWN;  break;
				  case 0x74: c = KEY_ARROW_RIGHT; break;
				  case 0x4A: c = KEY_SLASH;       break;
				  case 0x5A: c = KEY_RETURN;      break;
				  default: break;
				}
			} 
			else //---Read the key map-- one make char case
			{
			  if(mapShift)
			  {
			    if (s< PS2_KEYMAP_SIZE){c= pgm_read_byte(keymap->shift +s);}
			  }
			  else
			  {// in the typical case utilize the QWERTY layout
				  if (s < PS2_KEYMAP_SIZE){c = pgm_read_byte(keymap->noshift + s);}
				}
			}//given we grab something from the main keyset we are on the path pass control to main sketch
			state &= ~(BREAK | MODIFIER); 
			//if we got to this point break or modifier need to be removed if they exist 
			if (c) {return c;} 
			//return the char we convert, modification is taken care of by GLOBAL_MOD in out() 
		}//End else
	}//End while
}
//----------bluefruit functions---------
void keyCommand(uint8_t modifiers = 0, uint8_t keycode = 0)
{
    BT_OUTPUT(0xFD); // command signal
    BT_OUTPUT(modifiers); // modifiers combine bit shifted state information 
    // in order to get multiple modifications
    BT_OUTPUT((byte)0x00); // 0x00
    BT_OUTPUT(keycode); // key code #1
    BT_OUTPUT(0x00); // key code #2
    BT_OUTPUT(0x00); // key code #3
    BT_OUTPUT(0x00); // key code #4
    BT_OUTPUT(0x00); // key code #5
    BT_OUTPUT(0x00); // key code #6
}

//-------- Object Methodes ---------------
bool BTKeyboard::available() 
{
	if (CharBuffer || UTF8next) {return true;}
	CharBuffer = getCode();// CharBuffer? another global?
	if (CharBuffer) {return true;}
	return false;
}

int BTKeyboard::read() {
	uint8_t result;

	result = UTF8next;
	if (result) {UTF8next = 0;} 
	else 
	{
		result = CharBuffer;
		if (result) {CharBuffer = 0;} 
		else {result = getCode();}
		if (result >= 128) 
		{
			UTF8next = (result & 0x3F) | 0x80;
			result = ((result >> 6) & 0x1F) | 0xC0;
		}
	}
	if (!result) {return -1;}
	return result;
}

BTKeyboard::BTKeyboard() {} // nothing to do here, begin() does it all
  
void BTKeyboard::begin(uint8_t data_pin, uint8_t irq_pin, const PS2Keymap_t &map) 
{
  Serial.begin(9600);// start serial for Bluefruit
  uint8_t irq_num=255;

  DataPin = data_pin;
  keymap = &map;

  // initialize the pins
#ifdef INPUT_PULLUP
  pinMode(irq_pin, INPUT_PULLUP);
  pinMode(data_pin, INPUT_PULLUP);
#else
  pinMode(irq_pin, INPUT);
  digitalWrite(irq_pin, HIGH);
  pinMode(data_pin, INPUT);
  digitalWrite(data_pin, HIGH);
#endif

#ifdef CORE_INT_EVERY_PIN
  irq_num = irq_pin;

#else
  switch(irq_pin) 
  {
    #ifdef CORE_INT0_PIN
    case CORE_INT0_PIN:
      irq_num = 0;
      break;
    #endif
    #ifdef CORE_INT1_PIN
    case CORE_INT1_PIN:
      irq_num = 1;
      break;
    #endif
    #ifdef CORE_INT2_PIN
    case CORE_INT2_PIN:
      irq_num = 2;
      break;
    #endif
    #ifdef CORE_INT3_PIN
    case CORE_INT3_PIN:
      irq_num = 3;
      break;
    #endif
    #ifdef CORE_INT4_PIN
    case CORE_INT4_PIN:
      irq_num = 4;
      break;
    #endif
    #ifdef CORE_INT5_PIN
    case CORE_INT5_PIN:
      irq_num = 5;
      break;
    #endif
    #ifdef CORE_INT6_PIN
    case CORE_INT6_PIN:
      irq_num = 6;
      break;
    #endif
    #ifdef CORE_INT7_PIN
    case CORE_INT7_PIN:
      irq_num = 7;
      break;
    #endif
  }
#endif

  head = 0;
  tail = 0;
  if (irq_num < 255) {attachInterrupt(irq_num, ps2interrupt, FALLING);}
}


void BTKeyboard::out(uint8_t modifiers, uint8_t keycode)
{ 
    uint8_t mod;//switch variable for modifications
    
    if(modifiers) { mod = modifiers;}// given an argument passed, ignore keys
    else          { mod = GLOBAL_MOD;} // modifications as true to keys
    
    keyCommand(mod, keycode);
    //----------------- Send break ! --- important or keys stay depressed
    if(keycode)
    {
      keyCommand();//empty == break, arguments default to zero
    }
}

void BTKeyboard::toggle(void)
{
   mapShift = !mapShift;
}
