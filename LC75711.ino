/**********************************************************************/
/*This code is not clean and OFFCOURSE will be possible do it better! */
/*this is only a group of functions to be used as independent code,  */
/*and by this way will be possible to any person can do the changes   */
/*of code and see what happen.                                         */
/*The code don't depend of any external library or extenal functions  */
/*complicated.                                                         */
/*I'm let the values to sent as binnary, this allow swap bit by bit */
/* to is possible test segment by segment without convert to HEX    */
/**********************************************************************/
/* This code show how is possible work with the driver LC75711 Sanyo */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VFD_in 8    // This is the pin number 8 on Arduino UNO
#define VFD_clk 9   // This is the pin number 9 on Arduino UNO
#define VFD_ce 10   // This is the pin number 10 on Arduino UNO
#define VFD_rst 11  // This is the pin number 11 on Arduino UNO  //If you want, you can put this pin (/INH) at HIGH with a resistor of 10K
//ATT: On the Uno and other ATMEGA based boards, unsigned ints (unsigned integers) are the same as ints in that they store a 2 byte value.
//Long variables are extended size variables for number storage, and store 32 bits (4 bytes), from -2,147,483,648 to 2,147,483,647.

//*************************************************//
unsigned int shiftBit = 0;
unsigned int nBitOnBlock = 0;  // Used to count number of bits and split to 8 bits... (number of byte)
unsigned int nByteOnBlock = 0;
unsigned int sequencyByte = 0x00;
byte Aa, Ab, Ac, Ad, Ae, Af, Ag, Ah, Ai, Aj, Ak, Al, Am, An, Ao, Ap, Aq, Ar, As, At;
byte blockBit = 0x00;
byte low = 0x00;
byte high = 0x00;

byte data[12];
byte dataIndex;

unsigned char addr = 0b01100111; //This is the address of LC75711, see datasheet, it is the byte proceed before every commmand of 24 bits or 56 bits

//#define BUTTON_PIN 2  //Att check wich pins accept interrupts... Uno is 2 & 3
volatile byte buttonReleased = false;
uint16_t ascii = 0x0000;
int buttonState = 0;  // variable for reading the pushbutton status
//

