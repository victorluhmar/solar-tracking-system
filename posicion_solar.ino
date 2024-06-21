#include <Wire.h> // Biblioteca do módulo RTC      
#include "RTClib.h" // Biblioteca do módulo RTC      
#include <AccelStepper.h> // Biblioteca do motor de passo      
#include <Servo.h> // Biblioteca do servo motor      
#include <math.h> // Biblioteca das funções trigonometricas      
      
RTC_DS3231 rtc; // tipo do módulo rtc utilizado      
      
// Definição das portas analógicas do driver ULN2003 do motor de passo      
#define PIN1 6 // IN1      
#define PIN2 7 // IN2      
#define PIN3 8 // IN3      
#define PIN4 9 // IN4      
      
// Valor de PI      
#define PI 3.141592653589      
      
// Valores definidos de latitude, longitude, latitude (radiano) e longitude do meridiano      
const double latitude = -13.2526395; // latitude local      
const double longitude = -43.4082828; // longitude local      
const double lat_rad = latitude * PI/180.0; // latitude em radiano 
const double long_rad = longitude * PI/180; // longitude em radiano   
const double long_meridiano = -45;  

int j = 135; // O valor de j é o número do dia do ano atual (dia médio do mês de maio - 121 a 153 dias)
int posicao_servo = 0;   
      
// Quantidade de passos necessários para completar uma rotação completa, transformando em graus      
const float STEPS_PER_REVOLUTION = 2048;      
const float STEPS_PER_DEGREE = STEPS_PER_REVOLUTION / 360.0; // 5.7 rev/graus  
      
// Pins do motor de passo, comunicação do driver do motor de passo      
AccelStepper stepper(AccelStepper::FULL4WIRE, PIN1, PIN3, PIN2, PIN4);      
      
Servo servo; // Controlar o Servo com o nome de "servo"
      
// Variável booleana para controlar o movimento do motor de passo      
bool movimento_motor_ativo = true;      
  
// Função para mover o motor de passo para a posição especifica em graus      
void moveStepperToAzimuth(double azimute){       
  // conversão do valor de azimute em graus para passos do motor de passo      
  double position = azimute*STEPS_PER_DEGREE;      
  // mover o motor de passo para a posição desejada      
  stepper.moveTo(position);      
  // Enquanto o motor não alcançar a posição desejada, continue movendo      
  while (stepper.distanceToGo() != 0) {      
    stepper.run();      
  }      
}      
  
void setup(){      
      
  Serial.begin(9600);      
  // Aguarda 5 segundos após ter ligado o arduino para iniciar os módulos     
  delay(5000);      
  // Motor de passo      
  stepper.setMaxSpeed(1000);// Velocidade máxima do motor de passo em passos/segundos      
  stepper.setAcceleration(650);// Taxa de aceleração do motor de passo  
  moveStepperToAzimuth(0); // O motor de passo inicia na posição inicial (0 graus)  
  
  // Servo      
  servo.attach(11); // Anexa o servo ao pino 11      
  // RTC      
  rtc.begin();     
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));// Ajusta de data e hora automatico    
  //rtc.adjust(DateTime(2024, 4, 27, 10, 17, 00)); // Ajusta o tempo do RTC manualmente    
  //Ordem (Ano,Mes,Dia,hora,minutos,segundos)    
  //Executar apenas quando precisar ajustar a data e hora, depois comentar essa linha     
  delay(1000); // 1000 Milissegundos       
      
}      
      
