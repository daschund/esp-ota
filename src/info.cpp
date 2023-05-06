

#include <WiFi.h>
#include <esp-ota.h>
#include "info.h"

const char* const WIFI_MODES[] PROGMEM = { "NULL", "STA", "AP", "STA+AP" };

String esp_ota_t::getHTTPHead(String title) {
  String page;
  page += FPSTR(HTTP_HEAD_START);
  page.replace(FPSTR(T_v), title);
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;

  if (_bodyClass != "") {
    String p = FPSTR(HTTP_HEAD_END);
    p.replace(FPSTR(T_c), _bodyClass); // add class str
    page += p;
  }
  else {
    page += FPSTR(HTTP_HEAD_END);
  }

  return page;
}

String esp_ota_t::getInfo() {

  #ifdef ESP8266
  #elif defined(ESP32)
  infoID infoids[] = {
    inf_esphead,
    inf_uptime,
    //inf_chipid,
    //inf_chiprev,
    inf_chip_info,
    inf_idesize,
    inf_flashsize,
    inf_cpufreq,
    inf_freeheap,
    inf_memsketch,
    inf_memsmeter,
    inf_lastreset,
    inf_temp,
    // inf_hall,
    inf_wifihead,
    inf_conx,
    inf_stassid,
    inf_staip,
    inf_stagw,
    inf_stasub,
    inf_dnss,
    inf_host,
    inf_stamac,
    inf_apssid,
    inf_wifiaphead,
    //inf_apip,
    //inf_apmac,
    //inf_aphost,
    inf_apbssid,
    inf_END,
  };
  #endif

  String page = getHTTPHead(FPSTR("info"));

  for (size_t i=0; infoids[i] != inf_END; i++) {
    page += getInfoData(infoids[i]);
  }
  page += F("</dl>");
  page += F("<h3>About</h3><hr><dl>");
  page += getInfoData(inf_aboutver);
  page += getInfoData(inf_aboutarduinover);
  page += getInfoData(inf_aboutidfver);
  page += getInfoData(inf_aboutdate);
  page += F("</dl>");
  page += FPSTR(HTTP_HELP);
  page += FPSTR(HTTP_END);

  return page;
};


