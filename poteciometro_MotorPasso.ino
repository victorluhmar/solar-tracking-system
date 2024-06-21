#include <AccelStepper.h> 
 
// Definição das portas analógicas do driver ULN2003 do motor de passo 
#define PIN1 6 //IN1 
#define PIN2 7 //IN2 
#define PIN3 8 //IN3 
#define PIN4 9 //IN4 
 
// Quantidade de passos necessários para completar uma rotação completa, transformando em graus 
const float STEPS_PER_REVOLUTION = 2048; 
const float DEGREES_PER_STEP = 360.0 / STEPS_PER_REVOLUTION; 
 
// Pins do motor de passo, comunicação do driver do motor de passo 
AccelStepper myStepper(AccelStepper::FULL4WIRE, PIN1, PIN3, PIN2, PIN4); 
 
// Pino do potenciômetro 
const int potPin = A0; 
 
void setup() { 
  // Configurações iniciais 
  myStepper.setMaxSpeed(3000);    // Velocidade máxima do motor de passo em passos/segundos 
  myStepper.setAcceleration(650); // Taxa de aceleração do motor de passo 
   
  // Inicializa a comunicação serial 
  Serial.begin(9600); 
} 
 
void loop() { 
  // Lê o valor do potenciômetro 
  int potValue = analogRead(potPin); 
   
  // Mapeia o valor do potenciômetro (0-1023) para um intervalo de graus de 0 a 360 
  int targetAngle = map(potValue, 0, 1023, 0, 360); 
   
  // Converte o ângulo alvo em quantidade de passos 
  int stepsToMove = targetAngle / DEGREES_PER_STEP; 
 
  // Verifica a direção do movimento com base na posição atual do motor de passo 
  if (stepsToMove > myStepper.currentPosition()) { 
    // Gira no sentido anti-horário 
    myStepper.moveTo(stepsToMove); 
  } else { 
    // Gira no sentido horário 
    myStepper.moveTo(-stepsToMove); 
  } 
   
  // Executar o movimento 
  myStepper.run(); 
   
  // Mostra a quantidade de graus movidos no monitor serial 
  Serial.print("Graus movidos: "); 
  Serial.println(abs(myStepper.currentPosition()) * DEGREES_PER_STEP); 
   
  // Aguarda um curto período de tempo 
  delay(10); 
}