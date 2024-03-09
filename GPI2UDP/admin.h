char adminhtml[] = R"(<!DOCTYPE html>
<html>
    <head>
        <title>GPI2UDP Config #1</title>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <style>
            body {background: rgb(16,16,16); color: rgb(235,235,235); }
            * {font-family: sans-serif; font-size: 16px;}
            h1 { font-size: 30px; text-align: center; margin: 6px;  margin-top: 30px; margin-bottom: 25px;}
            h2 { font-size: 24px; text-align: center; margin: 6px;}
            h3 { font-size: 16px; text-align: center; margin: 15px; font-weight: normal;  margin-top: 25px; margin-bottom: 50px;}
            input {
                width: 18%;
                height: 20px;
                text-align: center;
                border: 1px solid white;
                background: black;
                color: white;
                border-radius: 10px;
            }
            input.big {
                width: 93%;
            }
            select { width: 99%; height: 26px; border: 1px solid white; background: black; color: white; border-radius: 10px; }
            .thl { width: 35%; text-align: left; }
            .thr { width: 65%; }
            tr {height: 26px;}
            .tbl {width: 100%; margin-top: 25px; margin-bottom: 25px;}
        </style>
    </head>
    <body>
        <h1>GPI2UDP Settings</h1>
        <table class="tbl">
            <tr>
                <td colspan="2"><h2>Ethernet</h2></td>
            </tr>
            <tr>
                <td class="thl"><label for="6">DHCP/Static IP:</label></td>
                <td class="thr"><select id="6"><option value="0" id="o1">DHCP</option><option value="1" id="o2">Static IP</option></select></td>
            </tr>
            <tr>
                <td class="thl"><label for="7">IP Address:</label></td>
                <td class="thr"><input type="number" id="7" value=""><input type="number" id="8" value=""><input type="number" id="9" value=""><input type="number" id="10" value=""></td>
            </tr>
            <tr>
                <td class="thl"><label for="19">Subnet:</label></td>
                <td class="thr"><input type="number" id="11" value=""><input type="number" id="12" value=""><input type="number" id="13" value=""><input type="number" id="14" value=""></td>
            </tr>
            <tr>
                <td class="thl"><label for="11">Gateway:</label></td>
                <td class="thr"><input type="number" id="15" value=""><input type="number" id="16" value=""><input type="number" id="17" value=""><input type="number" id="18" value=""></td>
            </tr>
            <tr>
                <td class="thl"><label for="15">DNS Server:</label></td>
                <td class="thr"><input type="number" id="19" value=""><input type="number" id="20" value=""><input type="number" id="21" value=""><input type="number" id="22" value=""></td>
            </tr>
            <tr>
                <td class="thl"><label>MAC Address:</label></td>
                <td class="thr"><span id="0"></span>:<span id="1"></span>:<span id="2"></span>:<span id="3"></span>:<span id="4"></span>:<span id="5"></span></td>
            </tr>
        </table>
        <table class="tbl">
            <tr>
                <td colspan="2" class="thl"><h2>Admin WiFi Settings</h2></td>
            </tr>
            <tr>
                <td class="thl"><label for="ssid">SSID:</label></td>
                <td class="thr"><input class="big" type="text" id="ssid" maxlength="32" value=""></td>
            </tr>
            <tr>
                <td class="thl"><label for="pwd">Password:</label></td>
                <td class="thr"><input class="big" type="text" id="pwd" maxlength="32" value=""></td>
            </tr>
        </table>
        <table class="tbl">
            <tr>
                <td colspan="2" class="thl"><h2>UDP Settings</h2></td>
            </tr>
            <tr>
                <td class="thl"><label for="host">Host IP:</label></td>
                <td class="thr"><input type="number" id="23" value=""><input type="number" id="24" value=""><input type="number" id="25" value=""><input type="number" id="26"  value=""></td>
            </tr>
            <tr>
                <td class="thl"><label for="port">Host Port:</label></td>
                <td class="thr"><input class="big" type="number" id="27" min="0" max="65535" value=""></td>
            </tr>
            <tr>
                <td colspan="2" class="thl">GPI 1 UDP Message: (max. 240 Character)</td>
            </tr>
            <tr>
                <td colspan="2" class="thl"><input class="big" type="text" id="udp1" maxlength="240" value=""></td>
            </tr>
            <tr>
                <td colspan="2" class="thl">GPI 2 UDP Message: (max. 240 Character)</td>
            </tr>
            <tr>
                <td colspan="2" class="thl"><input class="big" type="text" id="udp2" maxlength="240" value=""></td>
            </tr>
            <tr>
                <td colspan="2" class="thl">GPI 3 UDP Message: (max. 240 Character)</td>
            </tr>
            <tr>
                <td colspan="2" class="thl"><input class="big" type="text" id="udp3" maxlength="240" value=""></td>
            </tr>
            <tr>
                <td colspan="2" class="thl">GPI 4 UDP Message: (max. 240 Character)</td>
            </tr>
            <tr>
                <td colspan="2" class="thl"><input class="big" type="text" id="udp4" maxlength="240" value=""></td>
            </tr>
            <tr>
                <td colspan="2" class="thl"><h2><button onclick="saveSettings();">Save</button></h2></td>
            </tr>
        </table>
        <h3>Copyright &copy; 2024 testbild.media</h3>
        <script>
            var gateway = `ws://${window.location.hostname}/ws`;
            var websocket;
            var wsup = false;
            window.addEventListener('load', onload);
            function onload(event) {
                initWebSocket();
            }

            function initWebSocket() {
                console.log('Trying to open a WebSocket connection…');
                websocket = new WebSocket(gateway);
                websocket.onopen = onOpen;
                websocket.onclose = onClose;
                websocket.onmessage = onMessage;
            }

            // When websocket is established, call the getReadings() function
            function onOpen(event) {
                console.log('Connection opened');
                wsup = true;
            }

            function onClose(event) {
                console.log('Connection closed');
                wsup = false;
                setTimeout(initWebSocket, 2000);
            }

            // Function that receives the message from the ESP32 with the readings
            function onMessage(event) {
                wsup = true;
                var myObj = JSON.parse(event.data);
                console.log(myObj);

                if (myObj[6] < 1){
                     document.getElementById("o1").selected = true;
                     document.getElementById("o2").selected = false;
                } else {
                     document.getElementById("o1").selected = false;
                     document.getElementById("o2").selected = true;
                }
                
                for (var i = 0; i < 6; i++){
                    document.getElementById(i).innerHTML = myObj[i].toString(16).toUpperCase();
                }
                for (var i = 7; i < 28; i++){
                    document.getElementById(i).value = myObj[i];
                }
                document.getElementById("ssid").value = myObj["ssid"];
                document.getElementById("pwd").value = myObj["pwd"];
                document.getElementById("udp1").value = myObj["udp1"];
                document.getElementById("udp2").value = myObj["udp2"];
                document.getElementById("udp3").value = myObj["udp3"];
                document.getElementById("udp4").value = myObj["udp4"];
            }
            
            function saveSettings(){
                if (wsup){
                    var msg = {data:{}};
                    for (var i = 6; i < 28; i++){
                        msg.data[i] = document.getElementById(i).value;
                    }
                    msg.data["ssid"] = document.getElementById("ssid").value;
                    msg.data["pwd"] = document.getElementById("pwd").value;
                    msg.data["udp1"] = document.getElementById("udp1").value;
                    msg.data["udp2"] = document.getElementById("udp2").value;
                    msg.data["udp3"] = document.getElementById("udp3").value;
                    msg.data["udp4"] = document.getElementById("udp4").value;
                    var SendMsg = JSON.stringify(msg);
                    console.log(SendMsg);
                    websocket.send(SendMsg);
                    alert("Settings saved! Please restart the Device to apply all changes!");
                } else {
                    alert("Saving Failed! Please renew the Connection or Restart the Device!");
                }
            }
        </script>
    </body>
</html>)";
