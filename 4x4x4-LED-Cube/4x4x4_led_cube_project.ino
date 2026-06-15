
// LEDS PINS in Column and layer 
const int columnPins[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1};
const int layerPins[4] = {A2, A3, A4, A5};

void setup() {
  for (int i = 0; i < 16; i++) pinMode(columnPins[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(layerPins[i], OUTPUT);
}

void loop() {
  spiralRotation(); delay(200);
  vortexEffect(); delay(200);
  crossRotation(); delay(200);
  layerSpin(); delay(200);
  diagonal3DRotation(); delay(200);
  waveAnimation(); delay(200);
  rainEffect(); delay(200);
  expandContract(); delay(200);
  diagonalMove(); delay(200);
  sparkleEffect(); delay(200);
  randomBlink(); delay(200);
  layerWipe(); delay(200);
  cubeRotation(); delay(200);
  borderGlow(); delay(200);
  layerSnake(); delay(200);
  zigzagWave(); delay(200);
  expandingSquare(); delay(200);
  spiralMotion(); delay(200);
  checkerboardBlink(); delay(200);
  verticalWave(); delay(200);
  cornerRotation(); delay(200);
  layerRotation(); delay(200);
  edgeFlow(); delay(200);
  diagonalWipe(); delay(200);
  columnScan(); delay(200);
  tunnelEffect(); delay(200);
  flashingBorders(); delay(200);
  layerCascade(); delay(200);
  circularWave(); delay(200);
  centerSpiralRotation(); delay(200);
  expandingRings(); delay(1000);
}

void waveAnimation() {
  for (int l = 0; l < 4; l++) {
    digitalWrite(layerPins[l], LOW);
    for (int c = 0; c < 16; c++) {
      digitalWrite(columnPins[c], HIGH);
      delay(50);
      digitalWrite(columnPins[c], LOW);
    }
    digitalWrite(layerPins[l], HIGH);
  }
}

void rainEffect() {
  for (int i = 0; i < 20; i++) {
    int c = random(0, 16);
    for (int l = 3; l >= 0; l--) {
      digitalWrite(layerPins[l], LOW);
      digitalWrite(columnPins[c], HIGH);
      delay(100);
      digitalWrite(columnPins[c], LOW);
      digitalWrite(layerPins[l], HIGH);
    }
  }
}

void expandContract() {
  for (int l = 0; l < 4; l++) {
    digitalWrite(layerPins[l], LOW);
    for (int c = 0; c < 16; c++) digitalWrite(columnPins[c], HIGH);
    delay(200);
    for (int c = 0; c < 16; c++) digitalWrite(columnPins[c], LOW);
    digitalWrite(layerPins[l], HIGH);
  }
}

void diagonalMove() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(layerPins[i], LOW);
    for (int j = 0; j < 4; j++) {
      digitalWrite(columnPins[j * 4 + j], HIGH);
      delay(100);
      digitalWrite(columnPins[j * 4 + j], LOW);
    }
    digitalWrite(layerPins[i], HIGH);
  }
}

void sparkleEffect() {
  for (int i = 0; i < 40; i++) {
    int l = random(0, 4);
    int c = random(0, 16);
    digitalWrite(layerPins[l], LOW);
    digitalWrite(columnPins[c], HIGH);
    delay(50);
    digitalWrite(columnPins[c], LOW);
    digitalWrite(layerPins[l], HIGH);
  }
}

void randomBlink() {
  for (int i = 0; i < 30; i++) {
    int l = random(0, 4);
    int c = random(0, 16);
    digitalWrite(layerPins[l], LOW);
    digitalWrite(columnPins[c], HIGH);
    delay(100);
    digitalWrite(columnPins[c], LOW);
    digitalWrite(layerPins[l], HIGH);
  }
}

void layerWipe() {
  for (int l = 0; l < 4; l++) {
    digitalWrite(layerPins[l], LOW);
    for (int c = 0; c < 16; c++) digitalWrite(columnPins[c], HIGH);
    delay(200);
    for (int c = 0; c < 16; c++) digitalWrite(columnPins[c], LOW);
    digitalWrite(layerPins[l], HIGH);
  }
}

void cubeRotation() {
  for (int l = 0; l < 4; l++) {
    digitalWrite(layerPins[l], LOW);
    for (int i = 0; i < 4; i++) {
      digitalWrite(columnPins[i], HIGH);
      digitalWrite(columnPins[15 - i], HIGH);
      digitalWrite(columnPins[i * 4], HIGH);
      digitalWrite(columnPins[i * 4 + 3], HIGH);
    }
    delay(300);
    for (int i = 0; i < 16; i++) digitalWrite(columnPins[i], LOW);
    digitalWrite(layerPins[l], HIGH);
  }
}

