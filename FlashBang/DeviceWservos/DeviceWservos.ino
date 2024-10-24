/*
 * FlashBang
 *   -An OSC Trigger Device
 * (actually sending UDP messages)
 * --Wych--
 * Trigger connected to GPIO0 (default)
 * Status LED connected to GPIO2 (default)
 */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <Servo.h>  //TEMP SERVO HOOKUP FOR MATILDA 12/2022

#include "index.h" //HTML webpage contents with javascripts

String ssid;
String pass;
String ip;
String localip;

// CHANGE THESE TO DESIRED VALUES
char ap_ssid[] = "DEVICE";          // SSID to host network on if no connection is established
char ap_pass[] = "password";        // Password for network that is hosted if there is no network connection established
int connectionTimeout = 20;         // This number * 500 eqauls the number of ms to wait before assuming no network connection
const long discoverInterval = 5000; // Length in ms between discovery packets
int discoverCount = 20;             // number of times to send a discover packet
int StatusLEDpin = 2;               // GPIO pin for status LED D1
int buttonPin = D1;                  // GPIO pin for trigger D8
int LEDPin = 4;                     // Muzzle Flash D2
unsigned int local_port = 53001;    // UDP Port to listen for UDP Packets on
unsigned int serverPort = 55056;    // port to send discovery packets on
int servoPin1 = D0; // TEMP FOR MATILA board D5
int servoPin2 = D5; // TEMP FOR BEAST board D5
int servoPin3 = D6; // TEMP FOR BEAST board D5
int servoPin4 = D7; // TEMP FOR BEAST board D5
int servoPin5 = D8; // TEMP FOR BEAST board D5
int servoPin6 = D2; // TEMP FOR BEAST board D5

int intcue;                 // converted starting cue string to integer
int buttonPresses = intcue; // how many times the button has been pressed
byte lastPressCount = 0;    // to keep track of last press count
int buttonState;            // current button state
int lastButtonState;        // what state was the button in during the last code cycle
int ledState = -1;          // this variable tracks the state of the LED, negative if off, positive if on
String identifier;
String strAddress;
IPAddress address;
IPAddress serverAddress;
String port;                  // Port that packet is being sent to
String cmd;                   // Combined string
String cmdbegin;              // Start of command
String cmdend;                // End of command
String cue;                   // Starting cue number
String numofcue;              // Total number of cues
String resettime;             // Time before resetting to starting cue number
String debounce;              // Debounce Time in ms between allowed button presses
String flashlength;           // How long the LED is on
String rapid;                 // Toggle for allowing push and hold to advance cues. 0=No 1=Yes
String fadetime;              // LED fade out time
unsigned long intflashlength; // converted flashlength string to integer
unsigned long intfadetime;    // converted fadetime string to integer
int intPort;                  // converted Port string to integer
unsigned long intdebounce;    // converted debounce string to integer
unsigned long intresettime;   // converted resettime string to integer
int intnumofcue;              // converted number of cues string to integer

char packetBuffer[255];
unsigned long currentTime; // Current Time
unsigned long previousTime;
unsigned long previousTimeButton;
unsigned long previousTimeButton2;
unsigned long previousTimeFlash;
unsigned long fadeStartTime;
int mode; // connection state
bool discovered = false;
long brightness;
long brightness2;

void updateConfig();
bool tryConnection();
void setMode();
void apUpdate();
void handleRoot();
void giveInfo();
void sendUDP(IPAddress a, int p, String s);
void fileUpdate(String param, String newValue);
bool checkStatus();
void rcvUDP();
void getBroadcastAddress();

WiFiUDP UDP;
ESP8266WebServer server(80);
WiFiClient conn;
Servo Servo1;  //TEMP FOR MATILDA
Servo Servo2;  //TEMP FOR BEAST
Servo Servo3;  //TEMP FOR BEAST
Servo Servo4;  //TEMP FOR BEAST
Servo Servo5;  //TEMP FOR BEAST
Servo Servo6;  //TEMP FOR BEAST

