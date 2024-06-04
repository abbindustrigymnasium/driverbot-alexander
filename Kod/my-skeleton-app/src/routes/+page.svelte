<script>
  import { onMount } from 'svelte';

  // MQTT broker connection options
  const brokerUrl = 'maqiatto.com';
  const port = 8883; // MQTT over WebSockets port
  const clientId = 'clientId_' + Math.random().toString(16).substr(2, 8); // Generate a random client ID
  const username = 'kaipainenalexander@gmail.com'; // Your Maqiatto username
  const password = 'Orange123'; // Your Maqiatto password

  // MQTT client instance
  let client = null;

  // Speed variable
  let speed = 0;
  
  // Turning variable
  let rotation = 0;
  
  // Record variables
  let Record = false;
  let RecordT = false;

  // Message queues for recording
  let messageQueue = [];
  let messageQueueT = [];

  // Function to connect to the MQTT broker
  function connectToBroker() {
    // Initialize MQTT client
    client = new Paho.MQTT.Client(brokerUrl, port, clientId);
   
    // Set callback handlers
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;

    // Connect to MQTT broker
    client.connect({
      onSuccess: onConnect,
      userName: username,
      password: password,
      useSSL: false // Maqiatto does not support SSL
    });
  }

  // Called when the client connects successfully
  function onConnect() {
    console.log('Connected to MQTT broker.');
  }

  // Called when the client loses its connection
  function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
      console.error('Connection lost:', responseObject.errorMessage);
    }
  }

  // Called when a message arrives from the broker
  function onMessageArrived(message) {
    console.log('Message received:', message.payloadString);
  }

  // Function to send a message to the broker
  function sendMessage(topic, message) {
    if (Record || RecordT) {
      // Queue messages if recording is enabled
      if (Record) {
        messageQueue.push({ topic, message });
      }
      if (RecordT) {
        messageQueueT.push({ topic, message });
        const messageObject = new Paho.MQTT.Message(message);
        messageObject.destinationName = topic;
        client.send(messageObject);
        console.log('Message sent:', message);
      }
      console.log('Message queued:', message);
    } else {
      // Send message directly if not recording
      if (client && client.isConnected()) {
        const messageObject = new Paho.MQTT.Message(message);
        messageObject.destinationName = topic;
        client.send(messageObject);
        console.log('Message sent:', message);
      } else {
        console.error('Not connected to MQTT broker.');
      }
    }
  }

  // Event listener for button click to connect
  onMount(() => {
    document.getElementById('connectButton').addEventListener('click', () => {
      if (!client || !client.isConnected()) {
        connectToBroker();
      } else {
        console.log('Already connected to MQTT broker.');
      }
    });
  });

  // Event listener for record button click
  onMount(() => {
    let isRecording = false;
    document.getElementById('record').addEventListener('click', () => {
      isRecording = !isRecording;
      Record = isRecording;
      console.log(`Recording: ${Record}`);
      
      if (!Record) {
        // Send all queued messages when recording stops
        while (messageQueue.length > 0) {
          const { topic, message } = messageQueue.shift();
          sendMessage(topic, message);
        }
      }
    });
  });

  // Event listener for recordT button click
  onMount(() => {
    document.getElementById('recordT').addEventListener('click', () => {
      RecordT = !RecordT;
      console.log(`Recording with T: ${RecordT}`);
      
      if (!RecordT) {
        // Send all queued messages for T when recording stops
        while (messageQueueT.length > 0) {
          const { topic, message } = messageQueueT.shift();
          sendMessage(topic, message);
        }
      }
    });
  });

  // Boolean variables for key states
  let is_w_down = false; 
  let is_a_down = false; 
  let is_s_down = false; 
  let is_d_down = false; 

  // Track the active gear button
  let activeGear = null; 
  let gearboxActive = false; // Track the state of the gearbox button
  let recordActive = false; // Track the state of the record button
  let recordTActive = false; // Track the state of the recordT button
  
  // Function for pressing down a key
  function handleKeyDown(event) {
    switch (event.key) {
      case "w":
        is_w_down = true;
        document.querySelector("[data-key='w']").style.backgroundColor = "grey";
        console.log("Sending command: Forward");
        sendMessage('kaipainenalexander@gmail.com/Controlls', 'Forward');
        break;

      case "a":
        is_a_down = true;
        document.querySelector("[data-key='a']").style.backgroundColor = "grey";
        console.log("Sending command: Turning Left");
        sendMessage('kaipainenalexander@gmail.com/Controlls', 'SwerveLeft');
        break;

      case "s":
        is_s_down = true;
        document.querySelector("[data-key='s']").style.backgroundColor = "grey";
        console.log("Sending command: Brake");
        sendMessage('kaipainenalexander@gmail.com/Controlls', 'Backwards');
        break;

      case "d":
        is_d_down = true;
        document.querySelector("[data-key='d']").style.backgroundColor = "grey";
        console.log("Sending command: Turning Right");
        sendMessage('kaipainenalexander@gmail.com/Controlls', 'SwerveRight');
        break;

      case "1":
        setActiveGear('1');
        break;

      case "2":
        setActiveGear('2');
        break;

      case "3":
        setActiveGear('3');
        break;

      case "4":
        setActiveGear('4');
        break;

      case "r":
        setActiveGear('r');
        break;

      case "g":
        toggleGearbox();
        break;

      case "y":
        toggleRecord();
        break;

      case "t":
        toggleRecordT();
        break;
    }
  };

  // Function for when the key stops being pressed, used for stopping movement
  function handleKeyUp(event) {
    switch (event.key) {
      case "w":
        is_w_down = false;
        document.querySelector("[data-key='w']").style.backgroundColor = "#ccc";
        console.log("Sending command: Stop driving forward");
        sendMessage('kaipainenalexander@gmail.com/Controlls', 'NoForward');
        break;

      case "a":
        is_a_down = false;
        document.querySelector("[data-key='a']").style.backgroundColor = "#ccc";
        console.log("Sending command: Stop Turning left");
        sendMessage('kaipainenalexander@gmail.com/Controlls', 'NoSwerveLeft');
        break;

      case "s":
        is_s_down = false;
        document.querySelector("[data-key='s']").style.backgroundColor = "#ccc";
        console.log("Sending command: Stop driving backwards");
        sendMessage('kaipainenalexander@gmail.com/Controlls', 'NoBackward');
        break;

      case "d":
        is_d_down = false;
        document.querySelector("[data-key='d']").style.backgroundColor = "#ccc";
        console.log("Sending command: Stop Turning Right");
        sendMessage('kaipainenalexander@gmail.com/Controlls', 'NoSwerveRight');
        break;
    }
  }

  // Function for setting gears, ensures only one gear is active at a time
  function setActiveGear(gear) {
    if (activeGear) {
      document.querySelector(`[data-key='${activeGear}']`).style.backgroundColor = "#666";
      sendMessage('kaipainenalexander@gmail.com/Controlls', `NoGear${activeGear.toUpperCase()}`);
    }
    activeGear = gear;
    document.querySelector(`[data-key='${gear}']`).style.backgroundColor = "black";
    sendMessage('kaipainenalexander@gmail.com/Controlls', `Gear${gear.toUpperCase()}`);
    console.log(`Sending command: Shift to gear ${gear.toUpperCase()}`);
    
    // Update speed and max rotation based on gear
    switch (gear) {
      case '1':
        speed = 64;
        rotation = 100;
        break;
      case '2':
        speed = 128;
        rotation = 75;
        break;
      case '3':
        speed = 192;
        rotation = 50;
        break;
      case '4':
        speed = 256;
        rotation = 25;
        break;
      case 'r':
        speed = -256;
        break;
    }
  }

  // Set speed to 0 when 'W' is not pressed
  if (!is_w_down) {
    speed = 0;
  }
  if (is_w_down) {
    displayedSpeed = speed; // Update displayed speed if 'W' is pressed
  }

  // Function for toggling on and off the gearbox
  function toggleGearbox() {
    gearboxActive = !gearboxActive;
    const gearboxButton = document.querySelector("[data-key='g']");
    if (gearboxActive) {
      gearboxButton.style.backgroundColor = "grey";
      sendMessage('kaipainenalexander@gmail.com/Controlls', 'GearboxOn');
      console.log('Sending command: Gearbox On');
    } else {
      gearboxButton.style.backgroundColor = "#ccc";
      sendMessage('kaipainenalexander@gmail.com/Controlls', 'GearboxOff');
      console.log('Sending command: Gearbox Off');
    }
  }

  // Function to toggle recording state, prevents movement while recording
  function toggleRecord() {
    recordActive = !recordActive;
    const recordButton = document.querySelector("[data-key='y']");
    if (recordActive) {
      recordButton.style.backgroundColor = "black";
      Record = true;
    } else {
      recordButton.style.backgroundColor = "red";
      Record = false;
      // Send all queued messages when recording stops
      while (messageQueue.length > 0) {
        const { topic, message } = messageQueue.shift();
        sendMessage(topic, message);
      }
    }
  }

  // Function to toggle recording state, allows movement while recording
  function toggleRecordT() {
    recordTActive = !recordTActive;
    const recordTButton = document.querySelector("[data-key='t']");
    if (recordTActive) {
      recordTButton.style.backgroundColor = "black";
      RecordT = true;
    } else {
      recordTButton.style.backgroundColor = "red";
      RecordT = false;
      // Send all queued messages for T when recording stops
      while (messageQueueT.length > 0) {
        const { topic, message } = messageQueueT.shift();
        sendMessage(topic, message);
      }
    }
  }
