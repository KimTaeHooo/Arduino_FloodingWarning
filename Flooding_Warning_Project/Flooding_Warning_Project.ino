#include <LiquidCrystal_I2C.h>  //LCD 사용을 위한 라이브러리 include

class LiquidCrystal_I2C lcd(0x27, 16, 2);  //0x27 = 주소, 16X2 출력화면 범위 설정
enum RGB_LED { RED = 13, GREEN = 12, BLUE = 11 };   //RGB
const uint8_t WATER_SENSOR {A0};  //WATER_SENSOR ANALOG PIN A0
const uint8_t BUZZER_PIN = 10U;   //BUZZER_PIN 10
const uint8_t BUTTON_SW = 24U;    //BUTTON_SW 22
const uint8_t SERVO_MOTOR = 3U;   //SERVP_MOTOR 3
const uint8_t PIR_PIN = 2U;       //PIR 3
int i = 0;  //차단기에 사용할 변수선언
static bool fw_state = false;  //침수 상황
static bool sw_state = false;  //버튼 상황

void setup() {
  Serial.begin(115200UL);
  Serial1.begin(9600UL);  //블루투스
  pinMode(RED, OUTPUT);  //RGB
  pinMode(GREEN, OUTPUT); //RGB
  pinMode(BLUE, OUTPUT);  //RGB
  pinMode(WATER_SENSOR, INPUT);  //WATER_SENSOR
  pinMode(BUZZER_PIN, OUTPUT);  //BUZZER
  pinMode(BUTTON_SW, INPUT);  //BUTTON
  lcd.init(); //LCD 초기화
  lcd.home();
  lcd.backlight();
  lcd.clear();
  pinMode(SERVO_MOTOR, OUTPUT); //SERVO MOTOR
  pinMode(PIR_PIN, INPUT);  //PIR
}

void loop() {
  uint16_t water_value = analogRead(WATER_SENSOR);
  water_value = constrain(water_value, 50, 700);  //최소값 50, 최대값 700
  
  delay(2000UL);
  if(digitalRead(BUTTON_SW)) {   //대기중
    if(!sw_state) {
      delay(10UL);
      sw_state = true;
    }
  }

  if(!digitalRead(BUTTON_SW)) {    //버튼 눌렀을때
    if(sw_state) {
      delay(10UL);
      sw_state = false;
      if(!fw_state) {    //침수 경보 시스템 작동
        Serial.println(F("침수 경보 시스템이 시작 됩니다."));
        Serial.println(F("물 감지가 시작 됩니다."));

        Serial1.println(F("침수 경보 시스템이 시작 됩니다."));
        Serial1.println(F("물 감지가 시작 됩니다."));
        lcd.setCursor(0, 0); //출력 찍어야할 위치
        lcd.clear();
        lcd.print("FW START");  //LCD 문구 출력
        fw_state = true;
      } else {     //침수상황 종료,중지 할 때
         for(i=0; i < 256; ++i) {
          analogWrite(SERVO_MOTOR, i);   //차단기 올라감  
          delay(10UL);
          }
        digitalWrite(RED, 0); // 0~ 255
        digitalWrite(GREEN, 0);
        digitalWrite(BLUE, 0);
        Serial.println(F("침수 경보 시스템이 중지 됩니다."));
        Serial.println(F("물 감지가 중지 됩니다."));
        Serial.println(F("차단기가 올라갑니다."));

        Serial1.println(F("침수 경보 시스템이 중지 됩니다."));
        Serial1.println(F("물 감지가 중지 됩니다."));
        Serial1.println(F("차단기가 올라갑니다."));
        lcd.setCursor(0, 0); //출력 찍어야할 위치
        lcd.clear();
        lcd.print("FW STOP");  //LCD 문구 출력
        fw_state = false;
      }
    }
  }
if(fw_state) {
  if(water_value >= 500) {    //물의 양이 500mm보다 크면 자동 경보 상황 시작
    if(digitalRead(PIR_PIN)) {    //침수 상황일때 차량 감지 될 시 블루투스로 정보 전송
      Serial1.println(F("침수 상황 중 차량이 감지됩니다. 119 신고 바랍니다."));
    }
    Serial.print(F("침수 경고!!"));
    Serial.println(F("차단기가 내려갑니다. 대피하십시오."));

    Serial1.print(F("침수 경고!!"));
    Serial1.print(F("water value : "));
    Serial1.println(String(water_value) + String("mm"));
    Serial1.println(F("차단기가 내려갑니다. 조치하십시오."));

    digitalWrite(RED, 0); // 0~ 255 RGB
    digitalWrite(GREEN, 0);
    digitalWrite(BLUE, 0);  
    delay(100UL);  
    digitalWrite(RED, 255); // 0~ 255
    digitalWrite(GREEN, 0);
    digitalWrite(BLUE, 0);

    tone(BUZZER_PIN, 100.2);  //경보음 삐이익 삐이익
    delay(500UL);
    noTone(BUZZER_PIN); 

    lcd.setCursor(0, 0); //출력 찍어야할 위치
    lcd.clear();
    lcd.print("Flood warning!!");  //LCD 문구 출력
    
    if(water_value >= 200) {
     for(i; i < 256; ++i) {
      analogWrite(SERVO_MOTOR, i);   //차단기 내려감  (다시올릴때는 i=0 값줘서 다시 올리기)
      delay(10UL);
      }
    } 
  } else {
    lcd.setCursor(0, 0); //출력 찍어야할 위치
    lcd.clear();
    lcd.print("FW START");

    digitalWrite(RED, 0); // 0~ 255
    digitalWrite(GREEN, 0);
    digitalWrite(BLUE, 0);

    noTone(BUZZER_PIN);
  }
}
  delay(100UL);
  Serial.print(F("water value : "));
  Serial.println(String(water_value) + String("mm"));

  Serial1.print(F("water value : "));
  Serial1.println(String(water_value) + String("mm"));
}
