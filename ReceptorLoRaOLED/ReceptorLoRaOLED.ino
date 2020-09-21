#include "heltec.h"
#include "images.h"

#define BAND 433E6 //Define a frequência de operação de seu dispositivo

int tamPacote=0;
int txPower = 17; //Ganho de potência na transmissão
int spreadingFactor = 11; //Fator de espalhamento. Note que para haver comunicação, ambos dispositivos Sender e Receiver precisam estar operando com o mesmo fator.
//Caso deseje mudar o fator de espalhamento, basta enviar um pacote com o Sender com o valor desejado, por exemplo: 10. Dessa forma, o receiver ajustará o valor de seu fator de espalhamento.

void logoIFSC() //Ao iniciar, exibe o logo do Intituto Federal
{  
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,128,64,IFSC);
  Heltec.display->display();  
  delay(2000);
  Heltec.display->clear();
}

void imprimirOLED(String msg)
{
  Heltec.display->clear();
  Heltec.display->drawString(0,0,"Pacote foi recebido"); 
  Heltec.display->drawString(0,15,"Pacote: "+ msg); //0 - posição no eixo X, 15 posição no eixo Y
  Heltec.display->display();
}

void setup() {
  Heltec.begin(true, true, true, true, BAND); //Heltec.begin(Display, LoRa, Serial, Registrador PABOOST, Frequência);
  Serial.begin(9600);
  LoRa.setTxPower(txPower,RF_PACONFIG_PASELECT_PABOOST); //Define o ganho de potência em dBm definido no início do código
  LoRa.setSpreadingFactor(spreadingFactor); //Define o fator de espalhamento com o valor definido no início do código (7 ~ 12)
  LoRa.setCodingRate4(5); //Define a taxa de código da comunicação, valores vão de 5 até 8
  Heltec.display->init(); //Inicia o display da placa
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  logoIFSC(); //Exibe o logo do Institulo Federal, caso deseje pode ser alterado no arquivo images.h
  LoRa.receive(); //Coloca o dispositivo em recepção contínua, sempre aguardando um pacote.
}

void enviaResposta(String pacote)
{
  LoRa.beginPacket(); //Começa a sequencia para enviar algum pacote
  LoRa.setTxPower(txPower,RF_PACONFIG_PASELECT_PABOOST); //Configura o ganho de acordo com a variável que recebeu no pacote enviado pelo Sender.
  LoRa.setSpreadingFactor(spreadingFactor); //Configura o fator de espalhamento caso um número de 7 a 12 tenha sido enviado pelo Sender. Note que após configurar o fator de espalhamento, deve-se alterar para o mesmo valor no Sender para que haja novamente comunicação.
  LoRa.print(pacote); //Envia o pacote novamente ao Sender, onde fará a verificação.
  Serial.println("Pacote enviado: " + pacote);
  LoRa.endPacket(); //Finaliza a sequencia de enviar pacote, continuando a esperar por outro pacote.
  //Não consegue receber pacotes enquanto estiver enviando
}

void onReceive(int tamanhoPacote)
{
  String pacote = ""; //Variável criada dentro da função para ser preenchida toda vez que houver uma chamada na função onReceive().
  if(tamanhoPacote) //Verifica se realmente recebeu um pacote
  {    
    while(LoRa.available()) //Continuará fazendo até chegar ao final do pacote.
    {
      pacote += (char)LoRa.read(); //Preenche a String pacote com as letras da mensagem recebida.
    }
        
    if(tamanhoPacote<=2) //Caso deseje enviar com o Sender um pacote com apenas um número de 7 a 12 para que seja o novo fator de espalhamento.
    {
      spreadingFactor = pacote.toInt();
    }else //Caso a mensagem for maior, irá verificar qual o numerador do pacote, que corresponde ao ganho em potência da transmissão sendo utilizado.
    { 
      txPower = pacote.toInt();           
    }     
    //Imprime os dados desejados no Serial
    Serial.print("Pacote: " + pacote);
    Serial.print(" | txPOWER:  " + (String)txPower);
    Serial.print(" | Tamanho pacote: " + (String)tamanhoPacote); 
    Serial.println(" | Fator de espalhamento: " + (String)spreadingFactor);
    //Imprime o pacote recebido no display do ESP32
    imprimirOLED(pacote); 
    
    delay(10);
    enviaResposta(pacote); //Envia o pacote recebido para que o Sender possa comparar com a última mensagem que foi enviada.
    }
  }


void loop() {
  onReceive(LoRa.parsePacket()); //Caso o dispositivo receba um pacote, será executada esta função. LoRa.parsePacket() retorna o tamanho do pacote recebido.
}
