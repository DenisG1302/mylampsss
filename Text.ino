#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "Xiaomi_81A4";
const char* password = "9137864906";

ESP8266WebServer server(80);

#define PIN 15

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 16, PIN, NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);

int x = matrix.width();
String displayText = "Введите текст";
String tempText = "Введите текст";
uint16_t textColor = matrix.Color(255, 255, 255);
uint16_t bgColor = matrix.Color(0, 0, 0);
uint16_t htmlColorToMatrix(String htmlColor);
int y = 4;
int delays = 50;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
int mode = 0;
int ihue = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  server.on("/", handleRoot);
  server.on("/set", setParameters);
  server.on("/get", HTTP_GET, handleGetParameters);
  server.onNotFound(handleNotFound);

  server.begin();

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(30);
  matrix.setTextColor(textColor);
}
void random_burst();
void rainbow_fade();

void loop() {
  currentMillis = millis();
  server.handleClient();

  if (currentMillis - previousMillis >= (unsigned long)delays) {
    previousMillis = currentMillis;

    if (mode == 0) {
      matrix.fillScreen(bgColor);
      String modText = utf8rus(displayText);
      int textWidth = modText.length() * 6 + matrix.width();
      matrix.setCursor(x, y);
      matrix.setTextColor(textColor);
      matrix.print(modText);
      if (--x < -textWidth) {
        x = matrix.width();
      }
    } else if (mode == 1) {
      random_burst();

      // Добавление текста в mode 1
      String modText = utf8rus(displayText);
      int textWidth = modText.length() * 6 + matrix.width();
      matrix.setCursor(x, y);
      matrix.setTextColor(textColor);
      matrix.print(modText);
      if (--x < -textWidth) {
        x = matrix.width();
      }
    } else if (mode == 2) {
      rainbow_fade();

      String modText = utf8rus(displayText);
      int textWidth = modText.length() * 6 + matrix.width();
      matrix.setCursor(x, y);
      matrix.setTextColor(textColor);
      matrix.print(modText);
      if (--x < -textWidth) {
        x = matrix.width();
      }
    }
    matrix.show();
  }
}

void random_burst() {
  uint16_t idex = random(0, matrix.numPixels()); // Выберите случайный индекс пикселя
  uint8_t ihue = random(0, 255);                 // Выберите случайный оттенок цвета
  matrix.fillScreen(bgColor);                    // Очистите экран
  matrix.setPixelColor(idex, matrix.ColorHSV(ihue * 255, 255, 255)); // Установите цвет случайного пикселя
  matrix.show();                                // Обновите матрицу
}

void rainbow_fade() {                         //-m2-FADE ALL LEDS THROUGH HSV RAINBOW
  ihue++;
  if (ihue > 255) {
    ihue = 0;
  }
  for (int idex = 0 ; idex < matrix.numPixels(); idex++ ) {
    matrix.setPixelColor(idex, matrix.ColorHSV(ihue * 255, 255, 255));
  }
  matrix.show();
}

void handleGetParameters() {
  String json = "{";
  json += "\"text\":\"" + tempText + "\",";
  json += "\"textColor\":\"" + matrixColorToHtml(textColor) + "\",";
  json += "\"bgColor\":\"" + matrixColorToHtml(bgColor) + "\",";
  json += "\"y\":" + String(y) + ",";
  json += "\"delays\":" + String(delays) + ",";
  json += "\"mode\":" + String(mode);
  json += "}";
  server.send(200, "application/json", json);
}

String matrixColorToHtml(uint16_t color) {
  byte r = (color >> 11) * 255 / 31;
  byte g = ((color >> 5) & 0x3F) * 255 / 63;
  byte b = (color & 0x1F) * 255 / 31;
  char htmlColor[8];
  sprintf(htmlColor, "#%02X%02X%02X", r, g, b);
  return String(htmlColor);
}

