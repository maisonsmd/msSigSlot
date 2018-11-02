#include "msSigSlot.h"

void FunctionNoReturn_1(int a, int b) {
	Serial.print("func no return 1 called with: a=");
	Serial.print(a);
	Serial.print(", b=");
	Serial.println(b);
}

void FunctionNoReturn_2(int a, int b) {
	Serial.print("func no return 2 called with: a=");
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

	Signal<void(int, int), 8> signal1;
	signal1.attach(functionSlot);
	signal1.attach(methodSlot);

	////or
	signal1.attach(FunctionSlot<void(int, int)>(FunctionNoReturn_2));
	////or
	signal1 += MethodSlot<FooClass, void(int, int)>(&barObject, &FooClass::MethodNoReturn);
	////or
	signal1 += FunctionNoReturn_1;

	///* Executing */
	signal1.fire(2, 3);
	Serial.println("---------------------------");
	////or
	signal1(2, 3);
	Serial.println("---------------------------");

	signal1 -= FunctionNoReturn_1;
	signal1.detach(FunctionSlot<void(int, int)>(FunctionNoReturn_1));
	signal1 -= (methodSlot);

	signal1(1, 22);
	Serial.println("---------------------------");
	delay(1000);

	//or using Lambda expression:
	Signal<void(float), 8> signal2;
	signal2.attach([](float a) -> void {
		Serial.println("Lambda expression called with a=" + String(a));
	});

	signal2 += [](float a) -> void {
		Serial.println("Lambda expression 1 called with a=" + String(a));
		barObject.MethodNoReturn(a, a);
	};

	signal2(10.23);

	//pinMode(PC13, OUTPUT);
}

void loop()
{
	/*digitalWrite(PC13, HIGH);
	delay(100);
	digitalWrite(PC13, LOW);
	delay(500);

	attachInterrupt(1, []() -> void {Serial.println("triggered"); }, FALLING);
	//or using Lambda expression:
	Signal<void(uint32_t), 8> signal2;
	signal2.attach([](uint32_t a) -> void {
		Serial.println("Lambda expression called with a=" + String(a));
	});

	signal2 += [](uint32_t a) -> void {
		Serial.println("Lambda expression 1 called with a=" + String(a));
	};

	signal2(millis());

	Serial.println("--------------------------------------");*/


    Signal<void(), 3> sig;
    sig += []() {Serial.println(1); };
    sig += []() {Serial.println(2); };
    sig += []() {Serial.println(3); };
    sig += []() {Serial.println(4); };

    sig();

    delay(1000);
}