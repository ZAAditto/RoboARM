#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

const char* ssid = "UnileverIPE";
const char* password = "2018334064";

const char* PARAM_1 = "AngleX";
const char* PARAM_2 = "SpeedX";
const char* PARAM_3 = "AngleY";
const char* PARAM_4 = "SpeedY";
const char* PARAM_5 = "AngleZ";
const char* PARAM_6 = "SpeedZ";
const char* PARAM_7 = "Commands";
const char* PARAM_8 = "Count";

const String inputParam1 = "AngleX";
const String inputParam2 = "SpeedX";
const String inputParam3 = "AngleY";
const String inputParam4 = "SpeedY";
const String inputParam5 = "AngleZ";
const String inputParam6 = "SpeedZ";
const String inputParam7 = "Commands";
const String inputParam8 = "Count";

const int stepPinX = 0; //
const int dirPinX = 17;
const int stepPinY = 2; //
const int dirPinY= 16;
const int stepPinZ = 4; //
const int dirPinZ = 15;
//const int sol = 13;
int n;
unsigned int X;
//or int X;
unsigned int Y;
//or int Y;
unsigned int P;
unsigned int Q;
char direc[100] = "";
int x, xs, y, ys, z, zs;
int AngleX, AngleY, AngleZ;
String inputMessage1, inputMessage2, inputMessage3, inputMessage4, inputMessage5, inputMessage6, inputMessage7, inputMessage8;
int checking = 0;
const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>RoboArm</title>
  <style>
    body {
      margin: 0;
      padding: 0;
      font-family: Arial, Helvetica, sans-serif;
      background-color: #2c257a;
    }

    .box {
      width: 90%; /* Adjusted width for responsiveness */
      padding: 10px;
      position: absolute;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
      background-color: #191919;
      color: white;
      border-radius: 24px;
      box-shadow: 0px 1px 32px 0px rgba(0, 227, 197, 0.59);
      display: flex;
      flex-direction: column;
      align-items: center;
      text-align: center;
    }

    h1 {
      font-weight: 500;
    }

    .part {
      display: flex;
      align-items: center;
      width: 80%;
      margin-bottom: 10px;
    }

    label {
      width: 30%;
      margin-right: 10px;
      color: #22a6b3;
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    }

    input {
      border: 0;
      display: block;
      background: none;
      text-align: center;
      border: 2px solid #4834d4;
      padding: 14px 10px;
      width: 70%;
      outline: none;
      border-radius: 24px;
      color: white;
      font-size: smaller;
      transition: 0.3s;
    }

    input:focus {
      width: 90%;
      border-color: #22a6b3;
    }

    input[type='submit'] {
      border: 0;
      display: block;
      background: none;
      margin-top: 10px;
      text-align: center;
      border: 2px solid #22a6b3;
      padding: 14px 10px;
      width: 140px;
      outline: none;
      border-radius: 24px;
      color: white;
      transition: 0.3s;
      cursor: pointer;
    }

    input[type='submit']:hover {
      background-color: #22a6b3;
    }

    /* Added responsive styles */
    @media screen and (max-width: 600px) {
      .box {
        width: 90%;
      }

      input {
        width: 90%;
      }

      input[type='submit'] {
        width: 90%;
      }
    }

    /* Styling for the stop button */
    .stopButton {
      border: 0;
      background: none;
      margin-top: 10px;
      padding: 14px 10px;
      width: 140px;
      border-radius: 24px;
      color: white;
      transition: 0.3s;
      cursor: pointer;
      background-color: #e74c3c; /* Red color for stop button */
    }

    .stopButton:hover {
      background-color: #c0392b; /* Darker red color on hover */
    }
  </style>
</head>
<body>
  <form action="/get" class="box" id="my-form">
    <h1>RoboArm Control</h1>

    <div class="part">
      <label for="AngleX">Angle X:</label>
      <input name="AngleX" type="text" placeholder="AngleX">
    </div>

    <div class="part">
      <label for="SpeedX">Speed X:</label>
      <input name="SpeedX" type="text" placeholder="SpeedX">
    </div>

    <div class="part">
      <label for="AngleY">Angle Y:</label>
      <input name="AngleY" type="text" placeholder="AngleY">
    </div>

    <div class="part">
      <label for="SpeedY">Speed Y:</label>
      <input name="SpeedY" type="text" placeholder="SpeedY">
    </div>

    <div class="part">
      <label for="AngleZ">Angle Z:</label>
      <input name="AngleZ" type="text" placeholder="AngleZ">
    </div>

    <div class="part">
      <label for="SpeedZ">Speed Z:</label>
      <input name="SpeedZ" type="text" placeholder="SpeedZ">
    </div>
  
    <div class="part">
      <label for="Commands">Commands:</label>
      <input name="Commands" type="text" placeholder="Commands(LRUDFB)">
    </div>
    <div class="part">
      <label for="Count">Count:</label>
      <input name="Count" type="text" placeholder="Count">
    </div>

    <input type="submit" value="Submit">
    <button type="button" class="stopButton" onclick="stop()">STOP</button>
  </form>
