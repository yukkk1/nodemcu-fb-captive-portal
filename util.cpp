#include <Arduino.h>
#include <WString.h>
#include <FS.h>

/*
 * Display the error on the serial monitor and return a pattern to be handled by other functions.
 * e.g. ERROR:error message
 */
String err(String message) {
  Serial.println("\nERROR:" + message);
  return "ERROR:" + message;
}


/*
 * Read file content
 */
String readFile(String path) {
      File file = SPIFFS.open(path, "r");
      
      // Validate file read access
      if (!file) {
        return err("Failed to open file for reading");
      }

      // Read file bytes, convert to `char` and append to `result` string.
      String fileContent = "";
      while (file.available())
      {
        uint8_t b = file.read();
        
        char str = b;
        fileContent += str;
      }
      file.close();
      
      return fileContent;
}


/*
 * Write to file and return `true` if successful 
 */
bool writeFile(String path, String content, bool append) {

  // Stores the data to be written to the file 
  String fileContent = "";

  // Read current file content before write
  if(append) {
    fileContent = readFile(path);
    fileContent += content;
  } else {
    fileContent = content;
  }

  File file = SPIFFS.open(path, "w");

  // Validate file write access
  if (!file) {
    err("Failed to open file: "+ path +" for writing");
    return false;
  }

  // Write data
  int bytesWritten = file.print(fileContent);
  if (bytesWritten == 0) {
    err("File write failed");
    return false;
  }

  file.close();
  return true;
}


/*
 * Redirect to some page using just HTML
 */
String redirectHTML(String location) {
  return "<html><head><meta http-equiv=\"refresh\" content=\"0; url="+location+"\" /></head></html>"; 
}

/*
 * Inject c++ variable on page via Javascript element
 */
String injectJS(String data) {
  return "\n<script>var data = `"+ data +"`;</script>\n";
}
