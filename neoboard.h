/*
  neoboard.h - fork of PS2Keyboard library for experimentation purposes
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

  ** Mostly rewritten Paul Stoffregen <paul@pjrc.com>, June 2010
  ** Modified for use with Arduino 13 by L. Abraham Smith, 
     <n3bah@microcompdesign.com> ** 
  ** Modified for easy interrup pin assignement 
     on method begin(datapin,irq_pin). 
     Cuningan <cuninganreset@gmail.com> **
  ** Reperposed for PS2 --> bluefruit conversions 
     Paul Beaudet <inof8or@gmail.com> Fall 2014 **

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
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 
  USA
*/


#ifndef neoboard_h
#define neoboard_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h" // for attachInterrupt, FALLING
#else
#include "WProgram.h"
#endif

#include "utility/int_pins.h"

#define PS2_KEYMAP_SIZE 136 // why 136? 

//Bluefruit keymaping - Outgoing communication
// might as well reuse these bytes if possible rigt?
// Key definitions for sending raw key data to bluefruit
#define MODIFIER_CONTROL_LEFT  byte((1<<0))
#define MODIFIER_SHIFT_LEFT    byte((1<<1))
#define MODIFIER_ALT_LEFT      byte((1<<2))
#define MODIFIER_GUI_LEFT      byte((1<<3))
#define MODIFIER_CONTROL_RIGHT byte((1<<4))
#define MODIFIER_SHIFT_RIGHT   byte((1<<5))
#define MODIFIER_ALT_RIGHT     byte((1<<6))
#define MODIFIER_GUI_RIGHT     byte((1<<7))
//---------------Keys----------------------
#define KEY_A                  byte(0x04)
#define KEY_B                  byte(0x05)
#define KEY_C                  byte(0x06)
#define KEY_D                  byte(0x07)
#define KEY_E                  byte(0x08)
#define KEY_F                  byte(0x09)
#define KEY_G                  byte(0x0A)
#define KEY_H                  byte(0x0B)
#define KEY_I                  byte(0x0C)
#define KEY_J                  byte(0x0D)
#define KEY_K                  byte(0x0E)
#define KEY_L                  byte(0x0F)
#define KEY_M                  byte(0x10)
#define KEY_N                  byte(0x11)
#define KEY_O                  byte(0x12)
#define KEY_P                  byte(0x13)
#define KEY_Q                  byte(0x14)
#define KEY_R                  byte(0x15)
#define KEY_S                  byte(0x16)
#define KEY_T                  byte(0x17)
#define KEY_U                  byte(0x18)
#define KEY_V                  byte(0x19)
#define KEY_W                  byte(0x1A)
#define KEY_X                  byte(0x1B)
#define KEY_Y                  byte(0x1C)
#define KEY_Z                  byte(0x1D)
#define KEY_1                  byte(0x1E)
#define KEY_2                  byte(0x1F)
#define KEY_3                  byte(0x20)
#define KEY_4                  byte(0x21)
#define KEY_5                  byte(0x22)
#define KEY_6                  byte(0x23)
#define KEY_7                  byte(0x24)
#define KEY_8                  byte(0x25)
#define KEY_9                  byte(0x26)
#define KEY_0                  byte(0x27)
#define KEY_RETURN             byte(0x28)
#define KEY_ESCAPE             byte(0x29)
#define KEY_BACKSPACE          byte(0x2A)
#define KEY_TAB                byte(0x2B)
#define KEY_SPACE              byte(0x2C)
#define KEY_MINUS              byte(0x2D)
#define KEY_EQUAL              byte(0x2E)
#define KEY_BRACKET_LEFT       byte(0x2F)
#define KEY_BRACKET_RIGHT      byte(0x30)
#define KEY_BACKSLASH          byte(0x31)
#define KEY_SEMICOLON          byte(0x33)
#define KEY_APOSTROPHE         byte(0x34)
#define KEY_GRAVE              byte(0x35)
#define KEY_COMMA              byte(0x36)
#define KEY_PERIOD             byte(0x37)
#define KEY_SLASH              byte(0x38)
#define KEY_CAPS_LOCK          byte(0x39)
#define KEY_F1                 byte(0x3A)
#define KEY_F2                 byte(0x3B)
#define KEY_F3                 byte(0x3C)
#define KEY_F4                 byte(0x3D)
#define KEY_F5                 byte(0x3E)
#define KEY_F6                 byte(0x3F)
#define KEY_F7                 byte(0x40)
#define KEY_F8                 byte(0x41)
#define KEY_F9                 byte(0x42)
#define KEY_F10                byte(0x43)
#define KEY_F11                byte(0x44)
#define KEY_F12                byte(0x45)
#define KEY_PRINT_SCREEN       byte(0x46)
#define KEY_SCROLL_LOCK        byte(0x47)
#define KEY_PAUSE              byte(0x48)
#define KEY_INSERT             byte(0x49)
#define KEY_HOME               byte(0x4A)
#define KEY_PAGE_UP            byte(0x4B)
#define KEY_DELETE             byte(0x4C)
#define KEY_END                byte(0x4D)
#define KEY_PAGE_DOWN          byte(0x4E)
#define KEY_ARROW_RIGHT        byte(0x4F)
#define KEY_ARROW_LEFT         byte(0x50)
#define KEY_ARROW_DOWN         byte(0x51)
#define KEY_ARROW_UP           byte(0x52)
#define KEY_NUM_LOCK           byte(0x53)
#define KEY_KEYPAD_DIVIDE      byte(0x54)
#define KEY_KEYPAD_MULTIPLY    byte(0x55)
#define KEY_KEYPAD_SUBTRACT    byte(0x56)
#define KEY_KEYPAD_ADD         byte(0x57)
#define KEY_KEYPAD_ENTER       byte(0x58)
#define KEY_KEYPAD_1           byte(0x59)
#define KEY_KEYPAD_2           byte(0x5A)
#define KEY_KEYPAD_3           byte(0x5B)
#define KEY_KEYPAD_4           byte(0x5C)
#define KEY_KEYPAD_5           byte(0x5D)
#define KEY_KEYPAD_6           byte(0x5E)
#define KEY_KEYPAD_7           byte(0x5F)
#define KEY_KEYPAD_8           byte(0x60)
#define KEY_KEYPAD_9           byte(0x61)
#define KEY_KEYPAD_0           byte(0x62)
#define KEY_KEYPAD_DECIMAL     byte(0x63)
#define KEY_EUROPE_2           byte(0x64)
#define KEY_APPLICATION        byte(0x65)
#define KEY_POWER              byte(0x66)
#define KEY_KEYPAD_EQUAL       byte(0x67)
//-------alt layout----------------//Dvorak
//Change here --V--       True mapping-----v 
#define ALT_KEY_A              byte(0x04)//A
#define ALT_KEY_N              byte(0x05)//B
#define ALT_KEY_I              byte(0x06)//C
#define ALT_KEY_H              byte(0x07)//D
#define ALT_KEY_D              byte(0x08)//E
#define ALT_KEY_Y              byte(0x09)//F
#define ALT_KEY_U              byte(0x0A)//G
#define ALT_KEY_J              byte(0x0B)//H
#define ALT_KEY_G              byte(0x0C)//I
#define ALT_KEY_C              byte(0x0D)//J
#define ALT_KEY_V              byte(0x0E)//K
#define ALT_KEY_P              byte(0x0F)//L
#define ALT_KEY_M              byte(0x10)//M
#define ALT_KEY_L              byte(0x11)//N
#define ALT_KEY_S              byte(0x12)//O
#define ALT_KEY_R              byte(0x13)//P
#define ALT_KEY_X              byte(0x14)//Q
#define ALT_KEY_O              byte(0x15)//R
#define ALT_KEY_SEMICOLON      byte(0x16)//S
#define ALT_KEY_K              byte(0x17)//T
#define ALT_KEY_F              byte(0x18)//U
#define ALT_KEY_PERIOD         byte(0x19)//V
#define ALT_KEY_COMMA          byte(0x1A)//W
#define ALT_KEY_B              byte(0x1B)//X
#define ALT_KEY_T              byte(0x1C)//Y
#define ALT_KEY_SLASH          byte(0x1D)//Z
#define ALT_KEY_APOSTROPHE     byte(0x2D)//-
#define ALT_KEY_BRACKET_RIGHT  byte(0x2E)//=
#define ALT_KEY_MINUS          byte(0x2F)//{
#define ALT_KEY_EQUAL          byte(0x30)//}
#define ALT_KEY_BACKSLASH      byte(0x31)// backslash
#define ALT_KEY_Z              byte(0x33)// semicolon
#define ALT_KEY_Q              byte(0x34)//'
#define ALT_KEY_GRAVE          byte(0x35)//`
#define ALT_KEY_W              byte(0x36)//,
#define ALT_KEY_E              byte(0x37)//.
#define ALT_KEY_BRACKET_LEFT   byte(0x38)///
//             |^|                        |^|
//Change Here>>|*|-----actual mapping--->>|^|                   

