#include <WiFi.h>
#include <WebServer.h>

// ==================== 修改这里 ====================
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
// =================================================

WebServer server(80);

#define LED_PIN     2
#define TOUCH_PIN   4

enum SystemState {
    DISARMED,
    ARMED,
    ALARM
};

SystemState systemState = DISARMED;
bool lastTouchState = false;
unsigned long alarmBlinkTime = 0;
bool alarmLedState = false;

const int TOUCH_THRESHOLD = 30;

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 安防报警器</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            padding: 30px;
            background: #1a1a2e;
            color: white;
            min-height: 100vh;
            margin: 0;
        }
        .status-box {
            padding: 30px;
            margin: 20px auto;
            border-radius: 15px;
            font-size: 24px;
            font-weight: bold;
            max-width: 400px;
            transition: all 0.3s;
        }
        .disarmed { background: #2ecc71; }
        .armed { background: #f39c12; }
        .alarm { background: #e74c3c; animation: pulse 0.5s infinite; }
        @keyframes pulse {
            0%, 100% { transform: scale(1); }
            50% { transform: scale(1.05); }
        }
        .btn {
            padding: 20px 50px;
            font-size: 20px;
            border: none;
            border-radius: 10px;
            cursor: pointer;
            margin: 10px;
            transition: all 0.3s;
        }
        .btn-arm { background: #e74c3c; color: white; }
        .btn-disarm { background: #2ecc71; color: white; }
        .btn:hover { transform: scale(1.05); }
    </style>
</head>
<body>
    <h1>ESP32 安防报警器</h1>
    <div class="status-box disarmed" id="statusBox">
        当前状态: <span id="statusText">撤防中</span>
    </div>
    <button class="btn btn-arm" onclick="arm()">布防</button>
    <button class="btn btn-disarm" onclick="disarm()">撤防</button>

    <script>
        function arm() {
            fetch('/arm')
                .then(r => r.text())
                .then(data => updateStatus(data));
        }
        function disarm() {
            fetch('/disarm')
                .then(r => r.text())
                .then(data => updateStatus(data));
        }
        function updateStatus(data) {
            const box = document.getElementById('statusBox');
            const text = document.getElementById('statusText');
            box.className = 'status-box ' + data.toLowerCase();
            if (data === 'DISARMED') text.textContent = '撤防中';
            else if (data === 'ARMED') text.textContent = '布防中';
            else if (data === 'ALARM') text.textContent = '报警中!';
        }
        setInterval(() => {
            fetch('/status')
                .then(r => r.text())
                .then(data => updateStatus(data));
        }, 1000);
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
    server.send(200, "text/html", htmlPage);
}

void handleArm() {
    if (systemState == DISARMED) {
        systemState = ARMED;
        digitalWrite(LED_PIN, LOW);
        Serial.println("[布防] 系统已布防");
        server.send(200, "text/plain", "ARMED");
    } else {
        server.send(200, "text/plain", getStateString());
    }
}

void handleDisarm() {
    systemState = DISARMED;
    digitalWrite(LED_PIN, LOW);
    Serial.println("[撤防] 系统已撤防，报警解除");
    server.send(200, "text/plain", "DISARMED");
}

void handleStatus() {
    server.send(200, "text/plain", getStateString());
}

String getStateString() {
    switch (systemState) {
        case DISARMED: return "DISARMED";
        case ARMED: return "ARMED";
        case ALARM: return "ALARM";
        default: return "UNKNOWN";
    }
}

void checkTouchSensor() {
    int touchValue = touchRead(TOUCH_PIN);
    bool currentTouch = (touchValue < TOUCH_THRESHOLD);
    
    if (systemState == ARMED && currentTouch && !lastTouchState) {
        systemState = ALARM;
        Serial.println("[触发] 检测到入侵! 报警启动!");
        Serial.print("触摸值: ");
        Serial.println(touchValue);
    }
    lastTouchState = currentTouch;
}

void handleAlarm() {
    if (systemState == ALARM) {
        unsigned long now = millis();
        if (now - alarmBlinkTime > 100) {
            alarmBlinkTime = now;
            alarmLedState = !alarmLedState;
            digitalWrite(LED_PIN, alarmLedState ? HIGH : LOW);
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

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
    server.on("/arm", handleArm);
    server.on("/disarm", handleDisarm);
    server.on("/status", handleStatus);
    server.begin();

    Serial.print("访问地址: http://");
    Serial.println(WiFi.localIP());
}

void loop() {
    server.handleClient();
    checkTouchSensor();
    handleAlarm();
}