void loop(){      
      
  // Módulo RTC DS3231      
  DateTime now = rtc.now();      
  int hora_atual = now.hour();      
  int minutos_atual = now.minute();       
  // Converte os minutos em horas      
  float horas_convertidas = minutos_atual / 60.0;      
  // Adiciona as horas convertidas à hora atual      
  float hora_total = hora_atual + horas_convertidas;          
  
  // Equacões (Variaveis)      
  double declinAng = 0;      
  double corrig_long_ang = 0;
  double corrig_long_rad = 0;      
  double eot = 0;      
  double horaSolar = 0;   
  double detHora = 0;      
  double valor_x = 0;      
  double altitudeRad = 0;      
      
  // Equações da Posição do sol (Pelo Horario)      
  double altitude = 0;      
  double azimute = 0;      
  double zenite = 0;      
      
  // Calculos das equações      
  declinAng = 23.45*sin((PI/180.0)*(360*((j-80)/365.0))); // Cálculo da declinação solar em graus
  corrig_long_ang = (longitude-long_meridiano)/15.0; // Correção da longitude
  corrig_long_rad = corrig_long_ang * PI/180.0;    
  valor_x = (2*PI*(j-1))/365.0; // Ângulo em relação ao número do dia do ano 
  eot = 2.292*(0.0075+0.1868*cos(valor_x)-3.2077*sin(valor_x)-1.4615*cos(2*valor_x)-4.089*sin(2*valor_x)); // Equação Horária do Tempo (EOT) em radiano
  horaSolar = hora_total - corrig_long_rad + (eot/60.0) - 0; // Conversão da hora relógio em hora solar (h) 
  detHora = 15.0*(horaSolar - 12); // Determinação da hora solar aonde prever a posição do sol      
        
  // Conversão dos valores em radianos      
  double declin_rad = declinAng*PI/180.0;      
  double detHoraRad = detHora*PI/180.0;      
      
  // Cálculo das equações da Posição do sol (Pelo Horario)      
  altitudeRad = asin(sin(declin_rad)*sin(lat_rad)+cos(declin_rad)*cos(lat_rad)*cos(detHoraRad)); // Altitude Solar em radiano      
  altitude = altitudeRad*180.0/PI; // Conversão em graus      
  azimute = (acos((-sin(altitudeRad)*sin(lat_rad)+sin(declin_rad))/(cos(altitudeRad)*cos(lat_rad))))*180.0/PI; //Ângulo de azimute em Graus
  zenite = 90.0-altitude; // Ângulo de zênite      
      
  // Verifica se é 18h ou antes da 6h da manhã no RTC e o movimento do motor está ativo      
  if ((hora_total >= 18 || hora_total < 6) && movimento_motor_ativo){     
    // Move o motor de passo para a posição inicial (0 graus)  
    Serial.println("Desativando movimento do motor.");
    moveStepperToAzimuth(0);   
    // Para o movimento do motor de passo     
    stepper.stop();     
    // Para o movimento do servo motor     
    servo.detach();     
    // Desativa o movimento do motor para que ele não se mova novamente nesta condição      
    movimento_motor_ativo = false;     
  }  
  // Se passou das 6h da manhã e o movimento do motor não está ativo  
  else if (hora_total >= 6 && hora_total < 18 && !movimento_motor_ativo){  
    Serial.println("Ativando movimento do motor.");
    // reanexa o servo     
    servo.attach(11);     
    // Ativa o movimento do motor de passo novamente  
    movimento_motor_ativo = true;  
  }  
  // Se é 6h da manhã e antes das 18h e o movimento está ativo, mova o servo, mova o motor de passo e atualize o horário   
  else if(hora_total >= 6 && hora_total < 18 && movimento_motor_ativo){  
    if (hora_total >= 12){ 
        // Move o motor de passo para o novo azimute 
        if(hora_total == 12 && minutos_atual == 0){
          moveStepperToAzimuth(0);
        } 
        else{
          Serial.println("Movendo para azimute negativo.");
          moveStepperToAzimuth(-azimute); 
          // Mapeia o valor de atitude (0 a 90 graus) para a faixa de valores do servo (0 a 180 graus)   
          posicao_servo = map(altitude, 0, 90, 0, 180); 
          // Define a posição do servo de acordo com o valor de altitude calculado     
          servo.write(posicao_servo); 
          // Atualiza o tempo do RTC enquanto o motor de passo está se movendo 
          now = rtc.now(); 
        }
      } 
      else{ 
        Serial.println("Movendo para azimute POSITIVO.");
        posicao_servo = map(altitude, 0, 90, 0, 180);         
        servo.write(posicao_servo);      
        moveStepperToAzimuth(azimute);           
        now = rtc.now(); 
      }      
  }   

  Serial.print("declinação: ");  
  Serial.println(declinAng);
  Serial.print("declinação radiano: ");    
  Serial.println(declin_rad);
  Serial.print("Correção longitude em rad: ");
  Serial.println(corrig_long_rad,5);
  Serial.print("hora Solar: ");
  Serial.println(horaSolar);
  Serial.print("Determinação Solar em rad: ");
  Serial.println(detHoraRad);
  Serial.print("Determinação da hora Solar: ");
  Serial.println(detHora);
  Serial.print("Altitude Solar: ");
  Serial.println(altitude);
  Serial.print("Azimute: ");
  Serial.println(azimute);
  Serial.print("zenite: ");
  Serial.println(zenite);
  Serial.print("Horas: ");
  Serial.println(hora_total, 2);
  Serial.print("posição do servo: ");
  Serial.print(posicao_servo);
  Serial.println();
        
  // Aguarda um tempo antes de verificar novamente      
  delay(5000); // Verifica a cada 5 segundos       
}