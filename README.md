# AV-RK-MK2
 AVlabs AV-RK Redox Styled Split Mechanical Keyboard MK 2


The AV-RK MK II is the second iteration of the AVlabs Redox styled mechanical keyboard.

While the first was a "simple" handwired job with arduinos, this one features a XIAO SAMD21, communicating through I2C with IO Expanders to a numpad, a left half and a right half. 
The 3 parts connect together with cool magnetic pogo pins. these pogo pins allow a modular layout, i.e if the pogo pins are wired identically and laid out symetrically, you can put the numpad on the left, in the middle, on the right, or even add modules: as long as it communicates with I2C.

Available below are: 

CAD files: Fusion 360 source and STLs for 3D printed parts
FIRMWARE: the arudino code that runs the keyboard
PCB files: gerbers and boms

//TODO: things to fix!!!
    //Write a real guide/readme...

    //NUMPAD PCB: on the left side, the pins for the I2C/PWR pogo pins are too close to the edge
    //POGO PINS: put male on the left and female on the right on all modules to allow modularity
    //FIRMWARE: finish maps and add macros and layers and handle the trackball clicks (and trackball rgb???)
    //CAD: add better aesthetic for the top board? find out what looks better between tiny windows and top decorations
    //find a cool usb c magnetic pogo pin cable 

    //maybe port the keyboard.h lib to the NRF52 board???
    //Make cool modules: those cool I2C screens, connect cool temp sensors and shit to arduinos on the I2C bus
    //
 