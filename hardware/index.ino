

#include <driver/i2s.h>
#include <WiFi.h>
#include <WebSocketsClient_Generic.h>

//Micro
#define I2S_SD 23
#define I2S_WS 19
#define I2S_SCK 18
#define I2S_MIC_PORT I2S_NUM_0

//Speaker
#define I2S_DATA_OUT_TX  25
#define I2S_WS_TX  5
#define I2S_SCK_TX 17
#define I2S_SPEAKER_PORT I2S_NUM_1

#define bufferCnt 10
#define bufferLen 1024
int16_t sBuffer[bufferLen];

const char* ssid = "your-ssid";
const char* password = "your-pass";

WebSocketsClient webSocket;
bool isWebSocketConnected = false;
bool isListeningMode = false;
bool isSpeakingMode = false;


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("Desconectado!");
            isWebSocketConnected = false;
            break;
        case WStype_CONNECTED:
            Serial.printf("Conectado a URL: %s\n", payload);
            isWebSocketConnected = true;
            break;
        case WStype_TEXT:
            Serial.printf("[WS] Mensaje recibido: %s\n", payload);
            if (strcmp((char*)payload, "STOP_LISTENING") == 0) {
                isListeningMode = false;
                Serial.println("Stop Listening!");
                stopI2S_mic();
            }
            if (strcmp((char*)payload, "START_LISTENING") == 0) {
                i2s_install_mic();
                i2s_setpin_mic();
                i2s_start(I2S_MIC_PORT);
                isListeningMode = true;
                Serial.println("Listening!");
            }
            if (strcmp((char*)payload, "STOP_RECORDING") == 0) {
                stop_speaker_I2S();
                isSpeakingMode = false;
            }
            if (strcmp((char*)payload, "START_RECORDING") == 0) {
                i2s_install_speaker();
                i2s_setpin_speaker();
                isSpeakingMode = true;
            }

            break;
        case WStype_BIN:
            if(isSpeakingMode){
              Serial.printf("[WS] Paquete binario recibido, tamaño: %d bytes\n", length);
              if (!isWebSocketConnected) {
                  Serial.println("[I2S] Reiniciando...");
                  restartI2S_speaker();
                  isWebSocketConnected = true;
              }
              size_t bytesWritten = 0;
              i2s_write(I2S_SPEAKER_PORT, payload, length, &bytesWritten, portMAX_DELAY);
              Serial.printf("[I2S] Escrito %d bytes al I2S\n", bytesWritten);
              }
            break;
        // Puedes agregar más casos si es necesario.
    }
}
void loop() {
  webSocket.loop();
  // Envío de datos de audio si está en modo de escucha
  if (isListeningMode) {
    size_t bytesIn = 0;
    esp_err_t result = i2s_read(I2S_MIC_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
    if (result == ESP_OK && isWebSocketConnected) {
      Serial.println("Sending data!");
      webSocket.sendBIN((const uint8_t*)sBuffer, bytesIn);
    }
  }
}

void setup() {
  Serial.begin(115200);

  connectWiFi();
  //Para evitar que se oiga ruido en altavoz al abrir por primera vez
  i2s_install_speaker();
  i2s_setpin_speaker();
  stop_speaker_I2S();
  
  connectWSServer();
}


void connectWiFi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void connectWSServer() {
  webSocket.beginSSL("examiun.com", 8888);
  webSocket.onEvent(webSocketEvent);
}

//Micro

void stopI2S_mic() {
    i2s_stop(I2S_MIC_PORT);
    Serial.println("[I2S] Detenido");
    i2s_driver_uninstall(I2S_MIC_PORT);
    Serial.println("[I2S] Desinstalado");
}

void i2s_install_mic() {
  // Set up I2S Processor configuration
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    //.sample_rate = 44100,
    .sample_rate = 16000,
    //.sample_rate = 22050,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    //.intr_alloc_flags = 0,
    .dma_buf_count = bufferCnt,
    .dma_buf_len = bufferLen,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_driver_install(I2S_MIC_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin_mic() {
  // Set I2S pin configuration
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_MIC_PORT, &pin_config);
}


//Altavoz
void stop_speaker_I2S() {
    i2s_stop(I2S_SPEAKER_PORT);
    Serial.println("[I2S] Detenido");
    i2s_driver_uninstall(I2S_SPEAKER_PORT);
    Serial.println("[I2S] Desinstalado");
}
void restartI2S_speaker() {
    i2s_stop(I2S_SPEAKER_PORT);
    Serial.println("[I2S] Detenido");
    i2s_driver_uninstall(I2S_SPEAKER_PORT);
    Serial.println("[I2S] Desinstalado");
    i2s_install_speaker();
    i2s_setpin_speaker();
}

void i2s_install_speaker() {
  // Set up I2S Processor configuration
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 16000,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = bufferCnt,
    .dma_buf_len = bufferLen,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_driver_install(I2S_SPEAKER_PORT, &i2s_config, 0, NULL);
  Serial.println("[I2S] Instalado");
}

void i2s_setpin_speaker() {
  // Set I2S pin configuration
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK_TX,
    .ws_io_num = I2S_WS_TX,
    .data_out_num = I2S_DATA_OUT_TX,
    .data_in_num = -1
  };

  i2s_set_pin(I2S_SPEAKER_PORT, &pin_config);
  Serial.println("[I2S] Pines configurados");
}