void handleRoot() {
  String html = R"=====(
    <!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Matrix Control</title>
  <script>
    function updateMatrix() {
      var xhr = new XMLHttpRequest();
      xhr.open("POST", "/set", true);
      xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
      xhr.send(
        "text=" + encodeURIComponent(document.getElementById("text").value) +
        "&textColor=" + encodeURIComponent(document.getElementById("textColor").value) +
        "&bgColor=" + encodeURIComponent(document.getElementById("bgColor").value) +
        "&y=" + encodeURIComponent(document.getElementById("y").value) +
        "&delays=" + encodeURIComponent(document.getElementById("delays").value * 10) +
        "&mode=" + encodeURIComponent(document.getElementById("mode").value)
      );
    }

    function loadParameters() {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/get", true);
      xhr.onload = function() {
        if (xhr.status === 200) {
          var parameters = JSON.parse(xhr.responseText);
          document.getElementById("text").value = parameters.text;
          document.getElementById("textColor").value = parameters.textColor;
          document.getElementById("bgColor").value = parameters.bgColor;
          document.getElementById("y").value = parameters.y;
          document.getElementById("delays").value = parameters.delays/10;
          document.getElementById("mode").value = parameters.mode;
                } else {
          console.error("Failed to load parameters: " + xhr.statusText);
        }
      };
      xhr.onerror = function() {
        console.error("Request failed: " + xhr.statusText);
      };
      xhr.send();
    }
      function toggleText() {
        var textInput = document.getElementById("text");
        var textLabel = document.getElementById("textLabel");
        var textColorInput = document.getElementById("textColor");
        var textColorLabel = document.getElementById("textColorLabel");
        if (textInput.style.display === "none") {
          textInput.style.display = "inline";
          textLabel.style.display = "inline";
          textColorInput.style.display = "inline";
          textColorLabel.style.display = "inline";
          textInput.value = savedText;
          updateMatrix();
        } else {
          savedText = textInput.value;
          textInput.value = "";
          textInput.style.display = "none";
          textLabel.style.display = "none";
          textColorInput.style.display = "none";
          textColorLabel.style.display = "none";
          updateMatrix();
        }
      }
  </script>
</head>
<body onload="loadParameters()">
  <h1>Matrix Control</h1>
  <label for="mode">Режим</label>
  <select id="mode" name="mode" onchange="updateMatrix()" required>
    <option value="0">Статический фон с текстом</option>
    <option value="1">Цветные вспышки</option>
    <option value="2">Переливание цвета</option>
  </select>
  <br>
  <br>
  <label for="text" id="textLabel">Текст</label>
  <input type="text" id="text" name="text" oninput="updateMatrix()" required>
  <br>
  <label for="textColor" id="textColorLabel">Цвет текста</label>
  <input type="color" id="textColor" name="textColor" onchange="updateMatrix()" required>
  <br>
  <button onclick="toggleText()">Включить/Отключить текст</button>
  <br>
  <br>
  <label for="bgColor">Текст фона</label>
  <input type="color" id="bgColor" name="bgColor" onchange="updateMatrix()" required>
  <br>
  <br>
  <label for="y">Y расположение</label>
  <input type="number" id="y" name="y" min="-6" max="15" oninput="updateMatrix()" required>
  <br>
  <bt>
  <label for="delays">Скорость</label>
  <input type="range" id="delays" name="delays" min="0" max="100" step="1" onchange="updateMatrix()" required>
  <br>
  </body>
</html>
)=====";
server.send(200, "text/html", html);
}

void setParameters() {
if (server.hasArg("text")) {
displayText = server.arg("text");
tempText = displayText;
}
if (server.hasArg("textColor")) {
textColor = htmlColorToMatrix(server.arg("textColor"));
}
if (server.hasArg("bgColor")) {
bgColor = htmlColorToMatrix(server.arg("bgColor"));
}
if (server.hasArg("y")) {
y = server.arg("y").toInt();
}
if (server.hasArg("delays")) {
delays = server.arg("delays").toInt();
}
if (server.hasArg("mode")) {
  mode = server.arg("mode").toInt();
}
server.send(200, "text/plain", "OK");
}

void handleNotFound() {
server.send(404, "text/plain", "404: Not found");
}

uint16_t htmlColorToMatrix(String htmlColor) {
  long number = (long)strtol(&htmlColor[1], NULL, 16);
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;
  return matrix.Color(r, g, b);
}

String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xBF){
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x2F;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB7; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x6F;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
}


