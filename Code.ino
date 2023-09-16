// connect to WiFi networks.
#include <WiFi.h>
// Provides asynchronous network connections.
#include <AsyncTCP.h>

#include <ESPAsyncWebServer.h>

//Provides functions to control hobby servo motors from ESP32 pins.
#include <ESP32Servo.h>
#include <iostream>
#include <sstream>

//

struct ServoPins
{
  Servo servo;
  int servoPin;
  String servoName;
  int initialPosition;
};
//This is for the robotic arm
std::vector<ServoPins> servoPins =
{
  { Servo(), 27 , "Base", 90},
  { Servo(), 26 , "Shoulder", 0},
  { Servo(), 33 , "Gripper", 165},
};

// This is for the robotic cars, the connection from the l293n driver to the esp 43

int motor1Pin1 = 16;
int motor1Pin2 = 17;
int enable1Pin = 22;

int motor2Pin1 = 18;
int motor2Pin2 = 19;
int enable2Pin = 23;

int speed1 = 150;
#define MAX_MOTOR_SPEED 250  //Its value can range from 0-250. 250 is maximum speed.

const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 200;


const char* ssid     = "RobotArm";
const char* password = "12345678";
// This is basically like setting up a website on your Arduino,
 //and it also creates a special channel for sending and receiving messages in real-time.
 //Other devices can visit your website, and  can also use this special channel to talk to your Arduino in real-time. This can be handy for things like controlling a robot arm over the internet.





AsyncWebServer server(80);
AsyncWebSocket wsRobotArmInput("/RobotArmInput");
//This is the Html, Css and Javacript code for the website
const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
    <style>

    input[type=button]
    {
      background-color:red;color:white;border-radius:30px;width:100%;height:40px;font-size:20px;text-align:center;
    }

    .noselect {
      -webkit-touch-callout: none; /* iOS Safari */
        -webkit-user-select: none; /* Safari */
        -khtml-user-select: none; /* Konqueror HTML */
         -moz-user-select: none; /* Firefox */
            -ms-user-select: none; /* Internet Explorer/Edge */
               user-select: none; /* Non-prefixed version, currently
                                      supported by Chrome and Opera */
   }

    .slidecontainer {
      width: 100%;
    }

    .slider {
      -webkit-appearance: none;
      width: 100%;
      height: 20px;
      border-radius: 5px;
      background: #d3d3d3;
      outline: none;
      opacity: 0.7;
      -webkit-transition: .2s;
      transition: opacity .2s;
    }

    .slider:hover {
      opacity: 1;
    }
    .arrows {
            font-size:40px;
            color:red;
        }
        td.button {
            background-color:black;
            border-radius:25%;
            box-shadow: 5px 5px #888888;
        }
        td.button:active {
            transform: translate(5px,5px);
            box-shadow: none;
        }

    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 40px;
      height: 40px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }

    .slider::-moz-range-thumb {
      width: 40px;
      height: 40px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }

    </style>

  </head>
  <body class="noselect" align="center" style="background-color:white">

    <h1 style="color: teal;text-align:center;"> Rayan Adoum</h1>
     <h1 style="color: teal;text-align:center;"> Robo Cross Robot  </h1>
    <h2 style="color: teal;text-align:center;">Robot Arm Control</h2>

    <table id="mainTable" style="width:400px;margin:auto;table-layout:fixed" CELLSPACING=10>
      <tr/><tr/>
      <tr/><tr/>
      <tr>
        <td style="text-align:left;font-size:25px"><b>Gripper:</b></td>
        <td colspan=2>
         <div class="slidecontainer">
         <!-- So it start of close and then you can open using the Slider, any time you move the slider it will send a new value -->
            <input type="range" min="135" max="165" value="165" class="slider" id="Gripper" oninput='sendButtonInput("Gripper",value)'>
          </div>
        </td>
      </tr>
      <tr/><tr/>
      <tr>
        <td style="text-align:left;font-size:25px"><b>Shoulder:</b></td>
        <td colspan=2>
         <div class="slidecontainer">
         <!-- Start up high and then you can move it down  -->
            <input type="range" min="0" max="120" value="0" class="slider" id="Shoulder" oninput='sendButtonInput("Shoulder",value)'>
          </div>
        </td>
      </tr>
      <tr/><tr/>
      <tr>
        <td style="text-align:left;font-size:25px"><b>Base:</b></td>
        <td colspan=2>
         <div class="slidecontainer">
         <!-- Start off in the middle and then you can turn it righ tor left -->
            <input type="range" min="0" max="180" value="90" class="slider" id="Base" oninput='sendButtonInput("Base",value)'>
          </div>
        </td>
      </tr>
      <tr/><tr/>
    </table>
  <h2>-----------------------------------------------------------------------------------</h2>
<h2 style="color: teal;text-align:center;">Car Control</h2>
<table id="mainTable" style="width:400px;margin:auto;table-layout:fixed" CELLSPACING=10>

    <tr>
        <td></td>
        <td class="button" ontouchstart='sendButtonInput("MoveCar","1")' ontouchend='sendButtonInput("MoveCar","0")'><span class="arrows" >&#8679;</span></td>
        <td></td>
    </tr>
    <tr>
        <td class="button" ontouchstart='sendButtonInput("MoveCar","3")' ontouchend='sendButtonInput("MoveCar","0")'><span class="arrows" >&#8678;</span></td>
        <td class="button"></td>
        <td class="button" ontouchstart='sendButtonInput("MoveCar","4")' ontouchend='sendButtonInput("MoveCar","0")'><span class="arrows" >&#8680;</span></td>
    </tr>
    <tr>
        <td></td>
        <td class="button" ontouchstart='sendButtonInput("MoveCar","2")' ontouchend='sendButtonInput("MoveCar","0")'><span class="arrows" >&#8681;</span></td>
        <td></td>
    </tr>
    <tr/><tr/>
    <tr>
        <td style="text-align:left"><b>Speed:</b></td>
        <td colspan=2>
            <div class="slidecontainer">
                <input type="range" min="0" max="255" value="150" class="slider" id="Speed" oninput='sendButtonInput("Speed",value)'>
            </div>
        </td>
    </tr>
