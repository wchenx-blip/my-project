// ex05.ino - 多档位触摸调速呼吸灯
// 功能：LED持续呼吸灯效果，触摸引脚切换速度档位（1->2->3->1循环）
// 硬件：ESP32 开发板，GPIO2输出PWM，GPIO4触摸输入

#define LED_PIN     2       // LED引脚（PWM输出）
#define TOUCH_PIN   4       // 触摸引脚 GPIO4

// ESP32 PWM配置（使用ledc库）
#define PWM_CHANNEL     0
#define PWM_FREQ        5000
#define PWM_RESOLUTION  8   // 8位分辨率：0~255

int speedLevel = 1;         // 当前档位：1, 2, 3
bool lastTouchState = false;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;
const int TOUCH_THRESHOLD = 30;

// 各档位参数：步长越大、延时越短，呼吸越快
struct BreathConfig {
  int step;       // 亮度步长
  int delayMs;    // 每步延时(ms)
};

BreathConfig configs[4] = {
  {0, 0},         // 索引0占位，不使用
  {1, 30},        // 1档：缓慢呼吸（步长1，延时30ms）
  {3, 15},        // 2档：中速呼吸（步长3，延时15ms）
  {8, 5}          // 3档：急促呼吸（步长8，延时5ms）
};

void setup() {
  Serial.begin(115200);
  
  // 初始化PWM通道
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED_PIN, PWM_CHANNEL);
  
  Serial.println("========================================");
  Serial.println("实验5：多档位触摸调速呼吸灯");
  Serial.println("触摸 GPIO4 切换速度档位");
  Serial.println("当前档位: 1（缓慢）");
  Serial.println("========================================");
}

void loop() {
  // 触摸检测与档位切换
  checkTouchAndSwitch();
  
  // 获取当前档位配置
  BreathConfig cfg = configs[speedLevel];
  
  // 渐亮阶段：0 -> 255
  for (int duty = 0; duty <= 255; duty += cfg.step) {
    ledcWrite(PWM_CHANNEL, duty);
    delay(cfg.delayMs);
    if (checkTouchAndSwitch()) break;  // 若触摸则立即响应
  }
  
  // 渐暗阶段：255 -> 0
  for (int duty = 255; duty >= 0; duty -= cfg.step) {
    ledcWrite(PWM_CHANNEL, duty);
    delay(cfg.delayMs);
    if (checkTouchAndSwitch()) break;
  }
}

// 触摸检测与档位切换函数
// 返回值：true表示发生了档位切换，false表示无切换
bool checkTouchAndSwitch() {
  int touchValue = touchRead(TOUCH_PIN);
  bool currentTouchState = (touchValue < TOUCH_THRESHOLD);
  
  // 边缘检测 + 防抖
  if (currentTouchState && !lastTouchState) {
    if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
      // 切换档位：1->2->3->1
      speedLevel++;
      if (speedLevel > 3) speedLevel = 1;
      
      Serial.print("[档位切换] 当前档位: ");
      Serial.print(speedLevel);
      Serial.print(" | 步长: ");
      Serial.print(configs[speedLevel].step);
      Serial.print(" | 延时: ");
      Serial.print(configs[speedLevel].delayMs);
      Serial.println("ms");
      
      lastDebounceTime = millis();
      lastTouchState = true;
      return true;
    }
  }
  
  lastTouchState = currentTouchState;
  return false;
}