#include <SPI.h>
#include <RF24.h>

// 핀 정의 (9 ~ 13번 GPIO 사용 안 함)
#define RED_PIN 8        // 빨간 LED (GPIO 8 사용)
#define GREEN_PIN 7      // 초록 LED (GPIO 7 사용)
#define BLUE_PIN 6       // 파란 LED (GPIO 6 사용)
#define VIBRATION_PIN 5  // 진동 모터 (GPIO 5 사용)
#define CE_PIN 15        // nRF24L01의 CE 핀 (GPIO 15 사용)
#define CSN_PIN 14       // nRF24L01의 CSN 핀 (GPIO 14 사용)

// SPI0 핀 설정 (MOSI: GPIO 3, MISO: GPIO 4, SCK: GPIO 2)
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
    radio.setPALevel(RF24_PA_MIN);  // 전력 레벨 설정
    radio.openReadingPipe(0, address);  // 수신 주소 설정
    radio.startListening();  // 수신 모드로 전환

    // 기본 상태로 파란색 LED 켜기
    digitalWrite(BLUE_PIN, HIGH);
}

void loop() {
    if (radio.available()) {
        char receivedMessage[32] = "";    // 수신 데이터 저장용 배열
        radio.read(&receivedMessage, sizeof(receivedMessage));  // 데이터 수신

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

// 경고 메시지에 따른 알림 반응
void alertResponse() {
    // 빨간색 LED 및 진동 모터 작동
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    digitalWrite(VIBRATION_PIN, HIGH);  // 진동 모터 작동
    delay(3000);  // 3초 동안 작동

    // 경고 후 기본 상태로 복구
    resetToStandby();
}

// 기본 상태로 복구
void resetToStandby() {
    // 파란색 LED 켜기 (기본 상태)
    digitalWrite(BLUE_PIN, HIGH);
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(VIBRATION_PIN, LOW);  // 진동 모터 끔
}
