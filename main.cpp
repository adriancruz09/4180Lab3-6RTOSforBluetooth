#include "mbed.h"
#include "USBHostMSD.h"
#include "wave_player.h"
#include "uLCD_4DGL.h"

AnalogOut DACout(p18);
PwmOut PWMout(p26);

wave_player waver(&DACout,&PWMout);

PwmOut Rgb(p23);
PwmOut rGb(p24);
PwmOut rgB(p25);

uLCD_4DGL uLCD(p9, p10, p11); // serial tx, serial rx, reset pin;

Mutex uLCD_mutex;

void thread1(void const *args)
{
    float x = 0.0;
    while(1) {
        //get a new random number for PWM
        x = rand() / float(RAND_MAX);
        //add some exponential brightness scaling
        //for more of a fast flash effect
        x = x*x*x;
        Rgb = x;
        rGb = x;
        rgB = x;
        //fast update rate for welding flashes
        Thread::wait(20);
        //add a random pause between welds
        if (rand() / float(RAND_MAX) > 0.95) {
            Rgb = 0;
            rGb = 0;
            rgB = 0;
            Thread::wait(4000.0 * rand() / float(RAND_MAX));
        }
    }

}

void thread2(void const *args)
{
    while(1) {
        uLCD_mutex.lock();
        uLCD.locate(1,2);
        uLCD.printf("Lightning!!!");
        uLCD_mutex.unlock();

        Thread::wait(20);

        uLCD_mutex.lock();
        uLCD.locate(1,2);
        uLCD.printf("            ");
        uLCD_mutex.unlock();

        if (rand() / float(RAND_MAX) > 0.95) {
            Thread::wait(4000.0 * rand() / float(RAND_MAX));
        }
    }
}

void thread3(void const *args)
{
    while (1) {
        uLCD_mutex.lock();
        uLCD.locate(1,5);
        uLCD.printf("Thunder!!!");
        uLCD_mutex.unlock();

        Thread::wait(20);

        uLCD_mutex.lock();
        uLCD.locate(1,5);
        uLCD.printf("          ");
        uLCD_mutex.unlock();

        if (rand() / float(RAND_MAX) > 0.95) {
            Thread::wait(4000.0 * rand() / float(RAND_MAX));
        }
    }
}

int main()
{
    Thread t1(thread1);
    Thread t2(thread2);
    Thread t3(thread3);

    USBHostMSD msd("usb");
    FILE *wave_file;
    //setup PWM hardware for a Class D style audio output
    PWMout.period(1.0/400000.0);
    printf("\n\n\nHello, wave world!\n");
    // wait until connected to a USB device
    while(!msd.connect()) {
        Thread::wait(500);
    }
    while (1) {
        //open wav file and play it
        wave_file=fopen("/usb/storm-thunder.wav","r");
        waver.play(wave_file);
        fclose(wave_file);
        Thread::wait(4000.0 * rand() / float(RAND_MAX));
    }
}