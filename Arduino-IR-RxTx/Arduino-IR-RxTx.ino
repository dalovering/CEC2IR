// This Arduino firmware serves two purposes: 
//    + Bridge from serial to an IR blaster output on pin 3
//    + Print out any IR codes recieved on IR Receiver TSOP4838 connected to Pin 2
//
// It is originally intended to work on an arduino nano clone, 
// but should work on any MCU that supports the IRremote.h library
//
// This firmware works with NEC codes. Your device may use different types of codes.
// See IRremote documentation to determine how to modify firmware to send different codes.
//
// Upon receiving an IR code as a hex string e.g. "0xFF827D\r\n" on the serial bus, the
// device will interpret that code as an NEC IR Code and transmit it on Pin 3.

//------------------------------------------------------------------------------
// Include the IRremote library header
//
#include <IRremote.h>
//------------------------------------------------------------------------------
// Tell IRremote which Arduino pin is connected to the IR Receiver (TSOP4838)

#define IR_RECEIVE_PIN 2
#define IR_SEND_PIN 3
IRrecv IrReceiver(IR_RECEIVE_PIN);
IRsend IrSender;
uint64_t sData = 0xFFFFFF;
String inputString = "";         // a String to hold incoming data
bool stringComplete = false; 

//+=============================================================================
// Configure the Arduino
//
void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    inputString.reserve(200);

    IrReceiver.enableIRIn();  // Start the receiver
    IrReceiver.blink13(true); // Enable feedback LED

    Serial.print(F("Ready to receive IR signals at pin "));
    Serial.println(IR_RECEIVE_PIN);
}

//+=============================================================================
// Dump out the decode_results structure.
//
void dumpInfo() {
    // Check if the buffer overflowed
    if (IrReceiver.results.overflow) {
        Serial.println("IR code too long. Edit IRremoteInt.h and increase RAW_BUFFER_LENGTH");
        return;
    }

    IrReceiver.printResultShort(&Serial);

    Serial.print(" (");
    Serial.print(IrReceiver.results.bits, DEC);
    Serial.println(" bits)");
}

//+=============================================================================
// The repeating section of the code
//
void loop() {

    if (stringComplete) {
      Serial.print("Sending ");
      Serial.println(inputString);
      sData = StrToHex(inputString);
      IrSender.sendNEC(sData,32);
      //Serial.println(sData);
      inputString = "";
      stringComplete = false;
    }
    
    if (IrReceiver.decode()) {  // Grab an IR code
      dumpInfo();             // Output the results
        //IrReceiver.printIRResultRawFormatted(&Serial);  // Output the results in RAW format
        //Serial.println();                               // blank line between entries
        //IrReceiver.printIRResultAsCArray(&Serial);      // Output the results as source code array
        //IrReceiver.printIRResultAsCVariables(&Serial);  // Output address and data as source code variables
      delay(50);
      IrReceiver.resume();                            // Prepare for the next value
    }
}



void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if ( (inChar == '\n') or (inChar == '\r')) {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}

uint64_t StrToHex(const String str)
{
  return (uint64_t) strtoul(str.c_str(), 0, 16);
}