</script>

<style>
  /* The appearance of the keybind */
  .Wkey {
    width: 75px;
    height: 75px;
    margin: 5px;
    background-color: #ccc;
    border: none;
    border-radius: 5px;
    font-size: 24px;
    font-weight: bold;
    position: absolute;
    top: 350px;
    left: 150px;
  }

  /* The appearance of the keybind */
  .Dkey {
    width: 75px;
    height: 75px;
    margin: 5px;
    background-color: #ccc;
    border: none;
    border-radius: 5px;
    font-size: 24px;
    font-weight: bold;
    position: absolute;
    top: 425px;
    left: 225px;
  }

  /* The appearance of the keybind */
  .Skey {
    width: 75px;
    height: 75px;
    margin: 5px;
    background-color: #ccc;
    border: none;
    border-radius: 5px;
    font-size: 24px;
    font-weight: bold;
    position: absolute;
    top: 425px;
    left: 150px;
  }

  /* The appearance of the keybind */
  .Akey {
    width: 75px;
    height: 75px;
    margin: 5px;
    background-color: #ccc;
    border: none;
    border-radius: 5px;
    font-size: 24px;
    font-weight: bold;
    position: absolute;
    top: 425px;
    left: 75px;
  }

  .controls {
    display: flex;
    justify-content: center;
    align-items: center;
    flex-wrap: wrap;
  }

  /* Appearance of the gearbox */
  .gearbox {
    height: 300px;
    width: 200px;
    position: absolute;
    right: 100px;
    top: 250px;
    border-radius: 50px;
    background-color: #ccc;
  }

  /* Uniform appearance for the gears */
  .gear1, .gear2, .gear3, .gear4, .gearR {
    height: 50px;
    width: 50px;
    position: absolute;
    border-radius: 50px;
    background-color: #666;
  }

  /* The position of the gears */
  .gear1 {
    top: 280px;
    right: 225px;
  }

  .gear2 {
    top: 330px;
    right: 125px;
  }

  .gear3 {
    top: 380px;
    right: 225px;
  }

  .gear4 {
    top: 430px;
    right: 125px;
  }

  .gearR {
    top: 480px;
    right: 225px;
  }

  /* Appearance of the connect button */
  .connectButton {
    top: 20px;
    right: 20px;
    position: absolute;
    height: 30px;
    width: 75px;
    border-radius: 10px;
    background-color: #ccc;
  }

  /* Style for the image container */
  .image-container {
    display: flex;
    justify-content: center;
    align-items: center;
    margin-top: 20px;
  }

  /* Size of the image */
  .your-image-class {
    max-width: 60%;
    height: auto;
    border: 1px solid #ccc;
    border-radius: 10px;
  }

  /* Appearance of the speed display */
  .speed-display {
    position: absolute;
    top: 180px;
    left: 38%;
    transform: translateX(-50%);
    font-size: 12px;
    font-weight: bold;
    background-color: #000000;
    color: rgba(255, 0, 0, 0.795);
    padding: 10px;
    border-radius: 5px;
    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
  }

  /* Appearance of the rotation display */
  .turn-display {
    position: absolute;
    top: 180px;
    left: 62%;
    transform: translateX(-50%);
    font-size: 14px;
    font-weight: bold;
    background-color: #000000;
    color: rgba(255, 0, 0, 0.795);
    padding: 10px;
    border-radius: 5px;
    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
  }

  /* Appearance of the record button */
  .record {
    position: absolute;
    top: 450px;
    right: 600px;
    width: 75px;
    height: 75px;
    border-radius: 75px;
    background-color: red;
  }

  /* Appearance of the other record button */
  .recordT {
    position: absolute;
    top: 450px;
    right: 500px;
    width: 75px;
    height: 75px;
    border-radius: 75px;
    background-color: red;
  }
