#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Пины для управления (настройте под вашу схему) GPIO
const int azDirPin = 5;
const int azStepPin = 4;
const int altDirPin = 0;
const int altStepPin = 2;

// Настройки WiFi
const char* ssid = "Kroks_152";         // наименование сети wi-fi
const char* password = "19734682Spg!";  // пароль сети wi-fi

// Настройки Stellarium
WiFiServer stellariumServer(10001);     //port
WiFiClient stellariumClient;

// Текущие координаты
float currentAzimuth = 0;
float currentAltitude = 0;
bool newDataAvailable = false;

void setup() {
  Serial.begin(115200);

  // Настройка пинов
  pinMode(azDirPin, OUTPUT);
  pinMode(azStepPin, OUTPUT);
  pinMode(altDirPin, OUTPUT);
  pinMode(altStepPin, OUTPUT);

  // Подключение к WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");                                           // ПРИНТ после настройки закомментировать

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");                                                          // ПРИНТ после настройки закомментировать
  }

  Serial.println("\nConnected! IP address: ");    // server 192.168.1.116       // ПРИНТ после настройки закомментировать
  Serial.println(WiFi.localIP());                                               // ПРИНТ после настройки закомментировать

  // Запуск сервера для Stellarium
  stellariumServer.begin();
  Serial.println("Stellarium server started on port 10001");                    // ПРИНТ после настройки закомментировать
}

void loop() {
  handleStellariumConnection();

  if (newDataAvailable) {
    processNewCoordinates();
    newDataAvailable = false;
  }

  // Дополнительные задачи могут быть здесь
  delay(10);
}

void handleStellariumConnection() {
  if (!stellariumClient || !stellariumClient.connected()) {
    stellariumClient = stellariumServer.available();
    if (stellariumClient) {
      Serial.println("Stellarium connected!");                                  // ПРИНТ после настройки закомментировать
    }
  }

  if (stellariumClient && stellariumClient.available()) {
    String message = stellariumClient.readStringUntil('\n');
    message.trim();

    if (parseCoordinates(message)) {
      newDataAvailable = true;
      // Подтверждение получения
      stellariumClient.println("OK");
    }
  }
}

bool parseCoordinates(String data) {                    // ВОТ ТУТ ПРОБЛЕМА с выводом формата
  // Различные форматы от Stellarium
  if (data.startsWith("AZALT:")) {       // Формат: AZALT:123.45,67.89
    int commaPos = data.indexOf(',');
    if (commaPos != -1) {
      String azStr = data.substring(6, commaPos);
      String altStr = data.substring(commaPos + 1);

      currentAzimuth = azStr.toFloat();
      currentAltitude = altStr.toFloat();
      return true;
    }
  }
  else if (data.indexOf("AZ:") != -1 && data.indexOf("ALT:") != -1) {
                                        // Формат: AZ: 123.45 ALT: 67.89
    int azStart = data.indexOf("AZ:") + 3;
    int altStart = data.indexOf("ALT:") + 4;

    String azStr = data.substring(azStart, data.indexOf(' ', azStart));
    String altStr = data.substring(altStart);

    azStr.trim();
    altStr.trim();

    currentAzimuth = azStr.toFloat();
    Serial.print(currentAzimuth);                                                // ПРИНТ после настройки закомментировать
    currentAltitude = altStr.toFloat();
    Serial.print(currentAltitude);                                                // ПРИНТ после настройки закомментировать
    return true;
  }
  else if (data.startsWith("{")) {
                                          // JSON формат: {"azimuth": 123.45, "altitude": 67.89}
    return parseJSONCoordinates(data);
  }

  Serial.println("Unknown format: " + data);                                    // ПРИНТ после настройки закомментировать
  return false;

                  //  Stellarium может отправлять данные в нескольких форматах:

                  //  -Telescope Control Protocol: AZALT:123.45,67.89
                  //  -Custom format: AZ: 123.45 ALT: 67.89
                  //  -JSON: {"azimuth": 123.45, "altitude": 67.89}
}

bool parseJSONCoordinates(String json) {
  // Простой парсинг JSON
  int azStart = json.indexOf("\"azimuth\":");
  int altStart = json.indexOf("\"altitude\":");

  if (azStart != -1 && altStart != -1) {
    azStart += 9; // Длина "\"azimuth\":"
    String azStr = json.substring(azStart, json.indexOf(',', azStart));
    Serial.print(azStr);                                                // ПРИНТ после настройки закомментировать
    azStr.trim();

    altStart += 10; // Длина "\"altitude\":"
    String altStr = json.substring(altStart, json.indexOf('}', altStart));
    Serial.print(altStr);                                              // ПРИНТ после настройки закомментировать
    altStr.trim();

    currentAzimuth = azStr.toFloat();
    currentAltitude = altStr.toFloat();
    return true;
  }
  return false;
}

void processNewCoordinates() {
  Serial.print("New coordinates - Azimuth: ");                                  // ПРИНТ после настройки закомментировать
  Serial.print(currentAzimuth);                                                 // ПРИНТ после настройки закомментировать
  Serial.print("°, Altitude: ");                                                // ПРИНТ после настройки закомментировать
  Serial.print(currentAltitude);                                                // ПРИНТ после настройки закомментировать
  Serial.println("°");                                                          // ПРИНТ после настройки закомментировать

  // Проверка безопасности
  if (!validateCoordinates(currentAzimuth, currentAltitude)) {
    Serial.println("Coordinates validation failed!");                           // ПРИНТ после настройки закомментировать
    return;
  }

  // Движение к новым координатам
  moveToCoordinates(currentAzimuth, currentAltitude);
}

bool validateCoordinates(float az, float alt) {
  // Проверка допустимых значений
  if (alt < 0 || alt > 90) {
    Serial.println("Altitude out of range (0-90°)");
    return false;
  }

  if (az < 0 || az >= 360) {
    Serial.println("Azimuth out of range (0-360°)");
    return false;
  }

  return true;
}

void moveToCoordinates(float targetAz, float targetAlt) {
  // Здесь код управления шаговыми двигателями
  // Пример простой реализации:

  Serial.println("Moving telescope...");                                        // ПРИНТ после настройки закомментировать

  // Преобразование координат в шаги
  int azSteps = calculateAzimuthSteps(targetAz);
  int altSteps = calculateAltitudeSteps(targetAlt);

  // Выполнение движения
  moveAzimuth(azSteps);
  moveAltitude(altSteps);

  Serial.println("Movement completed");                                         // ПРИНТ после настройки закомментировать
}

int calculateAzimuthSteps(float azimuth) {
  // Настройте под ваш редуктор (шагов на градус)
  const int stepsPerDegree = 100;
  return (int)(azimuth * stepsPerDegree);
}

int calculateAltitudeSteps(float altitude) {
  const int stepsPerDegree = 100;
  return (int)(altitude * stepsPerDegree);
}

void moveAzimuth(int steps) {
  // Определение направления
  digitalWrite(azDirPin, steps >= 0 ? HIGH : LOW);

  // Выполнение шагов
  steps = abs(steps);
  for (int i = 0; i < steps; i++) {
    digitalWrite(azStepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(azStepPin, LOW);
    delayMicroseconds(500);
  }
}

void moveAltitude(int steps) {
  digitalWrite(altDirPin, steps >= 0 ? HIGH : LOW);

  steps = abs(steps);
  for (int i = 0; i < steps; i++) {
    digitalWrite(altStepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(altStepPin, LOW);
    delayMicroseconds(500);
  }
}