/* 
PROJETO SISTEMA DE MONITORAMENTO DE LIXO: IoT com ESP8266 
AUTORES: EMANUEL TIAGO, EDUARDO NUNES E DOUGLAS ROSAS
*/

#include <PubSubClient.h> //Biblioteca para utilizanção do MQTT
#include <ESP8266WiFi.h> //Biblioteca do ESP8266 wifi
#include <Ultrasonic.h>//Biblioteca do sensor ultrasonico
#define tempo 400
#define LED D2
#define BUZZ D8
#define trigger D5
#define echo D6


 
Ultrasonic ultrasonic(trigger, echo);// define as porta trigger e echo, respectivamente
int distancia;
char variavel[16];
const char* ssid = "Polux";
const char* password = "odisseia";
//const char* ssid = "REDE EMANUEL 2.4"; //Nome da Rede WIFI que está usando
//const char* password = "hellboy0"; // Senha da Rede Wifi que está usando
//const char* ssid = "Fablab";
//const char* password = "@fablab2020!"; 

const char* topic_sub1 = "ultra-34"; //Topico onde o esp é um assinant
const char* topic_pub = "ultra-455";
const char* mqtt_id = "esp8266-Emanuel";//ID do usuario, deve ser unico para cada esp8266

const char* BROKER_MQTT = "test.mosquitto.org"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; //Porta do Broker MQTT

WiFiClient espClient; //Cria o objeto espClient do tipo WiFiclient
PubSubClient MQTT(espClient); //Instancia o cliente Mqtt passando o objeto espClient

void initMQTT(void); //Função responsavel por iniciar a comunicação com o broker mqtt
void mqtt_callback(char* topic, byte* payload, unsigned int length); //Função responsavel receber os dados do broker mqtt e definir as variaveis
void reconnectMQTT(void); //Função responsavel por realizar a conexão com o broker mqtt
void VerificaConexaoMQTT(void); //Função responsavel por verificar a conexão com o broker mqtt

void setup() { 
  Serial.begin(115200); // inicia a serial
  pinMode(BUZZ,OUTPUT);// Define o pino de saida para o controle do Buzzer
  pinMode(LED, OUTPUT);// Define o pino de saida para o LED de aviso
  Serial.println("Lendo dados do sensor...");
  WiFi.begin(ssid, password);// Faz a conexão com o Wifi
  while (WiFi.status() != WL_CONNECTED) { //Verifica se a conexão com a rede foi realizada e só vai sair do laço quando a conexão foi realizada
    delay(500);
    Serial.print(".");
   }
  Serial.println("WiFi Connected .... IP Address:"); //IP da rede
  Serial.println(WiFi.localIP());
  initMQTT(); // Chama a função initMQTT que pode ser vista depois do loop 
  
}

void loop() {
distancia = ultrasonic.read();
  
  Serial.print("Distance in CM: ");
  Serial.println(distancia);
  if (distancia <10){
    
  tone(BUZZ,392,tempo); //LA
  delay(tempo);
  tone(BUZZ,528,tempo); //LA
  delay(tempo);
  Serial.println("tira o lixo");
  }
  VerificaConexaoMQTT(); //Chama a função VerificaConexaoMQTT
  MQTT.loop(); //Necessario para ficar em contato comunicação com o broker
  MQTT.publish(topic_pub,itoa(distancia, variavel, 10));//Envio do valor da distancia pelo mqtt
  delay(1000);
}

void initMQTT(void)
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}

void mqtt_callback (char* topic, byte* payload, unsigned int length)
{
    String mensagem; // Obtem a string do payload recebido 
    for(int i = 0; i < length; i++) // laço de repetição responsável por converter a mensagem
    {
       char c = (char)payload[i];
       mensagem += c;
    }