void borderGlow() {
  for (int l = 0; l < 4; l++) {
    digitalWrite(layerPins[l], LOW);
    for (int i = 0; i < 4; i++) {
      digitalWrite(columnPins[i], HIGH);
      digitalWrite(columnPins[i + 12], HIGH);
      digitalWrite(columnPins[i * 4], HIGH);
      digitalWrite(columnPins[i * 4 + 3], HIGH);
    }
    delay(200);
    for (int i = 0; i < 16; i++) digitalWrite(columnPins[i], LOW);
    digitalWrite(layerPins[l], HIGH);
  }
}

void layerSnake() {
  for (int l = 0; l < 4; l++) {
    digitalWrite(layerPins[l], LOW);
    for (int c = 0; c < 16; c++) {
      digitalWrite(columnPins[c], HIGH);
      delay(50);
      digitalWrite(columnPins[c], LOW);
    }
    digitalWrite(layerPins[l], HIGH);
  }
}

void zigzagWave() {
  for (int l = 0; l < 4; l++) {
    digitalWrite(layerPins[l], LOW);
    for (int c = 0; c < 16; c += 2) {
      digitalWrite(columnPins[c], HIGH);
      delay(50);
      digitalWrite(columnPins[c], LOW);
    }
    digitalWrite(layerPins[l], HIGH);
  }
}

void expandingSquare() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(layerPins[i], LOW);
    for (int j = 0; j < 4; j++) {
      digitalWrite(columnPins[j * 4], HIGH);
      digitalWrite(columnPins[j * 4 + 3], HIGH);
    }
    delay(200);
    for (int j = 0; j < 16; j++) digitalWrite(columnPins[j], LOW);
    digitalWrite(layerPins[i], HIGH);
  }
}

void spiralMotion() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(layerPins[i], LOW);
    for (int j = 0; j < 4; j++) {
      digitalWrite(columnPins[j], HIGH);
      delay(100);
      digitalWrite(columnPins[j], LOW);
    }
    digitalWrite(layerPins[i], HIGH);
  }
}

void checkerboardBlink() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 16; j += 2) digitalWrite(columnPins[j], HIGH);
    delay(200);
    for (int j = 0; j < 16; j++) digitalWrite(columnPins[j], LOW);
    delay(200);
  }
}

void verticalWave() {
  for (int c = 0; c < 16; c++) {
    for (int l = 0; l < 4; l++) {
      digitalWrite(layerPins[l], LOW);
      digitalWrite(columnPins[c], HIGH);
      delay(50);
      digitalWrite(columnPins[c], LOW);
      digitalWrite(layerPins[l], HIGH);
    }
  }
}

void cornerRotation() {
  int corners[8] = {0, 3, 12, 15, 48, 51, 60, 63};
  for (int i = 0; i < 8; i++) {
    digitalWrite(layerPins[i / 4], LOW);
    digitalWrite(columnPins[corners[i] % 16], HIGH);
    delay(100);
    digitalWrite(columnPins[corners[i] % 16], LOW);
    digitalWrite(layerPins[i / 4], HIGH);
  }
}

void layerRotation() {
  int sequence[4][4] = {
    {0, 1, 3, 2},
    {4, 5, 7, 6},
    {8, 9, 11, 10},
    {12, 13, 15, 14}
  };
  for (int l = 0; l < 4; l++) {
    digitalWrite(layerPins[l], LOW);
    for (int i = 0; i < 4; i++) {
      digitalWrite(columnPins[sequence[l][i]], HIGH);
      delay(100);
      digitalWrite(columnPins[sequence[l][i]], LOW);
    }
    digitalWrite(layerPins[l], HIGH);
  }
}

void edgeFlow() {
  int edges[12][2] = {
    {0, 1}, {1, 3}, {3, 2}, {2, 0},
    {4, 5}, {5, 7}, {7, 6}, {6, 4},
    {8, 9}, {9, 11}, {11, 10}, {10, 8}
  };
  for (int e = 0; e < 12; e++) {
    digitalWrite(layerPins[e / 4], LOW);
    digitalWrite(columnPins[edges[e][0]], HIGH);
    digitalWrite(columnPins[edges[e][1]], HIGH);
    delay(100);
    digitalWrite(columnPins[edges[e][0]], LOW);
    digitalWrite(columnPins[edges[e][1]], LOW);
    digitalWrite(layerPins[e / 4], HIGH);
  }
}

void diagonalWipe() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(layerPins[i], LOW);
    for (int j = 0; j < 16; j += 5) digitalWrite(columnPins[j], HIGH);
    delay(100);
    for (int j = 0; j < 16; j++) digitalWrite(columnPins[j], LOW);
    digitalWrite(layerPins[i], HIGH);
  }
}

void columnScan() {
  for (int c = 0; c < 16; c++) {
    digitalWrite(columnPins[c], HIGH);
    delay(100);
    digitalWrite(columnPins[c], LOW);
  }
}

void tunnelEffect() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(layerPins[i], LOW);
    for (int j = 0; j < 16; j++) {
      digitalWrite(columnPins[j], HIGH);
      delay(50);
      digitalWrite(columnPins[j], LOW);
    }
    digitalWrite(layerPins[i], HIGH);
  }
}

