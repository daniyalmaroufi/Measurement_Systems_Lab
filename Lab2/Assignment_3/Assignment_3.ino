const int encoder = 2; // The encoder output is connected to the PIN 2 of the arduino uno
const int led = 4;     // The LED is connected to the PIN 4 of the arduino uno

const int debouncing_treshold = 1000; // The bouncing time is set to 1 milisecond

const int slots_count = 36; // The number of the slots on the paper
float resolution;           // The resolution of the encoder

volatile long current_time;            // To save the time of the current pulse
volatile long previous_pulse_time = 0; // To save the time of the previous pulse
volatile long pulse_counter = 0;       // The variable to count the number of the pulses

volatile bool pulse_flag = false; // To check the pulse occurance to print in the serial monitor
volatile float theta, omega;      // The values of theta and omega

void setup()
{
    resolution = 360.0 / slots_count; // The resolution is related to the number of slots
    Serial.begin(9600);               // The baud rate is set to 9600 Hz
    pinMode(encoder, INPUT_PULLUP);   // The encoder is defined as pullup input
    pinMode(led, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(encoder), encoder_pulse_handler_rising, RISING);   // Attach the encoder_pulse_handler_rising function for the rising pulse
    attachInterrupt(digitalPinToInterrupt(encoder), encoder_pulse_handler_falling, FALLING); // Attach the encoder_pulse_handler_falling function for the falling pulse
}

void loop()
{
    // if recieved a pulse, blink the LED and print the values of position and speed
    if (pulse_flag)
    {
        digitalWrite(led, 1);
        delay(10);
        digitalWrite(led, 0);
        Serial.print("theta= ");
        Serial.print(theta);
        Serial.print("(Deg)");
        Serial.print("\tomega= ");
        Serial.print(omega);
        Serial.println("(Deg/s)");
        delay(10);
        pulse_flag = false;
    }
}

// The function for the rising pulse
void encoder_pulse_handler_rising()
{
    current_time = micros();
    if (current_time - previous_pulse_time > debouncing_treshold) // To prevent bouncing
        previous_pulse_time = micros();                           // Save the time of the first pulse
}

// The function for the falling pulse
void encoder_pulse_handler_falling()
{
    current_time = micros();
    if (current_time - previous_pulse_time > debouncing_treshold) // To prevent bouncing
    {
        pulse_counter += 1;                                                                 // Count the pulse
        theta = pulse_counter * resolution;                                                 // Update the value of theta
        omega = resolution * 1000.0 * 1000.0 / float((current_time - previous_pulse_time)); // Update the value of the omega
        pulse_flag = true;
        previous_pulse_time = current_time; // Update the previous time to the current time
    }
}
