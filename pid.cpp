float y; // variável de processo
float sp = 30; // setpoint - valor entre -40 e 125 graus Celsius
int k = 120; // ganho de referência
float h = 0.1; // amostragem (digitalização)
int ti = 10; // tempo de integração
float kp = k * (1 + h / (2 * ti)); // ganho proporcional
float ki = k / ti; // ganho integral
float kd = k * (h / (2 * ti)); // ganho derivativo
float p = 0;
float i = 0;
float d = 0;
float prev_e = 0;

float feedforwardDisturb = 0; // Valor esperado do distúrbio

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(0)); // Inicializa a semente para gerar números aleatórios
}

void loop()
{
  if (Serial.available() > 0) {
    sp = Serial.parseFloat(); // Lê o valor digitado no Serial Monitor e atualiza o setpoint (sp)
  	Serial.print("Setpoint alterado para: ");
    Serial.println(sp);
  }
  
  y = map(analogRead(A0), 20, 358, -40, 125); // leitura da variável do processo
  
  // Verifica se deve ocorrer o distúrbio de forma aleatória
  if (random(3) == 0) {
    float disturbio = random(-3, 3); // Gera um valor aleatório entre -3 e 3 para o distúrbio
    y += disturbio; // Adiciona o distúrbio à temperatura
    feedforwardDisturb = disturbio; // Define o valor esperado do distúrbio
  } else {
    feedforwardDisturb = 0; // Sem distúrbio
  }
  
  Serial.print("VP: ");
  Serial.println(y);
  
  float e = sp - y; // erro do processo
  Serial.print("Erro: ");
  Serial.println(e);

  p = kp * e; // controle proporcional
  i += ki * h * e; // controle integral
  
  // Controle feedforward
  float ff = kp * feedforwardDisturb; // Termo de feedforward
  
  d = kd * (e - prev_e) / h; // controle derivativo
  prev_e = e;

  float u = p + i + d + ff; // sinal de controle (variável manipulada)
  u = map(u, -4098, 4098, 0, 255);
  Serial.print("MV = ");
  Serial.println(u);
  analogWrite(10, u); // escreve o sinal de controle no atuador
  
  delay(500); // ajuste o tempo de amostragem conforme necessário
}