void setup()
{
  pinMode(buttonPin, INPUT);
  pinMode(StatusLEDpin, OUTPUT);
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, LOW);
  digitalWrite(StatusLEDpin, LOW);
  Serial.begin(115200);         // Start Serial
  SPIFFS.begin();               // Start File System
  updateConfig();               // Read current configuration from memory
  setMode();                    // Try to connect to ssid in memory, if unsucceful start AP
  UDP.begin(local_port);        // start UDP Server
  server.on("/info", giveInfo); // navigating to /info will call giveInfo()
  server.on("/", giveInfo);
  server.begin(); // begin http server
  Servo1.attach(servoPin1,544,2200); //TEMP FOR MATILDA
  Servo1.write(0);  //TEMP FOR MATILDA
  Servo2.attach(servoPin2,544,2200); //TEMP FOR BEAST
  Servo2.write(0);  //TEMP FOR BEAST
  Servo3.attach(servoPin3,544,2200); //TEMP FOR BEAST
  Servo3.write(0);  //TEMP FOR BEAST
  Servo4.attach(servoPin4,544,2200); //TEMP FOR BEAST
  Servo4.write(0);  //TEMP FOR BEAST
  Servo5.attach(servoPin5,544,2200); //TEMP FOR BEAST
  Servo5.write(0);  //TEMP FOR BEAST
  Servo6.attach(servoPin6,544,2200); //TEMP FOR BEAST
  Servo6.write(0);  //TEMP FOR BEAST
}

void loop()
{
  currentTime = millis();
  buttonState = digitalRead(buttonPin);
  // filter out any noise by setting a time buffer
  if (((currentTime - previousTimeButton) > intdebounce) && (buttonState == LOW) && ((buttonState != lastButtonState) || (rapid != "No")))
  {
    previousTimeButton = currentTime; // set the current time
    buttonPresses++;                  // increment buttonPresses count
    cmd = cmdbegin + buttonPresses + cmdend;
    sendUDP(address, intPort, cmd); // send UDP message with content from memory
    if (intflashlength > 0)
    {
      digitalWrite(LEDPin, HIGH); // turn LED on
      Servo1.write(180);  //TEMP FOR MATILDA
      ledState = 1;
      previousTimeFlash = currentTime;
      fadeStartTime = currentTime;
      Serial.println("LED on");
    }
  }
  if (((currentTime - previousTimeButton) > intresettime) || (buttonPresses < intcue) || (buttonPresses == (intcue + intnumofcue - 1)))
  {
    buttonPresses = intcue - 1; // resets cut number once the max number of cues or reset time is reached
  }
  if ((ledState == 1) && ((currentTime - previousTimeFlash) > intflashlength))
  {
    if (intfadetime == 0)
    {
      brightness == 0;
    }
    else
    {
      brightness = map(currentTime - fadeStartTime - intflashlength, 0, intfadetime, 255, 0);
      analogWrite(LEDPin, brightness);
    }
    if (brightness <= 0)
    {
      ledState = -1;
      digitalWrite(LEDPin, LOW);
      Servo1.write(0);  //TEMP FOR MATILDA
      Serial.println("LED off");
    }
  }

  if ((currentTime - previousTimeButton2) > 20) // debounce for when the button is released
  {
    lastButtonState = buttonState;
    previousTimeButton2 = currentTime;
  }

  if (!discovered && discoverCount > 0)
  { // check if device has been discovered or if the number of discover messages have been reached
    if ((currentTime - previousTime) >= discoverInterval)
    { // check if there has been enough time since the last discover packet
      previousTime = currentTime;
      String s = identifier + ","; // construct the discover packet in the format the server will be looking for it in
      s += "from:" + localip + ",";
      s += "to:" + strAddress + ",";
      s += port + ",";
      s += cmd;
      Serial.println("Discover Message: " + s);
      sendUDP(serverAddress, serverPort, s); // send the message
      discoverCount--;                       // decrease the discover count to keep track of the number of discover packets sent
    }
  }
  rcvUDP();              // handle incoming UDP packets
  server.handleClient(); // hand http connections
}

