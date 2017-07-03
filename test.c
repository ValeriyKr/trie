// vim: sw=2 ts=2 et :

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "trie.h"

#define say(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#define sayn(fmt, ...) fprintf(stderr, fmt "\n", __VA_ARGS__)

#define OK sayn("%s", "[ OK ]");

#define FAIL sayn("%s", "[FAIL]"); exit(0);

#define puti(t, key, val)                    \
  say("Putting %s=%d: ", key, val);          \
  if (trie_put(t, key, &val, sizeof(val))) { \
    FAIL;                                    \
  }                                          \
  OK;                                        \

#define geti(t, key, val)                         \
  do {                                            \
    say("Getting %s=%d: ", key, val);             \
    const int *v = (const int*)trie_get(t, key);  \
    if (NULL == v) {                              \
      FAIL;                                       \
    }                                             \
    if (*v != val) {                              \
      FAIL;                                       \
    }                                             \
    OK;                                           \
  } while (0);

#define putst(t, key, val)                   \
  say("Putting %s=%lu: ", key, val);         \
  if (trie_put(t, key, &val, sizeof(val))) { \
    FAIL;                                    \
  }                                          \
  OK;                                        \

#define getst(t, key, val)                             \
  do {                                                 \
    say("Getting %s=%lu: ", key, val);                 \
    const size_t *v = (const size_t*)trie_get(t, key); \
    if (NULL == v) {                                   \
      say("%s", "[NULL ret] ");                        \
      FAIL;                                            \
    }                                                  \
    if (*v != val) {                                   \
      say("Need: %lu, got: %lu ", val, *v);            \
      FAIL;                                            \
    }                                                  \
    OK;                                                \
  } while (0);


int main(int argc, char *argv[]) {
  sayn("%s", "Starting tests");
  
  trie_t *t = trie_init(NULL);

  char key5[] = "ccccccccc";
  //char key5[] = "ccccccccccccccccccccccccccccccccccccccccccccccccccccc";
  size_t val5 = 2;
  putst(t, key5, val5);
  getst(t, key5, val5);
  int val = 888;
  char key[] = "fstv";
  int val2 = 13;
  char key2[] = "sstv";
  int val3 = 666;
  char key3[] = "qwer";
  size_t val4 = 0;
  char key4[] = "aaa";
  puti(t, key, val);
  geti(t, key, val);
  puti(t, key2, val2);
  geti(t, key2, val2);
  puti(t, key3, val3);
  geti(t, key3, val3);
  putst(t, key4, val4);
  getst(t, key4, val4);

  puti(t, key, val);
  geti(t, key, val);
  puti(t, key, val);
  geti(t, key, val);
  puti(t, key2, val2);
  geti(t, key2, val2);
  geti(t, key, val);
  puti(t, key, val);
  geti(t, key, val);
  geti(t, key2, val2);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  puti(t, key3, val3);
  geti(t, key3, val3);
  geti(t, key2, val2);
  geti(t, key, val);

  sayn("%s", "Big data");
  const size_t tests_cnt = 100;
  char **keys = (char**)malloc(sizeof(char*) * tests_cnt);
  keys[0] = (char*)malloc(tests_cnt+1);
  strcpy(keys[0], "a");
  for (size_t i = 1; i < tests_cnt; ++i) {
    keys[i] = (char*)malloc(tests_cnt+1);
    strcpy(keys[i], keys[i-1]);
    keys[i][0]++;
    if (keys[i][0] == '\0')
      keys[i][0] = '\1';
  }
  size_t *vals = (size_t*)malloc(sizeof(size_t) * tests_cnt);
  for (size_t i = 0; i < tests_cnt; ++i) {
    strcat(keys[0], "a");
    for (size_t j = 1; j < i; ++j) {
      char s[] = "a";
      s[0] += j;
      if (s[0] == '\0') s[0] = '\1';
      strcat(keys[j], s);
    }
    vals[i] = i;
    for (size_t j = 0; j < i; ++j) {
      putst(t, keys[j], vals[j]);
      getst(t, keys[j], vals[j]);
    }
  }

  //sayn("%s", "Long lines");
  //char *ln = (char*)

  trie_destroy(t);
  (void)argc, (void)argv;
  return 0;
}