void flashingBorders() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(columnPins[j], HIGH);
      digitalWrite(columnPins[j + 12], HIGH);
    }
    delay(200);
    for (int j = 0; j < 16; j++) digitalWrite(columnPins[j], LOW);
    delay(200);
  }
}

void layerCascade() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(layerPins[i], LOW);
    for (int j = 0; j < 16; j++) digitalWrite(columnPins[j], HIGH);
    delay(100);
    for (int j = 0; j < 16; j++) digitalWrite(columnPins[j], LOW);
    digitalWrite(layerPins[i], HIGH);
  }
}

void circularWave() {
  int waveOrder[4][4] = {
    {5, 6, 9, 10},
    {1, 2, 4, 8},
    {7, 11, 13, 14},
    {0, 3, 12, 15}
  };
  for (int w = 0; w < 4; w++) {
    for (int l = 0; l < 4; l++) {
      digitalWrite(layerPins[l], LOW);
      for (int i = 0; i < 4; i++) {
        digitalWrite(columnPins[waveOrder[w][i]], HIGH);
      }
      delay(200);
      for (int i = 0; i < 4; i++) {
        digitalWrite(columnPins[waveOrder[w][i]], LOW);
      }
      digitalWrite(layerPins[l], HIGH);
    }
  }
}

void centerSpiralRotation() {
  int spiralOrder[16] = {5, 6, 9, 10, 1, 2, 3, 7, 11, 15, 14, 13, 12, 8, 4, 0};
  for (int l = 0; l < 4; l++) {
    digitalWrite(layerPins[l], LOW);
    for (int i = 0; i < 16; i++) {
      digitalWrite(columnPins[spiralOrder[i]], HIGH);
      delay(100);
      digitalWrite(columnPins[spiralOrder[i]], LOW);
    }
    digitalWrite(layerPins[l], HIGH);
  }
}

void expandingRings() {
  int ring1[] = {5, 6, 9, 10};
  int ring3[] = {0, 3, 12, 15};
  for (int step = 0; step < 2; step++) {
    for (int l = 0; l < 4; l++) {
      digitalWrite(layerPins[l], LOW);
      int* ring = (step == 0) ? ring1 : ring3;
      int size = 4;
      for (int i = 0; i < size; i++) digitalWrite(columnPins[ring[i]], HIGH);
      delay(200);
      for (int i = 0; i < size; i++) digitalWrite(columnPins[ring[i]], LOW);
      digitalWrite(layerPins[l], HIGH);
    }
  }
}

void spiralRotation() {
  int spiralOrder[16] = {0, 1, 5, 9, 13, 14, 15, 11, 7, 3, 2, 6, 10, 12, 8, 4};
  for (int l = 0; l < 4; l++) {
    digitalWrite(layerPins[l], LOW);
    for (int i = 0; i < 16; i++) {
      digitalWrite(columnPins[spiralOrder[i]], HIGH);
      delay(100);
      digitalWrite(columnPins[spiralOrder[i]], LOW);
    }
    digitalWrite(layerPins[l], HIGH);
  }
}

void vortexEffect() {
  for (int step = 0; step < 3; step++) {
    for (int l = 0; l < 4; l++) {
      digitalWrite(layerPins[l], LOW);
      for (int c = 0; c < 16; c++) {
        digitalWrite(columnPins[c], HIGH);
        delay(30);
        digitalWrite(columnPins[c], LOW);
      }
      digitalWrite(layerPins[l], HIGH);
    }
  }
}

void crossRotation() {
  for (int l = 0; l < 4; l++) {
    digitalWrite(layerPins[l], LOW);
    for (int i = 0; i < 4; i++) {
      digitalWrite(columnPins[i], HIGH);
      digitalWrite(columnPins[i + 12], HIGH);
      digitalWrite(columnPins[i * 4], HIGH);
      digitalWrite(columnPins[i * 4 + 3], HIGH);
    }
    delay(300);
    for (int i = 0; i < 16; i++) digitalWrite(columnPins[i], LOW);
    digitalWrite(layerPins[l], HIGH);
  }
}

void layerSpin() {
  for (int step = 0; step < 4; step++) {
    for (int l = 0; l < 4; l++) {
      digitalWrite(layerPins[l], LOW);
      for (int i = 0; i < 4; i++) {
        digitalWrite(columnPins[(i + step) % 4], HIGH);
        digitalWrite(columnPins[12 + (i + step) % 4], HIGH);
      }
      delay(200);
      for (int i = 0; i < 16; i++) digitalWrite(columnPins[i], LOW);
      digitalWrite(layerPins[l], HIGH);
    }
  }
}

void diagonal3DRotation() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(layerPins[i], LOW);
      digitalWrite(columnPins[j * 4 + (i % 4)], HIGH);
      delay(100);
      digitalWrite(columnPins[j * 4 + (i % 4)], LOW);
      digitalWrite(layerPins[i], HIGH);
    }
  }
}
