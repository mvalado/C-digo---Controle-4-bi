#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);  // Inicializa o display LCD

#define BASE 354  // Define o valor de base do microfone
#define KP 0.5 // Define o teu ganho proporcional
#define KI 0.00001 // Define o teu ganho integral
#define KD 0.1 // Define o teu ganho derivativo

#define NUM_READINGS 10  // Define o número de leituras para calcular a média

int readings[NUM_READINGS];  // Cria um array para armazenar as leituras do sensor
int readIndex = 0;  // Define o índice da leitura atual
int total = 0;  // Define a soma das leituras
int average = 0;  // Define a média das leituras
int lastError = 0;  // Define o último erro

void setup() {
  pinMode(A2, INPUT);  // Configura o pino A2 como entrada
  pinMode(3, OUTPUT);  // Configura o pino 3 como saída (este pino suporta PWM)

  // Inicia o display
  lcd.init();
  lcd.backlight();

  // Inicializa todas as leituras para 0
  for (int i = 0; i < NUM_READINGS; i++) {
    readings[i] = 0;
  }

  // Inicia a comunicação serial
  Serial.begin(9600);
}

void loop() {
  
  // Subtrai a última leitura
  total = total - readings[readIndex];
  
  // Lê o valor do sensor e armazena no array
  readings[readIndex] = analogRead(A2);

  // Adiciona a leitura ao total
  total = total + readings[readIndex];

  // Avança para a próxima posição no array
  readIndex = readIndex + 1;

  // Se estiveres no final do array, volta para o início
  if (readIndex >= NUM_READINGS) {
    readIndex = 0;
  }

  // Calcula a média
  average = total / NUM_READINGS;

  // Calcula o erro
  int error = average - BASE;

  // Calcula a saída de controle
  static int integral = 0;  // Integral do erro
  integral += error;
  int derivative = error - lastError;  // Derivada do erro
  lastError = error;
  int output = KP * error + KI * integral + KD * derivative;

  // Limpa o display
  lcd.clear();

  // Escreve o valor do sensor no display
  lcd.setCursor(0, 0);
  lcd.print("Valor: ");
  lcd.print(average);

  // Lineariza a saída
  int linearOutput = map(output, 0, 1023, -3, 150);

  lcd.setCursor(0, 1);
  lcd.print("Saida: ");
  lcd.print(linearOutput);

  // Ajusta a intensidade do LED com base no valor de saída
  int ledOutput = constrain(linearOutput, 0, 255);  // Garante que a saída esteja entre 0 e 255
  analogWrite(3, ledOutput);

  // Imprime a variável saída do controle na porta serial
  Serial.println(linearOutput);
}
