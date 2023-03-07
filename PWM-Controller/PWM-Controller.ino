/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-web-server-http-authentication/
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#define analogin A0
int val;
#define blue 5
#define red 4
#define green 2

// Replace with your network credentials
const char* ssid = "FRITZ!Box 7430 VA AM";
const char* password = "39996964366763325983";

const char* http_username = "admin";
const char* http_password = "admin";

const char* PARAM_INPUT_1 = "state";
const char* PARAM_INPUT_2 = "red";
const char* PARAM_INPUT_3 = "green";
const char* PARAM_INPUT_4 = "blue";
int redv=0;
int greenv=0;
int bluev=1023;
int output = 1;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.6rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 10px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  <button onclick="logoutButton()">Logout</button>
  <p>Ouput - LED Farbe -  <span id="state">%STATE%</span></p>
  %BUTTONPLACEHOLDER%
   <form action="/get">
    Enter an integer: <input type="text" name="red">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Enter an integer: <input type="text" name="green">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Enter an integer: <input type="text" name="blue">
    <input type="submit" value="Submit">
  </form><br>
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ 
    xhr.open("GET", "/update?state=1", true); 
    document.getElementById("state").innerHTML = "Automatisch";  
  }
  else { 
    xhr.open("GET", "/update?state=0", true); 
    document.getElementById("state").innerHTML = "Handbetrieb";      
  }
  xhr.send();
}
function logoutButton() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/logout", true);
  xhr.send();
  setTimeout(function(){ window.open("/logged-out","_self"); }, 1000);
}
</script>
</body>
</html>
)rawliteral";

const char logout_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <p>Logged out or <a href="/">return to homepage</a>.</p>
  <p><strong>Note:</strong> close all web browser tabs to complete the logout process.</p>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    String outputStateValue = outputState();
    buttons+= "<p><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"output\" " + outputStateValue + "><span class=\"slider\"></span></label></p>";
    return buttons;
  }
  if (var == "STATE"){
    if(output){
      return "Automatisch";
    }
    else {
      return "Handbetrieb";
    }
  }
  return String();
}

String outputState(){
  if(output){
    return "checked";
  }
  else {
    return "";
  }
  return "";
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(9600);

pinMode(analogin,INPUT);
pinMode(red,OUTPUT);
pinMode(green,OUTPUT);
pinMode(blue,OUTPUT);
analogWriteFreq(200);
analogWriteRange(1024);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send_P(200, "text/html", index_html, processor);
  });
    
  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(401);
  });

  server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", logout_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?state=<inputMessage>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      output=inputMessage.toInt();
    }else if(request->hasParam(PARAM_INPUT_2)) {
     inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
      redv=inputParam.toInt();
    }else if(request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
      greenv=inputMessage.toInt();
    }else if(request->hasParam(PARAM_INPUT_4)) {
      inputMessage = request->getParam(PARAM_INPUT_4)->value();
      inputParam = PARAM_INPUT_4;
      bluev=inputParam.toInt();
    }
    
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "HTTP GET request sent to your ESP on input field ("+ inputParam + ") with value: " + inputMessage + "<br><a href=\"/\">Return to Home Page</a>");
    
    //request->send(200, "text/plain", "OK");
  });
  
  // Start server
  server.begin();
}
  
void loop() {
  if(output==1){
  val=analogRead(analogin)-5;
Serial.println(val);
analogWrite(red,map(val,100,1350,0,1024));
analogWrite(green,map(val,100,1024,100,500));
analogWrite(blue,map(val,100,1024,1024,200));
  }else{
    analogWrite(green,greenv);
    analogWrite(red,redv);
    analogWrite(blue,bluev);
  }
}
