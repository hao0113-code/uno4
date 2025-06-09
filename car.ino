// ------------------ 感測器與腳位宣告區塊 ------------------
const int IR_L = 8;   // 左紅外線
const int IR_R = 9;   // 右紅外線

const int ENA = 3, IN1 = 2, IN2 = 4;  // 左馬達
const int ENB = 5, IN3 = 6, IN4 = 7;  // 右馬達

const int trigPin = 10;  // 超音波 Trig
const int echoPin = 11;  // 超音波 Echo

const int LED_L = 12;  // 左LED
const int LED_R = 13;  // 右LED

// 速度與延遲設定
#define STEP_TIME_FORWARD   100
#define STEP_TIME_TURN      250
#define SPEED_FORWARD        60
#define SPEED_TURN           75
#define BOOST_SPEED         150
#define BOOST_TIME           80

bool hasStarted = false;

void setup() {
  // --- 初始化感測器與裝置腳位 ---
  pinMode(IR_L, INPUT); pinMode(IR_R, INPUT);

  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  pinMode(trigPin, OUTPUT); pinMode(echoPin, INPUT);

  pinMode(LED_L, OUTPUT); pinMode(LED_R, OUTPUT);

  Serial.begin(9600);
}

// ------------------ 主流程 loop() ------------------
void loop() {
  int l = digitalRead(IR_L);
  int r = digitalRead(IR_R);
  long distance = readDistance();

  Serial.print("左IR: "); Serial.print(l);
  Serial.print(" | 右IR: "); Serial.print(r);
  Serial.print(" | 距離: "); Serial.print(distance); Serial.println(" cm");

  if (distance > 0 && distance < 15) {
    Serial.println("⚠ 偵測到障礙物，停止");
    stopMotors();
  }
  else if (l == LOW && r == LOW) {
    Serial.println("→ 黑線在中間，走一步");
    moveForwardStep();
  }
  else if (l == LOW && r == HIGH) {
    Serial.println("↶ 黑線偏右，左轉一步");
    turnLeftStep();
  }
  else if (l == HIGH && r == LOW) {
    Serial.println("↷ 黑線偏左，右轉一步");
    turnRightStep();
  }
  else {
    Serial.println("✖ 黑線不見，停止");
    stopMotors();
  }

  delay(30);
}

// ------------------ 動作控制函式區塊 ------------------
void moveForwardStep() {
  ledForward();
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);

  if (!hasStarted) {
    analogWrite(ENA, BOOST_SPEED);
    analogWrite(ENB, BOOST_SPEED);
    delay(BOOST_TIME);
    hasStarted = true;
  }

  analogWrite(ENA, SPEED_FORWARD);
  analogWrite(ENB, SPEED_FORWARD);
  delay(STEP_TIME_FORWARD);
  stopMotors();
}

void turnLeftStep() {
  ledLeft();
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, SPEED_TURN);
  analogWrite(ENB, SPEED_TURN);
  delay(STEP_TIME_TURN);
  stopMotors();
}

void turnRightStep() {
  ledRight();
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, SPEED_TURN);
  analogWrite(ENB, SPEED_TURN);
  delay(STEP_TIME_TURN);
  stopMotors();
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  ledOff();
}

// ------------------ 超音波避障區塊 ------------------
long readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 20000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

// ------------------ LED 控制區塊 ------------------
void ledForward() {
  digitalWrite(LED_L, HIGH);
  digitalWrite(LED_R, HIGH);
}

void ledLeft() {
  digitalWrite(LED_L, HIGH);
  digitalWrite(LED_R, LOW);
}

void ledRight() {
  digitalWrite(LED_L, LOW);
  digitalWrite(LED_R, HIGH);
}

void ledOff() {
  digitalWrite(LED_L, LOW);
  digitalWrite(LED_R, LOW);
}
