  #include <WiFi.h>
  #include <HTTPClient.h>

  // Configurações de Wi-Fi
  const char* ssid = "Flávio";
  const char* password = "Senha1234";

  // Configurações do CallMeBot
  const String token = "5475968";
  const String phoneNumber = "557996919880";
  const String message = "Olá, esta é uma mensagem enviada pelo ESP32 via CallMeBot!";

  void setup() {
    // Inicializa a comunicação serial
    Serial.begin(115200);

    // Conecta no Wi-Fi
    Serial.println("Conectando ao WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("Conectado ao WiFi");

    // Envia a mensagem
    sendMessage();
  }

  void loop() {
    // O loop pode estar vazio ou com outras funções a serem realizadas.
  }

  // Função para enviar a mensagem via WhatsApp usando CallMeBot
  void sendMessage() {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      // Codificando a mensagem para garantir que os caracteres especiais sejam tratados corretamente
      String encodedMessage = urlencode(message);

      // Monta a URL da API do CallMeBot
      String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&text=" + encodedMessage + "&apikey=" + token;

      // Envia a requisição GET para a API do CallMeBot
      http.begin(url);
      int httpCode = http.GET();

      // Verifica o status da resposta
      if (httpCode > 0) {
        Serial.println("Mensagem enviada com sucesso!");
        String payload = http.getString();
        Serial.println(payload); // Imprime a resposta da API (geralmente confirmação de envio)
      } else {
        Serial.println("Erro ao enviar a mensagem");
      }

      // Fecha a conexão HTTP
      http.end();
    } else {
      Serial.println("Erro na conexão WiFi");
    }
  }

  // Função para codificar URL
  String urlencode(String str) {
    String encoded = "";
    char c;
    for (int i = 0; i < str.length(); i++) {
      c = str.charAt(i);
      if (c == ' ') {
        encoded += "%20";
      } else if (c == '!') {
        encoded += "%21";
      } else if (c == '"') {
        encoded += "%22";
      } else if (c == '#') {
        encoded += "%23";
      } else if (c == '$') {
        encoded += "%24";
      } else if (c == '%') {
        encoded += "%25";
      } else if (c == '&') {
        encoded += "%26";
      } else if (c == '\'') {
        encoded += "%27";
      } else if (c == '(') {
        encoded += "%28";
      } else if (c == ')') {
        encoded += "%29";
      } else if (c == '*') {
        encoded += "%2A";
      } else if (c == '+') {
        encoded += "%2B";
      } else if (c == ',') {
        encoded += "%2C";
      } else if (c == '-') {
        encoded += "%2D";
      } else if (c == '.') {
        encoded += "%2E";
      } else if (c == '/') {
        encoded += "%2F";
      } else if (c == ':') {
        encoded += "%3A";
      } else if (c == ';') {
        encoded += "%3B";
      } else if (c == '<') {
        encoded += "%3C";
      } else if (c == '=') {
        encoded += "%3D";
      } else if (c == '>') {
        encoded += "%3E";
      } else if (c == '?') {
        encoded += "%3F";
      } else if (c == '@') {
        encoded += "%40";
      } else if (c == '[') {
        encoded += "%5B";
      } else if (c == '\\') {
        encoded += "%5C";
      } else if (c == ']') {
        encoded += "%5D";
      } else if (c == '^') {
        encoded += "%5E";
      } else if (c == '_') {
        encoded += "%5F";
      } else if (c == '`') {
        encoded += "%60";
      } else if (c == '{') {
        encoded += "%7B";
      } else if (c == '|') {
        encoded += "%7C";
      } else if (c == '}') {
        encoded += "%7D";
      } else if (c == '~') {
        encoded += "%7E";
      } else {
        encoded += c;
      }
    }
    return encoded;
  }