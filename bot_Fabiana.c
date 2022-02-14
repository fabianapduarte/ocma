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

//Status de um barco em um determinado ponto do mapa
enum boatStatus {
  noBoat,     //Não há barco nessa posição
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

//Barco
typedef struct {
  char id[MAX_LINE];  //ID do barco
  int stock;          //Estoque de peixes do barco
  int row;
  int column;
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
  myBoat.stock = 0;

  return myBoat;
}

//Lê os dados do jogo e atualiza os dados do bot
Map readData(int h, int w, Boat* myBoat) {
  char idBot[MAX_LINE];
  int pointValue, numBots, row, column;

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
    scanf("%s %i %i", idBot, &row, &column); // lê o id dos bots e suas posições

    map.points[row][column].hasBoat = containBoat;

    if (strcmp(idBot, myBoat->id) == 0) {
      myBoat->row = row;
      myBoat->column = column;
    }
  }

  return map;
}

//Calcula a distância entre dois pontos do mapa, dadas as suas coordenadas
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

  minDistance = calculateDistance(0, (map.height-1), 0, (map.width-1));

  //percorre o mapa e procura o porto mais próximo, ou seja, com a menor distância até o barco
  for (int i = 0; i < map.height; i++) {
    for (int j = 0; j < map.width; j++) {
      if (map.points[i][j].value == 1 && map.points[i][j].hasBoat == noBoat) {
        distance = calculateDistance(myBoat.row, i, myBoat.column, j);
        if (distance <= minDistance) {
          minDistance = distance;
          portCoords[0] = i; //linha
          portCoords[1] = j; //coluna
        }
      }
    }
  }

  return portCoords;
}

//Verifica se é uma área proibida
int isForbiddenPoint(int value) {
  if (value == 10 || value == 11 || value == 20 || value == 21 || value == 30 ||  value == 31)
    return 1;
  else
    return 0;
}

//Verifica se um ponto do mapa é válido para pesca
int isFishingArea(int value) {
  if (value != 0 && value != 1 && isForbiddenPoint(value) == 0)
    return 1;
  else
    return 0;
}

//Retorna coordenadas do ponto de pesca válido mais próximo ao barco
int* getTheNearestFishableArea(Map map, Boat myBoat) {
  int* portCoords = calloc(2, sizeof(int));
  double minDistance, distance;

  //Calcula o máximo valor possível para servir de comparação posteriormente ao percorrer o mapa
  minDistance = calculateDistance(0, (map.height-1), 0, (map.width-1));

  //percorre o mapa e procura o ponto de pesca válido mais próximo, ou seja, com a menor distância até o barco
  for (int i = 0; i < map.height; i++) {
    for (int j = 0; j < map.width; j++) {
      if (isFishingArea(map.points[i][j].value) == 1 && map.points[i][j].hasBoat == noBoat) {
        distance = calculateDistance(myBoat.row, i, myBoat.column, j);
        if (distance < minDistance) {
          minDistance = distance;
          portCoords[0] = i; //linha
          portCoords[1] = j; //coluna
        }
      }
    }
  }

  return portCoords;
}

//Movimenta barco em direção a uma posição dadas as suas coordenadas
char* goToPosition(Map map, Boat myBoat, int* coords) {
  char* command = (char*) calloc(MAX_LINE, sizeof(char));

  //verifica se o movimento não vai para fora da área de pesca e se não há barco no destino
  //se houver, verifica outras possibilidades de movimento

  if (myBoat.column > coords[1]) {
    if (myBoat.column - 1 >= 0 && map.points[myBoat.row][myBoat.column-1].hasBoat == noBoat) {
      strcpy(command, setAction(left));
    }
    else if (myBoat.row - 1 >= 0 && map.points[myBoat.row-1][myBoat.column].hasBoat == noBoat) {
      strcpy(command, setAction(up));
    }
    else if (myBoat.row + 1 <= map.height - 1 && map.points[myBoat.row+1][myBoat.column].hasBoat == noBoat) {
      strcpy(command, setAction(down));
    }
    else if (myBoat.column + 1 <= map.width - 1) {
      strcpy(command, setAction(right));
    }
  }
  
  if (coords[1] > myBoat.column) {
    if (myBoat.column + 1 <= map.width - 1 && map.points[myBoat.row][myBoat.column+1].hasBoat == noBoat) {
      strcpy(command, setAction(right));
    }
    else if (myBoat.row - 1 >= 0 && map.points[myBoat.row-1][myBoat.column].hasBoat == noBoat) {
      strcpy(command, setAction(up));
    }
    else if (myBoat.row + 1 <= map.height - 1 && map.points[myBoat.row+1][myBoat.column].hasBoat == noBoat) {
      strcpy(command, setAction(down));
    }
    else if (myBoat.column - 1 >= 0) {
      strcpy(command, setAction(left));
    }
  }

  if (myBoat.row > coords[0]) {
    if (myBoat.row - 1 >= 0 && map.points[myBoat.row-1][myBoat.column].hasBoat == noBoat) {
      strcpy(command, setAction(up));
    }
    else if (myBoat.column - 1 >= 0 && map.points[myBoat.row][myBoat.column-1].hasBoat == noBoat) {
      strcpy(command, setAction(left));
    }
    else if (myBoat.column + 1 <= map.width - 1 && map.points[myBoat.row][myBoat.column+1].hasBoat == noBoat) {
      strcpy(command, setAction(right));
    }
    else if (myBoat.row + 1 <= map.height - 1) {
      strcpy(command, setAction(down));
    }
  }
  else if (coords[0] > myBoat.row) {
    if (myBoat.row + 1 <= map.height - 1 && map.points[myBoat.row+1][myBoat.column].hasBoat == noBoat) {
      strcpy(command, setAction(down));
    }
    else if (myBoat.column - 1 >= 0 && map.points[myBoat.row][myBoat.column-1].hasBoat == noBoat) {
      strcpy(command, setAction(left));
    }
    else if (myBoat.column + 1 <= map.width - 1 && map.points[myBoat.row][myBoat.column+1].hasBoat == noBoat) {
      strcpy(command, setAction(right));
    }
    else if (myBoat.row - 1 >= 0) {
      strcpy(command, setAction(up));
    }
  }

  return command; 
}

