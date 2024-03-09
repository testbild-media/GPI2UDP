#include <string>
#include "admin.h"
#include <EEPROM.h>
#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#define ETHER_CS 5  // D1
#define GPI_PIN_1 4 // D2
#define GPI_PIN_2 0 // D3
#define GPI_PIN_3 2 // D4
#define GPI_PIN_4 3 // RX

byte MAC[6];
int Settings[28];
String SettingsSSID;
String SettingsPWD;
String SettingsGPI1MSG;
String SettingsGPI2MSG;
String SettingsGPI3MSG;
String SettingsGPI4MSG;

int PinStatus[4] = {LOW, LOW, LOW, LOW};
int PinStatusLast[4] = {LOW, LOW, LOW, LOW};

EthernetUDP Udp;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ###################################
// ##  GPIO Functions
// ###################################
  void initGPIO(){
    pinMode(GPI_PIN_1, INPUT_PULLUP);
    pinMode(GPI_PIN_2, INPUT_PULLUP);
    pinMode(GPI_PIN_3, INPUT_PULLUP);
    pinMode(GPI_PIN_4, INPUT_PULLUP);
  }

  void ReadInputs() {
    PinStatus[0] = digitalRead(GPI_PIN_1);
    PinStatus[1] = digitalRead(GPI_PIN_2);
    PinStatus[2] = digitalRead(GPI_PIN_3);
    PinStatus[3] = digitalRead(GPI_PIN_4);
  }

// ###################################
// ##  Ethernet & UDP Functions
// ###################################
  void initEthernet(byte mac[6], IPAddress ip, IPAddress subnet, IPAddress gateway, IPAddress dns, int port){

    Ethernet.init(ETHER_CS);

    // if DHCP or Static IP
    if (Settings[6] >= 1){
      Ethernet.begin(mac, ip, dns, gateway, subnet);
    } else {
      Ethernet.begin(mac);
    }

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet Interface was not found.");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    } else {
      
      Serial.println("Ethernet Interface successfully integrated!");
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    } else {
      Serial.println("Ethernet connected.");

    }
    Udp.begin(port);
    
    Serial.println("Network success ###########################");
    Serial.print("IP Address: "); Serial.println(Ethernet.localIP());
    Serial.print("Subnet:     "); Serial.println(Ethernet.subnetMask());
    Serial.print("Gateway:    "); Serial.println(Ethernet.gatewayIP());
    Serial.print("DNS-Server: "); Serial.println(Ethernet.dnsServerIP());
    Serial.println("###########################################");

  }

  void EthernetStatus(){
    switch (Ethernet.maintain()) {
      case 1:
        Serial.println("###########################################");
        Serial.println("##  Error: renewed fail");
        Serial.println("###########################################");
        break;
      case 2:
        Serial.println("Renewed success ###########################");
        Serial.print("IP Address: "); Serial.println(Ethernet.localIP());
        Serial.print("Subnet:     "); Serial.println(Ethernet.subnetMask());
        Serial.print("Gateway:    "); Serial.println(Ethernet.gatewayIP());
        Serial.print("DNS-Server: "); Serial.println(Ethernet.dnsServerIP());
        Serial.println("###########################################");
        break;
      case 3:
        Serial.println("###########################################");
        Serial.println("##  Error: rebind fail");
        Serial.println("###########################################");
        break;
      case 4:
        Serial.println("Rebind success ###########################");
        Serial.print("IP Address: "); Serial.println(Ethernet.localIP());
        Serial.print("Subnet:     "); Serial.println(Ethernet.subnetMask());
        Serial.print("Gateway:    "); Serial.println(Ethernet.gatewayIP());
        Serial.print("DNS-Server: "); Serial.println(Ethernet.dnsServerIP());
        Serial.println("###########################################");
        break;
      default:
        break;
    }
  }

  void SendUDP(String MSG) {
    int MSGlength = MSG.length() + 1;
    char temp[MSGlength];
    int i;
    for (i=0;i<MSGlength;i++){
      temp[i] = MSG[i];
    }
    IPAddress udphost(Settings[23],Settings[24],Settings[25],Settings[26]);
    Udp.beginPacket(udphost, Settings[27]);
    Udp.write(temp);
    Udp.endPacket();
    Serial.print("UDP Send: "); Serial.println(temp);
  }

