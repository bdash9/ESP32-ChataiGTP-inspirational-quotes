#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Replace with your Wi-Fi network credentials
const char* ssid = "xxxxxx";
const char* password = "xxxxxxxxx";

// Replace with your OpenAI API key
const char* apiKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

// Create an instance of the ST7789 TFT display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

//#define ST77XX_TRANSPARENT 0xFFFF

void setup() {
  Serial.begin(115200);

  // turn on backlight
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // Initialize the TFT display
  tft.init(135, 240);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(3);

  tft.setTextColor(ST77XX_BLUE);
  tft.println();
  tft.println("Inspirational"); 
  tft.println("   ChatGPT"); 
  tft.println("   Quote");  
  delay(1000);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.println();
  tft.print("Connecting");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    tft.print(".");
  }
  // Print ESP Local IP Address
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(0,0);

  // Make a request to the OpenAI GPT API
  HTTPClient http;
  http.begin("https://api.openai.com/v1/engines/davinci/completions");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", String("Bearer ") + apiKey);
  String requestData = "{\"prompt\": \"Generate an inspirational quote about overcoming obstacles and achieving success. The quote should be no more than 15 words.\", \"max_tokens\": 15, \"temperature\": 0.5}";
  int httpResponseCode = http.POST(requestData);

if (httpResponseCode > 0) {
    // Parse the response as JSON
    const size_t bufferSize = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 1200;
    DynamicJsonDocument jsonDocument(bufferSize);
    DeserializationError error = deserializeJson(jsonDocument, http.getStream());
    
    if (error) {
        tft.print("Error parsing JSON: ");
        tft.println(error.c_str());
    } else if (!jsonDocument.containsKey("choices") || !jsonDocument["choices"][0].containsKey("text")) {
        tft.print("Error: Response format is incorrect");
    } else {
        // Extract the quote from the JSON object
        String quote = jsonDocument["choices"][0]["text"].as<String>();
        quote.trim();
      
        // Print the quote to the display
        tft.print(quote);
        delay(2000);
        
        // Get the height of the quote text
        int16_t x1, y1;
        uint16_t w, h;
        tft.getTextBounds(quote, 0, 0, &x1, &y1, &w, &h);
      
        // Set the text color to white and the background color to black
        tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
      
        // Print the quote, scrolling it up off the top of the screen
        int y = 0;
        while (y > -h) {
            tft.setCursor(0, y);
            tft.print(quote);
            y -= 2;
            delay(100);
        }
        
        // Clear the screen
        tft.fillScreen(ST77XX_BLACK);
    }
    http.end();
}
}
void loop() {
}
