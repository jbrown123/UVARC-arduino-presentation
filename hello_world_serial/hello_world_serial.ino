void setup() {
	Serial.begin(57600);
}

void loop() {
	Serial.print(millis());
	Serial.println("  Hello, World!");
	delay(1000);
}
