#include "tbmIO.h"

// set this one time to true for loading Defaults
#define ForceDefaults false

void setup() {
  Serial.begin(115200);
  Serial.println(""); Serial.println("");
  delay(1000);
  Serial.println(""); Serial.println("");
  Serial.println("##########################");
  Serial.println("##   GPI2UDP Started!   ##");
  Serial.println("##########################");
  Serial.println("");
  Serial.println("Check First Boot...");
  delay(100);
  firstLoader(ForceDefaults);
  Serial.println("Loading Config...");

  initGPIO();
  loadSettings();

  MAC[0] = Settings[0];
  MAC[1] = Settings[1];
  MAC[2] = Settings[2];
  MAC[3] = Settings[3];
  MAC[4] = Settings[4];
  MAC[5] = Settings[5];
  IPAddress ETHIP(Settings[7],Settings[8],Settings[9],Settings[10]);
  IPAddress ETHDNS(Settings[19],Settings[20],Settings[21],Settings[22]);
  IPAddress ETHGW(Settings[15],Settings[16],Settings[17],Settings[18]);
  IPAddress ETHNET(Settings[11],Settings[12],Settings[13],Settings[14]);
  Serial.println("Loading Completed!");

  delay(100);
  Serial.println("--------------------------------------------------");
  Serial.println("Start Ethernet Interface...");
  initEthernet(MAC, ETHIP, ETHNET, ETHGW, ETHDNS, 8888);
  
  Serial.println("--------------------------------------------------");
  Serial.println("Start WiFi Interface...");
  initWiFi(SettingsSSID, SettingsPWD, 8);
  Serial.println("--------------------------------------------------");
  Serial.println("Start Webserver...");
  initWebServer();
  initWebSocket();
  Serial.println("Webserver Running on Port 80!");
  Serial.println("--------------------------------------------------");
}

void loop() {
  ReadInputs();
  if (!(std::equal(std::begin(PinStatus), std::end(PinStatus), std::begin(PinStatusLast)))) {
    std::copy(PinStatus, PinStatus+4, PinStatusLast);
    if (PinStatus[0] == LOW) SendUDP(SettingsGPI1MSG);
    if (PinStatus[1] == LOW) SendUDP(SettingsGPI2MSG);
    if (PinStatus[2] == LOW) SendUDP(SettingsGPI3MSG);
    if (PinStatus[3] == LOW) SendUDP(SettingsGPI4MSG);
    delay(100);
  }
  EthernetStatus();
  ws.cleanupClients();
}