//Compara os valores dos pontos acima, abaixo, à esquerda e à direita e retorna o maior entre eles
int getHigherValue(int upValue, int downValue, int leftValue, int rightValue) {
  int higher = -1;

  if (upValue > higher)
    higher = up;

  if (downValue > higher)
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

  //Verifica se o barco está nos limites da área de pesca
  //acima
  if (myBoat.row - 1 >= 0) {
    Point point = map.points[myBoat.row-1][myBoat.column];
    if (isFishingArea(point.value) == 1 && point.hasBoat == noBoat)
      upValue = point.value;
  }

  //abaixo
  if (myBoat.row + 1 <= map.height - 1) {
    Point point = map.points[myBoat.row+1][myBoat.column];
    if (isFishingArea(point.value) == 1 && point.hasBoat == noBoat)
      downValue = point.value;
  }

  //à esquerda
  if (myBoat.column - 1 >= 0) {
    Point point = map.points[myBoat.row][myBoat.column-1];
    if (isFishingArea(point.value) == 1 && point.hasBoat == noBoat)
      leftValue = point.value;
  }

  //à direita
  if (myBoat.column + 1 <= map.width - 1) {
    Point point = map.points[myBoat.row][myBoat.column+1];
    if (isFishingArea(point.value) == 1 && point.hasBoat == noBoat)
      rightValue = point.value;
  }

  //retorna qual ponto é mais vantajoso para pesca
  bestChoice = getHigherValue(upValue, downValue, leftValue, rightValue);

  if (bestChoice == -1) {
    //se nenhuma das direções (cima, baixo, esquerda e direita) for vantajosa para pesca,
    //procura um ponto de pesca mais próximo
    int* coords = getTheNearestFishableArea(map, myBoat);
    strcpy(command, goToPosition(map, myBoat, coords));
  }
  else {
    strcpy(command, setAction(bestChoice));
  }

  return command;
}

//Movimenta o barco no mar
char* moveBoat(Map map, Boat myBoat) {
  char* command = (char*) calloc(MAX_LINE, sizeof(char));
  int myPoint = map.points[myBoat.row][myBoat.column].value;
  
  //verifica se o estoque está cheio
  if (myBoat.stock == 10) {
    //verifica se está no porto
    if (myPoint == 1)
      //se sim, vende os peixes
      strcpy(command, setAction(sell));
    else {
      // se não, procura o porto mais próximo para vender os peixes
      int* coords = getTheNearestPort(map, myBoat);
      strcpy(command, goToPosition(map, myBoat, coords));
    }
  }
  //verifica se o ponto é bom para pesca
  else if (isFishingArea(myPoint)) {
    //se sim, pesca um peixe
    strcpy(command, setAction(fish));
  }
  else {
    //se não, analisa no entorno qual o melhor movimento a ser feito
    strcpy(command, getBestMoviment(map, myBoat));
  }

  return command;
}

//Atualiza valores do barco após venda ou pesca dos peixes
void updateMyBoat(Boat* myBoat, char line[MAX_LINE], char command[MAX_LINE]) {
  if (strcmp(command, "FISH") == 0) {
    if (strcmp(line, "NONE") != 0) {
      //se a ação foi de pesca, aumenta o estoque
      myBoat->stock++;
    }
  }
  else if (strcmp(command, "SELL") == 0) {
    //se a ação foi de venda, zera o estoque
    myBoat->stock = 0;
  }
}

int main() {
  char line[MAX_LINE];
  char myId[MAX_LINE]; // identificador do bot em questão
  char* command = (char*) calloc(MAX_LINE, sizeof(char));

  setbuf(stdin, NULL); // stdin, stdout e stderr não terão buffers
  setbuf(stdout, NULL); // assim, nada é "guardado temporariamente"
  setbuf(stderr, NULL);

  // === INÍCIO DA PARTIDA ===
  int h, w;
  scanf("AREA %i %i", &h, &w); // dimensão da área de pesca: altura (h) x largura (w)
  scanf(" ID %s", myId); // id do meu barco

  Boat myBoat = initBoat(myId);

  // === PARTIDA ===
  Map map;
  // fica num laço infinito, pois quem vai terminar seu programa é o SIMULADOR.
  while (1) {
    map = readData(h, w, &myBoat);

    // movimenta barco
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