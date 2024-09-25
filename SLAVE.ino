#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//LED는 굳이 3색 RGB를 써야할까 의문
#define RED_PIN 8        // 아두이노 우노의 D8 핀에 빨간 LED 연결
#define GREEN_PIN 7      // 아두이노 우노의 D7 핀에 초록 LED 연결
#define BLUE_PIN 6       // 아두이노 우노의 D6 핀에 파란 LED 연결
#define VIBRATION_PIN 5  // 진동 모터를 D5 핀에 연결
#define CE_PIN 9         // nRF24L01의 CE 핀
#define CSN_PIN 10       // nRF24L01의 CSN 핀

RF24 radio(CE_PIN, CSN_PIN);  // nRF24L01 모듈 설정

const byte address[6] = "00001";  // 마스터와 동일한 주소 


bool isAlert = false;

void setup() {
    // LED 핀 설정
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    
    // 진동 모터 핀 설정
    pinMode(VIBRATION_PIN, OUTPUT);

    // nRF24L01 초기화
    radio.begin();
    radio.openReadingPipe(0, address);  // 수신 주소 설정
    radio.setPALevel(RF24_PA_MIN);      // 근거리 통신을 위한 최소 출력
    radio.startListening();             // 수신 모드로 전환

    // 기본 상태로 파란색 LED 켜기
    digitalWrite(BLUE_PIN, HIGH);
}

void loop() {
    if (radio.available()) {
        char receivedMessage[32] = "";    // 수신 데이터 저장용 배열
        radio.read(&receivedMessage, sizeof(receivedMessage)); // 데이터 수신

        if (strcmp(receivedMessage, "A") == 0) {
            // 경고 메시지 수신 시 (진동 및 빨간 LED)
            isAlert = true;
            alertResponse();  // 경고 처리 함수 호출
        }
        else if (strcmp(receivedMessage, "C") == 0) {
            // 정상 메시지 수신 시 (파란 LED로 복구)
            isAlert = false;
            resetToStandby();  // 기본 상태로 복구 함수 호출
        }
    }
}
//즉 마스터에서 경고 신호를 받으면 빨간색 led와 진동을 3초 방송 
void alertResponse() {
    // 빨간색 LED 및 진동 모터 작동
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    analogWrite(VIBRATION_PIN, 255);  // 진동 모터 작동
    delay(3000);                      // 3초 동안 작동

    // 경고 후 기본 상태로 복구
    resetToStandby();
}
//디폴트 상태를 함수로 선언
void resetToStandby() {
    // 파란색 LED 켜기 (기본 상태)
    digitalWrite(BLUE_PIN, HIGH);
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    analogWrite(VIBRATION_PIN, 0);  // 진동 모터 끔
}