</style>

<svelte:window on:keydown={handleKeyDown} on:keyup={handleKeyUp} />
<!-- Setting the language of the HTML, the title, and the Paho script that is necessary for this project -->
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>MQTT Button</title>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.2/mqttws31.min.js"></script>
</head>
<body>
  <!-- Making the button elements on my website -->
  <button id="connectButton" class="connectButton">Connect</button>
  <div role="group" aria-label="Keyboard Controls" class="controls">
    <button class="Wkey" data-key="w">W</button>
    <button class="Akey" data-key="a">A</button>
    <button class="Skey" data-key="s">S</button>
    <button class="Dkey" data-key="d">D</button>
    <button class="gearbox" data-key="g"></button>
    <button class="gear1" data-key="1">1</button>
    <button class="gear2" data-key="2">2</button>
    <button class="gear3" data-key="3">3</button>
    <button class="gear4" data-key="4">4</button>
    <button class="gearR" data-key="r">R</button>
    <button id="record" class="record" data-key="y"></button>
    <button id="recordT" class="recordT" data-key="t"></button>
  </div>

  <!-- Image -->
  <div class="image-container">
    <img src="United.jpg" alt="" class="your-image-class">
  </div>
  
  <!-- Speed display -->
  <div class="speed-display">MaxSpeed: {speed}</div>
  <div class="turn-display">MaxTurn: {rotation}%</div>

</body>
</html>
