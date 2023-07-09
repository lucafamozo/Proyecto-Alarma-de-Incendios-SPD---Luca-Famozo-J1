#include <Servo.h>
#include <LiquidCrystal.h>
#include <IRremote.h>

LiquidCrystal lcd(12, 10, 5, 4, 3, 2);
Servo servo;
#define sensor_temp A0
#define led_azul 7
#define led_rojo 8
#define boton_on_off 0xFF00BF00
#define boton_0 0xF30CBF00
#define boton_1 0xEF10BF00
#define boton_2 0xEE11BF00
#define boton_3 0xED12BF00
#define boton_4 0xEB14BF00


bool estado_actual_sistema = false;
bool estado_anterior_sistema = false;
decode_results resultadosIR;

bool temporada_control = false;

bool girar_servo = false;
bool estado_incendio_actual = false;
bool estado_incendio_anterior = false;

float temperatura_actual = 0;
int temperatura_anterior = 0;
int temporada_anterior = 0;
int temporada_actual = 0;
int temp_incendio = 10000;
int angulo = 0;
int sensor_ir = 11;

void setup()
{
  lcd.begin(16, 2);
  servo.attach(9);
  pinMode(led_azul, OUTPUT);
  pinMode(led_rojo, OUTPUT);
  IrReceiver.begin(sensor_ir, DISABLE_LED_FEEDBACK);
  
   Serial.begin(9600);
}

void loop()
{
  detector_botones();
  if (estado_actual_sistema == false){
    sistema_apagado();
  }
  else{
    saber_estado_incendio();
    if (estado_incendio_actual == true){
      dar_alerta();
      encender_led();
      saber_estado_incendio();
    }
    mostrar_temp();
    mostrar_temporada();
    encender_led();
  }
}
  
void detector_botones()
{
  if(IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.decodedRawData == boton_on_off){
      estado_actual_sistema = !estado_actual_sistema;
      estado_anterior_sistema = estado_actual_sistema;
      
    }
    else if (IrReceiver.decodedIRData.decodedRawData == boton_1){
      temporada_actual = 1;
    }
    else if (IrReceiver.decodedIRData.decodedRawData == boton_2){
      temporada_actual = 2;
    }
    else if (IrReceiver.decodedIRData.decodedRawData == boton_3){
      temporada_actual = 3;
    }
    else if (IrReceiver.decodedIRData.decodedRawData == boton_4){
      temporada_actual = 4;
    }
   IrReceiver.resume();
  }
}

void sistema_apagado()
{
  lcd.setCursor(0, 0);
  lcd.print("SISTEMA APAGADO");
  delay(1000);
  lcd.clear();
  estado_incendio_actual = false;
}
  
void saber_estado_incendio()
{
  temperatura_actual = map(analogRead(sensor_temp), 0, 1023, -50, 450);
  if (temperatura_actual >= temp_incendio)
  {
    estado_incendio_actual = true;
    girar_servo = true;
  }
  else {
    estado_incendio_actual = false;
    girar_servo = false;
  }
  
  estado_incendio_anterior = estado_incendio_actual;
}
  
void dar_alerta()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALERTA INCENDIO!");
  
  if (girar_servo == true)
  {
    for (angulo = 0; angulo < 181; angulo += 20)
    {
      servo.write(angulo);
      delay(100);
    }
  }
  lcd.clear();
}
  
void encender_led()
{
  if (temperatura_actual < -10){
    digitalWrite(led_azul, 1);
    digitalWrite(led_rojo, 0);
  }
  else if (temperatura_actual > 42){
    digitalWrite(led_rojo, 1);
    digitalWrite(led_azul, 0);
  }
  else {
    digitalWrite(led_azul, 0);
    digitalWrite(led_rojo, 0);
  }
}
  
void mostrar_temp()
{
  temperatura_actual = map(analogRead(sensor_temp), 0, 1023, -50, 450);
  lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperatura_actual);
    lcd.print("C");

    lcd.setCursor(0, 1);
}
  
void mostrar_temporada()
{
  if (temporada_actual != temporada_anterior){
    lcd.clear();
    
  lcd.setCursor(0, 1);
  switch (temporada_actual)
    {
      case 1:
        lcd.print("Invierno");
    	temp_incendio = 50;
        break;
      case 2:
        lcd.print("Otono");
    	temp_incendio = 55;
        break;
      case 3:
        lcd.print("Primavera");
    	temp_incendio = 60;
        break;
      case 4:
        lcd.print("Verano");
    	temp_incendio = 65;
        break;
    }
  temporada_anterior = temporada_actual;
  }
}
