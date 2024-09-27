//2아두이노 ,1 esp
#include <SPI.h>
#include <MFRC522.h>
#include <nRF24L01.h>
#include <RF24.h>

#define SS_PIN 10     // RFID 모듈의 SS (Slave Select) 핀
#define RST_PIN 9     // RFID 모듈의 RST (Reset) 핀
#define CE_PIN 8      // nRF24L01 CE 핀
#define CSN_PIN 7     // nRF24L01 CSN 핀
#define TRACKING_SENSOR_PIN 2  // 트레킹 센서 핀

MFRC522 mfrc522(SS_PIN, RST_PIN);  // MFRC522 인스턴스 생성
RF24 radio(CE_PIN, CSN_PIN);       // nRF24L01 인스턴스 생성

const byte address1[6] = "00001";  // 슬레이브 1 주소 (기존 기능)
const byte address2[6] = "00002";  // 슬레이브 2 주소 (도난 감지 메시지)

bool rfidTagged = false;            // RFID 태그 상태
bool motionHandled = false;         // 트래킹 센서 처리 상태
unsigned long motionStartTime = 0;  // 타이머 시작 시간 저장

void setup() {
  Serial.begin(9600);          // 시리얼 통신 시작
  SPI.begin();                 // SPI 통신 시작
  mfrc522.PCD_Init();          // RFID 모듈 초기화

  radio.begin();               // nRF24L01 초기화
  radio.setPALevel(RF24_PA_MIN);  // 낮은 전력 모드

  pinMode(TRACKING_SENSOR_PIN, INPUT);  // 트래킹 센서 핀 입력 모드 설정
}

void loop() {
  bool currentMotionState = digitalRead(TRACKING_SENSOR_PIN);  // 트래킹 센서 값 읽기

  // 검은 물체 감지 시 타이머 시작
  if (currentMotionState == LOW && !motionHandled) {
    Serial.println("검은 물체 감지됨! 3초 안에 RFID 태그 필요");
    motionStartTime = millis();  // 타이머 시작
    motionHandled = true;        // 감지 처리 시작
    rfidTagged = false;          // RFID 태그 초기화
  }

  // 3초 동안 RFID 태그가 감지되지 않으면 슬레이브 1과 2에게 경고 메시지 전송
  if (motionHandled && millis() - motionStartTime > 3000 && !rfidTagged) {
    Serial.println("3초 안에 RFID 태그 안됨, 슬레이브 1과 2에 경고 전송");

    // 슬레이브 1에게 경고 메시지 전송 (A 메시지)
    const char alertMessage[] = "A";
    radio.openWritingPipe(address1);  // 슬레이브 1로 전송
    radio.write(&alertMessage, sizeof(alertMessage));

    // 슬레이브 2에게 도난 경고 메시지 전송
    const char warningMessage[] = "도난이 감지되었습니다";
    radio.openWritingPipe(address2);  // 슬레이브 2로 전송
    radio.write(&warningMessage, sizeof(warningMessage));

    delay(1000);
    motionHandled = false;
  }

  // RFID 인식 시 슬레이브 1로 정상 메시지 전송
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("RFID 태그 인식됨. 접근 허가.");

    const char successMessage[] = "C";
    radio.openWritingPipe(address1);  // 슬레이브 1로 전송
    radio.write(&successMessage, sizeof(successMessage));

    rfidTagged = true;
    motionHandled = false;
    delay(1000);
    
    // RFID 통신 종료
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}
