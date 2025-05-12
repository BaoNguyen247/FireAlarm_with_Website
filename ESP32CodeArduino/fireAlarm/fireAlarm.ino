#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <DHT.h>

// Cấu hình WiFi
const char* ssid = "Redmi11";          
const char* password = "24702470";      
// Cấu hình Server
const char* serverAddress = "https://firewebsite-226337333515.us-central1.run.app";  // Địa chỉ HTTPS của máy chủ Flask

const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIJAjCCB+qgAwIBAgIQdRDXFH9DbloKXkCCju6anjANBgkqhkiG9w0BAQsFADA7\n" \
"MQswCQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZpY2VzMQww\n" \
"CgYDVQQDEwNXUjIwHhcNMjUwNDIxMDg0MDIzWhcNMjUwNzE0MDg0MDIyWjAWMRQw\n" \
"EgYDVQQDDAsqLmEucnVuLmFwcDBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABI+Q\n" \
"Ws9SFLagfJTIRRIXXCq6X9kubjGqK/3tsYqLv0K7r1cif65MquHmU9AjrEGIswl8\n" \
"5AKLvlyS+mH7pYi1i6WjggbwMIIG7DAOBgNVHQ8BAf8EBAMCB4AwEwYDVR0lBAww\n" \
"CgYIKwYBBQUHAwEwDAYDVR0TAQH/BAIwADAdBgNVHQ4EFgQUiajYNeZo3wRtuSK6\n" \
"fRS+UzjhR/8wHwYDVR0jBBgwFoAU3hse7XkV1D43JMMhu+w0OW1CsjAwWAYIKwYB\n" \
"BQUHAQEETDBKMCEGCCsGAQUFBzABhhVodHRwOi8vby5wa2kuZ29vZy93cjIwJQYI\n" \
"KwYBBQUHMAKGGWh0dHA6Ly9pLnBraS5nb29nL3dyMi5jcnQwggTHBgNVHREEggS+\n" \
"MIIEuoILKi5hLnJ1bi5hcHCCB3J1bi5hcHCCFyouYWZyaWNhLXNvdXRoMS5ydW4u\n" \
"YXBwghQqLmFzaWEtZWFzdDEucnVuLmFwcIIUKi5hc2lhLWVhc3QyLnJ1bi5hcHCC\n" \
"GSouYXNpYS1ub3J0aGVhc3QxLnJ1bi5hcHCCGSouYXNpYS1ub3J0aGVhc3QyLnJ1\n" \
"bi5hcHCCGSouYXNpYS1ub3J0aGVhc3QzLnJ1bi5hcHCCFSouYXNpYS1zb3V0aDEu\n" \
"cnVuLmFwcIIVKi5hc2lhLXNvdXRoMi5ydW4uYXBwghkqLmFzaWEtc291dGhlYXN0\n" \
"MS5ydW4uYXBwghkqLmFzaWEtc291dGhlYXN0Mi5ydW4uYXBwghkqLmFzaWEtc291\n" \
"dGhlYXN0My5ydW4uYXBwgh4qLmF1c3RyYWxpYS1zb3V0aGVhc3QxLnJ1bi5hcHCC\n" \
"HiouYXVzdHJhbGlhLXNvdXRoZWFzdDIucnVuLmFwcIIZKi5ldXJvcGUtY2VudHJh\n" \
"bDIucnVuLmFwcIIXKi5ldXJvcGUtbm9ydGgxLnJ1bi5hcHCCFyouZXVyb3BlLW5v\n" \
"cnRoMi5ydW4uYXBwghsqLmV1cm9wZS1zb3V0aHdlc3QxLnJ1bi5hcHCCFiouZXVy\n" \
"b3BlLXdlc3QxLnJ1bi5hcHCCFyouZXVyb3BlLXdlc3QxMC5ydW4uYXBwghcqLmV1\n" \
"cm9wZS13ZXN0MTIucnVuLmFwcIIXKi5ldXJvcGUtd2VzdDE1LnJ1bi5hcHCCFiou\n" \
"ZXVyb3BlLXdlc3QyLnJ1bi5hcHCCFiouZXVyb3BlLXdlc3QzLnJ1bi5hcHCCFiou\n" \
"ZXVyb3BlLXdlc3Q0LnJ1bi5hcHCCFiouZXVyb3BlLXdlc3Q1LnJ1bi5hcHCCFiou\n" \
"ZXVyb3BlLXdlc3Q2LnJ1bi5hcHCCFiouZXVyb3BlLXdlc3Q4LnJ1bi5hcHCCFiou\n" \
"ZXVyb3BlLXdlc3Q5LnJ1bi5hcHCCFSoubWUtY2VudHJhbDEucnVuLmFwcIIVKi5t\n" \
"ZS1jZW50cmFsMi5ydW4uYXBwghIqLm1lLXdlc3QxLnJ1bi5hcHCCISoubm9ydGhh\n" \
"bWVyaWNhLW5vcnRoZWFzdDEucnVuLmFwcIIhKi5ub3J0aGFtZXJpY2Etbm9ydGhl\n" \
"YXN0Mi5ydW4uYXBwgh0qLm5vcnRoYW1lcmljYS1zb3V0aDEucnVuLmFwcIIcKi5z\n" \
"b3V0aGFtZXJpY2EtZWFzdDEucnVuLmFwcIIcKi5zb3V0aGFtZXJpY2Etd2VzdDEu\n" \
"cnVuLmFwcIIVKi51cy1jZW50cmFsMS5ydW4uYXBwghUqLnVzLWNlbnRyYWwyLnJ1\n" \
"bi5hcHCCEioudXMtZWFzdDEucnVuLmFwcIISKi51cy1lYXN0NC5ydW4uYXBwghIq\n" \
"LnVzLWVhc3Q1LnJ1bi5hcHCCEioudXMtZWFzdDcucnVuLmFwcIITKi51cy1zb3V0\n" \
"aDEucnVuLmFwcIISKi51cy13ZXN0MS5ydW4uYXBwghIqLnVzLXdlc3QyLnJ1bi5h\n" \
"cHCCEioudXMtd2VzdDMucnVuLmFwcIISKi51cy13ZXN0NC5ydW4uYXBwghIqLnVz\n" \
"LXdlc3Q4LnJ1bi5hcHAwEwYDVR0gBAwwCjAIBgZngQwBAgEwNgYDVR0fBC8wLTAr\n" \
"oCmgJ4YlaHR0cDovL2MucGtpLmdvb2cvd3IyLzlVVmJOMHc1RTZZLmNybDCCAQMG\n" \
"CisGAQQB1nkCBAIEgfQEgfEA7wB1ABLxTjS9U3JMhAYZw48/ehP457Vih4icbTAF\n" \
"hOvlhiY6AAABlle4aQcAAAQDAEYwRAIgfXQ+Smx5wlmny1CwKeTCKsTKZDPcncKg\n" \
"APjdHtnFwtUCIGOkoUxHCCiRGhJZoHvsgHyl30ZT67P19GRj3NtBoo4KAHYArxga\n" \
"KNaMo+CpikycZ6sJ+Lu8IrquvLE4o6Gd0/m2Aw0AAAGWV7hvVgAABAMARzBFAiEA\n" \
"xBAeR5nud9gjWTAwvtmb+c5QHmV1ix/U1fbiJGbEit8CIHECNDhr/srkDhrDbxta\n" \
"yK3+Wsq/xxDu9BS1Axrn8mNzMA0GCSqGSIb3DQEBCwUAA4IBAQAKE2OkM0HRqybV\n" \
"H4Mpj+dr55zZLK3xcaUfhkVGNx+j9zKsxX8p76iKDROgxOZBrH8RhynNUQo3pav6\n" \
"t0VXXZKlqfNF/VQaO950bj3OubRPKGPuUkYB/a33lQ8a3NOWS+kBsxyJxH3O6rxl\n" \
"dEG2BftqQyckYKI7cMiIHHpR/Qh8TKgNsIyKrBcDiQ7R0zIFu8BY0+KBUlfF2+ma\n" \
"e2ylzpaCIpfg5wFXxVxILmhJ1C47vNk7mhrfGYrhfZXYwVTBSCcqdswTfaOrpxy8\n" \
"mKgy8SdHJ/R9zUHK3bWATc3WCU+ftbvQLtWfhdW3La5ssRmJvZtaeW9NYSwFyLyc\n" \
"dSNRxT/r\n" \
"-----END CERTIFICATE-----\n" \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFCzCCAvOgAwIBAgIQf/AFoHxM3tEArZ1mpRB7mDANBgkqhkiG9w0BAQsFADBH\n" \
"MQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExM\n" \
"QzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMjMxMjEzMDkwMDAwWhcNMjkwMjIw\n" \
"MTQwMDAwWjA7MQswCQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNl\n" \
"cnZpY2VzMQwwCgYDVQQDEwNXUjIwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n" \
"AoIBAQCp/5x/RR5wqFOfytnlDd5GV1d9vI+aWqxG8YSau5HbyfsvAfuSCQAWXqAc\n" \
"+MGr+XgvSszYhaLYWTwO0xj7sfUkDSbutltkdnwUxy96zqhMt/TZCPzfhyM1IKji\n" \
"aeKMTj+xWfpgoh6zySBTGYLKNlNtYE3pAJH8do1cCA8Kwtzxc2vFE24KT3rC8gIc\n" \
"LrRjg9ox9i11MLL7q8Ju26nADrn5Z9TDJVd06wW06Y613ijNzHoU5HEDy01hLmFX\n" \
"xRmpC5iEGuh5KdmyjS//V2pm4M6rlagplmNwEmceOuHbsCFx13ye/aoXbv4r+zgX\n" \
"FNFmp6+atXDMyGOBOozAKql2N87jAgMBAAGjgf4wgfswDgYDVR0PAQH/BAQDAgGG\n" \
"MB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjASBgNVHRMBAf8ECDAGAQH/\n" \
"AgEAMB0GA1UdDgQWBBTeGx7teRXUPjckwyG77DQ5bUKyMDAfBgNVHSMEGDAWgBTk\n" \
"rysmcRorSCeFL1JmLO/wiRNxPjA0BggrBgEFBQcBAQQoMCYwJAYIKwYBBQUHMAKG\n" \
"GGh0dHA6Ly9pLnBraS5nb29nL3IxLmNydDArBgNVHR8EJDAiMCCgHqAchhpodHRw\n" \
"Oi8vYy5wa2kuZ29vZy9yL3IxLmNybDATBgNVHSAEDDAKMAgGBmeBDAECATANBgkq\n" \
"hkiG9w0BAQsFAAOCAgEARXWL5R87RBOWGqtY8TXJbz3S0DNKhjO6V1FP7sQ02hYS\n" \
"TL8Tnw3UVOlIecAwPJQl8hr0ujKUtjNyC4XuCRElNJThb0Lbgpt7fyqaqf9/qdLe\n" \
"SiDLs/sDA7j4BwXaWZIvGEaYzq9yviQmsR4ATb0IrZNBRAq7x9UBhb+TV+PfdBJT\n" \
"DhEl05vc3ssnbrPCuTNiOcLgNeFbpwkuGcuRKnZc8d/KI4RApW//mkHgte8y0YWu\n" \
"ryUJ8GLFbsLIbjL9uNrizkqRSvOFVU6xddZIMy9vhNkSXJ/UcZhjJY1pXAprffJB\n" \
"vei7j+Qi151lRehMCofa6WBmiA4fx+FOVsV2/7R6V2nyAiIJJkEd2nSi5SnzxJrl\n" \
"Xdaqev3htytmOPvoKWa676ATL/hzfvDaQBEcXd2Ppvy+275W+DKcH0FBbX62xevG\n" \
"iza3F4ydzxl6NJ8hk8R+dDXSqv1MbRT1ybB5W0k8878XSOjvmiYTDIfyc9acxVJr\n" \
"Y/cykHipa+te1pOhv7wYPYtZ9orGBV5SGOJm4NrB3K1aJar0RfzxC3ikr7Dyc6Qw\n" \
"qDTBU39CluVIQeuQRgwG3MuSxl7zRERDRilGoKb8uY45JzmxWuKxrfwT/478JuHU\n" \
"/oTxUFqOl2stKnn7QGTq8z29W+GgBLCXSBxC9epaHM0myFH/FJlniXJfHeytWt0=\n" \
"-----END CERTIFICATE-----\n" \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFVzCCAz+gAwIBAgINAgPlk28xsBNJiGuiFzANBgkqhkiG9w0BAQwFADBHMQsw\n" \
"CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n" \
"MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw\n" \
"MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n" \
"Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwggIiMA0GCSqGSIb3DQEBAQUA\n" \
"A4ICDwAwggIKAoICAQC2EQKLHuOhd5s73L+UPreVp0A8of2C+X0yBoJx9vaMf/vo\n" \
"27xqLpeXo4xL+Sv2sfnOhB2x+cWX3u+58qPpvBKJXqeqUqv4IyfLpLGcY9vXmX7w\n" \
"Cl7raKb0xlpHDU0QM+NOsROjyBhsS+z8CZDfnWQpJSMHobTSPS5g4M/SCYe7zUjw\n" \
"TcLCeoiKu7rPWRnWr4+wB7CeMfGCwcDfLqZtbBkOtdh+JhpFAz2weaSUKK0Pfybl\n" \
"qAj+lug8aJRT7oM6iCsVlgmy4HqMLnXWnOunVmSPlk9orj2XwoSPwLxAwAtcvfaH\n" \
"szVsrBhQf4TgTM2S0yDpM7xSma8ytSmzJSq0SPly4cpk9+aCEI3oncKKiPo4Zor8\n" \
"Y/kB+Xj9e1x3+naH+uzfsQ55lVe0vSbv1gHR6xYKu44LtcXFilWr06zqkUspzBmk\n" \
"MiVOKvFlRNACzqrOSbTqn3yDsEB750Orp2yjj32JgfpMpf/VjsPOS+C12LOORc92\n" \
"wO1AK/1TD7Cn1TsNsYqiA94xrcx36m97PtbfkSIS5r762DL8EGMUUXLeXdYWk70p\n" \
"aDPvOmbsB4om3xPXV2V4J95eSRQAogB/mqghtqmxlbCluQ0WEdrHbEg8QOB+DVrN\n" \
"VjzRlwW5y0vtOUucxD/SVRNuJLDWcfr0wbrM7Rv1/oFB2ACYPTrIrnqYNxgFlQID\n" \
"AQABo0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4E\n" \
"FgQU5K8rJnEaK0gnhS9SZizv8IkTcT4wDQYJKoZIhvcNAQEMBQADggIBAJ+qQibb\n" \
"C5u+/x6Wki4+omVKapi6Ist9wTrYggoGxval3sBOh2Z5ofmmWJyq+bXmYOfg6LEe\n" \
"QkEzCzc9zolwFcq1JKjPa7XSQCGYzyI0zzvFIoTgxQ6KfF2I5DUkzps+GlQebtuy\n" \
"h6f88/qBVRRiClmpIgUxPoLW7ttXNLwzldMXG+gnoot7TiYaelpkttGsN/H9oPM4\n" \
"7HLwEXWdyzRSjeZ2axfG34arJ45JK3VmgRAhpuo+9K4l/3wV3s6MJT/KYnAK9y8J\n" \
"ZgfIPxz88NtFMN9iiMG1D53Dn0reWVlHxYciNuaCp+0KueIHoI17eko8cdLiA6Ef\n" \
"MgfdG+RCzgwARWGAtQsgWSl4vflVy2PFPEz0tv/bal8xa5meLMFrUKTX5hgUvYU/\n" \
"Z6tGn6D/Qqc6f1zLXbBwHSs09dR2CQzreExZBfMzQsNhFRAbd03OIozUhfJFfbdT\n" \
"6u9AWpQKXCBfTkBdYiJ23//OYb2MI3jSNwLgjt7RETeJ9r/tSQdirpLsQBqvFAnZ\n" \
"0E6yove+7u7Y/9waLd64NnHi/Hm3lCXRSHNboTXns5lndcEZOitHTtNCjv0xyBZm\n" \
"2tIMPNuzjsmhDYAPexZ3FL//2wmUspO8IFgV6dtxQ/PeEMMA3KgqlbbC1j+Qa3bb\n" \
"bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c\n" \
"-----END CERTIFICATE-----\n";

