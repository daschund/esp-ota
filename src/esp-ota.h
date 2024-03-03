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

enum infoID {
  inf_esphead    = 1,
  inf_uptime     = 2,
  inf_chipid     = 3,
  inf_chiprev    = 4,
  inf_idesize    = 5,
  inf_flashsize  = 6,
  inf_cpufreq    = 7,
  inf_freeheap   = 8,
  inf_memsketch  = 9,
  inf_memsmeter  = 10,
  inf_lastreset  = 11,
  inf_temp       = 12,
  inf_hall       = 13,
  inf_wifihead   = 14,
  inf_conx       = 15,
  inf_stassid    = 16,
  inf_staip      = 17,
  inf_stagw      = 18,
  inf_stasub     = 19,
  inf_dnss       = 20,
  inf_host       = 21,
  inf_stamac     = 22,
  inf_apssid     = 23,
  inf_wifiaphead = 24,
  inf_apip       = 25,
  inf_apmac      = 26,
  inf_aphost     = 27,
  inf_apbssid    = 28,

  inf_aboutver        = 29,
  inf_aboutarduinover = 30,
  inf_aboutidfver     = 31,
  inf_aboutsdkver     = 32,
  inf_aboutdate       = 33,
  inf_corever         = 34,

  inf_psram_size      = 35,
  inf_bootver         = 36,
  inf_chip_info       = 37,

  inf_END             = -1,
};

  class esp_ota_t{
    public:
      esp_ota_t();
      void enable(WebServerType *server);
      void onStart(void (*fn)(void)) {
        onStart_cb = fn;
      }
      void onEnd(void (*fn)(void)) {
        onEnd_cb = fn;
      }
      void onError(void (*fn)(int code, const char* msg)) {
        onError_cb = fn;
      }
      void beforeReboot(void (*fn)(void)) {
        beforeReboot_cb = fn;
      }
      void restart();
    private:
      WebServerType *webServer;
      #if defined(ESP8266)
      ESP8266HTTPUpdateServer _httpUpdater;
      #endif
      
      void (*onStart_cb)(void);
      void (*onEnd_cb)(void);
      void (*onError_cb)(int code, const char* msg);
      void (*beforeReboot_cb)(void);
      String getInfo();
      String getHTTPHead(String title);
      String getInfoData(infoID id);
  };
  extern esp_ota_t ESP_OTA;

  #ifndef ESP_OTA_WEB_PATH
  #define ESP_OTA_WEB_PATH "/ota"
  #endif

  #ifndef ESP_OTA_TITLE
  #define ESP_OTA_TITLE "OTA Update"
  #endif
#endif