</table>


    <script>
      var webSocketRobotArmInputUrl = "ws:\/\/" + window.location.hostname + "/RobotArmInput";
      var websocketRobotArmInput;
      var speed = 150;
      function initRobotArmInputWebSocket()
      {
      // Setting the WebSocket connection
        websocketRobotArmInput = new WebSocket(webSocketRobotArmInputUrl);
        websocketRobotArmInput.onopen    = function(event){
          var speedButton = document.getElementById("Speed");
            sendButtonInput("Speed", speedButton.value); //Storing the speed value and sending it
            };


        websocketRobotArmInput.onclose   = function(event){setTimeout(initRobotArmInputWebSocket, 2000);};
        websocketRobotArmInput.onmessage    = function(event)
        {
          var keyValue = event.data.split(",");
          var button = document.getElementById(keyValue[0]);
          button.value = keyValue[1];
        };
      }

      function sendButtonInput(key, value)
      {
        var data = key + "," + value;
        websocketRobotArmInput.send(data);
      }

      window.onload = initRobotArmInputWebSocket;
      document.getElementById("mainTable").addEventListener("touchend", function(event){
        event.preventDefault()
      });
    </script>
  </body>
</html>
)HTMLHOMEPAGE";
//this function will be called when a client makes an HTTP request to the root path of the web server
void handleRoot(AsyncWebServerRequest *request)
{
  request->send_P(200, "text/html", htmlHomePage);//Sending the Html File
}

void handleNotFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "File Not Found");
}

void moveCar(int inputValue)
{
  // zero is stop
  //forward is 1
  //left is a 3
  //right is 4
  //down is 2
  Serial.printf("Got value as %d\n", inputValue);
  ledcWrite(pwmChannel, speed1);
  if(inputValue == 0){
     digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, LOW);
      digitalWrite(motor2Pin1, LOW);
      digitalWrite(motor2Pin2, LOW);
  }
  else if(inputValue == 1){

      digitalWrite(motor1Pin1, LOW);
       digitalWrite(motor1Pin2, HIGH);
      digitalWrite(motor2Pin1, HIGH);
       digitalWrite(motor2Pin2, LOW);
  }
   else if(inputValue == 2){
 digitalWrite(motor1Pin1, HIGH);
      digitalWrite(motor1Pin2, LOW);
      digitalWrite(motor2Pin1, HIGH);
      digitalWrite(motor2Pin2, LOW);
  }
   else if(inputValue == 3){
    digitalWrite(motor1Pin1, HIGH);
       digitalWrite(motor1Pin2, LOW);
       digitalWrite(motor2Pin1, LOW);
       digitalWrite(motor2Pin2, HIGH);
  }
   else if(inputValue == 4){
          digitalWrite(motor1Pin1, LOW);
       digitalWrite(motor1Pin2, HIGH);
       digitalWrite(motor2Pin1, HIGH);
       digitalWrite(motor2Pin2, LOW);
  }

}
void onRobotArmInputWebSocketEvent(AsyncWebSocket *server,
                      AsyncWebSocketClient *client,
                      AwsEventType type,
                      void *arg,
                      uint8_t *data,
                      size_t len)
{
  switch (type)
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        std::istringstream ss(myData);
        std::string key, value;
        std::getline(ss, key, ',');
        std::getline(ss, value, ',');
        Serial.printf("Key [%s] Value[%s]\n", key.c_str(), value.c_str());
        int valueInt = atoi(value.c_str());
        else if (key == "Base")
        {
   servoPins[0].servo.write(valueInt); // This is  used to set the position or value of the servo motor.
         we send the value which is a integer value that determines the desired position or value for the servo motor.
        }
        else if (key == "Shoulder")
        {
         servoPins[1].servo.write(valueInt);
        }
        else if (key == "Gripper")
        {
      servoPins[2].servo.write(valueInt);
        }

         else if (key == "MoveCar")
        {
         moveCar(valueInt); // calling the moveCar method so it can be able to move the car
        }
        else if (key == "Speed")
        {
          speed1 =  valueInt;//changing the Speed
        }

      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;
  }
}


void setUpPinModes()
{
  for (int i = 0; i < servoPins.size(); i++)
  {
    servoPins[i].servo.attach(servoPins[i].servoPin); //attach the servo motor to a specific pin on esp32
    servoPins[i].servo.write(servoPins[i].initialPosition);
  }
  //Setting all l293n driver pins to output
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
   pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
pinMode(enable1Pin, OUTPUT);
  pinMode(enable2Pin, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);
   ledcAttachPin(enable2Pin, pwmChannel);
}


void setup(void)
{
  setUpPinModes();
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

//This line sets up a route for the HTTP server.
//It specifies that when a client (typically a web browser) makes an HTTP GET request
//to the root path ("/"),it send the Html code stored in the htmlHomePage variable.
  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
//This line sets up an event handler for the WebSocket server.
 //It specifies that when a WebSocket event occurs on the "wsRobotArmInput" WebSocket,
  //the onRobotArmInputWebSocketEvent function should be called to handle the event.

  wsRobotArmInput.onEvent(onRobotArmInputWebSocketEvent);
  server.addHandler(&wsRobotArmInput);
//This line starts the HTTP server, making it ready to listen for incoming HTTP requests.

  server.begin();
  Serial.println("HTTP server started");

}

void loop()
{

}
