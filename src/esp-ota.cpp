#include "esp-ota.h"
#include "html.h"

esp_ota_t ESP_OTA;

esp_ota_t::esp_ota_t() {}

void esp_ota_t::enable(WebServerType *server) {
  webServer = server;

  webServer->on(F(ESP_OTA_WEB_PATH), HTTP_GET,
    [&]() {
      webServer->sendHeader(F("Connection"), F("close"));
      webServer->send(200, F("text/html"), indexHTML);
    }
  );

  webServer->on(F(ESP_OTA_WEB_PATH), HTTP_POST,
    [&]() {
      webServer->sendHeader(F("Connection"), F("close"));
      webServer->send(200, F("text/plain"), (Update.hasError()) ? F("FAIL\n") : F("OK\n"));
      delay(1000);
      ESP.restart();
    },
    [&]() {
      HTTPUpload& upload = webServer->upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
          Serial.printf("Updated: %u\nRebooting.\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
      }
    }
  );
}
