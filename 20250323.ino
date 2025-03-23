// 定義心電感測器的腳位
const int HEART_SENSOR_PIN = A0;

// 定義LED腳位陣列
const int LED_PINS[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
const int NUM_LEDS = 12;  // LED數量

// 定義心電閾值
const int HEART_THRESHOLD = 500;  // 當心電值大於此值時才點亮LED

// 定義波紋相關變數
int wavePosition = 0;  // 波紋位置
int waveWidth = 3;     // 波紋寬度
int waveSpeed = 50;    // 波紋移動速度

// 定義訊號處理相關變數
const int SAMPLE_SIZE = 10;  // 取樣數量
int samples[SAMPLE_SIZE];    // 取樣陣列
int sampleIndex = 0;        // 取樣索引
int lastValue = 0;          // 上一次的值
int stableCount = 0;        // 穩定計數
const int STABLE_THRESHOLD = 5;  // 穩定閾值

void setup() {
  // 初始化所有LED腳位為輸出
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  
  // 初始化序列通訊，用於除錯
  Serial.begin(9600);
  
  // 初始化取樣陣列
  for (int i = 0; i < SAMPLE_SIZE; i++) {
    samples[i] = analogRead(HEART_SENSOR_PIN);
  }
}

// 計算平均值
int getAverage() {
  long sum = 0;
  for (int i = 0; i < SAMPLE_SIZE; i++) {
    sum += samples[i];
  }
  return sum / SAMPLE_SIZE;
}

void loop() {
  // 更新取樣
  samples[sampleIndex] = analogRead(HEART_SENSOR_PIN);
  sampleIndex = (sampleIndex + 1) % SAMPLE_SIZE;
  
  // 計算平均值
  int currentValue = getAverage();
  
  // 檢查訊號穩定性
  if (abs(currentValue - lastValue) < 50) {  // 如果變化小於50
    stableCount++;
  } else {
    stableCount = 0;
  }
  
  // 只在訊號穩定時才處理
  if (stableCount >= STABLE_THRESHOLD) {
    // 將心電值映射到波紋速度（20-200毫秒）
    waveSpeed = map(currentValue, 0, 1023, 20, 200);
    
    // 輸出除錯訊息
    Serial.print("Heart Value: ");
    Serial.print(currentValue);
    Serial.print(" Wave Speed: ");
    Serial.println(waveSpeed);
    
    // 只在偵測到足夠的心電信號時才產生波紋效果
    if (currentValue > HEART_THRESHOLD) {
      // 清除所有LED
      for (int i = 0; i < NUM_LEDS; i++) {
        digitalWrite(LED_PINS[i], LOW);
      }
      
      // 產生波紋效果
      for (int i = 0; i < waveWidth; i++) {
        int ledIndex = (wavePosition + i) % NUM_LEDS;
        digitalWrite(LED_PINS[ledIndex], HIGH);
      }
      
      // 更新波紋位置
      wavePosition = (wavePosition + 1) % NUM_LEDS;
      
      // 等待映射後的時間
      delay(waveSpeed);
    } else {
      // 如果沒有偵測到心電信號，確保所有LED保持熄滅
      for (int i = 0; i < NUM_LEDS; i++) {
        digitalWrite(LED_PINS[i], LOW);
      }
      delay(100);  // 短暫延遲以避免過度讀取
    }
  } else {
    // 如果訊號不穩定，保持LED熄滅
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(LED_PINS[i], LOW);
    }
    delay(50);  // 較短的延遲以快速響應
  }
  
  lastValue = currentValue;  // 更新上一次的值
}
