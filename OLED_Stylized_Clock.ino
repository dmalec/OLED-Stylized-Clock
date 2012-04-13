// --------------------------------------------------------------------------------
// An Arduino sketch to render a stylized clock on an OLED.
//
// MIT license.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//
//      ******************************************************
//      Designed for the Adafruit Monochrome 128x64 OLED graphic display,
//      http://www.adafruit.com/products/326
//      and Adafruit DS1307 Real Time Clock breakout board kit
//      http://www.adafruit.com/products/264
//      ******************************************************
//
//
// --------------------------------------------------------------------------------
// Dependencies
// --------------------------------------------------------------------------------
// Adafruit Industries's core graphics library:
//       https://github.com/adafruit/Adafruit-GFX-Library
// Adafruit Industries's SSD1306 OLED driver library:
//       https://github.com/adafruit/Adafruit_SSD1306
// Adafruit Industries's DS1307 RTC library:
//       https://github.com/adafruit/RTClib
// --------------------------------------------------------------------------------
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

//! OLED display wired per tutorial.
#define OLED_DC 11
#define OLED_CS 12
#define OLED_CLK 10
#define OLED_MOSI 9
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

//! Real time clock.
RTC_DS1307 RTC;

//! Rectangles for clock display.
extern prog_uint8_t hour_rects[12][4];
extern prog_uint8_t minute_rects[60][4];
extern prog_uint8_t second_rects[60][4];

//! Store the last reading for conditional rendering of changed rectangles.
uint8_t last_hour = 0;
uint8_t last_minute = 0;
uint8_t last_second = 0;

//! Initialize the sketch.
void setup() {      
  // Initialize the serial connection for debugging.
  Serial.begin(9600);
  Serial.println(F("Serial initialized"));

  // Initialize the real time clock.
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  pinMode(17, OUTPUT);
  digitalWrite(17, HIGH);
  Wire.begin();
  RTC.begin();
 
  if (! RTC.isrunning()) {
    Serial.println(F("RTC is NOT running, starting"));
    RTC.adjust(DateTime(__DATE__, __TIME__));
  } else {
    Serial.println(F("RTC is running"));
  }
  
  // Initialize the OLED.
  display.begin(SSD1306_SWITCHCAPVCC);
  Serial.println(F("OLED initialized"));
  
  // Clear the display and draw hour/minute clock rectangles.
  display.clearDisplay();
  fill_all_rectangles(hour_rects, 12);
  fill_all_rectangles(minute_rects, 60);
  display.display();
  Serial.println(F("Clock rendered"));
}

//! Main execution loop.
void loop() {
  long start_time = millis();
  
  // Read the current time.
  DateTime now = RTC.now();
  uint8_t current_hour = now.hour() < 12 ? now.hour() : now.hour() - 12;
  uint8_t current_minute = now.minute();
  uint8_t current_second = now.second();
  boolean dirtyFlag = false;

  // If hours have changed, fill the rectangle for the old hour value and erase the rectangle for the new hour value.
  if (current_hour != last_hour) {
    fill_rect(hour_rects, last_hour, WHITE);
    fill_rect(hour_rects, current_hour, BLACK);
    last_hour = current_hour;
    dirtyFlag = true;
  }

  // If minutes have changed, fill the rectangle for the old minute value and erase the rectangle for the new minute value.
  if (current_minute != last_minute) {
    fill_rect(minute_rects, last_minute, WHITE);
    fill_rect(minute_rects, current_minute, BLACK);
    last_minute = current_minute;  
    dirtyFlag = true;
  }

  // If seconds have changed, erase the rectangle for the old second value and fill the rectangle for the new second value.
  if (current_second != last_second) {
    fill_rect(second_rects, last_second, BLACK);
    fill_rect(second_rects, current_second, WHITE);
    last_second = current_second;  
    dirtyFlag = true;
  }

  // If any changes were drawn, re-draw the display.
  if (dirtyFlag) {
    display.display();
  }
  
  delay(1000 - (millis() - start_time));
}

//! Fill all rectangles in an array.
/*!
  \param rectangles the array of rectangle values
  \param length the length of the array
*/
void fill_all_rectangles(prog_uint8_t rectangles[][4], uint8_t length) {
  for (int i=0; i<length; i++) {
    fill_rect(rectangles, i, WHITE);
  }
}

//! Fill a single rectangle with a specified color.
/*!
  \param rectangles the array of rectangle values
  \param i the index of the rectangle to fill
  \param color the color to fill the rectangle
*/
void fill_rect(prog_uint8_t rectangles[][4], int i, uint8_t color) {
  display.fillRect(pgm_read_byte_near(&rectangles[i][0]),
                   pgm_read_byte_near(&rectangles[i][1]),
                   pgm_read_byte_near(&rectangles[i][2]),
                   pgm_read_byte_near(&rectangles[i][3]),
                   color);
}  

