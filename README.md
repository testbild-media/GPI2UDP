# GPI2UDP
A little program, that is for using the preview function of an RCP to trigger <a href="https://github.com/bitfocus/companion">Bitfocus Companion</a> via UDP.

My main goal was that I can press the Iris Lever on RCP to trigger a Button in Companion, which switches a Aux Output of an Atem Switcher or anything else. It should be easy to configure, because of changing setups.
| Description | Image |
| :--- | ---: |
| ESP8266 and W5500 with only some wires. Maybe I print a housing for that later this year. The Buttons where only for testing. | <img src="https://github.com/testbild-media/GPI2UDP/blob/main/images/photo.jpg" width="400"> |
| I build a Admin Webpage with the build in WiFi Access Point. If you login to the default it http://192.168.1.1/admin you could set DHCP or Static IP Address for Ethernet, Subnet, Gateway and DNS Server. You also could rename the WiFi SSID and the Password, if you use more then one device or if you simply want to customize it. After those settings the ESP8266 has to unplug and replug the power. For UDP side you can edit the host or the port on the fly, if it's in range of the Ethernet Interface. Each GPI triggers one Message, which you can specify here too. No need for reboot here. All Settings are saved in the EEPROM of the ESP8266, so that after a reboot it comes back on, where it stopped. | <img src="https://github.com/testbild-media/GPI2UDP/blob/main/images/screenshot.png" width="200"> |

### Troubleshooting
If you done something stupid and the device don't boot as normal, you could flash the ESP8266 again the the `#define ForceDefaults true` flag and the EEPROM gets wiped and Default Settings are stored again. After set it to `#define ForceDefaults false` and flash again. Then you are back to normal.

### ToDo's
- [ ] Clean up the Code
- [ ] more and more testing
