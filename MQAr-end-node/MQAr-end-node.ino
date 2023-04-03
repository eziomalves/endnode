/*
 * Autor: Eziom Alves
 * Data: 21 de Outubro de 2022.
*/

//instalar a biblioteca pms através  do menu sketch (ctrl+shift+i) PMS Library versão 1.1.0
#include "PMS.h"
#include "heltec.h"

/*
 * Defines do projeto
 */
//Definições da comunicação LoRa
#define BAND    915E6  //Escolha da frequência
//Pinos da comunicação serial com o sensor PMS
#define MYPORT_RX 12
#define MYPORT_TX 13

/* Definições - deep sleep */
#define FATOR_US_PARA_S   1000000  /* Fator de conversão de micro-segundos para segundos */
#define TEMPO_DEEP_SLEEP  900       /* Tempo para o ESP32 ficar em deep sleep (segundos) */

//Sensor de material particulado
PMS pms(Serial2);
PMS::DATA data;

/* typedefs */
typedef struct __attribute__((__packed__))  
{
  float PM25;
  float PM10;
}TDadosLora;

/* prototypes */
void envia_informacoes_lora(float PM25, float PM10);

/*
 * Implementações
 */

/* 
 * Função: envia por LoRa as informações de concetração de PM2,5 e PM10
 * Parâmetros: - Pm2,5 lido 
 *             - PM10 lido
 * Retorno: nenhum
 */
void envia_informacoes_lora(float PM25, float PM10){
    TDadosLora dados_lora;

    dados_lora.PM25 = PM25;
    dados_lora.PM10 = PM10;
    
    LoRa.beginPacket();
    LoRa.write((unsigned char *)&dados_lora, sizeof(TDadosLora));
    LoRa.endPacket();
}

void setup() {
    Serial2.begin(9600, SERIAL_8N1, MYPORT_RX, MYPORT_TX, false); 

  
    Heltec.begin( false /*Habilita o Display*/, 
                  true /*Heltec.Heltec.Heltec.LoRa Disable*/, 
                  true /*Habilita debug Serial*/, 
                  true /*Habilita o PABOOST*/, 
                  BAND /*Frequência BAND*/);
    delay(1000);
    //configuração para modo sleep
    esp_sleep_enable_timer_wakeup(TEMPO_DEEP_SLEEP * FATOR_US_PARA_S);
}

/*
 * Programa principal
 */
void loop() {

  //retira o sensor de modo sleep
  pms.wakeUp();
  //tempo necessário para o sensor recalibrar
  delay(30000);
  
  /*Envia requisição de leitura*/
  pms.requestRead();

  
  if (pms.readUntil(data)){
    envia_informacoes_lora(data.PM_AE_UG_2_5, data.PM_AE_UG_10_0);
  }

  //coloca o sensor em modo sleep 
  pms.sleep();
  
  /*Coloca o esp32 em modo deep sleep*/
  esp_deep_sleep_start();
}