// ###################################
// ##  Save/Load Functions
// ###################################
  void loadSettings(){
    EEPROM.begin(4096);
    int i;
    int j;
    char temp[251];

    // Read IP Settings
    for (i=0;i<27;i++){
      int value = EEPROM.read(i);
      Settings[i] = value;
    }
    EEPROM.get(27, Settings[27]);

    // Read SSID Name
    j = 0;
    for (i=31;i<63;i++){ temp[j] = EEPROM.read(i); j++; }
    temp[32] = '\0';
    SettingsSSID = String(temp);

    // Read Password
    j = 0;
    for (i=63;i<95;i++){ temp[j] = EEPROM.read(i); j++; }
    temp[32] = '\0';
    SettingsPWD = String(temp);

    // Read GPI 1 MSG
    j = 0;
    for (i=95;i<354;i++){ temp[j] = EEPROM.read(i); j++; }
    temp[251] = '\0';
    SettingsGPI1MSG = String(temp);

    // Read GPI 2 MSG
    j = 0;
    for (i=345;i<595;i++){ temp[j] = EEPROM.read(i); j++; }
    temp[251] = '\0';
    SettingsGPI2MSG = String(temp);

    // Read GPI 3 MSG
    j = 0;
    for (i=595;i<845;i++){ temp[j] = EEPROM.read(i); j++; }
    temp[251] = '\0';
    SettingsGPI3MSG = String(temp);

    // Read GPI 4 MSG
    j = 0;
    for (i=845;i<1095;i++){ temp[j] = EEPROM.read(i); j++; }
    temp[251] = '\0';
    SettingsGPI4MSG = String(temp);

    EEPROM.end();
    Serial.println("");
    Serial.println("Settings Loaded");
    Serial.println("--------------------------------------------------");
    Serial.print("MAC-Adr:  ");Serial.print(Settings[0],HEX);Serial.print("-");Serial.print(Settings[1],HEX);Serial.print("-");Serial.print(Settings[2],HEX);Serial.print("-");Serial.print(Settings[3],HEX);Serial.print("-");Serial.print(Settings[4],HEX);Serial.print("-");Serial.println(Settings[5],HEX);
    Serial.print("DHCP :    ");Serial.println(Settings[6]);
    Serial.print("IP:       ");Serial.print(Settings[7]);Serial.print(".");Serial.print(Settings[8]);Serial.print(".");Serial.print(Settings[9]);Serial.print(".");Serial.println(Settings[10]);
    Serial.print("Subnet:   ");Serial.print(Settings[11]);Serial.print(".");Serial.print(Settings[12]);Serial.print(".");Serial.print(Settings[13]);Serial.print(".");Serial.println(Settings[14]);
    Serial.print("Gateway:  ");Serial.print(Settings[15]);Serial.print(".");Serial.print(Settings[16]);Serial.print(".");Serial.print(Settings[17]);Serial.print(".");Serial.println(Settings[18]);
    Serial.print("DNS:      ");Serial.print(Settings[19]);Serial.print(".");Serial.print(Settings[20]);Serial.print(".");Serial.print(Settings[21]);Serial.print(".");Serial.println(Settings[22]);
    Serial.print("Host:     ");Serial.print(Settings[23]);Serial.print(".");Serial.print(Settings[24]);Serial.print(".");Serial.print(Settings[25]);Serial.print(".");Serial.println(Settings[26]);
    Serial.print("Port:     ");Serial.println(Settings[27]);
    Serial.print("SSID:     ");Serial.println(SettingsSSID);
    Serial.print("Password: ");Serial.println(SettingsPWD);
    Serial.print("GPI1 MSG: ");Serial.println(SettingsGPI1MSG);
    Serial.print("GPI2 MSG: ");Serial.println(SettingsGPI2MSG);
    Serial.print("GPI3 MSG: ");Serial.println(SettingsGPI3MSG);
    Serial.print("GPI4 MSG: ");Serial.println(SettingsGPI4MSG);
    Serial.println("--------------------------------------------------");
  }

  void saveSettings(){
    int i;
    int j = 0;
    EEPROM.begin(4096);
    
    // Save IP Settings
    for (i=0;i<27;i++){
      EEPROM.write(i, Settings[i]);
    }
    EEPROM.put(27, Settings[27]);

    // Save SSID Name
    j = 0;
    for (i=31;i<63;i++){ EEPROM.write(i, SettingsSSID[j]); j++; }

    // Save Password
    j = 0;
    for (i=63;i<95;i++){ EEPROM.write(i, SettingsPWD[j]); j++; }

    // Save GPI 1 MSG
    j = 0;
    for (i=95;i<354;i++){ EEPROM.write(i, SettingsGPI1MSG[j]); j++; }

    // Save GPI 2 MSG
    j = 0;
    for (i=345;i<595;i++){ EEPROM.write(i, SettingsGPI2MSG[j]); j++; }

    // Save GPI 3 MSG
    j = 0;
    for (i=595;i<845;i++){ EEPROM.write(i, SettingsGPI3MSG[j]); j++; }

    // Save GPI 4 MSG
    j = 0;
    for (i=845;i<1095;i++){ EEPROM.write(i, SettingsGPI4MSG[j]); j++; }

    EEPROM.write(4085, 0xDE);
    EEPROM.write(4095, 0xDE);

    EEPROM.commit();
    EEPROM.end();

    Serial.println("Finished! EEPROM programmed!");
    Serial.println("--------------------------------------------------");
    Serial.print("MAC-Adr:  ");Serial.print(Settings[0],HEX);Serial.print("-");Serial.print(Settings[1],HEX);Serial.print("-");Serial.print(Settings[2],HEX);Serial.print("-");Serial.print(Settings[3],HEX);Serial.print("-");Serial.print(Settings[4],HEX);Serial.print("-");Serial.println(Settings[5],HEX);
    Serial.print("DHCP :    ");Serial.println(Settings[6]);
    Serial.print("IP:       ");Serial.print(Settings[7]);Serial.print(".");Serial.print(Settings[8]);Serial.print(".");Serial.print(Settings[9]);Serial.print(".");Serial.println(Settings[10]);
    Serial.print("Subnet:   ");Serial.print(Settings[11]);Serial.print(".");Serial.print(Settings[12]);Serial.print(".");Serial.print(Settings[13]);Serial.print(".");Serial.println(Settings[14]);
    Serial.print("Gateway:  ");Serial.print(Settings[15]);Serial.print(".");Serial.print(Settings[16]);Serial.print(".");Serial.print(Settings[17]);Serial.print(".");Serial.println(Settings[18]);
    Serial.print("DNS:      ");Serial.print(Settings[19]);Serial.print(".");Serial.print(Settings[20]);Serial.print(".");Serial.print(Settings[21]);Serial.print(".");Serial.println(Settings[22]);
    Serial.print("Host:     ");Serial.print(Settings[23]);Serial.print(".");Serial.print(Settings[24]);Serial.print(".");Serial.print(Settings[25]);Serial.print(".");Serial.println(Settings[26]);
    Serial.print("Port:     ");Serial.println(Settings[27]);
    Serial.print("SSID:     ");Serial.println(SettingsSSID);
    Serial.print("Password: ");Serial.println(SettingsPWD);
    Serial.print("GPI1 MSG: ");Serial.println(SettingsGPI1MSG);
    Serial.print("GPI2 MSG: ");Serial.println(SettingsGPI2MSG);
    Serial.print("GPI3 MSG: ");Serial.println(SettingsGPI3MSG);
    Serial.print("GPI4 MSG: ");Serial.println(SettingsGPI4MSG);
    Serial.println("--------------------------------------------------");
  }

  void firstLoader(bool forceDefault){
    EEPROM.begin(4096);
    int identify = 0xDE;

    int identifyPri = 2;
    int identifySec = 2;
    if (!forceDefault){
      identifyPri = EEPROM.read(4095);
      identifySec = EEPROM.read(4085);
    }
    EEPROM.end();
    if (identify != identifyPri || identify != identifySec){
      Serial.println("First Boot found! Programming EEPROM to Default...");
      Settings[0] = 0xA8;
      Settings[1] = 0x61;
      Settings[2] = 0x0A;
      Settings[3] = rand() % 256;
      Settings[4] = rand() % 256;
      Settings[5] = rand() % 256;
      Settings[6] = 1;
      Settings[7] = 192;
      Settings[8] = 168;
      Settings[9] = 0;
      Settings[10] = 200;
      Settings[11] = 255;
      Settings[12] = 255;
      Settings[13] = 255;
      Settings[14] = 0;
      Settings[15] = 192;
      Settings[16] = 168;
      Settings[17] = 0;
      Settings[18] = 1;
      Settings[19] = 192;
      Settings[20] = 168;
      Settings[21] = 0;
      Settings[22] = 1;
      Settings[23] = 192;
      Settings[24] = 168;
      Settings[25] = 0;
      Settings[26] = 100;
      Settings[27] = 16759;
      SettingsSSID = "GPI2UDP #0";
      SettingsPWD = "password";
      SettingsGPI1MSG = "BANK-PRESS 69 1";
      SettingsGPI2MSG = "BANK-PRESS 69 2"; 
      SettingsGPI3MSG = "BANK-PRESS 69 3";
      SettingsGPI4MSG = "BANK-PRESS 69 4";
      EEPROM.begin(4096);
      //Clear EEPROM from everything
      for (int i=0;i<4096;i++){
        EEPROM.write(i, 255);
      }
      EEPROM.commit();
      EEPROM.end();
      saveSettings();
    } else {
      Serial.println("No First Boot found! Going on...");
      return;
    }
  }

