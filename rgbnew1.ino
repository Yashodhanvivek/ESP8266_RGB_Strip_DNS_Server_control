#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#define DEBUG

#ifdef DEBUG
#define DP(...)   Serial.print(__VA_ARGS__)
#define DPL(...)  Serial.println(__VA_ARGS__)
#define DPF(...)  Serial.print(F(__VA_ARGS__))
#define DPLF(...) Serial.println(F(__VA_ARGS__))

#else
#define DP(...)     //blank line
#define DPL(...)   //blank line
#define DPF(...)    //blank line
#define DPLF(...)  //blank line

#endif

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 7, 1);
IPAddress netMsk(255, 255, 255, 0);

DNSServer dnsServer;


/* Set these to your desired credentials. */
const char *ssid = "RGB";
const char *password = "thereisnospoon";

unsigned int rColor = 0;
unsigned int gColor = 0;
unsigned int bColor = 0;

String cols;

ESP8266WebServer server(80);


void sendPage() {
String s = "<!DOCTYPE HTML>";
s += "<html>";
s += "<head> <meta charset='UTF-8'><meta name='viewport' content='width=100'/>";
s += "<title>RGB ColorControl</title>";
s += "<script>";
s += "function changeBackColor () {";
s += "    var hex = document.getElementById('myColor').value;";
s += "    document.body.style.backgroundColor = hex;";
s += "    var red = parseInt(hex.substring(1,3), 16);";
s += "    var green = parseInt(hex.substring(3,5), 16);";
s += "    var blue = parseInt(hex.substring(5,7), 16);";
s += "    document.getElementById('demo').innerHTML = hex + 'rgb(' + red + ',' + green + ',' + blue + ')';";
s += "    console.log('rgb(' + red + ',' + green + ',' + blue + ')');";
s += "}";
s += "</script>";
s += "</head>";
s += "<body onload='javascript:changeBackColor()'>";
s += "<form action='rgb' method='get'>";
s += "<center>";
s += "  <p id='demo'>Hello</p>";
s += "  <br>";
s += "  <input type='color' id='myColor' name='col' value='";
s += cols;
s += "' oninput='javascript:changeBackColor()' onchange='javascript:changeBackColor()'/>";
s += "  <br><br><br><button type='submit'>Update</button>";
s += "</center>";
s += "</form>";
s += "</body>";
s += "</html>";


  //server.send(200, "text/html", "<h1>You are connected</h1>");
  server.send(200, "text/html", s);
}

void handleNotFound() {
  sendPage();
}

void handleRGB() {
  
  String r = "";
  String g = "";
  String b = "";
  char req[4];

  cols = String(server.arg(0));
  
  DPL(server.arg(0));
  req[0] = server.arg(0)[1];
  req[1] = server.arg(0)[2];
  req[2] = '\n';
  int rColor = (int) strtol(req, NULL, 16);

  req[0] = server.arg(0)[3];
  req[1] = server.arg(0)[4];
  req[2] = '\n';
  int gColor = (int) strtol(req, NULL, 16);

  req[0] = server.arg(0)[5];
  req[1] = server.arg(0)[6];
  req[2] = '\n';
  int bColor = (int) strtol(req, NULL, 16);
  
  String crgb = "rgb(";
  crgb += rColor;
  crgb += ",";
  crgb += gColor;
  crgb += ",";
  crgb += bColor;
  crgb += ")";
  DP(crgb);


  analogWrite(12, 0);
  analogWrite(13, 0);
  analogWrite(14, 0);

  // Set LED Color
  analogWrite(12, rColor);
  analogWrite(13, gColor);
  analogWrite(14, bColor);

  sendPage();

}

void setup() {
  delay(1000);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);

  pinMode(15, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(ssid);
  delay(500); // Without delay I've seen the IP address blank
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/rgb", handleRGB);
  // replay to all requests with same HTML
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
