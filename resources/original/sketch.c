// #include "displayfull.h"
// #include "sketch.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define BITNR 6
#define NRTEST1 63
#define NRTEST2 32

state *newState()
{
  state *currentState = malloc(sizeof(state));
  currentState->x = currentState->y = currentState->tx = currentState->ty = currentState->start = currentState->data = currentState->end = 0;
  currentState->tool = LINE;
  return currentState;
}

void freeState(state *s)
{
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
    result = result ^ ~NRTEST1;
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

void obeyDY(display *d, state *s, int operand)
{
  s->ty = s->ty + operand;
  if (s->tool == LINE)
  {
    line(d, s->x, s->y, s->tx, s->ty);
  }
  else if (s->tool == BLOCK)
  {
    block(d, s->x, s->y, abs(s->tx - s->x), abs(s->ty - s->y));
  }
  s->x = s->tx;
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

bool processSketch(display *d, void *data, const char pressedKey)
{
  if (data == NULL)
  {
    return (pressedKey == 27);
  }
  state *s = (state *)data;
  char *filename = getName(d);
  FILE *fpointer = fopen(filename, "rb");
  fseek(fpointer, s->start, SEEK_SET);
  int position = 0;
  unsigned char c = fgetc(fpointer);
  while (!feof(fpointer) && s->end == false)
  {
    obey(d, s, c);
    c = fgetc(fpointer);
    position++;
  }
  show(d);
  s->x = s->y = s->tx = s->ty = 0;
  s->tool = LINE;
  if (s->end == true)
  {
    s->start = s->start + position;
  }
  else
  {
    s->start = 0;
  }
  s->data = s->end = 0;
  fclose(fpointer);
  return (pressedKey == 27);
}

void view(char *filename)
{
  display *d = newDisplay(filename, 200, 200);
  state *s = newState();
  run(d, s, processSketch);
  freeState(s);
  freeDisplay(d);
}

#ifndef TESTING
int main(int n, char *args[n])
{
  if (n != 2)
  {
    printf("Use ./sketch file\n");
    exit(1);
  }
  else
  {
    view(args[1]);
  }
  return 0;
}
#endif
