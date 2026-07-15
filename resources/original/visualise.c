#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>
#include <ctype.h>

#define SIZECHAR 8
#define SIZEINT 32
#define SIZEDOUBLE 64

int LikeBinary(char *s, int l)
{
  if (strlen(s) != l)
  {
    return 0;
  }
  for (int i = 0; i < strlen(s); i++)
  {
    if (s[i] != '0' && s[i] != '1')
    {
      return 0;
    }
  }
  return 1;
}

char *IntToBinary(char *s)
{
  int negative, val10, i;
  char *p = NULL;
  p = (char *)malloc((SIZEINT + 1) * sizeof(char));
  p[SIZEINT] = 0;
  if (s[0] == '-')
  {
    negative = 1;
    s++;
  }
  else
  {
    negative = 0;
  }
  if ((s[0] < '1') || (s[0] > '9'))
  {
    p[0] = 0;
    return p;
  }
  val10 = atoi(s);
  if (!negative)
  {
    if (val10 > INT_MAX)
    {
      p[0] = 0;
    }
    else
    {
      for (i = 0; i < SIZEINT; i++)
      {
        if (val10 & (1LL << i))
        {
          p[SIZEINT - 1 - i] = '1';
        }
        else
        {
          p[SIZEINT - 1 - i] = '0';
        }
      }
    }
  }
  else
  {
    if (val10 > INT_MAX)
    {
      p[0] = 0;
    }
    else
    {
      for (i = 0; i < SIZEINT; i++)
      {
        if (val10 & (1LL << i))
        {
          p[SIZEINT - 1 - i] = '1';
        }
        else
        {
          p[SIZEINT - 1 - i] = '0';
        }
      }
      for (i = 0; i < SIZEINT; i++)
      {
        if (p[i] == '0')
        {
          p[i] = '1';
        }
        else
        {
          p[i] = '0';
        }
      }
      i = SIZEINT - 1;
      while ((i > 0) && (p[i] == '1'))
      {
        p[i] = '0';
        i--;
      }
      p[i] = '1';
    }
  }
  return p;
}

char *UnsignedIntToBinary(char *s)
{
  int val10, i;
  char *p = NULL;
  p = (char *)malloc((SIZEINT + 1) * sizeof(char));
  p[SIZEINT] = 0;
  if ((s[0] < '1') || (s[0] > '9'))
  {
    p[0] = 0;
    return p;
  }
  val10 = atoi(s);
  if (val10 > UINT_MAX)
  {
    p[0] = 0;
  }
  else
  {
    for (i = 0; i < SIZEINT; i++)
    {
      if (val10 & (1LL << i))
      {
        p[SIZEINT - 1 - i] = '1';
      }
      else
      {
        p[SIZEINT - 1 - i] = '0';
      }
    }
  }
  return p;
}

char *CharToBinary(char *s)
{
  int negative, val10, i;
  char *p = NULL;
  p = (char *)malloc((SIZECHAR + 1) * sizeof(char));
  p[SIZECHAR] = 0;
  if (s[0] == '-')
  {
    negative = 1;
    s++;
  }
  else
  {
    negative = 0;
  }
  if ((s[0] < '1') || (s[0] > '9'))
  {
    p[0] = 0;
    return p;
  }
  val10 = atoi(s);
  if (!negative)
  {
    if (val10 > CHAR_MAX)
    {
      p[0] = 0;
    }
    else
    {
      for (i = 0; i < SIZECHAR; i++)
      {
        if (val10 & (1 << i))
        {
          p[SIZECHAR - 1 - i] = '1';
        }
        else
        {
          p[SIZECHAR - 1 - i] = '0';
        }
      }
    }
  }
  else
  {
    if (val10 > (CHAR_MAX + 1))
    {
      p[0] = 0;
    }
    else
    {
      for (i = 0; i < SIZECHAR; i++)
      {
        if (val10 & (1 << i))
        {
          p[SIZECHAR - 1 - i] = '1';
        }
        else
        {
          p[SIZECHAR - 1 - i] = '0';
        }
      }
      for (i = 0; i < SIZECHAR; i++)
      {
        if (p[i] == '0')
        {
          p[i] = '1';
        }
        else
        {
          p[i] = '0';
        }
      }
      i = SIZECHAR - 1;
      while (i > 0 && p[i] == '1')
      {
        p[i] = '0';
        i--;
      }
      p[i] = '1';
    }
  }
  return p;
}

char *UnsignedCharToBinary(char *s)
{
  int val10, i;
  char *p = NULL;
  p = (char *)malloc((SIZECHAR + 1) * sizeof(char));
  p[SIZECHAR] = 0;
  if ((s[0] < '1') || (s[0] > '9'))
  {
    p[0] = 0;
    return p;
  }
  val10 = atoi(s);
  if (val10 > UCHAR_MAX)
  {
    p[0] = 0;
  }
  else
  {
    for (i = 0; i < SIZECHAR; i++)
    {
      if (val10 & (1 << i))
      {
        p[SIZECHAR - 1 - i] = '1';
      }
      else
      {
        p[SIZECHAR - 1 - i] = '0';
      }
    }
  }
  return p;
}

