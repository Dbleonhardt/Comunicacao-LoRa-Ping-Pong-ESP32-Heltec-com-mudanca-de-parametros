//VERIFICAR SE AMBOS DISPOSITIVOS ESTÃO COM AS ANTENAS CONECTADAS ANTES DE TENTAR CONECTÁ-LOS

//INCLUDES
#include "heltec.h"
#include "images.h"

#define BAND 433E6 //Define a frequência de operação de seu dispositivo
#define MESSAGE "hello" //Conteúdo da mensagem enviada ao receptor
#define TEMPO 1000 //Intervalo de tempo entre cada envio de pacote
#define ZERARTEMPO 120000 //Intervalo de tempo para calcular as médias e zerar as contagens.
#define FESPALHAMENTO 11 //Valor que será mudado no receptor, caso envie a mensagem no MODO 1.
#define MODO 0 //0 = mensagem comum | 1 = mudança de fator de espalhamento
//Não esquecer de mudar o fator de espalhamento do Sender para ser a mesma do Receptor após mudar o fator do Receptor, caso contrário a conexão não irá ocorrer.


unsigned int counter = 0; //Contador de mensagens enviadas
unsigned int Rcounter = 0; //Contador de mensagens recebidas
unsigned long tempoAnterior; //variável para armazenar último período onde foi mandada a mensagem
unsigned long tempoZerar; //varíavel para armazenar último período onde foi calculado a média dos resultados
int enviou = 0;
String mensagemRecebida = "Aguardando";
String sentPacket = ""; //Variável onde é armazenado o último pacote enviado
int txPower = 14; //Ganho de potência na transmissão. Esse valor será utilizado pelo receptor também ao enviar a mensagem de volta
int spreadingFactor = 11; //Fator de espalhamento. Note que para haver comunicação, ambos dispositivos Sender e Receiver precisam estar operando com o mesmo fator.
double rssiSoma=0; //Variável para armazenar o valor RSSI do pacote recebido
double rssiAtual = 0; //Variável para armazenar o valor anterior do RSSI, de forma a captar apenas quando recebido
double snrSoma=0; //Variável para armazenar o valor SNR do pacote recebido
double snrAtual = 0;

void logoIFSC() //Ao iniciar o dispositivo, exibe o logo do Intituto Federal
{  
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,128,68,IFSC);
  Heltec.display->display();
  delay(2000);
  Heltec.display->clear();
}

void imprimirOLED(String msg, String receivedPacket, double rssiF, double snrF, int envOuRec)
{
  Heltec.display->clear();
  if(enviou == envOuRec) {rssiF = 0; snrF = 0;}  
  Heltec.display->drawString(0,0,"Enviado: "+ msg);
  Heltec.display->drawString(0,12,"Recebido: "+ receivedPacket); //0 é a posição no eixo X e 12 a posição no eixo Y.
  Heltec.display->drawString(0,26,"Pacotes recebidos: "+ (String)Rcounter);
  Heltec.display->drawString(0,37,"RSSI: " + (String)(rssiF) + "  SNR: " + (String)snrF);
  Heltec.display->drawString(0,48,"Tempo para Zerar: " + (String)((ZERARTEMPO - (millis()-tempoZerar))/1000) + "s");
  enviou = envOuRec; //Utiliza a variável enviou como um auxiliar para armazenar qual foi a ultima chamada da função
}

void enviarPacote(String mensagem)
{
  counter++; //Incrementa o contador de mensagens enviadas (após 2 minutos o contador é zerado).
  LoRa.beginPacket(); //Começa a sequencia para enviar algum pacote
  if(MODO)
  {
    sentPacket = FESPALHAMENTO; //O pacote enviado será apenas o fator de espalhamento que o Receptor irá usar como parâmetro.
  }else
  {
    sentPacket = (String)txPower + "|" + mensagem + " " + (String)counter; //O pacote enviado será a mensagem completa, por exemplo:   17|mensagem 25 .
  }
  LoRa.print(sentPacket); //Envia o pacote armazenado em sentPacket.
  LoRa.endPacket(); //Finaliza a sequencia de enviar pacote, continuando a esperar por outro pacote.
  //Lembrando que o dispositivo não consegue receber pacotes enquanto estiver enviando
  imprimirOLED(sentPacket, mensagemRecebida, rssiAtual,snrAtual,1); //Exibe na tela o pacote enviado
  //Serial.print("Pacote enviado: " + sentPacket);
}

