/* 
Universidad Del Valle de Guatemala
Facultad de Ingeniería
Departamento de Electrónica, Mecatrónica y Biomédica
Electrónica Digital 2

Susan Daniela Guevara Catalán - 22519

Laboratorio No. 6 - LCD

Parte 1: Utilizando dos potenciómetros como sensores analógicos, se lee el voltaje y se muestra en la LCD
Parte 2: Un botón incrementa y otro botón decrementa el contador

*/

/************************ Importación de librerías *******************************/
#include <Arduino.h>
#include <LiquidCrystal_I2C.h> // Permite usar la LCD
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; // Permite usar las interrupciones

/************************ Declaración de variables *******************************/
// Pines del potenciometro
#define Pot1 34
#define Pot2 35
// Estructura del botón
struct Button{
  const uint8_t PIN; // Indica el pin a donde está conectado
  volatile uint32_t numberKeyPresses; // Indica cuantas veces se ha presionado
  bool pressed; // Indica si está o no está presionado
};
// Definición del botón de incremento
Button Suma ={32, 0, false};
// Definición del botón de incremento
Button Resta ={33, 0, false};
// Configura la LCD a 0x27 para 16 columans y 2 filas
LiquidCrystal_I2C lcd(0x27,16,2); 
// Variable del contador de 8 bits
int counter;

/************************ Prototipo de funciones *******************************/
float leerVoltaje(int pin); // Obtiene el valor del voltaje de los potenciómetros. Parámetro: Pin del potenciómetro
void IRAM_ATTR BTN_SUMA_ISR(void); // Aumenta de valor el contador. Parámetro: No aplica
void IRAM_ATTR BTN_RESTA_ISR(void); // Disminuye de valor el contador. Parámetro: No aplica

/************************ Configuración de pines *******************************/
void setup(){
  // Empieza la conexión serial
  Serial.begin(115200);
  // Configurar el botón de suma como entrada
  pinMode(Suma.PIN,INPUT_PULLUP);
  attachInterrupt(Suma.PIN, BTN_SUMA_ISR, FALLING);
  // Configurar el botón de suma como entrada
  pinMode(Resta.PIN,INPUT_PULLUP);
  attachInterrupt(Resta.PIN, BTN_RESTA_ISR, FALLING);
  // Inicializa la LCD
  lcd.init(); 
}

void loop()
{
  lcd.backlight();
  // Muestra en la LCD "Pot1:"
  lcd.setCursor(0,0);
  lcd.print("Pot1:");
  // Obtiene el valor de voltaje del potenciómetro 1 y lo muestra en la LCD
  float voltaje1 = leerVoltaje(Pot1);
  lcd.setCursor(0,1);
  lcd.print(voltaje1);
  // Muestra en la LCD "Pot2:"
  lcd.setCursor(6,0);
  lcd.print("Pot2:");
  // Obtiene el valor de voltaje del potenciómetro 2 y lo muestra en la LCD
  float voltaje2 = leerVoltaje(Pot2);
  lcd.setCursor(6,1);
  lcd.print(voltaje2);
  // Muestra en la LCD "CPU:"
  lcd.setCursor(12,0);
  lcd.print("CPU:");
  lcd.setCursor(12,1);
  lcd.print(counter);
  delay(200);
  // Cuando se presione el botón de suma, aumentará el contador
  if(Suma.pressed){
    // Asegura que lee cuando se presiona el botón
    Serial.printf("Button add has been pressed %u times\n",Suma.numberKeyPresses);
    // Reinicia el contador si llega a 16
    if (counter >= 255) {
      counter = 0;  
    }else{
      counter++;  // Incrementa el contador en 1
    }
    Suma.pressed=false;
  }
  // Cuando se presione el botón de resta, disminuira el contador
  if(Resta.pressed){
    // Asegura que lee cuando se presiona el botón
    Serial.printf("Button add has been pressed %u times\n",Resta.numberKeyPresses);
    // Reinicia el contador si llega a 16
    if (counter <= 0) {
        counter = 255;  // Reinicia el contador a 15 si llega a 0
    } else {
        counter--;  // Decrementa el contador en 1
    }
    Resta.pressed=false;
  }
  lcd.clear();
}

/************************ Definición de funciones *******************************/
// Función para leer el voltaje de un potenciómetro
float leerVoltaje(int pin) {
    float valorAnalogico = analogRead(pin);
    float voltaje = (valorAnalogico / 4095.0) * 5; // Suponiendo Vref = 5V y resolución de 12 bits
    return voltaje;
}

// Interrupción para el botón que suma
void IRAM_ATTR BTN_SUMA_ISR(){
  portENTER_CRITICAL_ISR(&mux);
    Suma.numberKeyPresses++;
    Suma.pressed=true;
  portEXIT_CRITICAL_ISR(&mux);
}

// Interrupción para el botón que resta
void IRAM_ATTR BTN_RESTA_ISR(){
  portENTER_CRITICAL_ISR(&mux);
    Resta.numberKeyPresses++;
    Resta.pressed=true;
  portEXIT_CRITICAL_ISR(&mux);
}