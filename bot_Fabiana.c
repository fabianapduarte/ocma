#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 50

//barco
typedef struct {
  int numFish; //quantidade de kg de peixes pescados
  int cash; //valor total dos peixes vendidos pelo barco
  char id[MAX_LINE]; //id do barco
} Boat;

//ponto do mapa
typedef struct {
  int value; //valor
  int hasBoat; //informação de que há ou não barco
  Boat boat; //barco
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

// lê os dados do jogo e atualiza os dados do bot
Map readData(int h, int w, char myId[MAX_LINE]) {
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
    map.points[x][y].hasBoat = 1;

    if (strcmp(idBot, myId) == 0) { // se o id do bot for igual ao meu, inicia o barco com dados do bot
      map.points[x][y].boat.cash = 0;
      map.points[x][y].boat.numFish = 0;
      strcpy(map.points[x][y].boat.id, myId);
    }
  }

  return map;
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
  // readData(h, w); // lê os dados do jogo
  scanf(" ID %s", myId); // por fim, sabe qual seu próprio id
  fprintf(stderr, "%s\n", myId);

  // === PARTIDA ===
  // fica num laço infinito, pois quem vai terminar seu programa é o SIMULADOR.
  Map map;
  while (1) {
    map = readData(h, w, myId);

    // INSERIR UMA LÓGICA PARA ESCOLHER UMA AÇÃO A SER EXECUTADA

    // envia a ação escolhida (nesse exemplo, ir para esquerda)
    printf("LEFT\n");

    // lê qual foi o resultado da ação (e eventualmente atualiza os dados do bot).
    scanf("%s", line);
    // fgets(line, MAX_LINE, stdin);
  }

  free(map.points);

  return 0;
}