// Cấu hình cảm biến
#define DHT_PIN 26       // Chân GPIO 26 cho cảm biến DHT11
#define MQ2_PIN 25       // Chân GPIO 25 cho cảm biến MQ-2

// Khởi tạo cảm biến DHT11
DHT dht(DHT_PIN, DHT11);

void setup() {
  Serial.begin(115200);
  
  // Khởi tạo cảm biến
  dht.begin();
  pinMode(MQ2_PIN, INPUT); // Cấu hình chân MQ-2 là input
  
  // Kết nối Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Đang kết nối WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nĐã kết nối WiFi");
  Serial.print("Địa chỉ IP của ESP32: ");
  Serial.println(WiFi.localIP());
}

void sendDataToServer(String status) {
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client->setCACert(rootCACertificate);
    
    HTTPClient https;
    String url = String(serverAddress) + "/data"; // Thêm endpoint /data
    
    // Tạo JSON payload
    DynamicJsonDocument doc(1024);
    doc["status"] = status;
    String payload;
    serializeJson(doc, payload);
    
    Serial.print("Đang gửi dữ liệu đến: ");
    Serial.println(url);
    Serial.print("Payload: ");
    Serial.println(payload);
    
    if (https.begin(*client, url)) {
      https.addHeader("Content-Type", "application/json");
      
      int httpCode = https.POST(payload);
      Serial.print("HTTP Response code: ");
      Serial.println(httpCode);
      
      if (httpCode > 0) {
        String response = https.getString();
        Serial.println("Response: " + response);
      } else {
        Serial.print("Lỗi khi gửi request: ");
        Serial.println(https.errorToString(httpCode));
      }
      
      https.end();
    } else {
      Serial.println("Không thể kết nối đến server");
    }
    
    delete client;
  } else {
    Serial.println("Không thể tạo WiFiClientSecure");
  }
}

