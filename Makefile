#//Evan Nikitin Wed Oct  2 09:22:05 PM -00 2024
LIBS=alsa_risc/objects/libasound.so.2.0.0
INCLUDE=alsa_risc/include
mod_test:
	licheegcc $(LIBS) -I$(INCLUDE) transmitter.c alsa/alsa.c SyncedPSKASK_modulationLibrary/modulator.c SyncedPSKASK_modulationLibrary/Freq_lib/frequency_arrays.c SyncedPSKASK_modulationLibrary/Freq_lib/frequency_itr.c SyncedPSKASK_modulationLibrary/Freq_lib/convolution/lowpass.c -g -O2 -lm -o modgen
	licheegcc $(LIBS)  -I$(INCLUDE) receiver.c gpio/gpio.c alsa/alsa.c SyncedPSKASK_modulationLibrary/modulator.c SyncedPSKASK_modulationLibrary/Freq_lib/frequency_arrays.c SyncedPSKASK_modulationLibrary/Freq_lib/frequency_itr.c SyncedPSKASK_modulationLibrary/Freq_lib/convolution/lowpass.c -g -O2 -lm -o modrx