void updateConfig()
{
  File f = SPIFFS.open("/ssid.txt", "r"); // read each file from memory, one file = one variable
  ssid = f.readStringUntil('\n');
  ssid.trim(); // remove return characters and new lines

  f = SPIFFS.open("/pass.txt", "r");
  pass = f.readStringUntil('\n');
  pass.trim();

  f = SPIFFS.open("/ip.txt", "r");
  ip = f.readStringUntil('\n');
  ip.trim();
  strAddress = ip;
  char ipArray[ip.length() + 1];
  ip.toCharArray(ipArray, sizeof(ipArray)); // convert ip string into char array
  WiFi.hostByName(ipArray, address);        // convert ipArray to IPAddress

  f = SPIFFS.open("/port.txt", "r");
  port = f.readStringUntil('\n');
  port.trim();
  intPort = port.toInt(); // convert string port into an integer

  f = SPIFFS.open("/cmdbegin.txt", "r");
  cmdbegin = f.readStringUntil('\n');
  cmdbegin.trim();
  cmdbegin.replace(" ", "");

  f = SPIFFS.open("/cue.txt", "r");
  cue = f.readStringUntil('\n');
  cue.trim();
  intcue = cue.toInt(); // convert string port into an integer

  f = SPIFFS.open("/cmdend.txt", "r");
  cmdend = f.readStringUntil('\n');
  cmdend.trim();
  cmdend.replace(" ", "");

  f = SPIFFS.open("/numofcue.txt", "r");
  numofcue = f.readStringUntil('\n');
  numofcue.trim();
  intnumofcue = numofcue.toInt(); // convert string into an integer

  f = SPIFFS.open("/id.txt", "r");
  identifier = f.readStringUntil('\n');
  identifier.trim();
  identifier.replace(" ", "_");

  f = SPIFFS.open("/debounce.txt", "r");
  debounce = f.readStringUntil('\n');
  debounce.trim();
  intdebounce = debounce.toInt(); // convert string into an integer

  f = SPIFFS.open("/resettime.txt", "r");
  resettime = f.readStringUntil('\n');
  resettime.trim();
  intresettime = resettime.toInt(); // convert string into an integer

  f = SPIFFS.open("/rapid.txt", "r");
  rapid = f.readStringUntil('\n');
  rapid.trim();

  f = SPIFFS.open("/flashlength.txt", "r");
  flashlength = f.readStringUntil('\n');
  flashlength.trim();
  intflashlength = flashlength.toInt(); // convert string into an integer

  f = SPIFFS.open("/fadetime.txt", "r");
  fadetime = f.readStringUntil('\n');
  fadetime.trim();
  intfadetime = fadetime.toInt(); // convert string into an integer

  f.close();
}

bool tryConnection()
{
  if (pass == "")
  { // if no password in memory try connecting without using a password
    char ssidArray[ssid.length() + 1];
    ssid.toCharArray(ssidArray, sizeof(ssidArray)); // convert ssid to charArray
    WiFi.begin(ssidArray);                          // start trying connection
    Serial.print("Connecting to ");
    Serial.print(ssidArray);
  }
  else
  { // if there is a password try connecting with it
    char ssidArray[ssid.length() + 1];
    char passArray[pass.length() + 1];
    ssid.toCharArray(ssidArray, sizeof(ssidArray)); // convert ssid nd pass to charArray
    pass.toCharArray(passArray, sizeof(passArray));
    WiFi.begin(ssidArray, passArray); // start trying connection using ssid and pass
    Serial.print("Connecting to ");
    Serial.print(ssidArray);
  }

  int timeout = 0;
  while (timeout < connectionTimeout)
  { // start of loop that continually checks wifi status up to a user-defined amount
    if (WiFi.status() == WL_CONNECTED)
    {
      getBroadcastAddress();
      return true; // if wireless is connceted return true
    }
    digitalWrite(StatusLEDpin, LOW);
    Serial.print(".");
    delay(250);
    digitalWrite(StatusLEDpin, HIGH);
    delay(250);
    timeout++;
  }
  Serial.println("");
  WiFi.disconnect(); // if connection attempts reaches connection_timeout assume no connection was made; disconnect and return false
  return false;
}
void setMode()
{
  bool success = tryConnection(); // determine connection state
  if (!success)
  { // if unable to connect start an access point with user-defined ssid and pass and set root webpage to ap configuration
    mode = 0;
    WiFi.softAP(ap_ssid, ap_pass);
    Serial.println("Connection failed starting access point");
    Serial.println(WiFi.softAPIP());
    server.on("/", apUpdate);
    digitalWrite(StatusLEDpin, HIGH);
  }
  else
  { // if connection is succesful set appropriate page forwards
    mode = 1;
    server.on("/", handleRoot);
    server.on("/ap", apUpdate);
    Serial.println();
    Serial.println("Successful");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(StatusLEDpin, LOW);
  }
}