char *LongToBinary(char *s)
{
  int negative, i;
  long long val10;
  char *p = NULL;
  p = (char *)malloc((SIZEDOUBLE + 1) * sizeof(char));
  p[SIZEDOUBLE] = 0;
  if (s[0] == '-')
  {
    negative = 1;
    s++;
  }
  else
  {
    negative = 0;
  }
  if ((s[0] < '1') || (s[0] > '9'))
  {
    p[0] = 0;
    return p;
  }
  val10 = atol(s);
  if (!negative)
  {
    if (val10 > LONG_MAX)
    {
      p[0] = 0;
    }
    else
    {
      for (i = 0; i < SIZEDOUBLE; i++)
      {
        if (val10 & (1LU << i))
        {
          p[SIZEDOUBLE - 1 - i] = '1';
        }
        else
        {
          p[SIZEDOUBLE - 1 - i] = '0';
        }
      }
    }
  }
  else
  {
    if (val10 > LONG_MAX)
    {
      p[0] = 0;
    }
    else
    {
      for (i = 0; i < SIZEDOUBLE; i++)
      {
        if (val10 & (1LU << i))
        {
          p[SIZEDOUBLE - 1 - i] = '1';
        }
        else
        {
          p[SIZEDOUBLE - 1 - i] = '0';
        }
      }
      for (i = 0; i < SIZEDOUBLE; i++)
      {
        if (p[i] == '0')
        {
          p[i] = '1';
        }
        else
        {
          p[i] = '0';
        }
      }
      i = SIZEDOUBLE - 1;
      while ((i > 0) && (p[i] == '1'))
      {
        p[i] = '0';
        i--;
      }
      p[i] = '1';
    }
  }
  return p;
}

int BinaryToCharIntLong(char *s)
{
  int negative, val10, i;
  if (s[0] == '1')
  {
    negative = 1;
  }
  else
  {
    negative = 0;
    s++;
  }
  if (negative)
  {
    i = strlen(s) - 1;
    while ((i > 0) && (s[i] == '0'))
    {
      s[i] = '1';
      i--;
    }
    s[i] = '0';
    for (i = 0; i < strlen(s); i++)
    {
      if (s[i] == '1')
      {
        s[i] = '0';
      }
      else
      {
        s[i] = '1';
      }
    }
  }
  val10 = 0;
  for (i = strlen(s) - 1; i >= 0; i--)
  {
    if (s[i] == '1')
    {
      val10 += (1 << (strlen(s) - 1 - i));
    }
  }
  if (negative)
  {
    val10 = -val10;
  }
  return val10;
}

int BinaryToUnsignedCharInt(char *s)
{
  int val10, i;
  val10 = 0;
  for (i = strlen(s) - 1; i >= 0; i--)
  {
    if (s[i] == '1')
    {
      val10 += (1 << (strlen(s) - 1 - i));
    }
  }
  return val10;
}

void PrintString(char *p)
{
  if (p[0] != 0)
  {
    for (int i = 0; i < strlen(p); i++)
    {
      if ((i > 0) && (i % 4 == 0))
      {
        printf(" ");
      }
      printf("%c", p[i]);
    }
  }
  else
  {
    printf("Input error.\n");
  }
}

