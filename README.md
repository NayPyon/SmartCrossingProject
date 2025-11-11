# Smart Traffic Light (Smart Crossing) Simulation

This is a simulation project of a smart crossing system using **Arduino** and **Proteus**.

This system not only runs a normal traffic light cycle but also features an intelligent interruption feature for pedestrians, complete with visual (LCD) and audio (Buzzer) feedback.

## Key Features
* **State-Machine Logic:** Uses 6 defined states to manage cycles without `delay()`.
* **Pedestrian Interrupt:** Pedestrians can press a button to request a crossing.
* **Smart Time Management:** The system saves the remaining green light time and resumes it after the pedestrian cycle is complete.
* **Non-Blocking Code:** The entire system, including the LCD countdown and buzzer, uses `millis()` to remain responsive.

## Technologies Used
* **Language:** C++ (Arduino)
* **Simulation Software:** Proteus
* **Main Components:** Arduino Uno, 16x2 I2C LCD, Buzzer, LEDs, Push Button.

## How to Run the Simulation
1.  Open the `.pdsprj` file in Proteus.
2.  [cite_start]Compile the `.ino` file [cite: 4-54] in the Arduino IDE.
3.  Copy the `.hex` file path from the compilation output.
4.  Double-click the Arduino component in Proteus and paste the `.hex` path into the "Program File" field.
5.  Run the simulation.
