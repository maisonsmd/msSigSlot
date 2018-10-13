#include "msSigSlot.h"

void FunctionNoReturn_1(int a, int b) {
	Serial.print("func no return called with: a=");
	Serial.print(a);
	Serial.print(", b=");
	Serial.println(b);
}

void FunctionNoReturn_2(int a, int b) {
	Serial.print("func no return called with: a=");
	Serial.print(a);
	Serial.print(", b=");
	Serial.println(b);
}

float FunctionWithReturn(int a, int b) {
	Serial.print("func called with: a=");
	Serial.print(a);
	Serial.print(", b=");
	Serial.println(b);

	return (float)a / b;
}

class FooClass {
public:
	static void StaticMethod(int a, int b) {
		Serial.print("static method called with: a=");
		Serial.print(a);
		Serial.print(", b=");
		Serial.println(b);
	}
	void MethodNoReturn(int a, int b) {
		Serial.print("method no return called with: a=");
		Serial.print(a);
		Serial.print(", b=");
		Serial.println(b);
	}
	float MethodWithReturn(int a, int b) {
		Serial.print("method called with: a=");
		Serial.print(a);
		Serial.print(", b=");
		Serial.println(b);
		float dump = a / b;
		return dump;
	}
};

FooClass barObject;


void setup()
{
	Serial.begin(9600);
	Serial.println("hello");

	FunctionSlot<void(int, int)> functionSlot(FunctionNoReturn_1);
	MethodSlot<FooClass, void(int, int)> methodSlot(&barObject, &FooClass::MethodNoReturn);

	Signal<void(int, int), 4> signal1;
	signal1.attach(functionSlot);
	signal1.attach(methodSlot);

	//or
	signal1.attach(FunctionSlot<void(int, int)>(FunctionNoReturn_2));
	//or
	signal1 += MethodSlot<FooClass, void(int, int)>(&barObject, &FooClass::MethodNoReturn);

	/* Executing */
	signal1.fire(2, 3);
	//or
	signal1(2, 3);

	
	//or using Lambda expression:
	Signal<void(float)> signal2;
	signal2.attach([](float a) -> void {
		Serial.println("Lambda expression with a=" + String(a));
	});
	signal2(10.23);
	
	pinMode(13, OUTPUT);
}

void loop()
{
	digitalWrite(13, HIGH);
	delay(10);
	digitalWrite(13, LOW);
	delay(500);
}