void InputOutput(int argc, char *argv[])
{
  int i, iargv;
  char *p = NULL, *ps, BinaryString[100];
  for (i = 0; i < argc; i++)
  {
    printf("%s\n", argv[i]);
  }
  if (strcmp(argv[1], "int") == 0)
  {
    if (argc == 3)
    {
      p = IntToBinary(argv[2]);
      PrintString(p);
      printf("\n");
    }
    else
    {
      strcpy(BinaryString, argv[2]);
      iargv = 3;
      while (iargv < argc)
      {
        strcat(BinaryString, argv[iargv]);
        iargv++;
      }
      if (LikeBinary(BinaryString, SIZEINT))
      {
        printf("%d\n", BinaryToCharIntLong(BinaryString));
      }
      else
      {
        printf("Input error.\n");
      }
    }
  }
  else if (strcmp(argv[1], "unsigned") == 0 && strcmp(argv[2], "int") == 0)
  {
    if (argc == 4)
    {
      p = UnsignedIntToBinary(argv[3]);
      PrintString(p);
      printf("\n");
    }
    else
    {
      strcpy(BinaryString, argv[2]);
      iargv = 3;
      while (iargv < argc)
      {
        strcat(BinaryString, argv[iargv]);
        iargv++;
      }
      if (LikeBinary(BinaryString, SIZEINT))
      {
        printf("%d\n", BinaryToUnsignedCharInt(BinaryString));
      }
      else
      {
        printf("Input error.\n");
      }
    }
  }
  else if (strcmp(argv[1], "char") == 0)
  {
    if (argc == 3)
    {
      p = CharToBinary(argv[2]);
      PrintString(p);
      printf("\n");
    }
    else
    {
      strcpy(BinaryString, argv[2]);
      iargv = 3;
      while (iargv < argc)
      {
        strcat(BinaryString, argv[iargv]);
        iargv++;
      }
      if (LikeBinary(BinaryString, SIZECHAR))
      {
        printf("%d\n", BinaryToCharIntLong(BinaryString));
      }
      else
      {
        printf("Input error.\n");
      }
    }
  }
  else if ((strcmp(argv[1], "unsigned") == 0) && (strcmp(argv[2], "char") == 0))
  {
    if (argc == 4)
    {
      p = UnsignedCharToBinary(argv[3]);
      PrintString(p);
      printf("\n");
    }
    else
    {
      strcpy(BinaryString, argv[2]);
      iargv = 3;
      while (iargv < argc)
      {
        strcat(BinaryString, argv[iargv]);
        iargv++;
      }
      if (LikeBinary(BinaryString, SIZECHAR))
      {
        printf("%d\n", BinaryToUnsignedCharInt(BinaryString));
      }
      else
      {
        printf("Input error.\n");
      }
    }
  }
  else if (strcmp(argv[1], "long") == 0)
  {
    if (argc == 3)
    {
      p = LongToBinary(argv[2]);
      PrintString(p);
      printf("\n");
    }
    else
    {
      strcpy(BinaryString, argv[2]);
      iargv = 3;
      while (iargv < argc)
      {
        strcat(BinaryString, argv[iargv]);
        iargv++;
      }
      if (LikeBinary(BinaryString, SIZEDOUBLE))
      {
        printf("%d\n", BinaryToCharIntLong(BinaryString));
      }
      else
      {
        printf("Input error.\n");
      }
    }
  }
  else if (argv[1][0] == '{')
  {
    char temp[100];
    strcpy(temp, argv[1]);
    iargv = 2;
    while (temp[strlen(temp) - 1] != '}')
    {
      strcat(temp, argv[iargv]);
      iargv++;
    }
    ps = strtok(temp, "{}\\;");
    while (ps)
    {
      if (strcmp(ps, "int") == 0)
      {
        p = IntToBinary(argv[iargv++]);
        PrintString(p);
        printf(" ");
        if (p)
        {
          free(p);
          p = NULL;
        }
      }
      else if (strcmp(ps, "unsignedint") == 0)
      {
        p = UnsignedIntToBinary(argv[iargv++]);
        PrintString(p);
        printf(" ");
        if (p)
        {
          free(p);
          p = NULL;
        }
      }
      else if (strcmp(ps, "char") == 0)
      {
        p = CharToBinary(argv[iargv++]);
        PrintString(p);
        printf(" ");
        if (p)
        {
          free(p);
          p = NULL;
        }
      }
      else if (strcmp(ps, "unsignedchar") == 0)
      {
        p = UnsignedCharToBinary(argv[iargv++]);
        PrintString(p);
        printf(" ");
        if (p)
        {
          free(p);
          p = NULL;
        }
      }
      else if (strcmp(ps, "long") == 0)
      {
        p = LongToBinary(argv[iargv++]);
        PrintString(p);
        printf(" ");
        if (p)
        {
          free(p);
          p = NULL;
        }
      }
      ps = strtok(NULL, "{}\\;");
    }
  }
  if (p)
  {
    free(p);
    p = NULL;
  }
}

void assert(int line, bool b)
{
  if (b)
  {
    return;
  }
  printf("The test on line %d fails.\n", line);
  exit(1);
}

void testIntToBinary()
{
  char *res = IntToBinary("1000");
  assert(__LINE__, strcmp(res, "00000000000000000000001111101000") == 0);
  assert(__LINE__, strcmp(res, "10000000000000000000001111101000") != 0);
  free(res);
}

void testUnsignedIntToBinary()
{
  char *res = UnsignedIntToBinary("100");
  assert(__LINE__, strcmp(res, "00000000000000000000000001100100") == 0);
  assert(__LINE__, strcmp(res, "10000000000000000000000001100100") != 0);
  free(res);
}

void testCharToBinary()
{
  char *res = CharToBinary("120");
  assert(__LINE__, strcmp(res, "01111000") == 0);
  assert(__LINE__, strcmp(res, "11111000") != 0);
  free(res);
}

void testUnsignedCharToBinary()
{
  char *res = UnsignedCharToBinary("250");
  assert(__LINE__, strcmp(res, "11111010") == 0);
  assert(__LINE__, strcmp(res, "01111010") != 0);
  free(res);
}

void testLongToBinary()
{
  char *res = LongToBinary("320000");
  assert(__LINE__, strcmp(res, "0000000000000000000000000000000000000000000001001110001000000000") == 0);
  assert(__LINE__, strcmp(res, "1000000000000000000000000000000000000000000001001110001000000000") != 0);
  free(res);
}

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    InputOutput(argc, argv);
  }
  else
  {
    testIntToBinary();
    testUnsignedIntToBinary();
    testCharToBinary();
    testUnsignedCharToBinary();
    testLongToBinary();
    printf("All tests pass.");
  }
  return 0;
}
