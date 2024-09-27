#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

// WiFi 설정
const char* ssid = "U+NetDF25";
const char* password = "1C12040637";
const char* serverName = "http://192.168.219.102:5000/warning";

// 타임스탬프 생성 함수
String getTimeStamp() {
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return "No time";  // 시간 동기화 실패 시 기본값
    }
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(buffer);
}

void setup() {
    Serial.begin(115200);

    // WiFi 연결
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("WiFi 연결 중...");
    }
    Serial.println("WiFi 연결 완료");

    // NTP 서버 설정
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        http.begin(serverName); // 서버 경로 설정
        http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // POST 요청 헤더 설정

        String message = "도난이 감지되었습니다";  // 전송할 메시지
        String timestamp = getTimeStamp();  // 타임스탬프 생성

        // 메시지와 타임스탬프를 포함한 데이터 전송
        String postData = "message=" + message + "&timestamp=" + timestamp;
        
        int httpResponseCode = http.POST(postData);  // POST 요청 전송

        // 서버 응답 확인
        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println(httpResponseCode);
            Serial.println(response);
        } else {
            Serial.println("Error on HTTP request");
        }

        http.end(); // HTTP 연결 종료
    }

    delay(10000);  // 10초마다 반복
}
