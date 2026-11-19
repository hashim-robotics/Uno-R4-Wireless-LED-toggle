#include "WiFiS3.h"

char ssid[] = "UNO_R4_Hotspot";
char pass[] = "12345678";

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  
  // 1. Set the built-in LED pin (Pin 13) as an output
  pinMode(LED_BUILTIN, OUTPUT); // <-- NEW

  while (!Serial) {
    ; // Wait for Serial Monitor to open
  }

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  Serial.print("Creating Access Point named: ");
  Serial.println(ssid);

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
            // Send the HTML web page with clickable control buttons
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            
            client.println("<h1>Arduino UNO R4 Light Control</h1>");
            // 2. Add clickable links that send /H or /L to the Arduino // <-- NEW
            client.println("<p><a href=\"/H\"><button style='font-size:24px; padding:15px;'>Turn Light ON</button></a></p>");
            client.println("<p><a href=\"/L\"><button style='font-size:24px; padding:15px;'>Turn Light OFF</button></a></p>");
            
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        // 3. Check the incoming browser request URL and toggle the LED // <-- NEW
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH); // Turn LED ON
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);  // Turn LED OFF
        }
      }
    }
    client.stop();
  }
}