typedef struct 
{
	uint8_t noshift[PS2_KEYMAP_SIZE];
	uint8_t shift[PS2_KEYMAP_SIZE];
	uint8_t uses_altgr;
	uint8_t altgr[PS2_KEYMAP_SIZE];
} PS2Keymap_t;


extern const PROGMEM PS2Keymap_t PS2Keymap_US;

/**
 * Purpose: Provides an easy access to PS2 keyboards
 * Author:  Christian Weichel
 */
class BTKeyboard {
  public:
  	/**
  	 * This constructor does basically nothing. Please call the begin(int,int)
  	 * method before using any other method of this class.
  	 */
    BTKeyboard();
    
    /**
     * Starts the keyboard "service" by registering the external interrupt.
     * setting the pin modes correctly and driving those needed to high.
     * The propably best place to call this method is in the setup routine.
     */
    static void begin(uint8_t dataPin, uint8_t irq_pin, const PS2Keymap_t &map = PS2Keymap_US);
    
    /**
     * Returns true if there is a char to be read, false if not.
     */
    static bool available();
    
    /**
     * Returns the char last read from the keyboard.
     * If there is no char availble, -1 is returned.
     *?? how are keycobinations be returned? 
     */
    static int read();
    
    static void out(uint8_t modifiers = 0, uint8_t keycode=0); // sends output signal
    static void toggle(); 
};

#endif
