
#include <EEPROM.h>

// start reading from the first byte (address 0) of the EEPROM
// arduino uno r3 has 1024 bytes
int address = 0;
byte startup_buffer[10];
byte registers[256];
byte stack[256];
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
  //read the first 10 bytes of the program.
  /*
  starting Byte, little endian
  program data
  program data
  programming flags (N/A * 7, Verbose programming )
  program data (etc.)
  */
  Serial.println();
  for (int i=0;i<10;i++){
    startup_buffer[i] = EEPROM.read(i);
  }
  address = startup_buffer[0];
}
void reset_no_print(){
  //same as reset, without printing a new line. 
  for (int i=0;i<10;i++){
    startup_buffer[i] = EEPROM[i];
  }
  address = startup_buffer[0];
}
void inc_Addr(byte amt=1){
  address=address+amt;
  if (startup_buffer[4] & 0x02){ 
    if (address < 0x10){Serial.print("0");}
    Serial.print(address, 16);
    Serial.print("\t");
    if (value < 0x10){Serial.print("0");}
    Serial.print(value, 16);
    Serial.println();
    delay(500);
  }
}
void loop() {
  // read a byte from the current address of the EEPROM
  byte val;
  byte reg1;
  byte reg2;
  byte stackaddr;
  byte pointer;
  byte buff[1];
  if (address == EEPROM.length()) {
    return;
  }
  value = EEPROM.read(address);
  switch (value){
    case 0x00:// nop
      break;
    case 0x01:// print value at next address
      inc_Addr();
      Serial.write(EEPROM.read(address));
      Serial.flush();
      break;
    case 0x02:// print value in register
      inc_Addr();
      Serial.write(registers[address]);
      Serial.flush();
      break;
    case 0x03:// push value to register
      inc_Addr();
      val = EEPROM.read(address);
      inc_Addr();
      reg1 = EEPROM.read(address);
      registers[reg1] = val;
      break;
    case 0x04:// push register to stack address
      inc_Addr();
      reg1 = EEPROM.read(address);
      inc_Addr();
      stackaddr = EEPROM.read(address);
      stack[stackaddr] = registers;
      break;
    case 0x05:// push register to register
      inc_Addr();
      reg1 = EEPROM.read(address);
      inc_Addr();
      reg2 = EEPROM.read(address);
      registers[reg1] = registers[reg2];
      break;
    case 0x06:// pull stack address to register
      inc_Addr();
      stackaddr = EEPROM.read(address);
      inc_Addr();
      reg1 = EEPROM.read(address);
      registers[reg1] = stack[stackaddr];
      break;
    // jump to pointer if 00 register > 01 register
    case 0x07:
      inc_Addr();
      pointer = EEPROM.read(address);
      if (registers[0] > registers[1]){
        address = pointer;
        return;
      }
      break;
    // jump to pointer if 00 register < 01 register
    case 0x08:
      inc_Addr();
      pointer = EEPROM.read(address);
      if (registers[0] < registers[1]){
        address = pointer;
        return;
      }
      break;
    // jump to pointer if 00 register >= 01 register
    case 0x09:
      inc_Addr();
      pointer = EEPROM.read(address);
      if (registers[0] >= registers[1]){
        address = pointer;
        return;
      }
      break;
    // jump to pointer if 00 register <= 01 register
    case 0x0A:
      inc_Addr();
      pointer = EEPROM.read(address);
      if (registers[0] <= registers[1]){
        address = pointer;
        return;
      }
      break;
    // jump to pointer if 00 register == 01 register
    case 0x0B:
      inc_Addr();
      pointer = EEPROM.read(address);
      if (registers[0] == registers[1]){
        address = pointer;
        return;
      }
      break;
    // jump to pointer if 00 register != 01 register
    case 0x0C:
      inc_Addr();
      pointer = EEPROM.read(address);
      if (registers[0] != registers[1]){
        address = pointer;
        return;
      }
      break;
    // push available serial bytes to register
    case 0x0D:
      inc_Addr();
      reg1=EEPROM[address];
      registers[reg1] = Serial.available();
      break;
    // pop first available byte to register from serial
    case 0x0E:
      inc_Addr();
      reg1=EEPROM[address];
      Serial.readBytes(buff,1);
      registers[reg1] = buff[0];
      break;
    // legacy - print value at pointer
    case 0x0F:// print value in register
      inc_Addr();
      Serial.write(EEPROM.read(EEPROM.read(address)));
      Serial.flush();
      break;
    // jump ahead n bytes if 00 register > 01 register
    case 0x10:
      inc_Addr();
      pointer = EEPROM.read(address);
      if (registers[0] > registers[1]){
        address += pointer;
        return;
      }
      break;
    // jump ahead n bytes if 00 register < 01 register
    case 0x11:
      inc_Addr();
      pointer = EEPROM.read(address);
      if (registers[0] < registers[1]){
        address += pointer;
        return;
      }
      break;
    // jump ahead n bytes if 00 register >= 01 register
    case 0x12:
      inc_Addr();
      pointer = EEPROM.read(address);
      if (registers[0] >= registers[1]){
        address += pointer;
        return;
      }
      break;
    // jump ahead n bytesr if 00 register <= 01 register
    case 0x13:
      inc_Addr();
      pointer = EEPROM.read(address);
      if (registers[0] <= registers[1]){
        address += pointer;
        return;
      }
      break;
    // jump ahead n bytes if 00 register == 01 register
    case 0x14:
      inc_Addr();
      pointer = EEPROM.read(address);
      if (registers[0] == registers[1]){
        address += pointer;
        return;
      }
      break;
    // jump ahead n bytes if 00 register != 01 register
    case 0x15:
      inc_Addr();
      pointer = EEPROM.read(address);
      if (registers[0] != registers[1]){
        address += pointer;
        return;
      }
      break;
    //reset functions
    case 0xFF:
      reset();
      return;
    case 0xFE:
      reset_no_print();
      return;
  }
  inc_Addr();
  Serial.println("instruction processed");
}
