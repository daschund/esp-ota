#ifndef esp_ota_h
  #define esp_ota_h
  #include "Arduino.h"
  #include "stdlib_noniso.h"
  #if defined(ESP8266)
    #include "ESP8266WebServer.h"
    #include "ESP8266HTTPUpdateServer.h"
    #define  WebServerType ESP8266WebServer
  #elif defined(ESP32)
    #include "WebServer.h"
    #include "Update.h"
    #define  WebServerType WebServer
  #endif

  class esp_ota_t{
    public:
      esp_ota_t();
      void enable(WebServerType *server);
    private:
      WebServerType *webServer;
      #if defined(ESP8266)
        ESP8266HTTPUpdateServer _httpUpdater;
      #endif
  };
  extern esp_ota_t ESP_OTA;

  #ifndef ESP_OTA_WEB_PATH
  #define ESP_OTA_WEB_PATH "/ota"
  #endif

  #ifndef ESP_OTA_TITLE
  #define ESP_OTA_TITLE "OTA Update"
  #endif
#endif
