#include <WiFi.h>
#include <WebServer.h>

// ==================== 修改这里 ====================
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
// =================================================

WebServer server(80);

#define TOUCH_PIN   4

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 传感器仪表盘</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Arial, sans-serif;
            background: linear-gradient(135deg, #0f2027, #203a43, #2c5364);
            color: white;
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }
        .container { text-align: center; padding: 40px; }
        h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
            text-shadow: 0 0 20px rgba(0,200,255,0.5);
        }
        .subtitle { color: #aaa; margin-bottom: 40px; }
        .gauge-container {
            position: relative;
            width: 300px;
            height: 300px;
            margin: 30px auto;
        }
        .gauge-bg {
            width: 100%;
            height: 100%;
            border-radius: 50%;
            background: conic-gradient(
                from 180deg,
                #e74c3c 0deg,
                #f39c12 60deg,
                #2ecc71 120deg,
                #3498db 180deg
            );
            padding: 20px;
        }
        .gauge-inner {
            width: 100%;
            height: 100%;
            border-radius: 50%;
            background: #0f2027;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }
        .value {
            font-size: 72px;
            font-weight: bold;
            color: #00d4ff;
            text-shadow: 0 0 30px rgba(0,212,255,0.5);
            transition: all 0.3s;
        }
        .label { font-size: 18px; color: #888; margin-top: 10px; }
        .status {
            margin-top: 30px;
            padding: 15px 30px;
            border-radius: 25px;
            font-size: 18px;
            font-weight: bold;
            transition: all 0.3s;
        }
        .status-near { background: #e74c3c; box-shadow: 0 0 20px rgba(231,76,60,0.5); }
        .status-far { background: #2ecc71; box-shadow: 0 0 20px rgba(46,204,113,0.5); }
        .status-mid { background: #f39c12; }
        .info {
            margin-top: 30px;
            padding: 20px;
            background: rgba(255,255,255,0.1);
            border-radius: 15px;
            max-width: 400px;
        }
        .update-time { color: #666; font-size: 12px; margin-top: 20px; }
        .waveform {
            width: 100%;
            max-width: 500px;
            height: 100px;
            margin: 20px auto;
            background: rgba(0,0,0,0.3);
            border-radius: 10px;
            position: relative;
            overflow: hidden;
        }
        .wave-bar {
            position: absolute;
            bottom: 0;
            width: 8px;
            background: linear-gradient(to top, #00d4ff, #0099cc);
            border-radius: 4px 4px 0 0;
            transition: height 0.3s;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>实时传感器仪表盘</h1>
        <p class="subtitle">ESP32 触摸传感器数据监控</p>
        
        <div class="gauge-container">
            <div class="gauge-bg">
                <div class="gauge-inner">
                    <div class="value" id="sensorValue">--</div>
                    <div class="label">触摸数值</div>
                </div>
            </div>
        </div>
        
        <div class="status status-far" id="status">等待数据...</div>
        <div class="waveform" id="waveform"></div>
        
        <div class="info">
            <p>说明：</p>
            <p>数值越小表示越接近/触摸</p>
            <p>数值越大表示距离越远</p>
            <p>正常范围：0 - 100</p>
        </div>
        <p class="update-time" id="updateTime">最后更新: --</p>
    </div>

    <script>
        const waveform = document.getElementById('waveform');
        const history = [];
        const maxHistory = 50;
        
        for (let i = 0; i < maxHistory; i++) {
            const bar = document.createElement('div');
            bar.className = 'wave-bar';
            bar.style.left = (i * 10) + 'px';
            bar.style.height = '0%';
            waveform.appendChild(bar);
        }
        
        function updateWaveform(value) {
            history.push(value);
            if (history.length > maxHistory) history.shift();
            const bars = waveform.querySelectorAll('.wave-bar');
            history.forEach((val, i) => {
                if (bars[i]) {
                    bars[i].style.height = Math.min(100, (val / 100) * 100) + '%';
                    bars[i].style.opacity = 0.3 + (i / maxHistory) * 0.7;
                }
            });
        }
        
        function updateData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    const value = data.value;
                    const valueEl = document.getElementById('sensorValue');
                    const statusEl = document.getElementById('status');
                    const timeEl = document.getElementById('updateTime');
                    
                    valueEl.textContent = value;
                    
                    statusEl.className = 'status';
                    if (value < 20) {
                        statusEl.classList.add('status-near');
                        statusEl.textContent = '触摸中 (近距离)';
                    } else if (value < 50) {
                        statusEl.classList.add('status-mid');
                        statusEl.textContent = '接近中 (中距离)';
                    } else {
                        statusEl.classList.add('status-far');
                        statusEl.textContent = '无触摸 (远距离)';
                    }
                    
                    if (value < 20) valueEl.style.color = '#e74c3c';
                    else if (value < 50) valueEl.style.color = '#f39c12';
                    else valueEl.style.color = '#00d4ff';
                    
                    updateWaveform(value);
                    timeEl.textContent = '最后更新: ' + new Date().toLocaleTimeString();
                })
                .catch(err => console.error('获取数据失败:', err));
        }
        
        setInterval(updateData, 200);
        updateData();
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
    server.send(200, "text/html", htmlPage);
}

void handleData() {
    int touchValue = touchRead(TOUCH_PIN);
    String json = "{";
    json += "\"value\":" + String(touchValue);
    json += ",\"timestamp\":" + String(millis());
    json += "}";
    server.send(200, "application/json", json);
}

void setup() {
    Serial.begin(115200);

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
    server.on("/data", handleData);
    server.begin();

    Serial.print("访问地址: http://");
    Serial.println(WiFi.localIP());
}

void loop() {
    server.handleClient();
}