#include <SD.h> // Biblioteca do cartão SD
#include <SPI.h> // Biblioteca para comunicação SPI 
#include <Wire.h> //Biblioteca do RTC DS3231 
#include "RTClib.h" 
 
const int chipSelect = 4; // Pino CS do módulo SD  
RTC_DS3231 rtc; 
 
char diaDaSemana[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"}; 
unsigned long ultimoTempoSalvo = 0;
// Variável para armazenar o último tempo de salvamento  
const unsigned long intervaloSalvamento = 5000; 
 
void setup() { 
  Serial.begin(9600); 
 
  rtc.begin(); 
 
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));// Ajusta de data e hora automatico       
  //rtc.adjust(DateTime(2022, 7, 25, 9, 53, 00)); //Ajusta o tempo do RTC manualmente       
  //Ordem (Ano,Mes,Dia,hora,minutos,segundos)       
  //Executar apenas quando precisar ajustar a data e hora, depois comentar essa linha   
 
  // Inicializando o cartão SD 
  if (!SD.begin(chipSelect)) { 
    Serial.println("Falha ao inicializar o cartão SD"); 
    return; 
  } 
  Serial.println(); 
  Serial.println("Cartão SD inicializado com sucesso"); 
 
} 
 
void loop() { 
  DateTime now = rtc.now(); // Faz a leitura de dados de data e hora
 
  float tensaoFixa = analogRead(A0) * 5.0 * 5.0 / 1024.0; 
  float correnteFixa = tensaoFixa / 7.49; // 7.49k ohn corresponde a resistência do módulo de tensão
  float correnteResF = tensaoFixa / 10.0; //10K ohn corresponde ao resistor 
  float corrente_total_fixa = correnteFixa + correnteResF; 
 
  float tensaoMovel = analogRead(A1) * 5.0 * 5.0 / 1024.0; 
  float correnteMovel = tensaoMovel / 7.49; 
  float correnteResM = tensaoMovel / 10.0; 
  float corrente_total_movel = correnteMovel + correnteResM; 
 
  if (millis() - ultimoTempoSalvo >= intervaloSalvamento) { 
    ultimoTempoSalvo = millis();

    // Abre o arquivo "dadosF.txt" no cartão SD para escrita 
    File dadosFile1 = SD.open("DadosF.csv", FILE_WRITE); 
    if (dadosFile1) { 
      dadosFile1.print(diaDaSemana[now.dayOfTheWeek()]); 
      dadosFile1.print(","); 
      dadosFile1.print(now.day()); 
      dadosFile1.print("/"); 
      dadosFile1.print(now.month()); 
      dadosFile1.print("/"); 
      dadosFile1.print(now.year()); 
      dadosFile1.print(","); 
      dadosFile1.print(now.hour()); 
      dadosFile1.print(":"); 
      dadosFile1.print(now.minute()); 
      dadosFile1.print(":"); 
      dadosFile1.print(now.second()); 
      dadosFile1.print(","); 
      dadosFile1.print(tensaoFixa); 
      dadosFile1.print(","); 
      dadosFile1.print(corrente_total_fixa); 
      dadosFile1.println(); 
      dadosFile1.close(); // Fecha o arquivo 
      Serial.println("Dados da Placa Fixa gravados com sucesso"); 
    } else { 
      Serial.println("Erro ao abrir o arquivo DadosF.csv"); 
    } 
 
    File dadosFile2 = SD.open("DadosM.csv", FILE_WRITE); 
    if (dadosFile2) { 
      dadosFile2.print(diaDaSemana[now.dayOfTheWeek()]); 
      dadosFile2.print(","); 
      dadosFile2.print(now.day()); 
      dadosFile2.print("/"); 
      dadosFile2.print(now.month()); 
      dadosFile2.print("/"); 
      dadosFile2.print(now.year()); 
      dadosFile2.print(","); 
      dadosFile2.print(now.hour()); 
      dadosFile2.print(":"); 
      dadosFile2.print(now.minute()); 
      dadosFile2.print(":"); 
      dadosFile2.print(now.second()); 
      dadosFile2.print(","); 
      dadosFile2.print(tensaoMovel); 
      dadosFile2.print(","); 
      dadosFile2.print(corrente_total_movel); 
      dadosFile2.println(); 
      dadosFile2.close(); 
      Serial.println("Dados da Placa Movel gravados com sucesso"); 
    } else { 
      Serial.println("Erro ao abrir o arquivo DadosM.csv"); 
    } 
  } 
  delay(1000); 
}