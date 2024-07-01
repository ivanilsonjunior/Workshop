#include <WiFi.h>
#include <HTTPClient.h>

#define SERVER_IP "laica.ifrn.edu.br"

#ifndef STASSID
#define STASSID "wIFRN-IoT"
#define STAPSK "deviceiotifrn"
#endif





void setup() {

  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  coldStart();
}

void loop() {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;
    float result = temperatureRead();
    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP "/access-ng/log/");  // HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] POST...\n");
    String body = "{\"deviceMac\": \"";
    body += WiFi.macAddress();
    body += "\",\"topic\": \"AmbienteESP\", \"type\": \"INFO\",\"message\": \"Temperatura = ";
    body += String(result,2);
    body += F("\"}");
    Serial.println(body);
    int httpCode = http.POST(body);
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK ||  httpCode == HTTP_CODE_CREATED) {
          const String& payload = http.getString();
          Serial.println("received payload:\n<<");
          Serial.println(payload);
          Serial.println(">>");
        }
      
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    }

    delay (60000);
}


void coldStart() {
  if ((WiFi.status() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, "http://" SERVER_IP "/access-ng/log/");  // HTTP
    http.addHeader("Content-Type", "application/json");
    String body = "{\"deviceMac\": \"";
    body += WiFi.macAddress();
    body += "\",\"topic\": \"ANGeLUS\", \"type\": \"INFO\",\"message\": \"Cold Start\"}";
    int httpCode = http.POST(body);
    if (httpCode > 0) {
      digitalWrite(2, HIGH);
      delay(200);
      digitalWrite(2, LOW);
      } else {
      digitalWrite(2, HIGH);
      delay(1000);
      }
  }
}
