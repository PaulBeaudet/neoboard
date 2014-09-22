  /*  neoboard - forked PS2Keyboard library for ps2 -> bluefruit
    
    For more information you can read the original wiki in arduino.cc
    at http://www.arduino.cc/playground/Main/PS2Keyboard
    or http://www.pjrc.com/teensy/td_libs_PS2Keyboard.html
    
    Important!! reffer to output mapping in the modified library for key definitions
    Though one may likely be able to guess them based on the pattern shown
  */   
  #include <neoboard.h>

  const int DataPin = 3;
  const int IRQpin =  2;
  
  BTKeyboard keyboard;
  
  void setup() 
  {
    delay(2000);
    keyboard.begin(DataPin, IRQpin);
  }
  
  void loop() 
  {
    if (keyboard.available()) 
    { 
      char nextKey = keyboard.read();// read the next key  
      
      // Use else if and keyboard.out for personal modifications, this over-rides default keyboard actions
      if      (nextKey == KEY_ESCAPE){keyboard.out(MODIFIER_ALT_LEFT, KEY_F4);}
      // ESC to alt-F4 (close program)
      else if (nextKey == KEY_F1)    {keyboard.out(MODIFIER_ALT_LEFT, KEY_TAB);}
      //switch application !! just does last application in this way
      
      // otherwise, just print all normal characters
      else {keyboard.out(0, nextKey);} // 0 is passed as first argument to signal to use keyboard modifiers  
    }
  }
