#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#include <IthoSender.h>

ESP8266WebServer server(80);


void returnOK();

void handleRoot()
{
    server.send(200, "text/plain", "Hello world!"); // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleNotFound()
{
    server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}


void handleSend()
{
    printf("handleSend: \n");
    if (!server.hasArg("id") || !server.hasArg("cmd"))
    {
        server.send(400, "text/plain", "400: Invalid Request"); // The request is invalid, so send HTTP status 400
        return;
    }
    String id = server.arg("id");
    String command = server.arg("cmd");

    if (id == "k")
    {
        IthoSender.sendCommandRoom("auto1");
        IthoSender.sendCommand(command);
    }
    else
    {
        //IthoSender.sendCommand("eco");
        IthoSender.sendCommandRoom(command);
    }
    returnOK();
}

void returnOK()
{
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "OK\r\n");
}

String getContentType(String filename)
{ // convert the file extension to the MIME type
    if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    return "text/plain";
}

bool handleFileRead(String path)
{ // send the right file to the client (if it exists)
    Serial.println("handleFileRead: " + path);
    if (path.endsWith("/"))
        path += "index.html";                  // If a folder is requested, send the index file
    String contentType = getContentType(path); // Get the MIME type
    if (SPIFFS.exists(path))
    {                                                       // If the file exists
        File file = SPIFFS.open(path, "r");                 // Open it
        server.streamFile(file, contentType); // And send it to the client
        file.close();                                       // Then close the file again
        return true;
    }
    Serial.println("\tFile Not Found");
    return false; // If the file doesn't exist, return false
}

void setupWeb()
{

    SPIFFS.begin(); // Start the SPI Flash Files System

    //server.on("/", handleRoot);
    server.on("/send", HTTP_PUT, handleSend);
    server.onNotFound([]() {
        if (!handleFileRead(server.uri()))
            server.send(404, "text/plain", "404: Not Found");
    });
    server.begin(); // Actually start the server
    Serial.println("HTTP server started");

   // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);

}

void loopWeb()
{
  server.handleClient(); // Listen for HTTP requests from clients
}
