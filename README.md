# Dog collar
## Introduction 🐕

Welcome to the Dog Collar Audio Project! This innovative project aims to foster better communication with our canine friends, no matter the distance. With a built-in microphone and speaker on your dog's collar, you can listen to the sounds captured by the microphone from anywhere in the world and even send voice messages to your dog via the speaker.

---

## Components 🛠

The core components driving this project are:

**1. ESP32 Controller:** Enables the device's WiFi connectivity
**2. INMP441 Microphone:** Captures the audio.
**3. Speaker (Connected to MAX98357A Amplifier):** For playing back the audio.
---

## Software & Websocket Server 🌐

The audio transmission is powered by a websocket set up on a VPS using the **`server.js`** file.

**To Initialize the Websocket Server:**

Connect to your VPS console with the **`server.js`** file
Run the command: **`sudo node server.js`**.
Note: In a production environment, consider using a library such as **`pm2`** to keep the websocket server running continuously. Also, remember that the websocket is established over HTTPS. Therefore, you should specify your SSL certificate paths in the **`server.js`** file.
---

## Web Interface 🖥

The **`index.html`** file provides a simple interface with two primary buttons:

**1. Listen Button:** Upon pressing, it sends the "START_LISTENING" command to the websocket, prompting the INMP441 microphone to send audio to the websocket. This audio is then played back in the web interface. Releasing the button sends the "STOP_LISTENING" command to the websocket.

**2. Send Audio Button:**
Before sending audio from the browser to the websocket, ensure that the audio data is processed to match the correct sample rate and other specifications.
---

## Arduino Logic 🎛

The ESP32 device performs the following steps:

1. Connects to WiFi.
2. Connects to the websocket.
3. Listens for commands from the websocket and acts accordingly, based on the logic detailed above.

---

## Future Improvements & To-Dos 📈

1. Codebase cleanup and optimization.
2. Implementation of user logic to support multiple devices, ensuring users can only listen to their designated device.

---

## Conclusion 🐾

This project is a blend of hardware, software, and connectivity, aiming to bring pet owners closer to their furry friends. Whether you're across the room or across the globe, feel the presence of your canine companion right beside you.

**Happy Pet Communicating! 🎉🐶🔊**
