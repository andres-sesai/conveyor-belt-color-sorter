# conveyor-belt-color-sorter
Sistema de clasificación de piezas por color con Arduino Mega, sensor TCS

## Hardware usado
- Arduino Mega 2560
- Sensor de color TCS34725
- Sensor IR Sharp
- L298N (control de motores)
- Leds 
- Display 7 segmentos 
- Motorreductor
- Motor Dc

## Descripción
El sistema automatizado detecta el color de cada pieza colocada en la banda 
transportadora mediante un sensor TCS34725. Una vez identificado el color, 
el sistema activa el deflector correspondiente para redirigir la pieza hacia 
su contenedor asignado. Al momento en que la pieza llega al contenedor, 
un sensor IR Sharp confirma la recepción y se registra en el panel de control, 
donde displays de 7 segmentos muestran el conteo de piezas clasificadas 
por color en tiempo real.
