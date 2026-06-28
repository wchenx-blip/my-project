// ex04.ino - 基于触摸传感器的自锁开关
// 功能：摸一下触摸引脚，LED亮起并保持；再摸一下，LED熄灭
// 硬件：ESP32 开发板，使用 GPIO4 触摸引脚

#define LED_PIN     2       // 板载LED引脚
#define TOUCH_PIN   4       // ESP32触摸引脚 GPIO4 (T0)

bool ledState = false;      // LED当前状态（自锁核心）
bool lastTouchState = false; // 上一次触摸状态（边缘检测用）
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;  // 软件防抖50ms

// 触摸阈值（ESP32触摸值通常<50表示被触摸，具体值因环境而异）
const int TOUCH_THRESHOLD = 30;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("========================================");
  Serial.println("实验4：触摸自锁开关");
  Serial.println("触摸 GPIO4 引脚来切换 LED 状态");
  Serial.println("========================================");
}

void loop() {
  // 读取触摸值（值越小表示越接近/被触摸）
  int touchValue = touchRead(TOUCH_PIN);
  
  // 判断当前是否被触摸
  bool currentTouchState = (touchValue < TOUCH_THRESHOLD);
  
  // 边缘检测：从未触摸 -> 被触摸（上升沿）
  // 同时加入防抖：两次触发间隔必须大于 DEBOUNCE_DELAY
  if (currentTouchState && !lastTouchState) {
    if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
      // 翻转LED状态（自锁核心逻辑）
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
      
      Serial.print("[触发] 触摸值: ");
      Serial.print(touchValue);
      Serial.print(" | LED状态: ");
      Serial.println(ledState ? "ON" : "OFF");
      
      lastDebounceTime = millis();
    }
  }
  
  // 更新上一次状态
  lastTouchState = currentTouchState;
  
  delay(10);  // 降低CPU占用，稳定读取
}