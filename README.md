![](https://github.com/lucafamozo/Proyecto-Alarma-de-Incendios-SPD---Luca-Famozo-J1/blob/main/Parcial%20Practico%202%20SPD%20-%20Luca%20Famozo%20J1/img/proyecto_alarma_incendios.png))

## :space_invader: Alumno:
### Famozo, Luca

## :blue_book: Descripción
El proyecto consiste en la simulación de una alarma contra incendios. Este presenta un sensor de temperatura el cual detecta la temperatura actual; un display LCD 16x2 el cual mostrará la temperatura actual detectada por el sensor, así como la temporada del año (verano, otoño, invierno o primavera) determinada por un rango de temperatura, y en el caso de que la temperatura sea mayor a 60 °C mostrará un mensaje que indicará que se está produciendo un incendio a la vez de que un servomotor girará mientras la temperatura no disminuya. Este sistema contra incendios se puede controlar utilizando un control remoto y un sensor infrarrojo, que al presionar el botón de apagado/encendido desactiva y activa el sistema. Además, se cuenta con un led azul, que indica cuando la temperatura sea menor a -10 °C, y otro led de color rojo, el cual se encenderá mientras la temperatura sea mayor a 45 °C.

## :memo: Inicialización de datos y variables
```c
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
```

## :file_folder: Funciones
La función *'loop()'* utiliza dos funciones: *'saberEstadoSistema()'*, la cual se encarga de detectar cuando se es presionado el botón de encendido/apagado del control remoto, y así ir alternando entre estas dos variables.
Dentro de un bloque "if" se detecta si el sistema está activado, y si es así, se ejecutan las funciones *'saberSiHayIncendio()'* (encargada de que en caso de que la temperatura haya llegado a los 60 grados, los datos de"girarServo" y "estadoIncendio" cambien a true) y *'alarmaServo()'* (encargada de hacer girar el servoMotor).

Esté o no activado el sistema, las funciones *'saberTemporada'*; *'mostrarDatosLcd'*; *'limpiarLcd'* y *'prenderLeds'* se ejecutan siempre.
```c
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
```
La función que detecta si el sistema está encendido o apagado verifica que el dato recibido por el sensor IR posea el mismo valor que la variable denominada como "boton_control". Si esto se cumple, cambia el estado booleano del sistema y determina a las variables "estadoIncendio" y "girarServo" en falsas.
```c
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
```
La función que determina si se está produciendo un incendio utiliza el dato de la temperatura actual, y si se cumple ue esta es mayor o igual a 60 grados, determina a las variables "estadoIncendio" y "girarServo" en true. De no cumplirse la condición, se las determina en false.
```c 
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
```
*'saberTemporada()'* determina la temporada actual por medio de rangos de temperatura delimitados para cada una, y cada temporada está asignada a un valor entre 1 y 4 para luego poder comparar estos datos y determinar lo que se mostrará en el display. Por ejemplo, si la temperatura actual es mayor a 27 y menor que 42 grados, entonces se determina la temporada actual como verano.
```c
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
```
*'mostrarDatosLcd'* primero verifica que "estadoIncendio" esté en false, sino de lo contrario mostraría por display el mensaje de incendio. Una vez verifica ese dato, muestra tanto la temperatura actual, y (haciendo uso de las variables de "temporadaActual") la estación del año.
```c
void mostrarDatosLcd()
{
  if (estadoIncendio == false){
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperaturaActual);
    lcd.print("C");

    lcd.setCursor(0, 1);

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
```
La función de limpieza verifica que o la temperatura actual o la temporada actual sean distintas a los datos anteriores, y si esto es así, borra los elementos en el display.
```c
void limpiarLcd()
{
  if (temperaturaActual != temperaturaAnterior || temporadaActual != temporadaAnterior){
    lcd.clear();
  }
  temperaturaAnterior = temperaturaActual;
  temporadaAnterior = temporadaActual;
}
```

La función *'alarmaServo'* es la que hace girar al servomotor en caso de que la variable "girarServo" sea verdadera (este dato se obtenía al verificar si se está ocurriendo un incendio a través de la detección de temperatura).
```c
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
```
La función de prender leds verifica si la temperatura es menor a -10 grados para encender el led azul, y mayor a 42 grados para encender el led rojo.
```c
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
```
# :link: Link al proyecto
[Proyecto](https://www.tinkercad.com/things/7UsW8F5bJg8)
