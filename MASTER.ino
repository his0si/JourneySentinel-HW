#include <SPI.h>
#include <MFRC522.h>
#include <nRF24L01.h>
#include <RF24.h>


#define SS_PIN 10     // RFID 모듈의 SS (Slave Select) 핀
#define RST_PIN 9     // RFID 모듈의 RST (Reset) 핀
#define CE_PIN 8      // nRF24L01 CE 핀
#define CSN_PIN 7     // nRF24L01 CSN 핀
#define TRACKING_SENSOR_PIN 2  // 트레킹 센서 핀 or aka 라인트레이싱 

MFRC522 mfrc522(SS_PIN, RST_PIN);  // MFRC522 인스턴스 생성
RF24 radio(CE_PIN, CSN_PIN);       // nRF24L01 인스턴스 생성

const byte address[6] = "00001";    // nRF24 주소
bool rfidTagged = false;            // RFID 태그 상태
bool motionHandled = false;         // 트래킹 센서 처리 상태
unsigned long motionStartTime = 0;  // 타이머 시작 시간 저장

void setup() {
  Serial.begin(9600);          // 시리얼 통신 시작
  Serial.println("시리얼 통신 시작");
  
  SPI.begin();                 // SPI 통신 시작
  Serial.println("SPI 통신 시작");

  mfrc522.PCD_Init();          // RFID 모듈 초기화
  Serial.println("RFID 모듈 초기화 완료");

  radio.begin();               // nRF24L01 초기화
  Serial.println("nRF24L01 초기화 완료");
  //아래3개 radio는 필수선언요소
  radio.openWritingPipe(address); // 송신 주소 설정
  radio.setPALevel(RF24_PA_MIN);  // 낮은 전력 모드, 장거리시 ex)PA_MAX등으로 선언, 바이패스 커패시터 납떔한후라 변경가능
  radio.stopListening();        // 송신 모드로 설정
  Serial.println("nRF24L01 송신 모드 설정 완료");

  pinMode(TRACKING_SENSOR_PIN, INPUT);  // 트래킹 센서 핀 입력 모드 설정(적외선센서?, 라인트레이싱모듈?)
  Serial.println("트래킹 센서 설정 완료");
  
  Serial.println("카드를 리더에 가까이 가져다 대세요...");
}
//주의! , RFID나 nRF둘다 하드웨어SPI를 사용중이므로 동시에 사용 절대 불가!!
void loop() {
  // 트래킹 센서 상태 확인
  bool currentMotionState = digitalRead(TRACKING_SENSOR_PIN);  // 트래킹 센서 값 읽기

  // 검은 물체 감지 시 타이머 시작
  if (currentMotionState == LOW && !motionHandled) {
    Serial.println("검은 물체 감지됨! 3초 안에 RFID 태그 필요");
    motionStartTime = millis();  // 타이머 시작
    motionHandled = true;        // 감지 처리 시작
    rfidTagged = false;          // RFID 태그 초기화
  }

  // 3초 동안 RFID 태그가 감지되지 않으면 경고 메시지 전송
  if (motionHandled && millis() - motionStartTime > 3000 && !rfidTagged) {
    Serial.println("3초 안에 RFID 태그 안됨, 경고 전송");

    // nRF24로 슬레이브에게 경고 메시지 전송
    const char warningMessage[] = "A";  // 경고 메시지
    radio.write(&warningMessage, sizeof(warningMessage));
    
    delay(1000);  // 경고 전송 후 대기

    motionHandled = false;  // 처리 상태 초기화
  }

  // RFID 인식 시도
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // RFID 태그가 감지되면
    Serial.println("RFID 태그 인식됨. 접근 허가.");

    // 슬레이브에 정상 메시지 전송
    const char successMessage[] = "C";
    radio.write(&successMessage, sizeof(successMessage));

    rfidTagged = true;  // RFID 태그 상태를 true로 설정
    motionHandled = false;  // 트래킹 센서 상태 초기화
    delay(1000);  // 처리 후 대기

    // RFID 통신 종료
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}
