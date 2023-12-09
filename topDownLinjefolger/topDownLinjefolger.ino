#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Buzzer buzzer;


void setup() // run once, when the sketch starts
{
}

void loop() // run over and over again
{
    honk();
}

unsigned long previousBuzzerMillis = 0;

// Tuter to ganger 
void honk()
{
    const int interval = 200;                     // Intervallet mellom tutene
    const int freq = 400;                         // Frekvensen tutene er på 
    unsigned long currentBuzzerMillis = millis(); // Lagre nåværende tidspunkt

    if (currentBuzzerMillis - previousBuzzerMillis >= interval) // Sjekker om det har gått 200 millisek siden siste tut
    {   
        if (currentBuzzerMillis - previousBuzzerMillis < interval + 100)
        {   // Spill første tut i 100 millisek
            buzzer.playFrequency(freq, 100, 15);
        }
        else if (currentBuzzerMillis - previousBuzzerMillis < interval * 2)
        {   // Spill andre tut i 250 millisek
            buzzer.playFrequency(freq, 250, 15);
        }
        else
        {   // Reset tiden og slutt og spill
            previousBuzzerMillis = currentBuzzerMillis;
            buzzer.stopPlaying();
        }
    }
}