void apUpdate()
{ // serve up the content of ap.htm and check to see if user has updated & submitted the configuration fields
  File ap = SPIFFS.open("/ap.htm", "r");
  size_t sent = server.streamFile(ap, "text/html");
  ap.close();
  if (server.args() > 0)
  {
    for (uint8_t i = 0; i < server.args(); i++)
    {
      if (server.argName(i) == "SSID")
      {
        fileUpdate("ssid", server.arg("SSID"));
      }
      else if (server.argName(i) == "PASS")
      {
        fileUpdate("pass", server.arg("PASS"));
      }
    }
    updateConfig(); // update configuration
    ESP.restart();  // restart device to attempt to connect with new configuration
  }
}

void sendUDP(IPAddress a, int p, String s)
{                        // send UDP message containing string passed in
  UDP.beginPacket(a, p); // start packet
  UDP.print(s);          // send command
  UDP.endPacket();       // close packet
  Serial.println("UDP PACKET SENT");
  Serial.println(a);
  Serial.println(s);
  Serial.println(p);
}

void rcvUDP()
{
  int packetSize = UDP.parsePacket();
  if (packetSize)
  {
    UDP.read(packetBuffer, 255);
    Serial.print("UDP message received: ");
    Serial.println(packetBuffer);
    String message(packetBuffer);
    for (int i = 0; i < 255; i++)
      packetBuffer[i] = 0;
    if (message == "/" + identifier + "/LED/on")
    {
      digitalWrite(LEDPin, HIGH);
    }
    if (message == "/" + identifier + "/LED/off")
    {
      digitalWrite(LEDPin, LOW);
    }
    if (message == "/" + identifier + "/servos/down")
    {
      digitalWrite(LEDPin, HIGH);
      Servo1.write(100);  //TEMP FOR BEAST
      Servo2.write(100);  //TEMP FOR BEAST
      Servo3.write(100);  //TEMP FOR BEAST
      Servo4.write(100);  //TEMP FOR BEAST
      Servo5.write(100);  //TEMP FOR BEAST
      Servo6.write(100);  //TEMP FOR BEAST
    }
    if (message == "/" + identifier + "/servos/up")
    {
      digitalWrite(LEDPin, LOW);
      Servo1.write(0);  //TEMP FOR BEAST
      Servo2.write(0);  //TEMP FOR BEAST
      Servo3.write(0);  //TEMP FOR BEAST
      Servo4.write(0);  //TEMP FOR BEAST
      Servo5.write(0);  //TEMP FOR BEAST
      Servo6.write(0);  //TEMP FOR BEAST
    }    
    if (message.startsWith("/" + identifier + "/LED/brightness/"))
    {
      message.remove(0, message.lastIndexOf("/LED/brightness/") + 16);
      message.remove(message.lastIndexOf('/'), 255);
      int LEDbrightness = message.toInt();
      Serial.println(message);
      analogWrite(LEDPin, LEDbrightness);
    }
    if (message.startsWith("/" + identifier + "/servo1/"))  //TEMP FOR MATILDA
    {  //TEMP FOR MATILDA  //TEMP FOR MATILDA
      message.remove(0, message.lastIndexOf("/servo1/") + 8);  //TEMP FOR MATILDA
      message.remove(message.lastIndexOf('/'), 255);  //TEMP FOR MATILDA
      int servoPos1 = message.toInt();  //TEMP FOR MATILDA
      Serial.println(message);  //TEMP FOR MATILDA
      Servo1.write(servoPos1);  //TEMP FOR MATILDA
    }  //TEMP FOR MATILDA
   if (message.startsWith("/" + identifier + "/servo2/"))  //TEMP FOR BEAST
    {  //TEMP FOR BEASTILDA
      message.remove(0, message.lastIndexOf("/servo2/") + 8);  //TEMP FOR BEAST
      message.remove(message.lastIndexOf('/'), 255);  //TEMP FOR BEAST
      int servoPos2 = message.toInt();  //TEMP FOR BEAST
      Serial.println(message);  //TEMP FOR BEAST
      Servo2.write(servoPos2);  //TEMP FOR BEAST
    }  //TEMP FOR BEAST
    if (message.startsWith("/" + identifier + "/servo3/"))  //TEMP FOR BEAST
    {  //TEMP FOR BEASTILDA
      message.remove(0, message.lastIndexOf("/servo3/") + 8);  //TEMP FOR BEAST
      message.remove(message.lastIndexOf('/'), 255);  //TEMP FOR BEAST
      int servoPos3 = message.toInt();  //TEMP FOR BEAST
      Serial.println(message);  //TEMP FOR BEAST
      Servo3.write(servoPos3);  //TEMP FOR BEAST
    }  //TEMP FOR BEAST
    if (message.startsWith("/" + identifier + "/servo4/"))  //TEMP FOR BEAST
    {  //TEMP FOR BEASTILDA
      message.remove(0, message.lastIndexOf("/servo4/") + 8);  //TEMP FOR BEAST
      message.remove(message.lastIndexOf('/'), 255);  //TEMP FOR BEAST
      int servoPos4 = message.toInt();  //TEMP FOR BEAST
      Serial.println(message);  //TEMP FOR BEAST
      Servo4.write(servoPos4);  //TEMP FOR BEAST
    }  //TEMP FOR BEAST
    if (message.startsWith("/" + identifier + "/servo5/"))  //TEMP FOR BEAST
    {  //TEMP FOR BEASTILDA
      message.remove(0, message.lastIndexOf("/servo5/") + 8);  //TEMP FOR BEAST
      message.remove(message.lastIndexOf('/'), 255);  //TEMP FOR BEAST
      int servoPos5 = message.toInt();  //TEMP FOR BEAST
      Serial.println(message);  //TEMP FOR BEAST
      Servo5.write(servoPos5);  //TEMP FOR BEAST
    }  //TEMP FOR BEAST
    if (message.startsWith("/" + identifier + "/servo6/"))  //TEMP FOR BEAST
    {  //TEMP FOR BEASTILDA
      message.remove(0, message.lastIndexOf("/servo6/") + 8);  //TEMP FOR BEAST
      message.remove(message.lastIndexOf('/'), 255);  //TEMP FOR BEAST
      int servoPos6 = message.toInt();  //TEMP FOR BEAST
      Serial.println(message);  //TEMP FOR BEAST
      Servo6.write(servoPos6);  //TEMP FOR BEAST
    }  //TEMP FOR BEAST
    if (message == "/" + identifier + "/Cue/reset")
    {
     buttonPresses = intcue - 1;
    }
    if (message.startsWith("/" + identifier + "/Cue/"))
    {
      message.remove(0, message.lastIndexOf("/Cue/") + 16);
      message.remove(message.lastIndexOf('/'), 255);
     buttonPresses = message.toInt();
    }

    if (message == "acknowledged")
    {
      discovered = !discovered;
      Serial.println("Discover Toggled");
    }
  }
}

