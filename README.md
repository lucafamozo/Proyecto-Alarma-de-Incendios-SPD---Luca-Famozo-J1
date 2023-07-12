![](https://github.com/lucafamozo/Proyecto-Alarma-de-Incendios-SPD---Luca-Famozo-J1/blob/main/Parcial%20Practico%202%20SPD%20-%20Luca%20Famozo%20J1/img/proyecto_alarma_incendios.png))

## :space_invader: Alumno:
### Famozo, Luca

## :blue_book: Descripción
El proyecto consiste en la simulación de una alarma contra incendios. Este presenta un sensor de temperatura el cual detecta la temperatura actual; un display LCD 16x2 el cual mostrará la temperatura actual detectada por el sensor, así como la temporada del año (verano, otoño, invierno o primavera) asignada al presionar un botón entre el 1 y el 4 del control IR, y en el caso de que la temperatura actual registrada por el sensor de temperatura sea mayor al máximo de temperatura establecido para cada estación en particular, se mostrará un mensaje que indicará que se está produciendo un incendio a la vez de que un servomotor girará mientras la temperatura se mantenga en ese valor. Este sistema contra incendios se encuentra desactivado por defecto, y se puede encender o apagar utilizando el respectivo botón del control IR. Además, se cuenta con un led azul, que indica cuando la temperatura sea menor a -10 °C, y otro led de color rojo, el cual se encenderá mientras la temperatura sea mayor a 45 °C.

## :memo: Inicialización de datos y variables
```c
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
```

## :file_folder: Funciones
La función *'loop()'* utiliza "detectar_botones()" para saber cual botón se oprime, y con este dato, si el botón oprimido es el de encender/apagar, el sistema se enciende, de lo contrario continúa apagado. Luego se verifica si se está dando un incendio, y si es así entonces se dará la corespondiente alerta. Caso contrario, se mostrará, tanto la temperatura actual como la estación del año elegida desde el control remoto. La función "encender_led()" enciende el led azul en caso de que la temperatura alcance o disminuya de los -10 grados, y encenderá el led rojo si se alcanzan o superan los 42 grados.
```c
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
```
*'detector_botones()'* establece la temporada actual dependiendo del botón presionado en el control IR, además de encargarse de encender y apagar el sistema.
```c
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
```
*'sistema_apagado()'* Indica por el display que el sistema no se encuentra en funcionamiento.
```c
void sistema_apagado()
{
  lcd.setCursor(0, 0);
  lcd.print("SISTEMA APAGADO");
  delay(1000);
  lcd.clear();
  estado_incendio_actual = false;
}
```
La función que determina si se está produciendo un incendio utiliza el dato de la temperatura actual, y si se cumple ue esta es mayor o igual la variable "temp_incendio" (determinada por la estación del año indicada por el control IR), determina a las variables "estado_incendio_actual" y "girar_servo" en true. De no cumplirse la condición, se las determina en false. Al final del bucle se le asigna el valor de la variable "estado_incendio_actual" a "estado_incendio_anterior".
```c 
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
```
*'mostrar_temp()'* define a la variable "temperatura_actual" y muestra este dato por el display.
*'mostrar_temporada()'* realiza la misma función para la variable "temporada_actual", y al ser esta variable distinta de "temporada_anterior" se limpia el display.
```c
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
```
*'dar_alerta()'* será llamada cuando el estado actual de incendio sea True. Se encarga de hacer girar el servo y mostrar por el display el mensaje de que se está produciendo un incendio.
```c
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
```
La función de encender leds verifica si la temperatura es menor a -10 grados para encender el led azul, y mayor a 42 grados para encender el led rojo. No se encenderá ningún led si no se cumple ninguna de estas condiciones.
```c
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
```
# :link: Link al proyecto
[Proyecto](https://www.tinkercad.com/things/7UsW8F5bJg8)