const unsigned short asciiShortId[38] = {
  's', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'X', 'Y', 'Z'
};
//
void LC75711_init(void) {
  unsigned char i;
 
  digitalWrite(VFD_rst, LOW);
  delay(4);
  digitalWrite(VFD_rst, HIGH);
  delayMicroseconds(300);  //power_up delay

  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(2);
  cmd_without_stb(addr);
  delayMicroseconds(2);
  digitalWrite(VFD_ce, HIGH);
  delayMicroseconds(2);
  cmd_without_stb(0b00000000);  cmd_without_stb(0b00000000); cmd_without_stb(0b01110000);  //
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW);

  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(2);
  cmd_without_stb(addr);
  delayMicroseconds(2);
  digitalWrite(VFD_ce, HIGH);
  delayMicroseconds(2);
  cmd_without_stb(0b00000000);  cmd_without_stb(0b00000000); cmd_without_stb(0b00111111);  // Define number of GRIDs.
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW);

  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(2);
  cmd_without_stb(addr);
  delayMicroseconds(2);
  digitalWrite(VFD_ce, HIGH);
  delayMicroseconds(2);
  cmd_without_stb(0b00000000);  cmd_without_stb(0xF0); cmd_without_stb(0b01010000);  // Adjust the VFD intensity (240 levels), 0xFF is maximum.
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW);

  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(2);
  cmd_without_stb(addr);
  delayMicroseconds(2);
  digitalWrite(VFD_ce, HIGH);
  delayMicroseconds(2);
  cmd_without_stb(0x00);  cmd_without_stb(0x00); cmd_without_stb(0b00010111);  // Display ON/OFF (Must be executed after Grid definition and dimmer.)
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW);

  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(2);
  cmd_without_stb(addr);
  delayMicroseconds(2);
  digitalWrite(VFD_ce, HIGH);
  delayMicroseconds(2);
  cmd_without_stb(0xFF);  cmd_without_stb(0xFF); cmd_without_stb(0b00011111);  // Turn ON VFD for only the grids G1 to G15 in MDATA
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW);

 clear_VFD();
}
void cmd_without_stb(unsigned char a) {
  // send without stb
  //
  unsigned char data = 170;    //value to transmit, binary 10101010
  unsigned char mask = 1;      //our bitmask

  data = a;
  //This don't send the strobe signal, to be used in burst data send
  for (mask = 0b00000001; mask > 0; mask <<= 1) {  //iterate through bit mask
    digitalWrite(VFD_clk, LOW);
    if (data & mask) {  // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
    } else {  //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
    }
    delayMicroseconds(1);
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(1);
  }
}
void clear_VFD(void) {
      for (uint8_t s = 0x00; s < 16; s++){
      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(0x20);  cmd_without_stb(s); cmd_without_stb(0b01100000);  // Here I fill all memory with spaces to clear LCD
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(1);
    }
}
void shiftLeft(){
  for (uint8_t s = 0x00; s < 0x0C; s++){
      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(0x00);  cmd_without_stb(0x00); cmd_without_stb(0b00100101);  // Here I fill all memory with spaces to clear LCD. Here is importante define move ADATA or MDATA or both!
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(200);
    }
}
void shiftRight(){
  for (uint8_t s = 0x00; s < 0x0C; s++){
      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(0x00);  cmd_without_stb(0x00); cmd_without_stb(0b00100100);  // Here I fill all memory with spaces to clear LCD. Here is importante define move ADATA or MDATA or both!
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(200);
    }
}
void drawPackMan(){
      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(0b11101110);  cmd_without_stb(0b11010111); cmd_without_stb(0b00011111); //First 24 bits of matrix 5x8
      cmd_without_stb(0b01111111);  cmd_without_stb(0b00000101); //Bits from D24 until D34, the remaning unti D39 is dont care!
      cmd_without_stb(0x00);  cmd_without_stb(0b10000000);  // Here I fill all memory with spaces to clear LCD
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(1);

      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(0b11101110);  cmd_without_stb(0b11010111); cmd_without_stb(0b00011111); //First 24 bits of matrix 5x8
      cmd_without_stb(0b10111111);  cmd_without_stb(0b00000010); //Bits from D24 until D34, the remaning unti D39 is dont care!
      cmd_without_stb(0x01);  cmd_without_stb(0b10000000);  // Here I fill all memory with spaces to clear LCD
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(1);

      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(0b11101110);  cmd_without_stb(0b11010111); cmd_without_stb(0b00011111); //First 24 bits of matrix 5x8
      cmd_without_stb(0b11111111);  cmd_without_stb(0b00000010); //Bits from D24 until D34, the remaning unti D39 is dont care!
      cmd_without_stb(0x02);  cmd_without_stb(0b10000000);  // Here I fill all memory with spaces to clear LCD
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(1);

      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(0b11101110);  cmd_without_stb(0b11010111); cmd_without_stb(0b00011111); //First 24 bits of matrix 5x8
      cmd_without_stb(0b10111111);  cmd_without_stb(0b00000110); //Bits from D24 until D34, the remaning unti D39 is dont care!
      cmd_without_stb(0x03);  cmd_without_stb(0b10000000);  // Here I fill all memory with spaces to clear LCD
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(1);

      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(0xFF);  cmd_without_stb(0xFF); cmd_without_stb(0xFF); //First 24 bits of matrix 5x8
      cmd_without_stb(0xFF);  cmd_without_stb(0xFF); //Bits from D24 until D34, the remaning unti D39 is dont care!
      cmd_without_stb(0x04);  cmd_without_stb(0b10000000);  // Here I fill all memory with spaces to clear LCD
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(1);

      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(0xFF);  cmd_without_stb(0xFF); cmd_without_stb(0xFF); //First 24 bits of matrix 5x8
      cmd_without_stb(0xFF);  cmd_without_stb(0xFF); //Bits from D24 until D34, the remaning unti D39 is dont care!
      cmd_without_stb(0x05);  cmd_without_stb(0b10000000);  // Here I fill all memory with spaces to clear LCD
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(1);

      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(0xFF);  cmd_without_stb(0xFF); cmd_without_stb(0xFF); //First 24 bits of matrix 5x8
      cmd_without_stb(0xFF);  cmd_without_stb(0xFF); //Bits from D24 until D34, the remaning unti D39 is dont care!
      cmd_without_stb(0x06);  cmd_without_stb(0b10000000);  // Here I fill all memory with spaces to clear LCD
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(1);
      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(0xFF);  cmd_without_stb(0xFF); cmd_without_stb(0xFF); //First 24 bits of matrix 5x8
      cmd_without_stb(0xFF);  cmd_without_stb(0xFF); //Bits from D24 until D34, the remaning unti D39 is dont care!
      cmd_without_stb(0x07);  cmd_without_stb(0b10000000);  // Here I fill all memory with spaces to clear LCD
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(1);
}
void customChars_VFD(void) {
  //The fill of CGRAM is filled with a command length of 56 bits, where the D0~D35 is bit of matrix 5x7
      for (uint8_t s = 0x00; s < 8; s++){
      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(0xFF);  cmd_without_stb(0xFF); cmd_without_stb(0xFF); //First 24 bits of matrix 5x8
      cmd_without_stb(0xFF);  cmd_without_stb(0xFF); //Bits from D24 until D34, the remaning unti D39 is dont care!
      cmd_without_stb(s);  cmd_without_stb(0b10000000);  // Here I fill all memory with spaces to clear LCD
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(1);
    }
}
void CGRAM_VFD(void) {
      for (uint8_t s = 0x00; s < 8; s++){
      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(s);  cmd_without_stb(s); cmd_without_stb(0b01100000);  // Here I fill all memory with spaces to clear LCD
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(1);
    }
}
void packManMoveRight(uint8_t grid){
 for (uint8_t s = 0x00; s < 4; s++){
      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(s);  cmd_without_stb(grid); cmd_without_stb(0b01100000);  // Here I fill all memory with spaces to clear LCD
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(100);
      clear_VFD();
    }
}
void packManMoveLeft(uint8_t grid){
  for (int s = 0x03; s >= 0; s--){
      digitalWrite(VFD_ce, LOW);
      delayMicroseconds(2);
      cmd_without_stb(addr);
      delayMicroseconds(2);
      digitalWrite(VFD_ce, HIGH);
      delayMicroseconds(2);
      cmd_without_stb(s);  cmd_without_stb(grid); cmd_without_stb(0b01100000);  // Here I fill all memory with spaces to clear LCD
      delayMicroseconds(2);
      digitalWrite(VFD_ce, LOW);
      delay(100);
      clear_VFD();
    }
}
void LC75711_print(unsigned char address, unsigned char *text) {
  //This function receive the grid where must start write and the string of text as a pointer.
  unsigned char c;
  unsigned char mem = 0x00;
  mem = address;
  
      while (c = (*text++)) {
        digitalWrite(VFD_ce, LOW);
        delayMicroseconds(2);
        cmd_without_stb(addr);
        delayMicroseconds(2);
        digitalWrite(VFD_ce, HIGH);
        delayMicroseconds(2);
       // Serial.print(mem, HEX);
        cmd_without_stb(c); cmd_without_stb(mem); cmd_without_stb(0x60);  // & 0x7F); // 0x7F to 7bits ASCII code
        mem++;
      }
  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(6);
  //Serial.println(":");
}
void msgHiFolks(){
  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(2);
  cmd_without_stb(addr);
  delayMicroseconds(2);
  digitalWrite(VFD_ce, HIGH);
  delayMicroseconds(2);;
  cmd_without_stb(0x48);  cmd_without_stb(0x00); cmd_without_stb(0b01100000);  // Letter H
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW);
  delay(1);

  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(2);
  cmd_without_stb(addr);
  delayMicroseconds(2);
  digitalWrite(VFD_ce, HIGH);
  delayMicroseconds(2);;
  cmd_without_stb(0x49);  cmd_without_stb(0x01); cmd_without_stb(0b01100000);  // Letter I
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW);
  delay(1);

  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(2);
  cmd_without_stb(addr);
  delayMicroseconds(2);
  digitalWrite(VFD_ce, HIGH);
  delayMicroseconds(2);;
  cmd_without_stb(0x20);  cmd_without_stb(0x02); cmd_without_stb(0b01100000);  // Letter Space
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW);
  delay(1);

  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(2);
  cmd_without_stb(addr);
  delayMicroseconds(2);
  digitalWrite(VFD_ce, HIGH);
  delayMicroseconds(2);;
  cmd_without_stb(0x46);  cmd_without_stb(0x03); cmd_without_stb(0b01100000);  // Letter F
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW);
  delay(1);

  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(2);
  cmd_without_stb(addr);
  delayMicroseconds(2);
  digitalWrite(VFD_ce, HIGH);
  delayMicroseconds(2);;
  cmd_without_stb(0x4F);  cmd_without_stb(0x04); cmd_without_stb(0b01100000);  // Letter O
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW);
  delay(1);

  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(2);
  cmd_without_stb(addr);
  delayMicroseconds(2);
  digitalWrite(VFD_ce, HIGH);
  delayMicroseconds(2);;
  cmd_without_stb(0x4C);  cmd_without_stb(0x05); cmd_without_stb(0b01100000);  // Letter L
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW);

  delay(1);
  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(2);
  cmd_without_stb(addr);
  delayMicroseconds(2);
  digitalWrite(VFD_ce, HIGH);
  delayMicroseconds(2);;
  cmd_without_stb(0x4B);  cmd_without_stb(0x06); cmd_without_stb(0b01100000);  // Letter K
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW);

  delay(1);
  digitalWrite(VFD_ce, LOW);
  delayMicroseconds(2);
  cmd_without_stb(addr);
  delayMicroseconds(2);
  digitalWrite(VFD_ce, HIGH);
  delayMicroseconds(2);;
  cmd_without_stb(0x53);  cmd_without_stb(0x07); cmd_without_stb(0b01100000);  // Letter S
  delayMicroseconds(2);
  digitalWrite(VFD_ce, LOW);
  delay(1);
}
void strrevert1(char *string) {
  // Inverter the contents of pointer of string
  // and let it reverted until the next call of
  // function! exp: char letter[16] = "jogos.....tarde";
  // To do a declaration of pointer:  char* ptr=letter;
  // don't forget the null char "\0", this is a 1 more char
  // presente on the string.

  int len = 0;
  while (string[len])
    len++;

  int down = 0;
  int up = len - 1;

  while (up > down) {
    char c = string[down];
    string[down++] = string[up];
    string[up--] = c;
  }
}
void msgEmpty() {
  strcpy(data, "            ");  // Fill the string with 12 spaces to stay empty
  strrevert1(data);              // Do the string reverting
  LC75711_print(0, data);        // write a grid number 1
  delay(300);
}
void msgWelcome() {
  strcpy(data, "  Welcome!  ");  // Fill the string
  //strrevert1(data);             // Do the string reverting
  LC75711_print(0, data);        // write a grid number 1
  delay(700);                    // Give time to see the message on VFD
}
void msgNumbers() {
  strcpy(data, " 9876543210 ");  // Fill the string
  strrevert1(data);              // Do the string reverting
  LC75711_print(0, data);        // write a grid number 1
  delay(1000);
}
void msgSanyo() {
  strcpy(data, " I'm driver ");
  //strrevert1(data);
  LC75711_print(0, data);
  delay(1000);
  strcpy(data, "  LC75711  ");
  //strrevert1(data);
  LC75711_print(0, data);
  delay(1000);
  strcpy(data, " from SANYO ");
  //strrevert1(data);
  LC75711_print(0, data);
  delay(1000);
}
void msgChars() {
  strcpy(data, "abcdefghij  ");
  //strrevert1(data);
  LC75711_print(0, data);
  delay(1000);
  strcpy(data, "klmnopqrst  ");
  //strrevert1(data);
  LC75711_print(0, data);
  delay(1000);
  strcpy(data, "uvxyzw...   ");
  //strrevert1(data);
  LC75711_print(0, data);
  delay(1000);
}
void setup() {
  // put your setup code here, to run once:
  //initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);  // This is only to allow to use LED of Arduino during DEBUG.
  Serial.begin(115200);

  pinMode(VFD_in, OUTPUT);
  pinMode(VFD_clk, OUTPUT);
  pinMode(VFD_ce, OUTPUT);
  pinMode(VFD_rst, OUTPUT);

  LC75711_init();
}
void loop() {
  bool flag = true;
      while(1){
      clear_VFD();
      msgHiFolks();
      delay(1500);
      msgSanyo();
      msgNumbers();
      msgWelcome();
      delay(1000);
      shiftLeft();
      shiftRight();
      //
      msgChars();
      delay(500);
      digitalWrite(LED_BUILTIN, !(digitalRead(13)));
      clear_VFD();
      delay(200);
      customChars_VFD(); //Here we fill all 35 bit of matrix 5x7
      clear_VFD();
      CGRAM_VFD(); //Present the contents of positions of ASCII tabel memory 0x00 to 0x07, is possible see all bits ON on matrix
      delay(1000);
      drawPackMan(); //Here we draw the 4 types of PackMan
      CGRAM_VFD(); //Present the contents of positions of ASCII tabel memory 0x00 to 0x07 and now we can see the four packman
      delay(1000);
      clear_VFD();
          for(uint8_t L=11; L>0; L--){
            packManMoveLeft(L);
          }
          for(uint8_t R=0; R<12; R++){
            packManMoveRight(R);
          }


      strcpy(data, "abcdefghij");
            if(flag){
            strrevert1(data);
            flag=false;
            }
            else{
            flag=true;
            }
      LC75711_print(0x00, data);
      delay(1000);
      }
}
// void buttonReleasedInterrupt() {
//   buttonReleased = true;  // This is the line of interrupt button to advance one step on the search of segments!
// }
