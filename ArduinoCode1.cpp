#include <Arduino.h>
#include <AFMotor.h>

AF_DCMotor motor1(1); // Motor esquerdo
AF_DCMotor motor2(2); // Motor direito

const int numSensores = 5; // Número de sensores
int pinosSensores[numSensores] = {A0, A1, A2, A3, A4}; // Pinos dos sensores

int sensorCentral = 2; // Índice do sensor central
int limiteDistancia = 20; // Distância limite para centralização do alvo (em cm)

// Variaveis de controle
unsigned long tempoAtualizacao = 100; // Intervalo de atualização em milissegundos
unsigned long ultimoTempoAtualizacao = 0; // Último tempo de atualização

//funções
int lerDistancia(int pinoSensor);
int encontrarSensorMaisProximo(int* leiturasDistancia);
void girarNoSentidoHorario();
void girarNoSentidoAntiHorario();
void pararMotores();
void atacar();


void setup() {
  Serial.begin(9600);

  // Configurar os motores da ponte H
  motor1.setSpeed(255); // Velocidade máxima do motor esquerdo
  motor2.setSpeed(255); // Velocidade máxima do motor direito
}

void loop() {
  // Verificar se é hora de atualizar
  if (millis() - ultimoTempoAtualizacao >= tempoAtualizacao) {
    // Realizar as leituras de distância
    int leiturasDistancia[numSensores];
    for (int i = 0; i < numSensores; i++) {
      leiturasDistancia[i] = lerDistancia(pinosSensores[i]);
    }
    
    // Encontrar o sensor mais próximo do alvo
    int sensorMaisProximo = encontrarSensorMaisProximo(leiturasDistancia);
    
    // Centralizar o alvo à frente do sensor central
    if (sensorMaisProximo != sensorCentral) {
      if (leiturasDistancia[sensorCentral] < limiteDistancia) {
        if (sensorMaisProximo < sensorCentral) {
          girarNoSentidoHorario();
        } else {
          girarNoSentidoAntiHorario();
        }
      } else {
        pararMotores();
      }
    } else {
      atacar();
    }
    
    // Imprimir as leituras dos sensores no monitor serial
    for (int i = 0; i < numSensores; i++) {
      Serial.print("Sensor ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(leiturasDistancia[i]);
      Serial.println(" cm");
    }
    
    Serial.println();
    
    ultimoTempoAtualizacao = millis(); // Atualizar o último tempo de atualização
  }
  
  // Outras ações contínuas do robô podem ser adicionadas aqui
}

// Função para ler a distância de um sensor ultrassônico
int lerDistancia(int pinoSensor) {
  long duracao;
  int distancia;
  
  pinMode(pinoSensor, OUTPUT);
  digitalWrite(pinoSensor, LOW);
  delayMicroseconds(2);
  digitalWrite(pinoSensor, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinoSensor, LOW);
  
  pinMode(pinoSensor, INPUT);
  duracao = pulseIn(pinoSensor, HIGH);
  
// Converter a duração do pulso em distância em centímetros
distancia = duracao * 0.034 / 2;

return distancia;
}

// Função para encontrar o índice do sensor com a menor distância
int encontrarSensorMaisProximo(int* leiturasDistancia) {
int sensorMaisProximo = 0;
int distanciaMinima = leiturasDistancia[0];

for (int i = 1; i < numSensores; i++) {
if (leiturasDistancia[i] < distanciaMinima) {
distanciaMinima = leiturasDistancia[i];
sensorMaisProximo = i;
}
}

return sensorMaisProximo;
}

// Função para acionar os motores no sentido horário
void girarNoSentidoHorario() {
motor1.run(BACKWARD);
motor2.run(BACKWARD);
}

// Função para acionar os motores no sentido anti-horário
void girarNoSentidoAntiHorario() {
motor1.run(FORWARD);
motor2.run(FORWARD);
}

// Função para parar os motores
void pararMotores() {
motor1.run(RELEASE);
motor2.run(RELEASE);
}

// Função para avançar - atacar
void atacar() {
    motor1.run(FORWARD);
    motor2.run(BACKWARD);
}