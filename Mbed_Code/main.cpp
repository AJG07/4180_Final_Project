#include "Thread.h"
#include "mbed.h"
#include "rtos.h"
#include "Motor.h"
#include "Servo.h"

Motor rightMotor(p21, p5, p6); // pwm, fwd, rev
Motor leftMotor (p22, p7, p8); // pwm, fwd, rev

// Pi mbed USB Slave function
// connect mbed to Pi USB
RawSerial  pi(USBTX, USBRX);

// sonar
DigitalOut trigger(p11);
DigitalIn  echo(p12);

Timer sonar; 
Servo myservo(p24);
PwmOut speaker(p23);

// debug LEDs
DigitalOut myled(LED1); //monitor trigger
DigitalOut myled2(LED2); //monitor echo
DigitalOut myled3(LED3);
DigitalOut myled4(LED4);

// sonar variables
int distance = 0;
int correction = 0;

// control variables
volatile int stop = 0;
volatile float p = 0.5;

// function to receive and parse commands from the Pi
void dev_recv(void const *argument)
{
    while(1){
        signed char temp1 = 0;
        signed char temp2 = 0;
        signed char temp3 = 0;
        while(pi.readable()) {
            temp1 = pi.getc();
            pi.putc(temp1);
            temp2 = pi.getc();
            pi.putc(temp2);
            float rightSpeed = static_cast<int>(temp1) / 127.0f;
            float leftSpeed = static_cast<int>(temp2) / 127.0f;
            if(stop){
                if (rightSpeed < 0) {
                    rightMotor.speed(rightSpeed);
                } else {
                    rightMotor.speed(0);
                }
                if (leftSpeed < 0) {
                    leftMotor.speed(leftSpeed);
                } else {
                    leftMotor.speed(0);
                }
            } else {
                rightMotor.speed(rightSpeed);
                leftMotor.speed(leftSpeed);
            }

            
            temp3 = pi.getc();
            pi.putc(temp3);
            temp3 = static_cast<int>(temp3);
            if(temp3 == 1)
            {
                if(p < 1) p += 0.05;
            }
            if(temp3 == -1)
            {
                if(p > 0) p -= 0.05;
            }
            
        }
        Thread::wait(30);
        rightMotor.speed(0);
        leftMotor.speed(0);
    }
    
}

void measuredist(void const *argument)
{

    //Loop to read Sonar distance values, scale, and print
    while(1) {
        // trigger sonar to send a ping
        trigger = 1;
        //myled = 1;
        myled2 = 0;
        sonar.reset();
        wait_us(10.0);
        trigger = 0;
        //wait for echo high
        while (echo==0) {};
        myled2=echo;
        //echo high, so start timer
        sonar.start();
        //wait for echo low
        while (echo==1) {};
        //stop timer and read value
        sonar.stop();
        //subtract software overhead timer delay and scale to cm
        distance = (sonar.read_us()-correction)/58.0;
        myled2 = 0;
        if(distance > 15)
        {
            myled = 1;
            stop = 0;
        }
        else {
            myled = 0;
            stop = 1;
        }
        //printf(" %d cm \n\r",distance);
        //wait so that any echo(s) return before sending another ping
        Thread::wait(200);
    }
}

// function to raise alarm noise if distance too close
void alarm(void const *argument)
{
    while(1)
    {
        if(stop == 1)
        {
            myled4 = 1;
            speaker = 0.5;
        }
        else {
            myled4 = 0;
            speaker = 0;
        }
        Thread::wait(200);
    }    
}

// function to move camera servo
void camera(void const *argument)
{
    while(1)
    {
        myservo = p;
        Thread::wait(200);
    }
}

int main()
{
    // start all threads
    pi.baud(9600);
    speaker.period(1.0/500.0);
    Thread speakerThread(alarm);
    Thread sonarThread(measuredist);
    Thread motorThread(dev_recv);
    Thread servoThread(camera);
    //pi.attach(&dev_recv, Serial::RxIrq);
    
    while(1) {
        Thread::wait(100);
    }
}