void handleRoot()
{ // serve up contents of index.htm and check for user submit
  String s = MAIN_page;
  s.replace("@@ssid@@", ssid);
  s.replace("@@pass@@", pass);
  s.replace("@@mac@@", WiFi.macAddress());
  s.replace("@@id@@", identifier);
  s.replace("@@ip@@", ip);
  s.replace("@@port@@", port);
  s.replace("@@debounce@@", debounce);
  s.replace("@@rapid@@", rapid);
  s.replace("@@resettime@@", resettime);
  s.replace("@@cmdbegin@@", cmdbegin);
  s.replace("@@fullcmd@@", (cmdbegin + intcue + cmdend));
  s.replace("@@cue@@", cue);
  s.replace("@@cmdend@@", cmdend);
  s.replace("@@numofcue@@", numofcue);
  s.replace("@@flashlength@@", flashlength);
  s.replace("@@fadetime@@", fadetime);
  server.send(200, "text/html", s);
  //  File index = SPIFFS.open("/index.htm", "r");
  //  size_t sent = server.streamFile(index, "text/html");
  //  index.close();
  bool reboot = false;
  if (server.args() > 0)
  {
    for (uint8_t i = 0; i < server.args(); i++)
    {
      if (server.hasArg("SENDUDP"))
      { // check for send test button click
        sendUDP(address, intPort, (cmdbegin + cue + cmdend));
        previousTimeButton = currentTime;
        return;
      }
      if ((server.argName(i) == "IP") && (server.arg(i) != ip))
      { // check for submit and if the value is not blank update the variable on the file system
        fileUpdate("ip", server.arg(i));
      }
      else if ((server.argName(i) == "PORT") && (server.arg(i) != port))
      {
        fileUpdate("port", server.arg(i));
      }
      else if (server.argName(i) == "ID" && (server.arg(i) != identifier))
      {
        fileUpdate("id", server.arg(i));
      }
      else if ((server.argName(i) == "CMDBEGIN") && (server.arg(i) != cmdbegin))
      {
        fileUpdate("cmdbegin", server.arg(i));
      }
      else if (server.argName(i) == "CUE" && (server.arg(i) != cue))
      {
        fileUpdate("cue", server.arg(i));
      }
      else if (server.argName(i) == "CMDEND" && (server.arg(i) != cmdend))
      {
        fileUpdate("cmdend", server.arg(i));
      }
      else if (server.argName(i) == "NUMOFCUE" && (server.arg(i) != numofcue))
      {
        fileUpdate("numofcue", server.arg(i));
      }
      else if (server.argName(i) == "RESETTIME" && (server.arg(i) != resettime))
      {
        fileUpdate("resettime", server.arg(i));
      }
      else if (server.argName(i) == "DEBOUNCE" && (server.arg(i) != debounce))
      {
        fileUpdate("debounce", server.arg(i));
      }
      else if (server.argName(i) == "RAPID" && (server.arg(i) != rapid))
      {
        fileUpdate("rapid", server.arg(i));
      }
      else if (server.argName(i) == "FLASHLENGTH" && (server.arg(i) != flashlength))
      {
        fileUpdate("flashlength", server.arg(i));
      }
      else if (server.argName(i) == "FADETIME" && (server.arg(i) != fadetime))
      {
        fileUpdate("fadetime", server.arg(i));
      }
      else if (server.argName(i) == "SSID")
      { // if the SSID and PASS have been updated set reboot flag to true so device will restart
        fileUpdate("ssid", server.arg("SSID"));
        reboot = true;
      }
      else if (server.argName(i) == "PASS")
      {
        fileUpdate("pass", server.arg("PASS"));
        reboot = true;
      }
    }
    updateConfig(); // update the variable locally
    if (reboot)
    {
      ESP.restart();
    }
  }
}
void fileUpdate(String param, String newValue)
{ // removes old version of the file and replaces it with an updated version
  String path = "/" + param + ".txt";
  SPIFFS.remove(path);
  Serial.println(param + " FILE REMOVED");
  File f = SPIFFS.open(path, "w");
  Serial.println("NEW " + param + " FILE CREATED");
  Serial.println(newValue);
  f.println(newValue);
  Serial.println("NEW " + param + " FILE WRITTEN");
  f.close();
}

void giveInfo()
{ // serve up general information about the device's current settings
  String info = cmd;
  server.send(200, "text/html", info);
}

void getBroadcastAddress()
{                                        // function will drop the last octet of the IP Address and fill with 255
  String ip = WiFi.localIP().toString(); // get current IP Address
  localip = WiFi.localIP().toString();
  int index = ip.lastIndexOf('.') + 1; // get index right after the last '.'
  ip.remove(index);                    // remove everything after this dot, essentially the last octet
  ip = ip + "255";                     // append 255 octet for broadcast address
  char ipArray[ip.length() + 1];
  ip.toCharArray(ipArray, sizeof(ipArray)); // convert ip string into char array
  WiFi.hostByName(ipArray, serverAddress);  // set serverAddress to our newly created ip
  Serial.print("Broadcast Address: ");
  Serial.println(serverAddress);
}
