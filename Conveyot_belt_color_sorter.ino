// ============================================
// SISTEMA - DETECTA 3 COLORES
// CON 3 MOTORES + 2 DEFLECTORES (AZUL Y ROJO)
// VERSIÓN SIN FINALES DE CARRERA - POR TIEMPO
// SENSOR SHARP PARA CONFIRMAR LLEGADA
// ============================================

// --- DISPLAYS 7 SEGMENTOS ---
int pinesCentral[7]    = {28, 23, 29, 22, 25, 26, 24};
int pinesIzquierdo[7]  = {51, 52, 47, 53, 48, 50, 49};
int pinesDerecho[7]    = {65, 67, 62, 64, 63, 68, 66};

// --- MOTOR 1 (Banda principal) ---
const int motor1_IN3 = 30;
const int motor1_IN4 = 31;
const int motor1_ENB = 13;

// --- MOTOR 2 (Banda) ---
const int motor2_IN3 = 44;
const int motor2_IN4 = 45;
const int motor2_ENB = 12;

// --- MOTOR 3 (Banda) ---
const int motor3_IN3 = 46;
const int motor3_IN4 = 10;
const int motor3_ENB = 11;

// --- SENSOR COLOR ---
const int S0        = 33;
const int S1        = 34;
const int S2        = 35;
const int S3        = 36;
const int sensorOut = 37;

// --- SENSOR SHARP ---
const int SHARP_PIN    = A0;
// Si el sensor no detecta la última pieza, baja este valor (prueba 300 o 250)
const int SHARP_UMBRAL = 300;

// --- LEDS GENERALES ---
// LED_BANDA1(38), LED_BANDA2(39), LED_BANDA3(40) -> directo a 5V
// LED_D2(16), LED_SENSOR_COLOR, LED_SENSOR_INFRAROJO  -> directo a 5V
const int LED_AMARILLO = 18;
const int LED_AZUL     = 42;
const int LED_ROJO     = 43;

// --- LEDS INDICADORES DE MOTORREDUCTORES ---
const int LED_MOTOR_ROJO = 2;
const int LED_MOTOR_AZUL = 17;

// --- LEDS SIMULADOS DE FINALES DE CARRERA ---
const int LED_FINAL_1 = 15;  // Azul atras    -> enciende al completar retroceso
const int LED_FINAL_2 = 3;   // Azul adelante -> enciende al completar avance
const int LED_FINAL_3 = 5;   // Rojo atras    -> enciende al completar retroceso
const int LED_FINAL_4 = 6;   // Rojo adelante -> enciende al completar avance

// ============================================
// DEFLECTOR AZUL
// Pines 39 y 40 libres (antes LED_BANDA2/3)
// ============================================
const int motorAzul_ENA = 9;
const int motorAzul_IN1 = 40;
const int motorAzul_IN2 = 39;

bool activadoAzul = false;
int estadoDeflectorAzul = 0;
unsigned long tiempoInicioAzul = 0;
unsigned long tiempoEsperaAzul = 0;

const unsigned long TIEMPO_AVANCE_AZUL    = 170;
const unsigned long TIEMPO_ESPERA_AZUL    = 5000;
const unsigned long TIEMPO_RETROCESO_AZUL = 170;

// ============================================
// DEFLECTOR ROJO
// Pin 38 libre (antes LED_BANDA1)
// ============================================
const int motorRojo_ENB = 8;
const int motorRojo_IN3 = 32;
const int motorRojo_IN4 = 38;

bool activadoRojo = false;
int estadoDeflectorRojo = 0;
unsigned long tiempoInicioRojo = 0;
unsigned long tiempoEsperaRojo = 0;

const unsigned long TIEMPO_AVANCE_ROJO    = 170;
const unsigned long TIEMPO_ESPERA_ROJO    = 5000;
const unsigned long TIEMPO_RETROCESO_ROJO = 170;

// --- Contadores por color ---
int contadorAmarillo = 0;
int contadorAzul     = 0;
int contadorRojo     = 0;

