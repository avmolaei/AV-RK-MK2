# AV-RK-MK2


The AV-RK MK II is the second iteration of the AVlabs [AV-RK MK I](https://github.com/avmolaei/AV-RK-MK1), the AVlabs "Redox Styled" Mechanical Keyboard.
![AV-RK MK 2](https://cdn.discordapp.com/attachments/712626945023672370/1136623967264841788/AIL4fc9oPaMrQqEiUUiGKn0ue0uNpUhGLoGsiR-ZmI_d5t5b0r-WU9mKvzKnxqKiSDBv4QqQe6PU4JJ2Dw_a2II3Jc0MFzd7t7LpJvK8gHM_HqEeRpo8sYJr8nUEON0nudLFA9ZImRuV4uGQCOJoATzPSOXi-XK1oesAKJHStjPJx3bQYySWk12tcDynvKp1PMNYCd9F60jtGz6HKBTEuKBHFVDk4kavRRu6fdKhR9pD-q-R3UaUe1H3ncpsaCa1un5cZM9sOIzfdGT3MLhEUuaACSy6r7AydZTw6WUz7JtQbsCUUH_GpoL0vpJ3rCO9X-LHH0OKBgaURwBRLzTAXV98hEdmCB4hvSDWZXqyDh6CsKkhXyblXJgYSAZq5bXlc7DBoRJhMecXbj4pm4Bw5IcPtljWLdVJFkS2hfKcVseFt8_U39f1mhcxcx-c6K5qF9WZb-K61eZ1gLWFcFCENuaB8ampULj8I7xjkRYaMOPv-GraYxztZSJJpF-_..png)


![enter image description here](img/gif1.gif)





The AV-RK MK II uses a 3D printed body, Kailh Choc Switches and is based on a XIAO SAMD21 board. There is only one MCU running the whole thing: switches are connected to IO expanders (PCA9505), that share an I2C bus, which is propagated through magnetic pogo pins (like the Apple MagSafe connector). The goal was also to make an even thinner keyboad than the MK I for more comfort: 
![enter image description here](https://cdn.discordapp.com/attachments/712626945023672370/1136625625130606592/AIL4fc-uJq5dnbWmjo95AQXwqYADXsGC86jf2XeGO9atZ-D-YUMbOGZxCD6ksuw-NDuIfZrGumh54cNmfceZm4xgQHB96uhxSHrtycQgkYtuCjrDtu6HqJ1diADxWPdcET4tBW-3DqCy6vTohiOrHtiEQSXaCvVV6mmQP6-Kp7kIZBfvCLoZOWvMoB4MJhc2yo71diRGqAGCXYwZk3U9suX-4xr_VOhkbyu0A6meWe4I1uLoh2tckOz3Q18iRPPUMphBDFi6ab21H3SODwUuRby5urh7Nd7-75QUIK_4luc9jhR6u89M0LQqoIe_JLMmvR-t-OrocrBFvt9vTUnhSU1ZFMGgg_FmxeizAmU5DmwZKrZZQCdE6Nu21vYLe5FeNPVLk-vrxARTABKk7xOIbnnSKlTfdQFxRyqAWrmDS5mbZlUHlCOVbK0btNHxio70mYsDJFC7e-LiBLZArdqahwoVhNjBnAaWOoLxsy7RfLhjfs6r4RqjubZ1_Xcx..png)
(the MK I on the left, and the MK II on the right)
  
The [AV-RK-MK I](https://github.com/avmolaei/AV-RK-MK1) and the [AV-KO1](https://github.com/avmolaei/AV-KO1/) are both handwired keyboards (Expaination in details in the AV-KO1 repo). While handwire is easy to get started on, it has a few problems:

 - The switches are held to the top plate with nothing but friction, and over time (especially if you use "tall" switch+keycap combos, the switch can get loose, or if you knock the board it can get the switch to "pop out":
 - ![enter image description here](/img/gif2.gif)
 - If you don't plan out your tolerances and screw holes correctly, the top plate will "bow", and make a gap between the plate and body as there is nothing to keep it from bowig
 - Soldering 100 switches and diodes is a good exercise but it is a bit tedious...
 - As most shift registers/IO Expanders are small SMD parts, most "sane" Handwire builds have multiple microcontrôleurs, which is not always ideal. 

The 3 parts connect together with cool magnetic pogo pins. these pogo pins allow a modular layout, i.e if the pogo pins are wired identically and laid out symetrically, you can put the numpad on the left, in the middle, on the right, or even add modules: as long as it communicates with I2C. The architecture of the AV-RK-MK2 is as follows:
![enter image description here](https://media.discordapp.net/attachments/712626945023672370/1136631531734056960/39u2JEs1jxXFAAAAABJRU5ErkJggg.png?width=705&height=584)

 Here the goal was to design and build my "endgame" definition of a keyboard. It had to be "redox styled", it had to have kailh choc robins, and it had to have PCBs!
 
 Indeed, while PCBs do imply a higher cost, they make assembly super simple and allow for more complex layout! The PCA9505 couldn't have been used without the PCBS. As our keyboard has 3 parts, it has 3 PCBs:
 ![enter image description here](https://media.discordapp.net/attachments/712626945023672370/1136632255528321044/AIL4fc9wTbHCuxS_CXNgKaL6i6IuC0SyRDZC_Y46hHIA1XFs73FEWLTrvFwl3_eWXomB4Dtm-VlFSx6u6wjY52kfQ3hLB9SLV8EXyQKx9fg91Y1jOwH7CjFNvEzY7RDHsBjuphVyKfCykGHEdaP9giluNB8wXzlK2AS79A9qw7lQyZQVUYfs9nFJNDqN5rbFT7PsxpZPDw6i_VkUd9lWZprLoNF9szp8WyC65sOjD7tXOfwD-fiv6c3ZYg5hWjJz4cbIPmw2X-a7ARujc2MNCxfZKJKExGU5DQBCHt_kGubAFSopGQ-ODjpDm37YZdKoCA_wrC2pT8GBPdG9tt0NNmixfm5ssbGxyNT8Nw8qisDq3zVrWmOmSNdcnO9ccwYmQc4nqTOj68u2V6nVGZa8d_ouHt5qob-LZqDAbQmv2TSiQaWLtL9LXSENSmG0MWfL_WKOnbwB79d5InLGcHLf7EJy2NybGTGMc0-5M6TZ25PMI5jyciX_9ReG5R0Z..png?width=774&height=346)

The PCBs are pretty simple: they are just a bunch of switches connected to the PCA input pins, and the PCA's I2C line being distributed to pins which will be connected to the pogo pins. The middle PCB (numpad) has a small cutout for the XIAO board, and the right PCB has a cutout made to fit a pimoroni trackball, or any other I2C device you would want. 


Available below are:

  

CAD files: Fusion 360 source and STLs for 3D printed parts

FIRMWARE: the arudino code that runs the keyboard

PCB files: gerbers and boms

MODULES: modules, duh.

  

  
`//TODO: shit to fix and/or add
`

PCBs: on all PCBs, the I2C/PWR lines are too close to the edge. On the right PCB, the pins for the pimoroni trackball are reversed, which requires an annoying bodge.



FIRMWARE: needs to be ported from SAMD21 core to NRF52840. Also, need to fix trackball precision issue. Also, the USB HID lib needs to go from Keyboard.h to TinyUSB.


CAD: maybe enhance aesthetics for the top plate? also need to make some sort of alignement system for the pogo pins, sometimes when docking/undocking modules the pins may short and require a MCU reset


`MODULES:`

**__AV-RK SENSOR MODULE__**: It has a thin OLED display and a square one. It will have an RTC, a humidity/temp sensor and a heartbeat sensor, all that on the I2C BUS. V2 of this module will inclue an independant MCU so the code is not cluttering the main keyboard code. AAAAAAAAA


**__AV-RK MOUSE CHARGER__**: it will have a tiny arduino nano inside, and will recieve commands based on keyboard shortcuts on the I2C bus. The "independant" arduino will trigger LEDs for caps lock/num lock/scroll lock, and will control a tiny servo-actuated arm that shows pogo pins to charge the mouse overnight. The mouse needs to be modified with a USB connector soldered to pogo pins. 

Sensors have been designed, made; all is left is to integrate them in the main code. 

A better, more sensible solution would be to embedd a small low power MCU (e.g: arduino nano) in each module, and make them use a "protocol", where Fn + [NUMPAD NUMBER] sends a set command and the module is programmed to understand the command, instead of integrating everything in the main keyboard code, making a messs. 

 


