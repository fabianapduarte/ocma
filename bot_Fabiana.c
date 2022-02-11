#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_LINE 50

//status possíveis de um barco para um determinado ponto do mapa
enum boatStatus {
  noBoat, //0 - Não há barco nessa posição
  containBoat, //1 - Há um barco nessa posição
  containMyBoat //2 - Meu barco está nessa posição
};

//direções que o barco pode se movimentar
enum action {
  up,
  down,
  left,
  right,
  sell,
  fish
};

//barco
typedef struct {
  int mullet; //tainha - R$100
  int snapper; //cioba - R$150
  int seabass; //robalo - R$200
  int cash; //valor total dos peixes vendidos pelo barco
  char id[MAX_LINE]; //id do barco
  int x;
  int y;
} Boat;

//ponto do mapa
typedef struct {
  int value; //valor
  int hasBoat; //informação de que há ou não barco
} Point;

//mapa do jogo
typedef struct {
  Point** points; //composto por pontos
  int height;
  int width;
} Map;

// inicializa o mapa e aloca memória para os pontos
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

// inicializa o meu barco
Boat initBoat(char myId[MAX_LINE]) {
  Boat myBoat;

  strcpy(myBoat.id, myId);
  myBoat.mullet = 0;
  myBoat.seabass = 0;
  myBoat.snapper = 0;
  myBoat.cash = 0;

  return myBoat;
}

// lê os dados do jogo e atualiza os dados do bot
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

    if (strcmp(idBot, myBoat->id) == 0) {
      map.points[x][y].hasBoat = containMyBoat;
      myBoat->x = x;
      myBoat->y = y;
    }
    else {
      map.points[x][y].hasBoat = containBoat;
    }
  }

  return map;
}

//calcula distância entre dois pontos do mapa, dadas as suas coordenadas
double calculateDistance(int x1, int x2, int y1, int y2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

//seleciona uma ação
void setAction(int move) {
  switch (move) {
    case up:
      printf("UP\n");
      break;

    case down:
      printf("DOWN\n");
      break;
    
    case left:
      printf("LEFT\n");
      break;

    case right:
      printf("RIGHT\n");
      break;
    
    case sell:
      printf("SELL\n");
      break;

    case fish:
      printf("FISH\n");
      break;

    //apagar depois
    default:
      printf("LEFT\n");
      break;
  }
}

//pegar o porto mais próximo do barco
int* getNearestPort(Map map, Boat myBoat) {
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

void goToPort(Boat myBoat, int* port) {
  if (myBoat.y > port[1]) {
    setAction(left);
  }
  else if (port[1] > myBoat.y) {
    setAction(right);
  }
  else if (myBoat.y == port[1]) {
    if (myBoat.x > port[0]) {
      setAction(up);
    }
    else if (port[0] > myBoat.x) {
      setAction(down);
    }
    else if (port[0] == myBoat.x) {
      setAction(sell);
    }
  }
}

void getBestMoviment(Map map, Boat myBoat) {
  int upValue = 0, downValue = 0, leftValue = 0, rightValue = 0;
  int bestChoice;

  if (myBoat.x - 1 >= 0) {
    int t = map.points[myBoat.x-1][myBoat.y].value;
    if (t >= 32 || t >= 22 || t >= 12)
      upValue = t;
    else
      upValue = -1;
  }

  if (myBoat.x + 1 < map.height) {
    int t = map.points[myBoat.x+1][myBoat.y].value;
    if (t >= 32 || t >= 22 || t >= 12)
      downValue = t;
    else
      downValue = -1;
  }

  if (myBoat.y - 1 >= 0) {
    int t = map.points[myBoat.x][myBoat.y-1].value;
    if (t >= 32 || t >= 22 || t >= 12)
      leftValue = t;
    else
      leftValue = -1;
  }

  if (myBoat.x + 1 < map.height) {
    int t = map.points[myBoat.x][myBoat.y+1].value;
    if (t >= 32 || t >= 22 || t >= 12)
      rightValue = t;
    else
      rightValue = -1;
  }

  if (upValue != -1 || downValue != -1) {
    if (upValue >= downValue && upValue != -1)
      bestChoice = up;
    else
      bestChoice = down;
  }

  if (leftValue != -1 && leftValue > bestChoice)
    bestChoice = left;
  
  if (rightValue != -1 && rightValue > bestChoice)
    bestChoice = right;

  setAction(bestChoice);
}

//movimenta o barco no mar
void moveBoat(Map map, Boat myBoat) {
  int totalFish = myBoat.mullet + myBoat.seabass + myBoat.snapper;
  int myPoint = map.points[myBoat.x][myBoat.y].value;
  
  if (totalFish == 10) {
    int* port = getNearestPort(map, myBoat);
    goToPort(myBoat, port);
  }
  else if (myPoint != 0 && myPoint != 1) {
    if (myPoint >= 32)
      setAction(fish);
    else if (myPoint >= 22)
      setAction(fish);
    else if (myPoint >= 12)
      setAction(fish);
    else
      getBestMoviment(map, myBoat);
  }
  else {
    getBestMoviment(map, myBoat);
  }
}

int main() {
  char line[MAX_LINE]; // dados temporários
  char myId[MAX_LINE]; // identificador do bot em questão

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

    moveBoat(map, myBoat);

    // lê qual foi o resultado da ação (e eventualmente atualiza os dados do bot).
    scanf("%s", line);
  }

  free(map.points);

  return 0;
}