String determineStatus(float temperature, float humidity, int mq2Value) {
  // Ngưỡng nhiệt độ (DHT11, > 40°C là nguy hiểm)
  const float TEMP_THRESHOLD = 40.0;
  // Ngưỡng độ ẩm (DHT11, < 20% là nguy hiểm, tùy môi trường)
  const float HUMIDITY_THRESHOLD = 20.0;
  // Ngưỡng MQ-2 (giá trị analog, cần hiệu chuẩn, ví dụ: > 300 là có gas)
  const int MQ2_THRESHOLD = 300;

  Serial.print("Nhiệt độ: "); Serial.print(temperature); Serial.println("°C");
  Serial.print("Độ ẩm: "); Serial.print(humidity); Serial.println("%");
  Serial.print("MQ-2: "); Serial.println(mq2Value);

  // Quyết định trạng thái dựa trên ngưỡng
  if (temperature > TEMP_THRESHOLD || humidity < HUMIDITY_THRESHOLD || mq2Value > MQ2_THRESHOLD) {
    return "fire";
  } else {
    return "normal";
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Đọc dữ liệu từ cảm biến DHT11
    float temperature = dht.readTemperature(); // Nhiệt độ (°C)
    float humidity = dht.readHumidity();       // Độ ẩm (%)
    int mq2Value = analogRead(MQ2_PIN);       // Giá trị analog từ MQ-2

    // Kiểm tra nếu đọc dữ liệu thành công
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Lỗi đọc dữ liệu từ DHT11!");
      delay(2000);
      return;
    }

    // Xác định trạng thái
    String status = determineStatus(temperature, humidity, mq2Value);

    // Gửi dữ liệu lên server
    sendDataToServer(status);
  } else {
    Serial.println("Mất kết nối WiFi, đang thử kết nối lại...");
    WiFi.begin(ssid, password);
  }
  
  delay(10000); // Gửi dữ liệu mỗi 10 giây
}