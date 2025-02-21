# 📡 Decibelímetro Digital com Bitdoglab  

## 📝 **Descrição do Projeto**  
Este projeto desenvolve um **Decibelímetro Digital** utilizando a **placa Bitdoglab**, com o objetivo de medir a intensidade sonora do ambiente. Através de um microfone analógico conectado ao **ADC**, os níveis de som são processados e exibidos em um **display OLED**, indicando o valor médio e o pico da intensidade sonora em uma escala de **0 a 100** e em **decibéis (dB)**.  

---

## 🎯 **Objetivo**  
Criar uma ferramenta prática, de baixo custo e educativa para monitoramento de ruídos em ambientes internos, proporcionando dados precisos e uma interface amigável.  

---

## 💡 **Funcionalidades**  
- Medição do som ambiente em tempo real.  
- Exibição dos valores médios e de pico da intensidade sonora.  
- Conversão da intensidade sonora em uma escala de 0 a 100 e em decibéis.  
- Uso de display OLED para exibição clara das informações.  

---

## 🧠 **Componentes do Projeto**  

### **Hardware Utilizado**  
- **Placa Bitdoglab**  
- **Microfone Analógico** (Canal ADC 2)  
- **Display OLED SSD1306** (I2C: SDA no GPIO 14, SCL no GPIO 15)  
- **Botão de Amostragem** (GPIO 5)  
- **Fonte de Alimentação** (DC 5V)  
- Componentes adicionais: resistores, jumpers, protoboard.  

### **Software**  
- **Linguagem C**  
- **Bitdoglab SDK**  
- **Bibliotecas**: `ssd1306`, `pico/stdlib`, `hardware/adc`, `hardware/dma`, `hardware/i2c`.  
- **IDE**: Visual Studio Code (ou IDE compatível com Bitdoglab)  

---

## 🚦 **Como Executar o Projeto**  

### 1. **Pré-requisitos**  
- Clonar o repositório:  

```bash
git clone https://github.com/seu-usuario/nome-do-repositorio.git
cd nome-do-repositório

- Instalar o SDK da Bitdoglab e dependências.  

### 2. **Configurar o Hardware**
- Realizar as conexões do microfone, display OLED e botão conforme o diagrama de pinagem descrito na documentação.

### 3. **Compilar e Subir o Código**
- Compilar o código com o CMake:

```bash
mkdir build
cd build
cmake ..
make

- Enviar o firmware para a placa Bitdoglab.

### 4. **Executar o Decibelímetro**
- Conectar a placa Bitdoglab e iniciar o monitor serial para acompanhar as leituras do microfone.

## 🤝 **Contribuindo**
Sinta-se à vontade para contribuir com o projeto! Faça um fork, crie um branch para suas alterações e envie um pull request.

```bash
git checkout -b feature/nova-funcionalidade
git commit -m 'Adiciona nova funcionalidade'
git push origin feature/nova-funcionalidade


## 📧 **Contato**

Caso tenha dúvidas ou sugestões, entre em contato pelo e-mail: dennislopes@gmail.com
