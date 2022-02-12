/*
 * PROJETO FINAL DE ITP
 *
 * Aluna: Fabiana Pereira Duarte
 * Turma: T02 - 2021.2
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_LINE 50
#define VALUE_MULLET 100
#define VALUE_SNAPPER 150
#define VALUE_SEABASS 200

//Status de um barco em um determinado ponto do mapa
enum boatStatus {
  noBoat,      //Não há barco nessa posição
  containBoat  //Há um barco nessa posição
};

//Ações do barco
enum action {
  up,
  down,
  left,
  right,
  sell,
  fish
};

typedef struct {
  int mullet;     //Tainha
  int snapper;    //Cioba
  int seabass;    //Robalo
  int total;      //Total de peixes (kg)
} Stock;

//Barco
typedef struct {
  char id[MAX_LINE];  //ID do barco
  int cash;           //Valor total dos peixes vendidos pelo barco
  Stock stock;        //Estoque de peixes do barco
  int x;
  int y;
} Boat;

//Ponto do mapa
typedef struct {
  int value;    //Valor
  int hasBoat;  //Informação de que há ou não barco no ponto do mapa
} Point;

//Mapa do jogo
typedef struct {
  Point** points; //Pontos do mapa
  int height;     //Altura do mapa
  int width;      //Largura do mapa
} Map;

//Inicializa o mapa e aloca memória para os pontos
Map initMap(int h, int w) {
  Map map;

  map.height = h;
  map.width = w;
  map.points = (Point**) calloc(h, sizeof(Point*));

  for (int i = 0; i < h; i++) {
    map.points[i] = (Point*) calloc(w, sizeof(Point));
  }

  return map;
}

//Inicializa o meu barco
Boat initBoat(char myId[MAX_LINE]) {
  Boat myBoat;

  strcpy(myBoat.id, myId);
  myBoat.stock.mullet = 0;
  myBoat.stock.seabass = 0;
  myBoat.stock.snapper = 0;
  myBoat.stock.total = 0;
  myBoat.cash = 0;

  return myBoat;
}

//Lê os dados do jogo e atualiza os dados do bot
Map readData(int h, int w, Boat* myBoat) {
  char idBot[MAX_LINE];
  int pointValue, numBots, x, y;

  Map map = initMap(h, w);

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      scanf("%i", &pointValue);
      map.points[i][j].value = pointValue;
      map.points[i][j].hasBoat = 0;
    }
  }

  scanf(" BOTS %i", &numBots); // lê a quantidade de bots

  for (int i = 0; i < numBots; i++) {
    scanf("%s %i %i", idBot, &x, &y); // lê o id dos bots e suas posições

    map.points[x][y].hasBoat = containBoat;

    if (strcmp(idBot, myBoat->id) == 0) {
      myBoat->x = x;
      myBoat->y = y;
    }
  }

  return map;
}

//Calcula distância entre dois pontos do mapa, dadas as suas coordenadas
double calculateDistance(int x1, int x2, int y1, int y2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

//Seleciona uma ação
char* setAction(int move) {
  char* command = (char*) calloc(MAX_LINE, sizeof(char));

  switch (move) {
    case up:
      strcpy(command, "UP");
      break;

    case down:
      strcpy(command, "DOWN");
      break;
    
    case left:
      strcpy(command, "LEFT");
      break;

    case right:
      strcpy(command, "RIGHT");
      break;
    
    case sell:
      strcpy(command, "SELL");
      break;

    case fish:
      strcpy(command, "FISH");
      break;
  }
  return command;
}

//Retorna coordenadas do porto mais próximo ao barco
int* getTheNearestPort(Map map, Boat myBoat) {
  int* portCoords = calloc(2, sizeof(int));
  double minDistance, distance;

  minDistance = calculateDistance(myBoat.x, (map.height-1), myBoat.y, (map.width-1));

  for (int i = 0; i < map.height; i++) {
    for (int j = 0; j < map.width; j++) {
      if (map.points[i][j].value == 1) {
        distance = calculateDistance(myBoat.x, i, myBoat.y, j);
        if (distance < minDistance) {
          minDistance = distance;
          portCoords[0] = i;
          portCoords[1] = j;
        }
      }
    }
  }

  return portCoords;
}

//Movimenta barco em direção ao porto
char* goToPort(Boat myBoat, int* port) {
  char* command = (char*) calloc(MAX_LINE, sizeof(char));

  if (myBoat.y > port[1]) {
    strcpy(command, setAction(left));
  }
  else if (port[1] > myBoat.y) {
    strcpy(command, setAction(right));
  }
  else if (myBoat.y == port[1]) {
    if (myBoat.x > port[0]) {
      strcpy(command, setAction(up));
    }
    else if (port[0] > myBoat.x) {
      strcpy(command, setAction(down));
    }
    else if (port[0] == myBoat.x) {
      strcpy(command, setAction(sell));
    }
  }

  return command; 
}

//Verifica se é uma área proibida
int isForbiddenPoint(Point point) {
  if (
    point.value == 10 ||
    point.value == 11 ||
    point.value == 20 ||
    point.value == 21 ||
    point.value == 30 || 
    point.value == 31 ||
    point.hasBoat == containBoat
  ) {
    return 1;
  }
  else {
    return 0;
  }
}

//Compara os valores dos pontos acima, abaixo, à esquerda e à direita e retorna o maior entre eles
int getHigherValue(int upValue, int downValue, int leftValue, int rightValue) {
  int higher;

  if (upValue >= downValue)
    higher = up;
  else
    higher = down;

  if (leftValue > higher)
    higher = left;
  
  if (rightValue > higher)
    higher = right;

  return higher;
}

//Analisa os pontos ao redor do barco e decide qual o movimento a ser realizado
char* getBestMoviment(Map map, Boat myBoat) {
  int upValue = -1, downValue = -1, leftValue = -1, rightValue = -1;
  int bestChoice;
  char* command = (char*) calloc(MAX_LINE, sizeof(char));

  if (myBoat.x - 1 >= 0) {
    Point point = map.points[myBoat.x-1][myBoat.y];
    if ((!isForbiddenPoint(point)))
      upValue = point.value;
  }

  if (myBoat.x + 1 < map.height) {
    Point point = map.points[myBoat.x+1][myBoat.y];
    if ((!isForbiddenPoint(point)))
      downValue = point.value;
  }

  if (myBoat.y - 1 >= 0) {
    Point point = map.points[myBoat.x][myBoat.y-1];
    if ((!isForbiddenPoint(point)))
      leftValue = point.value;
  }

  if (myBoat.x + 1 < map.height) {
    Point point = map.points[myBoat.x][myBoat.y+1];
    if ((!isForbiddenPoint(point)))
      rightValue = point.value;
  }

  bestChoice = getHigherValue(upValue, downValue, leftValue, rightValue);

  strcpy(command, setAction(bestChoice));

  return command;
}

//Movimenta o barco no mar
char* moveBoat(Map map, Boat myBoat) {
  char* command = (char*) calloc(MAX_LINE, sizeof(char));
  int myPoint = map.points[myBoat.x][myBoat.y].value;
  
  if (myBoat.stock.total == 10) {
    int* port = getTheNearestPort(map, myBoat);
    strcpy(command, goToPort(myBoat, port));
  }
  else if (myPoint != 0 && myPoint != 1) {
    if ((myPoint >= 32 && myPoint <= 39) || (myPoint >= 22 && myPoint <= 29) || (myPoint >= 12 && myPoint <= 19))
      strcpy(command, setAction(fish));
    else
      strcpy(command, getBestMoviment(map, myBoat));
  }
  else {
    strcpy(command, getBestMoviment(map, myBoat));
  }

  return command;
}

void updateMyBoat(Boat* myBoat, char line[MAX_LINE], char command[MAX_LINE]) {
  if (strcmp(command, "FISH") == 0) {
    if (strcmp(line, "NONE") != 0) {
      myBoat->stock.total++;

      if (strcmp(line, "SEABASS") == 0)
        myBoat->stock.seabass++;
      else if (strcmp(line, "SNAPPER") == 0)
        myBoat->stock.snapper++;
      else if (strcmp(line, "MULLET") == 0)
        myBoat->stock.mullet++;
    }
  }
  else if (strcmp(command, "SELL") == 0) {
    myBoat->cash = atoi(line);
  }
}

int main() {
  char line[MAX_LINE]; // dados temporários
  char myId[MAX_LINE]; // identificador do bot em questão
  char* command = (char*) calloc(MAX_LINE, sizeof(char));

  setbuf(stdin, NULL); // stdin, stdout e stderr não terão buffers
  setbuf(stdout, NULL); // assim, nada é "guardado temporariamente"
  setbuf(stderr, NULL);

  // === INÍCIO DA PARTIDA ===
  int h, w;
  scanf("AREA %i %i", &h, &w); // dimensão da área de pesca: altura (h) x largura (w)
  scanf(" ID %s", myId); // por fim, sabe qual seu próprio id

  Boat myBoat = initBoat(myId);

  // === PARTIDA ===
  Map map;
  // fica num laço infinito, pois quem vai terminar seu programa é o SIMULADOR.
  while (1) {
    map = readData(h, w, &myBoat);

    strcpy(command, moveBoat(map, myBoat));
    printf("%s\n", command);

    // lê qual foi o resultado da ação (e eventualmente atualiza os dados do bot).
    scanf("%s", line);
    updateMyBoat(&myBoat, line, command);
  }

  free(map.points);
  free(command);

  return 0;
}