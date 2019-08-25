#include "msSigSlot.h"

class Buttons
{
public:
	Signal<void(), 5> OnButtonDown;
	void simulatePress() {
		Serial.println("simulating...");
		OnButtonDown.fire();
	}
};

Buttons globalButton;

class Menu
{
	void enter() {
		Serial.println("Enter!");
	}
public:
	Buttons menuButton;
	Menu() {
		menuButton.OnButtonDown += MethodSlot<Menu, void()>(this, &Menu::enter);
	}
};

Menu menu;

void setup() {
	Serial.begin(115200);
	Serial.setTimeout(5);
	Serial.println("Hello world!");

	globalButton.OnButtonDown += []() {
		Serial.println("global event!");
	};
}

void loop() {
	globalButton.simulatePress();
	menu.menuButton.simulatePress();
	menu.menuButton.OnButtonDown += []() {Serial.println("lamda"); };

	Serial.println();

	delay(2000);
}