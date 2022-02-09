#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 50
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

Map readData(int h, int w, char myId[MAX_LINE]) {
  char id[MAX_LINE];
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
    scanf("%s %i %i", id, &x, &y); // lê o id dos bots e suas posições
    map.points[x][y].hasBoat = 1;
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
  while (1) {
    // LÊ OS DADOS DO JOGO E ATUALIZA OS DADOS DO BOT
    Map map = readData(h, w, myId);

    fprintf(stderr, "CONTEEEEEEEEM %d\n", map.points[0][0].hasBoat);

    // INSERIR UMA LÓGICA PARA ESCOLHER UMA AÇÃO A SER EXECUTADA

    // envia a ação escolhida (nesse exemplo, ir para esquerda)
    printf("LEFT\n");

    // lê qual foi o resultado da ação (e eventualmente atualiza os dados do bot).
    scanf("%s", line);
    // fgets(line, MAX_LINE, stdin);
  }

  return 0;
}