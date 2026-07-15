// #include "displayfull.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define BITNR 6
#define NRTEST1 63
#define NRTEST2 32
#define MAXVAL 200

enum
{
  DX = NONE = 0,
  DY = LINE = 1,
  TOOL = BLOCK = 2,
  DATA = COLOUR = 3 TARGETX = 4,
  TARGETY = 5,
  SHOW = 6,
  PAUSE = 7,
  NEXTFRAME = 8
};

typedef struct state
{
  int x, y, tx, ty;
  unsigned char tool, **imageOut, colour;
  unsigned int start, data;
  bool end;
} state;

typedef unsigned char byte;

state *newState()
{
  state *currentState = malloc(sizeof(state));
  currentState->x = currentState->y = currentState->tx = currentState->ty = currentState->start = currentState->data = currentState->end = 0;
  currentState->tool = LINE;
  currentState->imageOut = (unsigned char **)malloc(MAXVAL * sizeof(unsigned char *));
  int i, j;
  for (i = 0; i < MAXVAL; i++)
  {
    currentState->imageOut[i] = (unsigned char *)malloc(MAXVAL * sizeof(unsigned char));
  }
  for (i = 0; i < MAXVAL; i++)
  {
    for (j = 0; j < MAXVAL; j++)
    {
      currentState->imageOut[i][j] = 0;
    }
  }
  return currentState;
}

void freeState(state *s)
{
  int i;
  for (i = 0; i < MAXVAL; i++)
  {
    free(s->imageOut[i]);
  }
  free(s->imageOut);
  free(s);
}

int getOpcode(byte b)
{
  int result = b >> BITNR;
  return result;
}

int getOperand(byte b)
{
  int result = b & NRTEST1;
  if (b & NRTEST2)
  {
    result = result ^ (~(NRTEST1));
  }
  return result;
}

void obeyTOOL(display *d, state *s, int operand)
{
  if ((operand == NONE) || (operand == LINE) || (operand == BLOCK))
  {
    s->tool = operand;
  }
  switch (operand)
  {
  case COLOUR:
    colour(d, s->data);
    s->colour = s->data;
    break;
  case TARGETX:
    s->tx = s->data;
    break;
  case TARGETY:
    s->ty = s->data;
    break;
  case SHOW:
    show(d);
    break;
  case PAUSE:
    pause(d, s->data);
    break;
  case NEXTFRAME:
    s->end = 1;
    break;
  }
  s->data = 0;
}

void obeyDX(display *d, state *s, int operand)
{
  s->tx = s->tx + operand;
}

void convertLine(display *d, state *s)
{
  int x0, x1, y0, y1, x, y;
  double ratio;
  if (s->x < s->tx)
  {
    x0 = s->x;
    x1 = s->tx;
  }
  else
  {
    x0 = s->tx;
    x1 = s->x;
  }
  if (s->y < s->ty)
  {
    y0 = s->y;
    y1 = s->ty;
  }
  else
  {
    y0 = s->ty;
    y1 = s->y;
  }
  if (y0 == y1)
  {
    y = y0;
    for (x = x0; x <= x1; x++)
    {
      s->imageOut[x][y] = s->colour;
    }
  }
  else if (x0 == x1)
  {
    x = x0;
    for (y = y0; y <= y1; y++)
    {
      s->imageOut[x][y] = s->colour;
    }
  }
  else
  {
    ratio = double((y1 - y0) / (x1 - x0));
    for (x = x0; x <= x1; x++)
    {
      y = x * ratio;
      s->imageOut[x][y] = s->colour;
    }
  }
}

void convertBlock(display *d, state *s)
{
  int x0, x1, y0, y1, x, y;
  if (s->x < s->tx)
  {
    x0 = s->x,
    x1 = s->tx;
  }
  else
  {
    x0 = s->tx,
    x1 = s->x;
  }
  if (s->y < s->ty)
  {
    y0 = s->y,
    y1 = s->ty;
  }
  else
  {
    y0 = s->ty,
    y1 = s->y;
  }
  for (x = x0; x <= x1; x++)
  {
    for (y = y0; y <= y1; y++)
    {
      s->imageOut[x][y] = s->colour;
    }
  }
}

void obeyDY(display *d, state *s, int operand)
{
  s->ty = s->ty + operand;
  if (s->tool == LINE)
  {
    convertLine(d, s);
  }
  else if (s->tool == BLOCK)
  {
    convertBlock(d, s);
  }
  s->x = s->tx,
  s->y = s->ty;
}

void obeyDATA(display *d, state *s, int operand)
{
  s->data = (s->data << BITNR) ^ (operand & NRTEST1);
}

void obey(display *d, state *s, byte op)
{
  int opcode = getOpcode(op), operand = getOperand(op);
  switch (opcode)
  {
  case TOOL:
    obeyTOOL(d, s, operand);
    break;
  case DX:
    obeyDX(d, s, operand);
    break;
  case DY:
    obeyDY(d, s, operand);
    break;
  case DATA:
    obeyDATA(d, s, operand);
    break;
  }
}

void readSketch(display *d, state *s)
{
  char *filename = getName(d);
  FILE *fpointer = fopen(filename, "rb");
  fseek(fpointer, s->start, SEEK_SET);
  unsigned char c = fgetc(fpointer);
  while (!feof(fpointer))
  {
    obey(d, s, c);
    c = fgetc(fpointer);
  }
  fclose(fpointer);
}

void writeFile(state *s, char *file_name_res)
{
  int W = MAXVAL, H = MAXVAL, a[MAXVAL][MAXVAL];
  FILE *ofp = fopen(file_name_res, "wb");
  if (ofp == NULL)
  {
    fprintf(stderr, "Cannot write myimage.pgm\n");
    exit(1);
  }
  fprintf(ofp, "P5 %d %d 255\n", W, H);
  fwrite(a, 1, H * W, ofp);
  fclose(ofp);
  printf("File %s has been written.\n", file_name_res);
}

void convert(char *filename, char *file_name_res)
{
  display *d = newDisplay(filename, MAXVAL, MAXVAL);
  state *s = newState();
  readSketch(d, s);
  writeFile(s, file_name_res);
  freeState(s);
  freeDisplay(d);
}

void createNameExt(char *args, char **file_name, char **file_ext)
{
  int i = 0;
  char temp[20];
  while (args[i] != '.')
  {
    i++;
  }
  strcpy(temp, args + i + 1);
  *file_ext = (char *)malloc((strlen(temp) + 1) * sizeof(char));
  strcpy(*file_ext, temp);
  strcpy(temp, args);
  temp[i] = 0;
  *file_name = (char *)malloc((strlen(temp) + 1) * sizeof(char));
  strcpy(*file_name, temp);
}

#ifndef TESTING
int main(int n, char *args[n])
{
  char *file_ext, *file_name, file_name_res[25];
  if (n != 2)
  {
    printf("Use ./sketch file\n");
    exit(1);
  }
  else
  {
    createNameExt(args[1], &file_name, &file_ext);
    if (strcmp(file_ext, "sk") == 0)
    {
      strcpy(file_name_res, file_name);
      strcat(file_name_res, ".pgm");
      convert(args[1], file_name_res);
      free(file_ext);
      free(file_name);
    }
  }
  return 0;
}
#endif