</body>
</html>
)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }

  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if (request->getParam(PARAM_1)->value() != "") {
      inputMessage1 = request->getParam(PARAM_1)->value();
    } else {
      inputMessage1 = "0";
    }

    if (request->getParam(PARAM_2)->value() != "") {
      inputMessage2 = request->getParam(PARAM_2)->value();
    } else {
      inputMessage2 = "0";
    }

    if (request->getParam(PARAM_3)->value() != "") {
      inputMessage3 = request->getParam(PARAM_3)->value();
    } else {
      inputMessage3 = "0";
    }

    if (request->getParam(PARAM_4)->value() != "") {
      inputMessage4 = request->getParam(PARAM_4)->value();
    } else {
      inputMessage4 = "0";
    }

    if (request->getParam(PARAM_5)->value() != "") {
      inputMessage5 = request->getParam(PARAM_5)->value();
    } else {
      inputMessage5 = "0";
    }

    if (request->getParam(PARAM_6)->value() != "") {
      inputMessage6 = request->getParam(PARAM_6)->value();
    } else {
      inputMessage6 = "0";
    }

    if (request->getParam(PARAM_7)->value() != "") {
      inputMessage7 = request->getParam(PARAM_7)->value();
    } else {
      inputMessage7 = "0";
    }
    if (request->getParam(PARAM_8)->value() != "") {
      inputMessage8 = request->getParam(PARAM_8)->value();
    } else {
      inputMessage8 = "0";
    }

    Serial.println(inputParam1 + ": " + inputMessage1);
    Serial.println(inputParam2 + ": " + inputMessage2);
    Serial.println(inputParam3 + ": " + inputMessage3);
    Serial.println(inputParam4 + ": " + inputMessage4);
    Serial.println(inputParam5 + ": " + inputMessage5);
    Serial.println(inputParam6 + ": " + inputMessage6);
    Serial.println(inputParam7 + ": " + inputMessage7);
    Serial.println(inputParam8 + ": " + inputMessage8);

    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" + inputParam1 + ") with value: " + inputMessage1 + ", (" + inputParam2 + ") with value: " + inputMessage2 + ", (" + inputParam3 + ") with value: " + inputMessage3 + ", (" + inputParam4 + ") with value: " + inputMessage4 + ", (" + inputParam5 + ") with value: " + inputMessage5 + ", (" + inputParam6 + ") with value: " + inputMessage6 + ", (" + inputParam7 + ") with value: " + inputMessage7 + ", (" + inputParam8 + ") with value: " + inputMessage8 + "<br><a href=\"/\">Return to Home Page</a>");
  });

  server.onNotFound(notFound);
  server.begin();

  pinMode(stepPinX, OUTPUT);
  pinMode(dirPinX, OUTPUT);
  pinMode(stepPinY, OUTPUT);
  pinMode(dirPinY, OUTPUT);
  pinMode(stepPinZ, OUTPUT);
  pinMode(dirPinZ, OUTPUT);
}

void loop() {
  if(checking == 0 && inputMessage8.toInt() > 0){
    n = inputMessage8.toInt();
    checking = 1;
  }
  else n = 0;
  if(inputMessage8.toInt() == 0) checking = 0;
   while(n > 0){
    AngleX = inputMessage1.toInt();
    xs = inputMessage2.toInt();
    AngleY = inputMessage3.toInt();
    ys = inputMessage4.toInt();
    AngleZ = inputMessage5.toInt();
    zs = inputMessage6.toInt();
    
    String dir = inputMessage7;
    Serial.println(n);
    x = map(AngleX, 0, 360, 0, 1155);
    y = map(AngleY, 0, 360, 0, 1155);
    z = map(AngleZ, 0, 360, 0, 1155);
        for(int i = 0; i < sizeof(dir)/sizeof(dir[0]); i++){
          turn(dir[i]);
        }
     n--;
   }
   return;
}

void CW(int x, int y, int n, int t) {
  digitalWrite(y, HIGH);
  for(int i = 0; i < n; i++) {
      digitalWrite(x, HIGH);
      delayMicroseconds(t);
      digitalWrite(x, LOW);
      delayMicroseconds(t);
  }
}

void CCW(int x, int y, int n, int t) {
  digitalWrite(y, LOW);
  for(int i = 0; i < n; i++) {
      digitalWrite(x, HIGH);
      delayMicroseconds(t);
      digitalWrite(x, LOW);
      delayMicroseconds(t);
  }
}
void turn(char dir){
  switch(dir){
    case 'R':
      CCW(stepPinX, dirPinX, x, xs);
      break;
    case 'L':
      CW(stepPinX, dirPinX, x, xs);
      break;
    case 'U':
     CCW(stepPinY, dirPinY, y, ys);
     break;
    case 'D':
      CW(stepPinY, dirPinY, y, ys);
      break;
    case 'F':
      CCW(stepPinZ, dirPinZ, z, zs);
      break;
    case 'B':
      CW(stepPinZ, dirPinZ, z, zs);
      break;
  }
}
