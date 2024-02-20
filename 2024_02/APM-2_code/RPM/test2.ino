#include <SoftwareSerial.h>

SoftwareSerial deviceSerial(8, 9); // RX, TX

const byte STX = 0x02;
const byte CMD_VALUE_QUERY = 0x10;
const byte CMD_VALUE_RETURN = 0x11;
const byte CHECKSUM_CONST = 0xFF;

void setup() {
  Serial.begin(19200);
  deviceSerial.begin(19200);
}

void loop() {
  send_value_query();
  delay(1000); // wait for the response
  int result = parse_value_return();

  switch (result) {
    case 0:
      Serial.println("Successfully parsed data!");
      break;
    case -1:
      Serial.println("Error: Invalid packet");
      break;
    case -2:
      Serial.println("Error: Checksum mismatch");
      break;
    case -3:
      Serial.println("Error: Not enough data");
      break;
  }

  delay(5000); // Wait 5 seconds before sending the next query
}

void send_value_query() {
  byte checksum = CHECKSUM_CONST - CMD_VALUE_QUERY;
  deviceSerial.write(STX);
  deviceSerial.write(CMD_VALUE_QUERY);
  deviceSerial.write((byte)0x00); // size1
  deviceSerial.write((byte)0x00); // size2
  deviceSerial.write(checksum);
}

int parse_value_return() {
  if (deviceSerial.available() >= 27) {
    byte stx = deviceSerial.read();
    byte cmd = deviceSerial.read();
    byte size1 = deviceSerial.read();
    byte size2 = deviceSerial.read();

    if (stx != STX || cmd != CMD_VALUE_RETURN) {
      return -1; // Invalid packet
    }

    byte values[24];
    for (int i = 0; i < 24; i++) {
      values[i] = deviceSerial.read();
    }

    byte received_checksum = deviceSerial.read();
    
    byte calculated_checksum = CHECKSUM_CONST - cmd - size1 - size2;
    for (int i = 0; i < 24; i++) {
      calculated_checksum -= values[i];
    }

    // Print received data for debugging
    Serial.print("Bytes available: "); Serial.println(deviceSerial.available());
    Serial.print("STX: "); Serial.println(stx, HEX);
    Serial.print("CMD: "); Serial.println(cmd, HEX);
    Serial.print("Size1: "); Serial.println(size1, HEX);
    Serial.print("Size2: "); Serial.println(size2, HEX);

    for (int i = 0; i < 24; i+=2) {
      Serial.print("Data "); Serial.print(i+1); Serial.print(", "); Serial.print(i+2); 
      Serial.print(": "); Serial.print(values[i], HEX); Serial.print(", "); Serial.println(values[i+1], HEX);
    }

    Serial.print("Received Checksum: "); Serial.println(received_checksum, HEX);
    Serial.print("Calculated Checksum: "); Serial.println(calculated_checksum, HEX);

    if (received_checksum != calculated_checksum) {
      return -2; // Checksum mismatch
    }

    // You can process the values here for further use.

    return 0; // Success
  }

  return -3; // Not enough data
}