// ============================================
// COLA DE COLORES PENDIENTES
// ============================================
const int TAM_COLA = 5;
int colaCola[TAM_COLA];
int colaInicio = 0;
int colaFin    = 0;
int colaTam    = 0;

void encolarColor(int color) {
  if (colaTam < TAM_COLA) {
    colaCola[colaFin] = color;
    colaFin = (colaFin + 1) % TAM_COLA;
    colaTam++;
  }
}

int desencolarColor() {
  if (colaTam == 0) return 0;
  int color = colaCola[colaInicio];
  colaInicio = (colaInicio + 1) % TAM_COLA;
  colaTam--;
  return color;
}

// --- Estado Sharp ---
bool sharpAnterior = false;

bool piezaEnProceso = false;
unsigned long tiempoInicioPieza = 0;
unsigned long tiempoLED = 0;
bool estadoLED     = false;
int colorActualLED = 0;

// Variables del sensor de color
int valorRojo  = 0;
int valorVerde = 0;
int valorAzul  = 0;

byte numeros[10] = {
  0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
  0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111
};

// ============================================
// SETUP
// ============================================
void setup() {
  // DISPLAYS
  for (int i = 0; i < 7; i++) {
    pinMode(pinesCentral[i],   OUTPUT);
    pinMode(pinesIzquierdo[i], OUTPUT);
    pinMode(pinesDerecho[i],   OUTPUT);
    digitalWrite(pinesCentral[i],   LOW);
    digitalWrite(pinesIzquierdo[i], LOW);
    digitalWrite(pinesDerecho[i],   LOW);
  }

  // MOTORES BANDAS
  pinMode(motor1_IN3, OUTPUT); pinMode(motor1_IN4, OUTPUT); pinMode(motor1_ENB, OUTPUT);
  pinMode(motor2_IN3, OUTPUT); pinMode(motor2_IN4, OUTPUT); pinMode(motor2_ENB, OUTPUT);
  pinMode(motor3_IN3, OUTPUT); pinMode(motor3_IN4, OUTPUT); pinMode(motor3_ENB, OUTPUT);

  // DEFLECTOR AZUL
  pinMode(motorAzul_IN1, OUTPUT);
  pinMode(motorAzul_IN2, OUTPUT);
  pinMode(motorAzul_ENA, OUTPUT);
  digitalWrite(motorAzul_IN1, LOW);
  digitalWrite(motorAzul_IN2, LOW);
  analogWrite(motorAzul_ENA, 0);

  // DEFLECTOR ROJO
  pinMode(motorRojo_IN3, OUTPUT);
  pinMode(motorRojo_IN4, OUTPUT);
  pinMode(motorRojo_ENB, OUTPUT);
  digitalWrite(motorRojo_IN3, LOW);
  digitalWrite(motorRojo_IN4, LOW);
  analogWrite(motorRojo_ENB, 0);

  // SENSOR COLOR
  pinMode(S0, OUTPUT); pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT); pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  // SENSOR SHARP
  pinMode(SHARP_PIN, INPUT);

  // LEDS
  pinMode(LED_AMARILLO,   OUTPUT);
  pinMode(LED_AZUL,       OUTPUT);
  pinMode(LED_ROJO,       OUTPUT);
  pinMode(LED_MOTOR_ROJO, OUTPUT);
  pinMode(LED_MOTOR_AZUL, OUTPUT);
  pinMode(LED_FINAL_1,    OUTPUT);
  pinMode(LED_FINAL_2,    OUTPUT);
  pinMode(LED_FINAL_3,    OUTPUT);
  pinMode(LED_FINAL_4,    OUTPUT);

  // Apagar todos los LEDs
  digitalWrite(LED_AMARILLO,   LOW);
  digitalWrite(LED_AZUL,       LOW);
  digitalWrite(LED_ROJO,       LOW);
  digitalWrite(LED_MOTOR_ROJO, LOW);
  digitalWrite(LED_MOTOR_AZUL, LOW);
  digitalWrite(LED_FINAL_1,    LOW);
  digitalWrite(LED_FINAL_2,    LOW);
  digitalWrite(LED_FINAL_3,    LOW);
  digitalWrite(LED_FINAL_4,    LOW);

  // Al arrancar ambos deflectores están en reposo (posición atrás)
  // Simulamos que los FC de atrás ya están activos
  digitalWrite(LED_FINAL_1, HIGH);
  digitalWrite(LED_FINAL_3, HIGH);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // Encender bandas
  digitalWrite(motor1_IN3, HIGH); digitalWrite(motor1_IN4, LOW); analogWrite(motor1_ENB, 200);
  digitalWrite(motor2_IN3, HIGH); digitalWrite(motor2_IN4, LOW); analogWrite(motor2_ENB, 200);
  digitalWrite(motor3_IN3, HIGH); digitalWrite(motor3_IN4, LOW); analogWrite(motor3_ENB, 200);

  mostrarTodosLosDisplay(0, 0, 0);

  Serial.begin(9600);
  Serial.println("Sistema iniciado");
}

