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
    printf("LEFT\n");
  }
  else if (port[1] > myBoat.y) {
    printf("RIGHT\n");
  }
  else if (myBoat.y == port[1]) {
    if (myBoat.x > port[0]) {
      printf("UP\n");
    }
    else if (port[0] > myBoat.x) {
      printf("DOWN\n");
    }
    else if (port[0] == myBoat.x) {
      printf("SELL\n");
    }
  }
}

//movimenta o barco no mar
void moveBoat(Map map, Boat myBoat) {
  int totalFish = myBoat.mullet + myBoat.seabass + myBoat.snapper;
  
  if (totalFish == 10) {
    int* port = getNearestPort(map, myBoat);
    goToPort(myBoat, port);
  }
  else {
    printf("LEFT\n");
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