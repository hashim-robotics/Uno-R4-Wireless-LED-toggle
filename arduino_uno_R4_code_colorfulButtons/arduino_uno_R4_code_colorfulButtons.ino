#include "WiFiS3.h"

char ssid[] = "UNO_R4_Hotspot";
char pass[] = "12345678";

int status = WL_IDLE_STATUS;
WiFiServer server(80);
bool ledState = false; // <-- NEW: Tracks whether the light is ON or OFF

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  while (!Serial) {
    ; // Wait for Serial Monitor to open
  }

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed!");
    while (true);
  }

  delay(3000);
  server.begin();
  Serial.println("Wi-Fi Broadcast Active!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Send HTTP header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // <-- NEW: Mobile scaling and CSS styling for clean buttons
            client.println("<!DOCTYPE html><html><head>");
            client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
            client.println("<style>");
            client.println("body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f9; margin-top: 40px; }");
            client.println(".btn { display: inline-block; width: 220px; padding: 18px; margin: 10px; font-size: 20px; font-weight: bold; color: white; text-decoration: none; border-radius: 10px; border: none; cursor: pointer; }");
            client.println(".on { background-color: #28a745; }");
            client.println(".off { background-color: #dc3545; }");
            client.println(".toggle { background-color: #007bff; }");
            client.println(".status { font-size: 22px; margin: 20px; font-weight: bold; }");
            client.println("</style></head><body>");

            client.println("<h1>UNO R4 WiFi Control</h1>");

            // <-- NEW: Display current light status on the web page
            client.print("<div class='status'>Light Status: ");
            client.print(ledState ? "<span style='color:#28a745;'>ON</span>" : "<span style='color:#dc3545;'>OFF</span>");
            client.println("</div>");

            // <-- NEW: Three styled action buttons (ON, OFF, and TOGGLE)
            client.println("<div><a href='/H' class='btn on'>TURN ON</a></div>");
            client.println("<div><a href='/L' class='btn off'>TURN OFF</a></div>");
            client.println("<div><a href='/T' class='btn toggle'>TOGGLE LIGHT</a></div>");

            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        // Check incoming URL requests
        if (currentLine.endsWith("GET /H")) {
          ledState = true;
          digitalWrite(LED_BUILTIN, HIGH);
        }
        if (currentLine.endsWith("GET /L")) {
          ledState = false;
          digitalWrite(LED_BUILTIN, LOW);
        }
        // <-- NEW: Handle the single Toggle button
        if (currentLine.endsWith("GET /T")) {
          ledState = !ledState;
          digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
        }
      }
    }
    client.stop();
  }
}
