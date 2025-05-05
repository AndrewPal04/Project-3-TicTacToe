#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "iPhone (8)";
const char* password = "palacios";
const char* mqtt_server = "34.82.67.83";//change here

WiFiClient espClient;
PubSubClient client(espClient);


char board[3][3];

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void printBoard() {
  Serial.println("Current Board:");
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      char ch = board[row][col];
      Serial.print(ch == '\0' ? '.' : ch);
      if (col < 2) Serial.print(" | ");
    }
    Serial.println();
    if (row < 2) Serial.println("--+---+--");
  }
  Serial.println();
}

bool checkWin(char symbol) {
  // Check all 3 rows
  for (int row = 0; row < 3; row++) {
    if (board[row][0] == symbol &&
        board[row][1] == symbol &&
        board[row][2] == symbol) {
      return true;
    }
  }

  // Check all 3 columns
  for (int col = 0; col < 3; col++) {
    if (board[0][col] == symbol &&
        board[1][col] == symbol &&
        board[2][col] == symbol) {
      return true;
    }
  }

  // Check top-left to bottom-right diagonal
  if (board[0][0] == symbol &&
      board[1][1] == symbol &&
      board[2][2] == symbol) {
    return true;
  }

  // Check top-right to bottom-left diagonal
  if (board[0][2] == symbol &&
      board[1][1] == symbol &&
      board[2][0] == symbol) {
    return true;
  }

  // If none matched, no win
  return false;
}


bool checkTie() {
  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++)
      if (board[r][c] == '\0')
        return false;
  return true;
}

void updateBoard(String move, char symbol) {
  int cell = move.toInt();
  if (cell < 1 || cell > 9) return;

  int row = (cell - 1) / 3;
  int col = (cell - 1) % 3;

  if (board[row][col] != '\0') {
    Serial.println("Invalid move: cell already taken.");
    return;
  }

  board[row][col] = symbol;
  printBoard();

  if (checkWin(symbol)) {
    Serial.print("Player ");
    Serial.print(symbol == 'X' ? "1" : "2");
    Serial.println(" wins!");
    // Optional: resetBoard();
  } else if (checkTie()) {
    Serial.println("It's a tie!");
    // Optional: resetBoard();
  }
}
void resetBoard() {
  memset(board, 0, sizeof(board));
  Serial.println("\n--- New Game ---");
  printBoard();
}


void callback(char* topic, byte* payload, unsigned int length) {
  String move = "";
  for (int i = 0; i < length; i++) {
    move += (char)payload[i];
  }

  if (String(topic) == "player1/move") {
    updateBoard(move, 'X');
  } else if (String(topic) == "player2/move") {
    updateBoard(move, 'O');
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("player1/move");
      client.subscribe("player2/move");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  memset(board, 0, sizeof(board)); // Clear board
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
