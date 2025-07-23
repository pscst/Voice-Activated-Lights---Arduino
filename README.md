# Voice-Activated-Lights-Arduino



Project Description
This project demonstrates a simple yet powerful voice-controlled lighting system using an Arduino Uno, HC-05 Bluetooth module, and the Dabble app. With this setup, users can control LED lights (or even LED matrices and animations) by sending voice commands through their smartphone.
The Dabble app captures the user's voice, converts it to text, and sends the corresponding command via Bluetooth to the Arduino. The Arduino processes the received command and triggers the appropriate action, such as turning lights on or off, changing colors, or activating different lighting patterns like rainbow, heart, wave, etc.
How it works
1.	The Dabble app is connected to the Arduino via the HC-05 Bluetooth module.
2.	The user taps the "Terminal" menu” then, “Voice Icon” in Dabble and speaks a command (e.g., "turn on light bulb", "turn off light", "show fireworks", "show a heart", etc.).
3.	The app sends the voice-recognized command text to the Arduino.
4.	The Arduino reads the command and executes a matching function, such as:
o	Turning lights on or off.
o	Activating specific lighting patterns.
o	Scrolling text or animations on an 8x8 LED matrix.
Main Components
•	Arduino Uno
•	HC-05 Bluetooth module
•	LED lights or 8x8 LED matrix
•	Dabble app (installed on Android phone available on Play Store)
•	Power source (USB, battery, or power bank)
Features
•	Hands-free control of lights and effects.
•	Multiple predefined lighting patterns.
•	Easy Bluetooth pairing and voice command interface.
•	Real-time feedback via serial monitor (Aruduino IDE) or mobile terminal.
Applications
•	DIY smart lighting system
•	Accessibility projects (for people with limited mobility)
•	Interactive art or LED displays
•	Educational projects to learn Arduino, Bluetooth, and voice recognition basics
List of Pattern Commands:

	make a fire
	demonstrate water flow
	demonstrate air
	signal right
	signal left
	upward arrow
	downward arrow
	show a heart
	show a rainbow
	show fireworks
	show spiral
	matrix
	turn on light bulb
	raindrop
	waves
	turn on party lights
	turn on relax mode
	show a happy face
	show a sad face
	hello aries
	red light
	green light
	show pac-man
	turn off light

Tutorial on How to use Voice Activated Lights Using Dabble App (Bluetooth)
1.	Power Up the Arduino Uno
o	Connect it via USB to a power bank, charger, or computer.
2.	Install and Open the Dabble App on Your Android Device
o	Ensure Bluetooth is enabled.
o	Grant any necessary permissions, including location access, for better Bluetooth visibility.
3.	Pair and Connect to the HC-05 Module
o	In your phone's Bluetooth settings, search for available devices.
o	Select "HC-05" and pair with it. The default password is usually 1234 or 0000.
o	Once paired, return to the Dabble app and connect to the HC-05 module.
4.	Access the Voice Module in Dabble
o	Tap on the Microphone icon within the app.
5.	Start Giving Voice Commands
o	Speak commands like:
	“show a rainbow”
	“downward arrow”
	" show a heart”
	"waves"
	" turn on party lights”
	"turn off light"
The Arduino will process these commands and activate the corresponding lighting patterns


![Picture1](https://github.com/user-attachments/assets/18a48dee-810f-4bc1-8c09-6d7efa8af125)
![Picture2](https://github.com/user-attachments/assets/ed3d9fd7-b574-4bd5-8a86-0e22929d10b2)
![Picture3](https://github.com/user-attachments/assets/c3a992b1-5217-4a67-8ca3-c14d05fd83b2)
![Picture4](https://github.com/user-attachments/assets/1c57fdc1-1024-467b-87e8-c1f69f780b71)




