# PWM-LED-Controller

I have a RGB-LED strip as passive light-source behind my computer. It was directly connected to the power supply of my PC to turn on automatically when the PC is turned on. It was controlled by a cheap bluetooth RGB controller to change the colour. This cheap controller has stoped working so i programmed a new one, using an ESP32. It uses three transistors to controll the three Colours. It also measures the PWM-Voltage from the motherboard to controll the PC-fans. You can either input a RGB value or the colour changes depending on the PC utilization. 
The controller can be controlled via an html-Server that gets hosted on the ESP itself. 
