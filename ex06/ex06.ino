// ex06.ino - 警车双闪灯效（双通道PWM反相）
// 功能：两个LED灯亮度呈反相关系，A变亮时B变暗，过渡柔和
// 硬件：ESP32 开发板，GPIO2接LED_A，GPIO4接LED_B

#define LED_A_PIN   2       // LED A 引脚
#define LED_B_PIN   4       // LED B 引脚

// 两个独立PWM通道
#define PWM_CHANNEL_A   0
#define PWM_CHANNEL_B   1
#define PWM_FREQ        5000
#define PWM_RESOLUTION  8   // 0~255

// 渐变速度控制
const int STEP = 1;         // 占空比步长（1最平滑）
const int DELAY_MS = 10;    // 每步延时（越小越快）

void setup() {
  Serial.begin(115200);
  
  // 初始化两个PWM通道
  ledcSetup(PWM_CHANNEL_A, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_B, PWM_FREQ, PWM_RESOLUTION);
  
  ledcAttachPin(LED_A_PIN, PWM_CHANNEL_A);
  ledcAttachPin(LED_B_PIN, PWM_CHANNEL_B);
  
  Serial.println("========================================");
  Serial.println("实验6：警车双闪灯效（双通道PWM反相）");
  Serial.println("LED_A 与 LED_B 呈平滑交替渐变");
  Serial.println("========================================");
}

void loop() {
  // 阶段1：A从暗到亮，B从亮到暗
  // dutyA: 0 -> 255, dutyB: 255 -> 0
  for (int i = 0; i <= 255; i += STEP) {
    int dutyA = i;           // A渐亮
    int dutyB = 255 - i;     // B渐暗（反相核心）
    
    ledcWrite(PWM_CHANNEL_A, dutyA);
    ledcWrite(PWM_CHANNEL_B, dutyB);
    
    delay(DELAY_MS);
  }
  
  // 阶段2：A从亮到暗，B从暗到亮
  // dutyA: 255 -> 0, dutyB: 0 -> 255
  for (int i = 255; i >= 0; i -= STEP) {
    int dutyA = i;           // A渐暗
    int dutyB = 255 - i;     // B渐亮（反相核心）
    
    ledcWrite(PWM_CHANNEL_A, dutyA);
    ledcWrite(PWM_CHANNEL_B, dutyB);
    
    delay(DELAY_MS);
  }
}