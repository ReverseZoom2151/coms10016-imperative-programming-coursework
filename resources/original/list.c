// #include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node
{
  item x;
  struct node *back, *next;
};

typedef struct node node;

struct list
{
  node *none, *current;
};

typedef struct list list;
typedef int item;

struct list;

list *newList(item e)
{
  node *np = malloc(sizeof(node));
  np->x = e;
  np->back = np->next = np;
  list *l = malloc(sizeof(list));
  l->current = l->none = np;
  return l;
}

void freeList(list *xs)
{
  xs->none->back->next = NULL;
  node *np = xs->none;
  while (np != NULL)
  {
    node *next = np->next;
    free(np);
    np = next;
  }
  free(xs);
}

void first(list *xs)
{
  if (xs->none != xs->none->next)
  {
    xs->current = xs->none->next;
  }
}

void last(list *xs)
{
  if (xs->none != xs->none->back)
  {
    xs->current = xs->none->back;
  }
}

bool none(list *xs)
{
  if (xs->current == xs->none)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool after(list *xs)
{
  if (xs->current == xs->none)
  {
    return false;
  }
  else
  {
    xs->current = xs->current->next;
    return true;
  }
}

bool before(list *xs)
{
  if (xs->current == xs->none)
  {
    return false;
  }
  else
  {
    xs->current = xs->current->back;
    return true;
  }
}

item get(list *xs)
{
  return xs->current->x;
}

bool set(list *xs, item x)
{
  if (xs->current == xs->none)
  {
    return false;
  }
  else
  {
    xs->current->x = x;
    return true;
  }
}

void insertAfter(list *xs, item x)
{
  node *np = malloc(sizeof(node));
  np->x = x;
  node *before = xs->current;
  node *after = xs->current->next;
  np->back = before;
  np->next = after;
  before->next = after->back = xs->current = np;
}

void insertBefore(list *xs, item x)
{
  node *np = malloc(sizeof(node));
  np->x = x;
  node *after = xs->current;
  node *before = xs->current->back;
  np->back = before;
  np->next = after;
  before->next = after->back = xs->current = np;
}

bool deleteToAfter(list *xs)
{
  if (xs->current == xs->none)
  {
    return false;
  }
  else
  {
    node *np = xs->current;
    node *before = xs->current->back;
    node *after = xs->current = xs->current->next;
    before->next = after;
    after->back = before;
    free(np);
    return true;
  }
}

bool deleteToBefore(list *xs)
{
  if (xs->current == xs->none)
  {
    return false;
  }
  else
  {
    node *np = xs->current;
    node *before = xs->current = xs->current->back;
    node *after = xs->current->next;
    before->next = after;
    after->back = before;
    free(np);
    return true;
  }
}

#ifdef test_list
list *build(char *s)
{
  list *xs = malloc(sizeof(list));
  int n = strlen(s);
  node *nodes[n];
  for (int i = 0; i < n; i++)
  {
    nodes[i] = malloc(sizeof(node));
  }
  for (int i = 0; i < n; i++)
  {
    nodes[i]->next = nodes[(i + 1) % n];
  }
  for (int i = 1; i < n + 1; i++)
  {
    nodes[i % n]->back = nodes[i - 1];
  }
  xs->none = nodes[0];
  xs->none->x = -1;
  node *p = xs->none->next;
  for (int i = 0; i < strlen(s); i++)
  {
    if (s[i] == '|')
    {
      xs->current = p;
    }
    else
    {
      p->x = s[i] - '0';
      p = p->next;
    }
  }
  return xs;
}

void destroy(list *xs, char *s)
{
  int n = strlen(s);
  node *nodes[n];
  nodes[0] = xs->none;
  for (int i = 1; i < n; i++)
  {
    nodes[i] = nodes[i - 1]->next;
  }
  for (int i = 0; i < n; i++)
  {
    free(nodes[i]);
  }
  free(xs);
}

bool match(list *xs, char *s)
{
  int n = strlen(s);
  node *nodes[n];
  nodes[0] = xs->none;
  for (int i = 1; i < n; i++)
  {
    nodes[i] = nodes[i - 1]->next;
  }
  if (nodes[n - 1]->next != xs->none)
  {
    return false;
  }
  for (int i = 1; i < n; i++)
  {
    if (nodes[i]->back != nodes[i - 1])
    {
      return false;
    }
  }
  node *p = xs->none->next;
  for (int i = 0; i < strlen(s); i++)
  {
    if (s[i] == '|')
    {
      if (p != xs->current)
      {
        return false;
      }
    }
    else
    {
      if (p->x != s[i] - '0')
      {
        return false;
      }
      p = p->next;
    }
  }
  return true;
}

enum
{
  First,
  Last,
  None,
  After,
  Before,
  Get,
  Set,
  InsertAfter,
  InsertBefore,
  DeleteToAfter,
  DeleteToBefore
};

typedef int function;

void assert(int line, bool b)
{
  if (b)
  {
    return;
  }
  printf("The test on line %d fails.\n", line);
  exit(1);
}

int call(function f, list *xs, int arg)
{
  int result = -1;
  switch (f)
  {
  case None:
    result = none(xs);
    break;
  case First:
    first(xs);
    break;
  case Last:
    last(xs);
    break;
  case After:
    result = after(xs);
    break;
  case Before:
    result = before(xs);
    break;
  case Get:
    result = get(xs);
    break;
  case Set:
    result = set(xs, arg);
    break;
  case InsertAfter:
    insertAfter(xs, arg);
    break;
  case InsertBefore:
    insertBefore(xs, arg);
    break;
  case DeleteToBefore:
    result = deleteToBefore(xs);
    break;
  case DeleteToAfter:
    result = deleteToAfter(xs);
    break;
  default:
    assert(__LINE__, false);
  }
  return result;
}

bool check(function f, int in, char *before, char *after, int out)
{
  list *xs = build(before);
  int result = call(f, xs, in);
  assert(__LINE__, (xs->none == xs->none->next->back));
  assert(__LINE__, (xs->none == xs->none->back->next));
  bool ok = (match(xs, after) && (result == out));
  destroy(xs, after);
  return ok;
}

void testNewList()
{
  list *xs = newList(-1);
  assert(__LINE__, (xs->none == xs->none->next));
  assert(__LINE__, (xs->none == xs->none->back));
  assert(__LINE__, match(xs, "|"));
  freeList(xs);
}

void testFirst()
{
  assert(__LINE__, check(First, -1, "|", "|", -1));
  assert(__LINE__, check(First, -1, "|37", "|37", -1));
  assert(__LINE__, check(First, -1, "3|7", "|37", -1));
  assert(__LINE__, check(First, -1, "37|", "|37", -1));
}

void testLast()
{
  assert(__LINE__, check(Last, -1, "|", "|", -1));
  assert(__LINE__, check(Last, -1, "|37", "3|7", -1));
  assert(__LINE__, check(Last, -1, "3|7", "3|7", -1));
  assert(__LINE__, check(Last, -1, "37|", "3|7", -1));
}

void testNone()
{
  assert(__LINE__, check(None, -1, "|", "|", true));
  assert(__LINE__, check(None, -1, "|37", "|37", false));
  assert(__LINE__, check(None, -1, "3|7", "3|7", false));
  assert(__LINE__, check(None, -1, "37|", "37|", true));
}

void testAfter()
{
  assert(__LINE__, check(After, -1, "|", "|", false));
  assert(__LINE__, check(After, -1, "|37", "3|7", true));
  assert(__LINE__, check(After, -1, "3|7", "37|", true));
  assert(__LINE__, check(After, -1, "37|", "37|", false));
}

void testBefore()
{
  assert(__LINE__, check(Before, -1, "|", "|", false));
  assert(__LINE__, check(Before, -1, "|37", "37|", true));
  assert(__LINE__, check(Before, -1, "3|7", "|37", true));
  assert(__LINE__, check(Before, -1, "37|", "37|", false));
}

void testGet()
{
  assert(__LINE__, check(Get, -1, "|", "|", -1));
  assert(__LINE__, check(Get, -1, "|37", "|37", 3));
  assert(__LINE__, check(Get, -1, "3|7", "3|7", 7));
  assert(__LINE__, check(Get, -1, "37|", "37|", -1));
}

void testSet()
{
  assert(__LINE__, check(Set, 5, "|", "|", false));
  assert(__LINE__, check(Set, 5, "|37", "|57", true));
  assert(__LINE__, check(Set, 5, "3|7", "3|5", true));
  assert(__LINE__, check(Set, 5, "37|", "37|", false));
}

void testInsertAfter()
{
  assert(__LINE__, check(InsertAfter, 5, "|", "|5", -1));
  assert(__LINE__, check(InsertAfter, 5, "|37", "3|57", -1));
  assert(__LINE__, check(InsertAfter, 5, "3|7", "37|5", -1));
  assert(__LINE__, check(InsertAfter, 5, "37|", "|537", -1));
}

void testInsertBefore()
{
  assert(__LINE__, check(InsertBefore, 5, "|", "|5", -1));
  assert(__LINE__, check(InsertBefore, 5, "|37", "|537", -1));
  assert(__LINE__, check(InsertBefore, 5, "3|7", "3|57", -1));
  assert(__LINE__, check(InsertBefore, 5, "37|", "37|5", -1));
}

void testDeleteToAfter()
{
  assert(__LINE__, check(DeleteToAfter, -1, "|", "|", false));
  assert(__LINE__, check(DeleteToAfter, -1, "|37", "|7", true));
  assert(__LINE__, check(DeleteToAfter, -1, "3|7", "3|", true));
  assert(__LINE__, check(DeleteToAfter, -1, "37|", "37|", false));
  assert(__LINE__, check(DeleteToAfter, -1, "|5", "|", true));
}

void testDeleteToBefore()
{
  assert(__LINE__, check(DeleteToBefore, -1, "|", "|", false));
  assert(__LINE__, check(DeleteToBefore, -1, "|37", "7|", true));
  assert(__LINE__, check(DeleteToBefore, -1, "3|7", "|3", true));
  assert(__LINE__, check(DeleteToBefore, -1, "37|", "37|", false));
  assert(__LINE__, check(DeleteToBefore, -1, "|5", "|", true));
}

int main()
{
  testNewList();
  testFirst();
  testLast();
  testNone();
  testAfter();
  testBefore();
  testGet();
  testSet();
  testInsertAfter();
  testInsertBefore();
  testDeleteToAfter();
  testDeleteToBefore();
  printf("List module tests run OK.\n");
  return 0;
}
#endif
