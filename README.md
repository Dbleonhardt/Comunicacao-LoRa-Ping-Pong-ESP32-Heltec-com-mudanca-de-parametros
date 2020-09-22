# Comunicação LoRa Ping Pong com configuração de parâmetros para dispositivo ESP32 da marca Heltec
Este é o projeto final da disciplina de sistemas de comunicações. Se trata de uma configuração para que haja uma comunicação LoRa do tipo Ping Pong onde pode ser configurados os parâmetros de transmissão, sendo analisados os dados dos pacotes recebidos do Receptor. 

O usuário pode ligar e desligar o áudio, controlar seu volume, controlar a frequência de sintonia, o método de demodulação empregado (ou nenhum para ouvir o sinal bruto) e ligar ou desligar um filtro de ruídos.

Os arquivos referentes ao projeto final estão na pasta nodejs. A pasta matlab contém os scripts de matlab usados para os testes iniciais do algoritmo e que foram mantidos aqui para fins didáticos.


## Objetivos
  - Verificar os efeitos da mudança dos parâmetros fator de espalhamento, ganho de potência na transmissão e distância na qualidade do envio de pacotes, sendo usados como fatores de resposta o RSSI, SNR e o PER (Taxa de perdas de pacotes).
  - Descobrir quais as melhores configurações do fator de espalhamento e ganho de potência na transmissão, dependendo da distância a ser empregada a comunicação.

## Dispositivo que envia e espera a recepção destes mesmos pacotes (Sender)
  O dispositivo envia um pacote para o outro dispositivo, e após isso fica esperando um pacote do Receptor. Existe um tempo configurável de 1 segundo, onde após passado esse tempo irá enviar novamente outro pacote para o Receptor, independentemente se recebeu um pacote de volta ou não. Caso o Sender receba um pacote de volta, o pacote somente será computado após uma verificação onde é comparado com o pacote enviado, caso afirmativo, será considerado como um envio válido.
  Ao receber a mensagem de volta que é então mensurado os parâmetros de qualidade do sinal, o RSSI e o SNR. Esses parâmetros são sempre somados, durante um período de dois minutos, onde será então calculada a média do RSSI, SNR e também feito o cálculo do PER. Note que, após o período de dois minutos, todas as variáveis de média e cálculo serão zeradas para que se comece um novo ciclo de envios.

## Dispositivo que recebe e envia novamente os pacotes (Receiver/Receptor)
  Esse dispositivo é responsável por receber o pacote enviado pelo Sender, analisando o conteúdo do pacote e configurar seu ganho de transmissão para o valor enviado no início da mensagem. Após fazer a configuração para o ganho de potência correto, o pacote recebido será enviado de volta. Exceto no momento de envio, o dispositivo sempre está à espera de uma mensagem do Sender.
  
## Materiais necessários:
  - 2x Dispositivo ESP32 LoRa v2 Heltec (433 MHz)  (Caso for outra frequência, basta mudar no código a variável BAND);
  - 2x Antenas da mesma frequência dos dispositivos.
  
## Iniciando
  Conecte as antenas nos dispositivos (### Não esquecer de conectar a antena sempre que for ligar o dispositivo), conecte ao computador, e instale a IDE do Arduino. Para instalar a biblioteca Heltec, basta acessar o arquivo "Como instalar biblioteca Heltec.txt" e seguir as instruções. Feito isso, basta copiar as duas pastas presentes aqui no GitHub, a pasta "ReceptorLoRaOLED" e "SenderLoRaOLED" para a pasta onde ficam salvos seus projetos do Arduino, por exemplo em "C:\Users\Usuario\Documents\Arduino".
  Ao compilar um código em cada dispositivo, lembrando de configurar o Coding Rate, frequência, fator de espalhamento e Bandwidth para os mesmos valores, basta esperar que o Sender começará a enviar os pacotes a cada 1 segundo. A média calculada a cada dois minutos será exibida no Serial do Arduino configurado como Sender.

  
