# Wasserwanne
A small project to automatically open a motorized ball valve or pulsed solenoid valve when a water container is filled to a certain point and close the valve again
when the water level drops below the sensor.

# Used hardware
- Motorized ball valve / pulsed solenoid valve
- Water sensor (floating magnet with reed switch)
- Switches for manual override
- ATTiny84A
- Li-Ion battery
- Boost converter (12V)
- Solar charger with MPPT
- 9V solar cell
- H-Bridge
- PNP Transistor (BC 328-25)
- Some resistors

# Circuit schematic
For the circuit schematic have a look at the Schaltplan.pdf file.

# About
Project is written in C with Microchip Studio. Should work fine when everything is copied into the Arduino IDE (no additional libraries required).

Peter Fleury's I2C and UART libraries are included for debugging. They are disabled by default and are not required.
A heavily modified version of Nico and Thomas Eichelmann's LCD library is also included, but only used for debugging. It was almost completely rewritten to comply with a HD44780 LCD display with PCF8574X I2C expander (modifications should not have been necessary, but for whatever reason it did not control the LCD as described in its datasheet).