void onReceive(int tamanho)
{
  String receivedPacket = ""; //Irá armazenar o pacote recebido do Receptor, toda vez que receber um pacote a variável irá iniciar vazia novamente.
  if(tamanho == 0) {return;} //Verifica se realmente recebeu um pacote.
  while(LoRa.available()) //Continuará fazendo até chegar ao final do pacote.
  {
    receivedPacket += (char)LoRa.read(); //Preenche a String receivedPacket com as letras da mensagem recebida.
  }
  mensagemRecebida = receivedPacket;
  if (receivedPacket != sentPacket) { //Verifica se a mensagem enviada por último é igual a recebida pelo receptor.
    Serial.println("\n### Mensagem diferente ###");
    return;                             
  }else Rcounter++; //Incrementa o contador de mensagens recebidas (após 2 minutos o contador é zerado).
  
  //Serial.println(" | Mensagem Recebida do receptor: " + receivedPacket);
  
  //Incremento dos resultados do pacote recebido (após 2 minutos são zerados)
  rssiAtual = LoRa.packetRssi();
  snrAtual = LoRa.packetSnr();
  rssiSoma = rssiSoma + rssiAtual;
  snrSoma = snrSoma + snrAtual;
  imprimirOLED(sentPacket, mensagemRecebida, rssiAtual, snrAtual,0); //Exibe na tela o pacote enviado e recebido com variaveis
}

void setup() {
  Heltec.begin(true, true, true, true, BAND); //Heltec.begin(Display, LoRa, Serial, Registrador PABOOST, Frequência);
  LoRa.setTxPower(txPower,RF_PACONFIG_PASELECT_PABOOST); //Define o ganho de potência em dBm definido no início do código.
  LoRa.setSpreadingFactor(spreadingFactor); //Define o fator de espalhamento com o valor definido no início do código (7 ~ 12)
  LoRa.setCodingRate4(5); //Define a taxa de código da comunicação, valores vão de 5 até 8
  Serial.begin(9600);
  Heltec.display->init(); //Inicia o display da placa.
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  logoIFSC(); //Exibe o logo do Institulo Federal, caso deseje pode ser alterado no arquivo images.h
  LoRa.receive(); //Coloca o dispositivo em recepção contínua, sempre aguardando um pacote, exceto enquanto estiver enviando.
  Serial.println();
}

void loop() {
  if((millis()-tempoAnterior) >= TEMPO) //Envia um pacote a cada valor configurado em TEMPO.
  {
    enviarPacote(MESSAGE);
    tempoAnterior = millis();
  }
  onReceive(LoRa.parsePacket()); //Caso o dispositivo receba um pacote, será executada esta função. LoRa.parsePacket() retorna o tamanho do pacote recebido.
  Heltec.display->display();
  if((millis()-tempoZerar) >= ZERARTEMPO) //A cada 2 minutos será feita a média dos resultados obtidos durante esse tempo.
  {
    if(Rcounter > counter) Rcounter = counter;
    double per = ((double)Rcounter/counter)*(double)100;
    Serial.println("Pacotes enviados: " + (String)counter + " | Pacotes recebidos:  " + (String)Rcounter);
    Serial.println("RSSI: " + (String)(rssiSoma/Rcounter) + " | SNR: " + (String)(snrSoma/Rcounter) + " | 1 - PER: " + (String)(100-per) + "%"); //RSSI e SNR divididos por Rcounter retornam o valor médio, ou seja, o valor por pacote recebido.
    //É imprimido 100-PER para melhor visualização de quantos % de pacotes não foram recebidos
    Serial.println("Ganho: " + (String)(txPower) + " | Fator de espalhamento: " + (String)spreadingFactor + "\n");
    Serial.println("----------------------------------------------------------------------------------------------");
    //Variáveis são zeradas para que nos próximos 2 minutos seja feito o somatório novamente.
    Rcounter = 0;
    counter = 0;
    mensagemRecebida = "Aguardando";
    rssiSoma = 0;
    snrSoma = 0;
    tempoZerar = millis();
  }
}
