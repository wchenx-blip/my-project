#include <WiFi.h>
#include <WebServer.h>

// ==================== 修改这里 ====================
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
// =================================================

WebServer server(80);

#define LED_PIN         2
#define PWM_CHANNEL     0
#define PWM_FREQ        5000
#define PWM_RESOLUTION  8

int currentBrightness = 128;

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 无极调光器</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            padding: 50px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            min-height: 100vh;
            margin: 0;
        }
        h1 { margin-bottom: 30px; }
        .slider-container {
            margin: 40px auto;
            width: 80%;
            max-width: 500px;
        }
        input[type="range"] {
            width: 100%;
            height: 20px;
            -webkit-appearance: none;
            appearance: none;
            background: rgba(255,255,255,0.3);
            border-radius: 10px;
            outline: none;
        }
        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 40px;
            height: 40px;
            background: #fff;
            border-radius: 50%;
            cursor: pointer;
            box-shadow: 0 4px 15px rgba(0,0,0,0.3);
        }
        .value-display {
            font-size: 48px;
            font-weight: bold;
            margin: 20px 0;
        }
        .led-indicator {
            width: 80px;
            height: 80px;
            border-radius: 50%;
            background: #333;
            margin: 20px auto;
            box-shadow: 0 0 20px rgba(0,0,0,0.5);
            transition: all 0.1s;
        }
    </style>
</head>
<body>
    <h1>ESP32 无极调光器</h1>
    <div class="led-indicator" id="led"></div>
    <div class="value-display" id="value">128</div>
    <div class="slider-container">
        <input type="range" id="brightness" min="0" max="255" value="128">
    </div>
    <p>拖动滑动条控制LED亮度</p>

    <script>
        const slider = document.getElementById('brightness');
        const valueDisplay = document.getElementById('value');
        const led = document.getElementById('led');

        slider.addEventListener('input', function() {
            const val = this.value;
            valueDisplay.textContent = val;
            const brightness = val / 255;
            led.style.background = `rgb(${Math.round(255*brightness)}, ${Math.round(200*brightness)}, 0)`;
            led.style.boxShadow = `0 0 ${30*brightness}px rgba(255,200,0,${brightness})`;
            fetch('/set?brightness=' + val)
                .then(response => response.text())
                .then(data => console.log(data))
                .catch(err => console.error(err));
        });
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
    server.send(200, "text/html", htmlPage);
}

void handleSet() {
    if (server.hasArg("brightness")) {
        currentBrightness = server.arg("brightness").toInt();
        currentBrightness = constrain(currentBrightness, 0, 255);
        ledcWrite(PWM_CHANNEL, currentBrightness);
        Serial.print("亮度设置为: ");
        Serial.println(currentBrightness);
        server.send(200, "text/plain", "OK: " + String(currentBrightness));
    } else {
        server.send(400, "text/plain", "Missing brightness parameter");
    }
}

void setup() {
    Serial.begin(115200);
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(LED_PIN, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, currentBrightness);

    WiFi.begin(ssid, password);
    Serial.print("连接WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi已连接!");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/set", handleSet);
    server.begin();

    Serial.print("请在浏览器访问: http://");
    Serial.println(WiFi.localIP());
}

void loop() {
    server.handleClient();
}