// ============================================
// LOOP
// ============================================
void loop() {
  leerColoresRapido();
  detectarColorRapido();
  verificarSharp();

  if (estadoLED && millis() - tiempoLED > 300) {
    digitalWrite(LED_AMARILLO, LOW);
    digitalWrite(LED_AZUL,     LOW);
    digitalWrite(LED_ROJO,     LOW);
    estadoLED = false;
  }

  manejarDeflectorAzul();
  manejarDeflectorRojo();
}

// ============================================
// SENSOR SHARP
// ============================================
void verificarSharp() {
  int lectura    = analogRead(SHARP_PIN);
  bool hayObjeto = (lectura > SHARP_UMBRAL);

  // Flanco de bajada: el objeto acaba de pasar
  if (sharpAnterior && !hayObjeto) {
    int colorConfirmado = desencolarColor();

    if (colorConfirmado == 1) {
      contadorAmarillo++;
      if (contadorAmarillo > 9) contadorAmarillo = 0;
      mostrarEnDisplay(pinesCentral, contadorAmarillo);
      Serial.println("AMARILLO confirmado por Sharp");
    }
    else if (colorConfirmado == 2) {
      contadorRojo++;
      if (contadorRojo > 9) contadorRojo = 0;
      mostrarEnDisplay(pinesDerecho, contadorRojo);
      Serial.println("ROJO confirmado por Sharp");
    }
    else if (colorConfirmado == 3) {
      contadorAzul++;
      if (contadorAzul > 9) contadorAzul = 0;
      mostrarEnDisplay(pinesIzquierdo, contadorAzul);
      Serial.println("AZUL confirmado por Sharp");
    }
  }

  sharpAnterior = hayObjeto;
}

// ============================================
// SENSOR DE COLOR
// ============================================
void leerColoresRapido() {
  const int timeout = 5000;

  digitalWrite(S2, LOW);  digitalWrite(S3, LOW);
  valorRojo = pulseIn(sensorOut, LOW, timeout);
  if (valorRojo == 0) valorRojo = 999;

  digitalWrite(S2, LOW);  digitalWrite(S3, HIGH);
  valorAzul = pulseIn(sensorOut, LOW, timeout);
  if (valorAzul == 0) valorAzul = 999;

  digitalWrite(S2, HIGH); digitalWrite(S3, HIGH);
  valorVerde = pulseIn(sensorOut, LOW, timeout);
  if (valorVerde == 0) valorVerde = 999;
}

String detectarColorPorSensor() {
  if (valorRojo > 90 && valorVerde > 110 && valorAzul > 85)
    return "NADA";
  else if (valorRojo < 45 && valorVerde < 55 && valorAzul > 45 && valorAzul < 70)
    return "AMARILLO";
  else if (valorRojo < 60 && valorVerde > 80 && valorAzul > 60)
    return "ROJO";
  else if (valorAzul < 60 && valorRojo > 80 && valorVerde > 70)
    return "AZUL";
  else
    return "DESCONOCIDO";
}

