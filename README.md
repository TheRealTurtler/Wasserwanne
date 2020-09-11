# Wasserwanne
A small project to automatically open a motorized ball valve or pulsed solenoid valve when a water container is filled to a certain point and close the valve again
when the water level drops below the sensor.

# Used hardware
- Motorized ball valve / Pulsed solenoid valve
- Water sensor (floating magnet with reed switch)
- Switches for manual override
- ATTiny84A
- Li-Ion Battery
- 2 x Boost converter (5V and 12V)
- Solar charger with MPPT
- H-Bridge

# Circuit schematic
For the circuit schematic have a look at the Schaltplan.pdf file.

# About
Project is written in C with ATMEL Studio. Should work fine when everything is copied into the Arduino IDE (no additional libraries required).