String esp_ota_t::getInfoData(infoID id) {

  String p;
  if (id==inf_esphead){
    p = FPSTR(HTTP_INFO_esphead);
    #ifdef ESP32
    p.replace(FPSTR(T_1), (String)ESP.getChipModel());
    #endif
  }
  else if (id==inf_wifihead){
    p = FPSTR(HTTP_INFO_wifihead);
    String m = FPSTR(S_NA);
    if (WiFi.getMode() <= 3) {
      m = WIFI_MODES[WiFi.getMode()];
    }
    p.replace(FPSTR(T_1), m);
  }
  else if (id==inf_uptime){
    // subject to rollover!
    p = FPSTR(HTTP_INFO_uptime);
    p.replace(FPSTR(T_1),(String)(millis() / 1000 / 60));
    p.replace(FPSTR(T_2),(String)((millis() / 1000) % 60));
  }
  else if (id==inf_chip_info) {
  #ifdef ESP32
    p = FPSTR(HTTP_INFO_chip_info);
    p.replace(FPSTR(T_1), (String)ESP.getChipModel());
    p.replace(FPSTR(T_2), (String)ESP.getChipRevision());
  #endif	
  }
  else if(id==inf_chipid){
    p = FPSTR(HTTP_INFO_chipid);
    p.replace(FPSTR(T_1),String(WIFI_getChipId(),HEX));
  }
  #ifdef ESP32
  else if(id==inf_chiprev){
      p = FPSTR(HTTP_INFO_chiprev);
      String rev = (String)ESP.getChipRevision();
      #ifdef _SOC_EFUSE_REG_H_
        String revb = (String)(REG_READ(EFUSE_BLK0_RDATA3_REG) >> (EFUSE_RD_CHIP_VER_RESERVE_S)&&EFUSE_RD_CHIP_VER_RESERVE_V);
        p.replace(FPSTR(T_1),rev+"<br/>"+revb);
      #else
        p.replace(FPSTR(T_1),rev);
      #endif
  }
  #endif
  #ifdef ESP8266
  else if(id==inf_fchipid){
      p = FPSTR(HTTP_INFO_fchipid);
      p.replace(FPSTR(T_1),(String)ESP.getFlashChipId());
  }
  #endif
  else if(id==inf_idesize){
    p = FPSTR(HTTP_INFO_idesize);
    p.replace(FPSTR(T_1),(String)ESP.getFlashChipSize());
  }
  else if(id==inf_flashsize){
    #ifdef ESP8266
      p = FPSTR(HTTP_INFO_flashsize);
      p.replace(FPSTR(T_1), (String)ESP.getFlashChipRealSize());
    #endif
  }
  else if(id==inf_psram_size){
    #if defined ESP32
      p = FPSTR(HTTP_INFO_psrsize);
      p.replace(FPSTR(T_1), (String)ESP.getPsramSize());
    #endif
  }
  else if( id==inf_corever) {
    #ifdef ESP8266
      p = FPSTR(HTTP_INFO_corever);
      p.replace(FPSTR(T_1),(String)ESP.getCoreVersion());
    #endif
  }
  else if(id==inf_bootver){
  #ifdef ESP8266
      p = FPSTR(HTTP_INFO_bootver);
      p.replace(FPSTR(T_1),(String)system_get_boot_version());
  #endif
  }
  else if(id==inf_cpufreq){
    p = FPSTR(HTTP_INFO_cpufreq);
    p.replace(FPSTR(T_1),(String)ESP.getCpuFreqMHz());
  }
  else if(id==inf_freeheap){
    p = FPSTR(HTTP_INFO_freeheap);
    p.replace(FPSTR(T_1),(String)ESP.getFreeHeap());
  }
  else if(id==inf_memsketch){
    p = FPSTR(HTTP_INFO_memsketch);
    p.replace(FPSTR(T_1),(String)(ESP.getSketchSize()));
    p.replace(FPSTR(T_2),(String)(ESP.getSketchSize()+ESP.getFreeSketchSpace()));
  }
  else if(id==inf_memsmeter){
    p = FPSTR(HTTP_INFO_memsmeter);
    p.replace(FPSTR(T_1),(String)(ESP.getSketchSize()));
    p.replace(FPSTR(T_2),(String)(ESP.getSketchSize()+ESP.getFreeSketchSpace()));
  }
  else if(id==inf_lastreset){
    #ifdef ESP8266
      p = FPSTR(HTTP_INFO_lastreset);
      p.replace(FPSTR(T_1),(String)ESP.getResetReason());
    #elif defined(ESP32) && defined(_ROM_RTC_H_) // requires #include <rom/rtc.h>
      p = FPSTR(HTTP_INFO_lastreset);
      for(int i=0;i<2;i++){
        int reason = rtc_get_reset_reason(i);
        String tok = (String)T_ss+(String)(i+1)+(String)T_es;
        switch (reason)
        {
          //@todo move to array
          case 1  : p.replace(tok,F("Vbat power on reset"));break;
          case 3  : p.replace(tok,F("Software reset digital core"));break;
          case 4  : p.replace(tok,F("Legacy watch dog reset digital core"));break;
          case 5  : p.replace(tok,F("Deep Sleep reset digital core"));break;
          case 6  : p.replace(tok,F("Reset by SLC module, reset digital core"));break;
          case 7  : p.replace(tok,F("Timer Group0 Watch dog reset digital core"));break;
          case 8  : p.replace(tok,F("Timer Group1 Watch dog reset digital core"));break;
          case 9  : p.replace(tok,F("RTC Watch dog Reset digital core"));break;
          case 10 : p.replace(tok,F("Instrusion tested to reset CPU"));break;
          case 11 : p.replace(tok,F("Time Group reset CPU"));break;
          case 12 : p.replace(tok,F("Software reset CPU"));break;
          case 13 : p.replace(tok,F("RTC Watch dog Reset CPU"));break;
          case 14 : p.replace(tok,F("for APP CPU, reseted by PRO CPU"));break;
          case 15 : p.replace(tok,F("Reset when the vdd voltage is not stable"));break;
          case 16 : p.replace(tok,F("RTC Watch dog reset digital core and rtc module"));break;
          default : p.replace(tok,F("NO_MEAN"));
        }
      }
    #endif
  }
  else if(id==inf_apip){
    p = FPSTR(HTTP_INFO_apip);
    p.replace(FPSTR(T_1),WiFi.softAPIP().toString());
  }
  else if(id==inf_apmac){
    p = FPSTR(HTTP_INFO_apmac);
    p.replace(FPSTR(T_1),(String)WiFi.softAPmacAddress());
  }
  #ifdef ESP32
  else if(id==inf_aphost){
      p = FPSTR(HTTP_INFO_aphost);
      p.replace(FPSTR(T_1),WiFi.softAPgetHostname());
  }
  #endif
  #ifdef ESP8266
  else if(id==inf_apssid){
    p = FPSTR(HTTP_INFO_apssid);
    p.replace(FPSTR(T_1),htmlEntities(WiFi.softAPSSID()));
  }
  #endif
  else if(id==inf_apbssid){
    p = FPSTR(HTTP_INFO_apbssid);
    p.replace(FPSTR(T_1),(String)WiFi.BSSIDstr());
  }
  // softAPgetHostname // esp32
  // softAPSubnetCIDR
  // softAPNetworkID
  // softAPBroadcastIP

  else if(id==inf_stassid){
    p = FPSTR(HTTP_INFO_stassid);
    //p.replace(FPSTR(T_1),htmlEntities((String)WiFi_SSID())); XXX
    p.replace(FPSTR(T_1), WiFi.SSID());
  }
  else if(id==inf_staip){
    p = FPSTR(HTTP_INFO_staip);
    p.replace(FPSTR(T_1),WiFi.localIP().toString());
  }
  else if(id==inf_stagw){
    p = FPSTR(HTTP_INFO_stagw);
    p.replace(FPSTR(T_1),WiFi.gatewayIP().toString());
  }
  else if(id==inf_stasub){
    p = FPSTR(HTTP_INFO_stasub);
    p.replace(FPSTR(T_1),WiFi.subnetMask().toString());
  }
  else if(id==inf_dnss){
    p = FPSTR(HTTP_INFO_dnss);
    p.replace(FPSTR(T_1),WiFi.dnsIP().toString());
  }
  else if(id==inf_host){
    p = FPSTR(HTTP_INFO_host);
    #ifdef ESP32
      p.replace(FPSTR(T_1),WiFi.getHostname());
    #else
    p.replace(FPSTR(T_1),WiFi.hostname());
    #endif
  }
  else if(id==inf_stamac){
    p = FPSTR(HTTP_INFO_stamac);
    p.replace(FPSTR(T_1),WiFi.macAddress());
  }
  else if(id==inf_conx){
    p = FPSTR(HTTP_INFO_conx);
    p.replace(FPSTR(T_1),WiFi.isConnected() ? FPSTR(S_y) : FPSTR(S_n));
  }
  #ifdef ESP8266
  else if(id==inf_autoconx){
    p = FPSTR(HTTP_INFO_autoconx);
    p.replace(FPSTR(T_1),WiFi.getAutoConnect() ? FPSTR(S_enable) : FPSTR(S_disable));
  }
  #endif
  #if defined(ESP32)
  else if(id==inf_temp){
    // temperature is not calibrated, varying large offsets are present, use for relative temp changes only
    p = FPSTR(HTTP_INFO_temp);
    p.replace(FPSTR(T_1),(String)temperatureRead());               // Celsius
    // p.replace(FPSTR(T_2),(String)((temperatureRead()+32)*1.8)); // F
  }
  // else if(id==inf_hall){
  //   p = FPSTR(HTTP_INFO_hall);
  //   p.replace(FPSTR(T_1),(String)hallRead()); // hall sensor reads can cause issues with adcs
  // }
  #endif
  else if(id==inf_aboutver){
    p = FPSTR(HTTP_INFO_aboutver);
    #ifdef VERSION_STR 
    p.replace(FPSTR(T_1),FPSTR(VERSION_STR));
    #else
    p.replace(FPSTR(T_1),FPSTR("?"));
    #endif
  }
  else if (id==inf_aboutarduinover){
    #ifdef VER_ARDUINO_STR
    p = FPSTR(HTTP_INFO_aboutarduino);
    p.replace(FPSTR(T_1),String(VER_ARDUINO_STR));
    #endif
  }
  /*
  else if (id==inf_aboutidfver) {
    #ifdef VER_IDF_STR
    p = FPSTR(HTTP_INFO_aboutidf);
    p.replace(FPSTR(T_1),String(VER_IDF_STR));
    #endif
  }
  */
  else if(id==inf_aboutsdkver){
    p = FPSTR(HTTP_INFO_sdkver);
    #ifdef ESP32
      p.replace(FPSTR(T_1),(String)esp_get_idf_version());
      // p.replace(FPSTR(T_1),(String)system_get_sdk_version()); // deprecated
    #else
    p.replace(FPSTR(T_1),(String)system_get_sdk_version());
    #endif
  }
  else if(id==inf_aboutdate){
    p = FPSTR(HTTP_INFO_aboutdate);
    p.replace(FPSTR(T_1),String(__DATE__ " " __TIME__));
  }
  return p;
}
