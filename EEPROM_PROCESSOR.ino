
#include <EEPROM.h>

// start reading from the first byte (address 0) of the EEPROM
// arduino uno r3 has 1024 bytes
int address = 0;
int startup_buffer[10];
int registers[256];
int stack[256];
byte value;

void setup() {
  // initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  //read the first 10 bytes of the rom, this will store the basic data like terminal size and starting address.
  reset();
}


void reset(){
  Serial.println();
  for (int i=0;i<10;i++){
    startup_buffer[i] = EEPROM.read(i);
  }
  address = startup_buffer[0];
}

void loop() {
  // read a byte from the current address of the EEPROM
  int val;
  int reg1;
  int reg2;
  int stackaddr;
  int pointer;
  if (address == EEPROM.length()) {
    return;
  }
  value = EEPROM.read(address);
  /*if (address < 0x10){Serial.print("0");}
  Serial.print(address, 16);
  Serial.print("\t");
  if (value < 0x10){Serial.print("0");}
  Serial.print(value, 16);
  Serial.println();*/
  switch (value){
    case 0x00:// nop
      break;
    case 0x01:// print value at next address
      address++;
      Serial.write(EEPROM.read(address));
      Serial.flush();
      break;
    case 0x02:// print value pointed to by next address
      address++;
      Serial.write(EEPROM.read(EEPROM.read(address)));
      Serial.flush();
      break;
    case 0x03:// push value to register
      address++;
      val = EEPROM.read(address);
      address++;
      reg1 = EEPROM.read(address);
      registers[reg1] = val;
      break;
    case 0x04:// push register to stack address
      address++;
      reg1 = EEPROM.read(address);
      address++;
      stackaddr = EEPROM.read(address);
      stack[stackaddr] = registers;
      break;
    case 0x05:// push register to register
      address++;
      reg1 = EEPROM.read(address);
      address++;
      reg2 = EEPROM.read(address);
      registers[reg1] = registers[reg2];
      break;
    case 0x06:// pull stack address to register
      address++;
      stackaddr = EEPROM.read(address);
      address++;
      reg1 = EEPROM.read(address);
      registers[reg1] = stack[stackaddr];
      break;
    // jump to pointer if 00 register > 01 register
    case 0x07:
      address++;
      pointer = EEPROM.read(address);
      if (registers[0] > registers[1]){
        address = pointer;
        return;
      }
      break;
    // jump to pointer if 00 register < 01 register
    case 0x08:
      address++;
      pointer = EEPROM.read(address);
      if (registers[0] < registers[1]){
        address = pointer;
        return;
      }
      break;
    // jump to pointer if 00 register >= 01 register
    case 0x09:
      address++;
      pointer = EEPROM.read(address);
      if (registers[0] >= registers[1]){
        address = pointer;
        return;
      }
      break;
    // jump to pointer if 00 register <= 01 register
    case 0x0A:
      address++;
      pointer = EEPROM.read(address);
      if (registers[0] <= registers[1]){
        address = pointer;
        return;
      }
      break;
    // jump to pointer if 00 register == 01 register
    case 0x0B:
      address++;
      pointer = EEPROM.read(address);
      if (registers[0] == registers[1]){
        address = pointer;
        return;
      }
      break;
    // jump to pointer if 00 register != 01 register
    case 0x0C:
      address++;
      pointer = EEPROM.read(address);
      if (registers[0] != registers[1]){
        address = pointer;
        return;
      }
      break;
    case 0xFF:
      reset();
      return;
  }
  address++;
}
