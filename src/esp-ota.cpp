#include "esp-ota.h"
#include "html.h"

esp_ota_t ESP_OTA;

esp_ota_t::esp_ota_t() :
  onStart_cb(nullptr),
  onEnd_cb(nullptr),
  onError_cb(nullptr),
  beforeReboot_cb(nullptr)
{}

void esp_ota_t::enable(WebServerType *server) {
  webServer = server;

  webServer->on(F(ESP_OTA_WEB_PATH), HTTP_GET,
    [&]() {
      webServer->sendHeader(F("Connection"), F("close"));
      webServer->send(200, F("text/html"), indexHTML);
    }
  );

  webServer->on(F("/info"), HTTP_GET,
    [&]() {
      webServer->sendHeader(F("Connection"), F("close"));
      webServer->send(200, F("text/html"), getInfo());
    }
  );

  webServer->on(F("/reboot"), HTTP_GET,
    [&]() {
      webServer->sendHeader(F("Connection"), F("close"));
      webServer->send(200, F("text/html"), "");
      restart();
    }
  );

  webServer->on(F(ESP_OTA_WEB_PATH), HTTP_POST,
    [&]() {
      webServer->sendHeader(F("Connection"), F("close"));
      webServer->send(200, F("text/plain"), (Update.hasError()) ? F("FAIL\n") : F("OK\n"));
      delay(1000);
      if (beforeReboot_cb) { beforeReboot_cb(); }
      ESP.restart();
    },
    [&]() {
      HTTPUpload& upload = webServer->upload();
      if (upload.status == UPLOAD_FILE_START) {
        if (onStart_cb) { onStart_cb(); }
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
	  if (onError_cb) {
	    onError_cb(-1, NULL);
	  }
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
          if (onEnd_cb) { onEnd_cb(); }
          Serial.printf("Updated: %u\nRebooting.\n", upload.totalSize);
        } else {
	  if (onError_cb) {
	    onError_cb(-1, NULL);
	  }
          Update.printError(Serial);
        }
      }
    }
  );
}

void esp_ota_t::restart() {
  delay(500);
  yield();
  delay(500);
  yield();
  delay(100);
  ESP.restart();
}