void detectarColorRapido() {
  String color = detectarColorPorSensor();

  if (!piezaEnProceso) {
    if (color != "NADA" && color != "DESCONOCIDO") {
      piezaEnProceso    = true;
      tiempoInicioPieza = millis();

      if (color == "AMARILLO") {
        digitalWrite(LED_AMARILLO, HIGH);
        colorActualLED = 1;
      }
      else if (color == "ROJO") {
        digitalWrite(LED_ROJO, HIGH);
        colorActualLED = 2;
        if (estadoDeflectorRojo == 0) activadoRojo = true;
      }
      else if (color == "AZUL") {
        digitalWrite(LED_AZUL, HIGH);
        colorActualLED = 3;
        if (estadoDeflectorAzul == 0) activadoAzul = true;
      }

      tiempoLED = millis();
      estadoLED = true;
    }
  }
  else {
    unsigned long duracion = millis() - tiempoInicioPieza;
    if (color == "NADA" || duracion > 500) {
      if (duracion > 50 && colorActualLED != 0) {
        encolarColor(colorActualLED);
        Serial.print("Color encolado: ");
        Serial.println(colorActualLED == 1 ? "AMARILLO" : (colorActualLED == 2 ? "ROJO" : "AZUL"));
      }
      piezaEnProceso = false;
      colorActualLED = 0;
    }
  }
}

// ============================================
// DEFLECTOR AZUL (POR TIEMPO)
//
//  case 0 -> Reposo:        LED_FINAL_1 (pin 15) encendido
//  case 1 -> Avanzando:     ambos LEDs apagados
//  case 2 -> Extendido:     LED_FINAL_2 (pin 3)  encendido
//  case 3 -> Retrocediendo: ambos LEDs apagados
//  vuelve a case 0:         LED_FINAL_1 (pin 15) encendido
// ============================================
void manejarDeflectorAzul() {
  switch (estadoDeflectorAzul) {

    case 0:  // Reposo
      if (activadoAzul) {
        digitalWrite(LED_FINAL_1, LOW);    // apaga FC atras (sale de reposo)
        digitalWrite(LED_MOTOR_AZUL, HIGH);
        avanzarDeflectorAzul();
        tiempoInicioAzul    = millis();
        estadoDeflectorAzul = 1;
        Serial.println("Deflector AZUL avanzando");
      }
      break;

    case 1:  // Avanzando
      if (millis() - tiempoInicioAzul >= TIEMPO_AVANCE_AZUL) {
        detenerDeflectorAzul();
        digitalWrite(LED_FINAL_2, HIGH);   // enciende FC adelante (llegó)
        tiempoEsperaAzul    = millis();
        estadoDeflectorAzul = 2;
        Serial.println("Deflector AZUL en espera");
      }
      break;

    case 2:  // Espera en posición extendida
      if (millis() - tiempoEsperaAzul >= TIEMPO_ESPERA_AZUL) {
        digitalWrite(LED_FINAL_2, LOW);    // apaga FC adelante (empieza retroceso)
        regresarDeflectorAzul();
        tiempoInicioAzul    = millis();
        estadoDeflectorAzul = 3;
        Serial.println("Deflector AZUL retrocediendo");
      }
      break;

    case 3:  // Retrocediendo
      if (millis() - tiempoInicioAzul >= TIEMPO_RETROCESO_AZUL) {
        detenerDeflectorAzul();
        digitalWrite(LED_FINAL_1, HIGH);   // enciende FC atras (volvió a reposo)
        activadoAzul        = false;
        digitalWrite(LED_MOTOR_AZUL, LOW);
        estadoDeflectorAzul = 0;
        Serial.println("Deflector AZUL en reposo");
      }
      break;
  }
}

void avanzarDeflectorAzul() {
  digitalWrite(motorAzul_IN1, LOW);
  digitalWrite(motorAzul_IN2, HIGH);
  analogWrite(motorAzul_ENA, 100);
}

void regresarDeflectorAzul() {
  digitalWrite(motorAzul_IN1, HIGH);
  digitalWrite(motorAzul_IN2, LOW);
  analogWrite(motorAzul_ENA, 100);
}

