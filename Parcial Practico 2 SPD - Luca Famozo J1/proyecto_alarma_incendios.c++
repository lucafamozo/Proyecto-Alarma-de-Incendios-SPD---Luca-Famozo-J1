#include <Servo.h>
#include <LiquidCrystal.h>
#include <IRremote.h>

LiquidCrystal lcd(12, 10, 5, 4, 3, 2);
Servo servo;
#define sensorTemp A0
#define led_azul 7
#define led_rojo 8
#define boton_control 0xFF00BF00

bool sistemaActivado = true;
decode_results resultadosIR;

bool girarServo = false;
bool estadoIncendio = false;
float temperaturaActual = 0;
int temperaturaAnterior = 0;
int temporadaAnterior = 0;
int temporadaActual = 0;
int angulo = 0;
int sensorIr = 11;

void setup()
{
  lcd.begin(16, 2);
  servo.attach(9);
  pinMode(led_azul, OUTPUT);
  pinMode(led_rojo, OUTPUT);
  //pinMode(sensorIr, INPUT);
  //digitalWrite(sensorIr, HIGH);
  IrReceiver.begin(sensorIr, DISABLE_LED_FEEDBACK);
}

void loop()
{
 saberEstadoSistema();
  
  if (sistemaActivado){
    saberSiHayIncendio();
    alarmaServo();
  }

  saberTemporada();
  mostrarDatosLcd();
  limpiarLcd();
  prenderLeds();
  
}

void saberTemporada()
{
  temperaturaActual = map(analogRead(sensorTemp), 0, 1023, -50, 450);

  if (temperaturaActual < 13 && temperaturaActual > -10)
  {
    temporadaActual = 1; // invierno
  }
  else if (temperaturaActual >= 13 && temperaturaActual < 22)
  {
    temporadaActual = 2; // otoño
  }
  else if (temperaturaActual >= 22 && temperaturaActual <= 27)
  {
    temporadaActual = 3; // primavera
  }
  else if (temperaturaActual > 27 && temperaturaActual < 42)
  {
    temporadaActual = 4; // verano
  }
  else
  {
   temporadaActual = 5; //Extremo
  }
}

void saberSiHayIncendio()
{
  if (temperaturaActual >= 60)
  {
    estadoIncendio = true;
    girarServo = true;
  }
  else {
    estadoIncendio = false;
    girarServo = false;
  }
}

void mostrarDatosLcd()
{
  if (estadoIncendio == false){
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperaturaActual);
    lcd.print("C");

    lcd.setCursor(0, 1);
    //lcd.print("Estacion: ");

    switch (temporadaActual)
    {
      case 1:
        lcd.print("Invierno");
        break;
      case 2:
        lcd.print("Otono");
        break;
      case 3:
        lcd.print("Primavera");
        break;
      case 4:
        lcd.print("Verano");
        break;
      case 5:
        lcd.print("Temp. extrema");
        break;
    }
  }
  else{
    lcd.setCursor(0, 0);
    lcd.print("ALERTA INCENDIO!");
  }
}

void limpiarLcd()
{
  if (temperaturaActual != temperaturaAnterior || temporadaActual != temporadaAnterior){
    lcd.clear();
  }
  temperaturaAnterior = temperaturaActual;
  temporadaAnterior = temporadaActual;
}

void alarmaServo()
{
  if (girarServo == true)
  {
    for (angulo = 0; angulo < 180; angulo += 20)
    {
      servo.write(angulo);
      delay(100);
    }
  }
}

void prenderLeds()
{
  if (temperaturaActual < -10){
    digitalWrite(led_azul, 1);
    digitalWrite(led_rojo, 0);
  }
  else if (temperaturaActual > 42){
    digitalWrite(led_rojo, 1);
    digitalWrite(led_azul, 0);
  }
  else {
    digitalWrite(led_azul, 0);
    digitalWrite(led_rojo, 0);
  }
}

void saberEstadoSistema()
{
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.decodedRawData == boton_control){
      sistemaActivado = !sistemaActivado;
      estadoIncendio = false;
      girarServo = false;
    }
   IrReceiver.resume();
  }
}