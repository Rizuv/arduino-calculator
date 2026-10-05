#include <Arduino.h>
#include <time.h>
#include <Wire.h>
#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

unsigned long last_display_update = 0;
const int INPUT_PIN = A0;
const int INPUT_PIN_2 = A2;
const int DIGITAL_SWITCH = 13;
const int debounce_delay = 50;
unsigned long last_debounce = 0;

int last_read_values[] = {-1, -1};
int previous_switch_read = LOW;
bool is_second_set = false;

char sets[2][5] = {
  {'0', '1', '2', '3', '4'},
  {'5', '6', '7', '8', '9'}
};

struct AnalogRange
{
  int min_value;
  int max_value;
  int index;
};

struct OperatorRange
{
  int min_value;
  int max_value;
  char type;
};

AnalogRange ranges[] = {
  {1010, 1023, 0}, {500, 520, 1}, {980, 1000, 2}, {895, 905, 3}, {150, 170, 4}
};

OperatorRange operator_ranges[] = {
  {980, 1023, '+'}, {670, 710, '-'}, {140, 180, '/'}, {895, 905, '*'}
};

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(INPUT_PIN, INPUT);
  Serial.begin(9600);
  delay(250);

  u8g2.begin();
  
}


void updateDisplay(){
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr); 
    u8g2.drawStr(0, 24, "");
    u8g2.drawStr(0, 50, is_second_set ? "Set: 5-9" : "Set: 0-4"); 
  } while ( u8g2.nextPage() );
}

void loop()
{

  int analog_read_2 = analogRead(INPUT_PIN_2);
  int switch_read = digitalRead(DIGITAL_SWITCH);

  //przycisk do zmiany liczb, sprawdzanie czy stan taki jak poprzedni
  if(switch_read == HIGH && previous_switch_read == LOW){
    is_second_set = !is_second_set;
    updateDisplay();
  } 
  previous_switch_read = switch_read;

  int val = analogRead(INPUT_PIN);


  if (analog_read_2 < 50){
    last_read_values[1] = -1;
  } else {
    for (const auto& r :operator_ranges){
      if (analog_read_2 >= r.min_value && analog_read_2 <= r.max_value){
        if(last_read_values[1] != r.type){
            Serial.println(r.type);
            last_read_values[1] = r.type;
        }
        break;
      }
    }
  }
      


  if(val < 50){
    last_read_values[0] = -1;
  } else {
    for (const auto& r : ranges){
      if (val >= r.min_value && val <= r.max_value){
        if(last_read_values[0] != r.index && (millis() - last_debounce >= debounce_delay)){
          Serial.println(sets[is_second_set][r.index]);
          last_read_values[0] = r.index;
          last_debounce = millis();
        }
        break;
      }
    }
  }


  if(millis() - last_display_update >= 1000){
    updateDisplay();
    last_display_update = millis();
  }
  
}
