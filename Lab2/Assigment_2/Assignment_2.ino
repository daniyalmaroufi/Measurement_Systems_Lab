#include <TimerOne.h>

const int led = 12;                       // The LED is connected to the PIN 12 of the arduino uno
const int start_stop_button = 2;          // The push button for the Start/Stop is connected to the PIN 2 of the arduino uno
const int reset_button = 3;               // The push button for the Reset is connected to the PIN 3 of the arduino uno
const long debouncing_treshold = 1000000; // The bouncing time is set to 1 second

volatile bool led_state = HIGH;              // The initial state of the LED is On
volatile bool is_active = false;             // The Timer is Off first time
volatile unsigned long chrono_time = 0;      // The Time of the chronometer
volatile unsigned long prev_chrono_time = 0; // The Time saved from the last start and stop of the chronometer
volatile unsigned long t_bouncing = 0;       // The time of bouncing to prevent bouncing in the toggle_chrono function
volatile unsigned long t2 = 0;               // The time of bouncing to prevent bouncing in the reset function
volatile unsigned long starting_time = 0;    // The time of pressing the start button
unsigned long total_time;                    // The total time spent since the start of the microcontroller
int mins, sec, mil;                          // The values of minutes, second, and miliseconds to print the values

bool start_flag = false; // The flag to print the start in the serial monitor
bool stop_flag = false;  // The flag to print the stop in the serial monitor
bool reset_flag = false; // The flag to print the reset in the serial monitor

void setup()
{
    Serial.begin(9600);                       // set the baud rate o 9600
    pinMode(start_stop_button, INPUT_PULLUP); // Define the start/stop button as pullup
    pinMode(reset_button, INPUT_PULLUP);      // Define the start/stop button as pullup
    pinMode(led, OUTPUT);
    Timer1.initialize(1000000);                                                        // The time interval of blinking LED is 1 second
    Timer1.attachInterrupt(blink_led);                                                 // Attach the blink_led function to the timer
    attachInterrupt(digitalPinToInterrupt(reset_button), reset_chrono, FALLING);       // Attach the reset_chrono function to the hardware interupt of reset button
    attachInterrupt(digitalPinToInterrupt(start_stop_button), toggle_chrono, FALLING); // Attach the toggle_chrono function to the hardware interupt of start/stop button
    reset_chrono();                                                                    // Reset the chronometer at the start
    Serial.println("00:00.000");                                                       // Show the initial time of the chronometer as 00:00.000
}

void loop()
{
    if (is_active) // If the chronometer is active, update the value of the chrono_time
    {
        total_time = millis();
        chrono_time = prev_chrono_time + (total_time - starting_time);
    }
    if (start_flag)
    {
        Serial.println("Start");
        start_flag = false;
    }
    if (stop_flag)
    {
        print_time();
        stop_flag = false;
    }

    if (reset_flag)
    {
        Serial.println("00:00.000");
        reset_flag = false;
    }
    delay(10);
}

// The function for the software interrupt to blink the LED
void blink_led()
{
    digitalWrite(led, led_state);
    led_state = !led_state;
}

// The function to toggle the state of the chronometer by the start/stop button
void toggle_chrono()
{
    if (micros() - t_bouncing > debouncing_treshold) // To prevent bouncing
    {
        if (is_active == false) // Start the chronometer as it is not active
        {
            start_flag = true;
            starting_time = millis();
            is_active = true;
        }
        else // Stop the chronometer as the stop button is pressed
        {
            stop_flag = true;
            prev_chrono_time = chrono_time;
            is_active = false;
        }
        t_bouncing = micros();
    }
}

// The function to reset the value of the chronometer
void reset_chrono()
{
    if (micros() - t2 > debouncing_treshold) // To prevent bouncing
    {
        reset_flag = true;
        prev_chrono_time = 0;
        starting_time = total_time;
        mins = 0;
        sec = 0;
        mil = 0;
        t2 = micros();
    }
}

// The function to print the time on the serial monitor
void print_time()
{
    mins = chrono_time / 60000;         // Convert the miliseconds to minutes
    sec = (chrono_time % 60000) / 1000; // Convert the miliseconds to seconds
    mil = (chrono_time % 60000) % 1000; // The remaning of the time to miliseconds
    if (mins < 10)                      // Print an extra 0 for the one-digit numbers
        Serial.print("0");
    Serial.print(mins);
    Serial.print(":");
    if (sec < 10) // Print an extra 0 for the one-digit numbers
        Serial.print("0");
    Serial.print(sec);
    Serial.print(".");
    Serial.println(mil);
}