// ###################################
// ##  WiFi Functions
// ###################################
  void initWiFi(String ssid, String pwd, int ch){
    IPAddress WifiIP(192, 168, 1, 1);
    IPAddress WiFiNet(255, 255, 255, 0);
    WiFi.softAPConfig(WifiIP, WifiIP, WiFiNet);
    if (!WiFi.softAP(ssid, pwd, ch, 0)){
      Serial.println("###########################################");
      Serial.println("##  Error: WiFi Access Point failed.");
      Serial.println("###########################################");
    return;
    }
    Serial.println("WiFi online ###############################");
    Serial.print("WiFi IP: "); Serial.println(WiFi.softAPIP());
  }

  void initWebServer(){

    server.on("/admin", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send_P(200, "text/html", adminhtml);
    });
    
    server.begin();
  }

// ###################################
// ##  Websocket
// ###################################
  void notifyClients() {
    JsonDocument doc;
    for (int i=0;i<28;i++){
      doc[String(i)] = Settings[i];
    }
    doc["ssid"] = SettingsSSID;
    doc["pwd"] = SettingsPWD;
    doc["udp1"] = SettingsGPI1MSG;
    doc["udp2"] = SettingsGPI2MSG;
    doc["udp3"] = SettingsGPI3MSG;
    doc["udp4"] = SettingsGPI4MSG;
    char data[512];
    size_t len = serializeJson(doc, data);
    ws.textAll(data, len);
  }

  void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

      JsonDocument json;
      DeserializationError error = deserializeJson(json, data);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      for (int i=6;i<28;i++){
        Settings[i] = json["data"][String(i)];
      }
      SettingsSSID = json["data"]["ssid"].as<String>();
      SettingsPWD = json["data"]["pwd"].as<String>();
      SettingsGPI1MSG = json["data"]["udp1"].as<String>();
      SettingsGPI2MSG = json["data"]["udp2"].as<String>();
      SettingsGPI3MSG = json["data"]["udp3"].as<String>();
      SettingsGPI4MSG = json["data"]["udp4"].as<String>();
      saveSettings();
    }
  }

  void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {

    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            notifyClients();
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
  }

  void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
  }
