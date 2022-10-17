# ChacalPotager

The final goal of the ChacalPotager project is to realize a complete machine that supervises the state of a vegetable garden, gives indications on what has to be done on it, even, the changes that have to be made.

We wish to have a vegetable garden that is as autonomous as possible. We mean, an automatic watering, asks the user to collect when it is necessary, to work the soil, to fill the water reserve (if it is not autonomous), information on the different fruits/vegetables that we can plant with other information on what we must plant after the next harvest.

## Our tools

The desire to make a connected vegetable garden goes with our desire to be eco-responsible. It would be contradictory to put sensors everywhere. Our challenge is to succeed in reaching our goal with the least amount of electronic tools possible.

### Home Assistant

We will use HomeAssistant to fulfill the display mission. It is an extremely powerful tool because it will allow us to control the status of our sensors, display the necessary changes, control our electrical consumption ... It is a tool often used in DIY projects in the world of home automation. It requires a machine, in order to be hosted. From the virtual machine, to the raspeberry, we will explain our choice of machine later.

### PLATFORMIO

We'll use Platformio as an IDE to developp the solution.

### ESP32

The ESP32 will act as a controller for our sensors, and send the data frames to our HomeAssistant server.

