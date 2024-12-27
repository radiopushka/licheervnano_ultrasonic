# researching ultraonic data transmission using the Lichee RV Nano chipset
- research regarding ultrasonic communication air and water
# full package
- due to the required RISCV alsa library required for this to run. I am not able to upload the whole thing to github. It has a symbolic link which even trips up scp.
- oh alsa alsa linux sound
- please find the full version here: https://disk.yandex.ru/d/a9By98uyEn9MNA
- you will have to change the makefile and change the compiler from licheegcc (which is not even a compiler but a dummy shell script i wrote)
- you will have to find the proper flags and toolchain for your LicherRV Nano 
- there should be binaries present but feel free to modify the code you can look at receiver.c and anything pins and gpio related
- you can edit the code to make it trigger pins on the chip on and off, send data back and forth, and so on.
- you can find the section where the final message is received in function process_message in receiver.c on line 144 where tbuff is the message
- you do not want to send too much data in one message, this will slow things down. Currently this protocol can do maxiumum 500 bps in a virtual environment.
- the fastest best condition through air speed is closer to 300 bps.
- my hopes for the future of this is to make a duplex underwater link. For example, I can check my email while sitting at the bottom of the ocean in my submarine.
- another idea i have is to make kind of like an underwater GPS system where there are beacons that send a code and the sub can measure the distance to the beacon knowing the timestamps and speed of sound underwater. (very inaccurate)
- It is easy to send voice and music on multiple frequencies using ultrasound. you can check out the ultrasonic toolkit project in my repositories.