void detenerDeflectorAzul() {
  analogWrite(motorAzul_ENA, 0);
  digitalWrite(motorAzul_IN1, LOW);
  digitalWrite(motorAzul_IN2, LOW);
}

// ============================================
// DEFLECTOR ROJO (POR TIEMPO)
//
//  case 0 -> Reposo:        LED_FINAL_3 (pin 5) encendido
//  case 1 -> Avanzando:     ambos LEDs apagados
//  case 2 -> Extendido:     LED_FINAL_4 (pin 6) encendido
//  case 3 -> Retrocediendo: ambos LEDs apagados
//  vuelve a case 0:         LED_FINAL_3 (pin 5) encendido
// ============================================
void manejarDeflectorRojo() {
  switch (estadoDeflectorRojo) {

    case 0:  // Reposo
      if (activadoRojo) {
        digitalWrite(LED_FINAL_3, LOW);    // apaga FC atras (sale de reposo)
        digitalWrite(LED_MOTOR_ROJO, HIGH);
        avanzarDeflectorRojo();
        tiempoInicioRojo    = millis();
        estadoDeflectorRojo = 1;
        Serial.println("Deflector ROJO avanzando");
      }
      break;

    case 1:  // Avanzando
      if (millis() - tiempoInicioRojo >= TIEMPO_AVANCE_ROJO) {
        detenerDeflectorRojo();
        digitalWrite(LED_FINAL_4, HIGH);   // enciende FC adelante (llegó)
        tiempoEsperaRojo    = millis();
        estadoDeflectorRojo = 2;
        Serial.println("Deflector ROJO en espera");
      }
      break;

    case 2:  // Espera en posición extendida
      if (millis() - tiempoEsperaRojo >= TIEMPO_ESPERA_ROJO) {
        digitalWrite(LED_FINAL_4, LOW);    // apaga FC adelante (empieza retroceso)
        regresarDeflectorRojo();
        tiempoInicioRojo    = millis();
        estadoDeflectorRojo = 3;
        Serial.println("Deflector ROJO retrocediendo");
      }
      break;

    case 3:  // Retrocediendo
      if (millis() - tiempoInicioRojo >= TIEMPO_RETROCESO_ROJO) {
        detenerDeflectorRojo();
        digitalWrite(LED_FINAL_3, HIGH);   // enciende FC atras (volvió a reposo)
        activadoRojo        = false;
        digitalWrite(LED_MOTOR_ROJO, LOW);
        estadoDeflectorRojo = 0;
        Serial.println("Deflector ROJO en reposo");
      }
      break;
  }
}

void avanzarDeflectorRojo() {
  digitalWrite(motorRojo_IN3, HIGH);
  digitalWrite(motorRojo_IN4, LOW);
  analogWrite(motorRojo_ENB, 100);
}

void regresarDeflectorRojo() {
  digitalWrite(motorRojo_IN3, LOW);
  digitalWrite(motorRojo_IN4, HIGH);
  analogWrite(motorRojo_ENB, 100);
}

void detenerDeflectorRojo() {
  analogWrite(motorRojo_ENB, 0);
  digitalWrite(motorRojo_IN3, LOW);
  digitalWrite(motorRojo_IN4, LOW);
}

// ============================================
// DISPLAYS 7 SEGMENTOS
// ============================================
void mostrarEnDisplay(int pines[], int contador) {
  int digito = contador % 10;
  if (contador > 9 && digito == 0) digito = 1;
  for (int i = 0; i < 7; i++) {
    digitalWrite(pines[i], bitRead(numeros[digito], i));
  }
}

void mostrarTodosLosDisplay(int amarillo, int azul, int rojo) {
  int digitoAmarillo = amarillo % 10;
  int digitoAzul     = azul     % 10;
  int digitoRojo     = rojo     % 10;
  for (int i = 0; i < 7; i++) {
    digitalWrite(pinesCentral[i],   bitRead(numeros[digitoAmarillo], i));
    digitalWrite(pinesIzquierdo[i], bitRead(numeros[digitoAzul],     i));
    digitalWrite(pinesDerecho[i],   bitRead(numeros[digitoRojo],     i));
  }
}
