// vim: sw=2 ts=2 et :

#include <string.h>
#include <stdlib.h>

typedef char** (*divisor_t)(const char *);

// Structure for containing data
typedef struct trie_leaf {
  size_t val_length; // size of user data
  void *value;       // pointer to user data
} trie_leaf_t;

typedef struct trie_node {
  char *key;                 // part of path to data
  struct trie_node **childs; // children of this node
  trie_leaf_t *leaf;         // user data or NULL
} trie_node_t;

typedef struct trie {
  divisor_t divisor; // function for splitting keys
  trie_node_t *root; // root node of trie
} trie_t;

#define TRIE_IMPL
#include "trie.h"

/*
 * Divides s into symbols
 */
static char** default_divisor(char *s) {
  if (NULL == s) return NULL;

  char **symbols = (char**)malloc(strlen(s) + 1);
  if (NULL == symbols) return NULL;

  size_t i = 0;
  for (i = 0; s[i] != '\0'; ++i) {
    if (NULL == (symbols[i] = (char*)malloc(2))) {
      for (size_t j = 0; j < i; ++j) {
        free(symbols[j]);
      }
      free(symbols);
      return NULL;
    }
    symbols[i][0] = s[i];
    symbols[i][1] = '\0';
  }
  symbols[i] = NULL;
  return symbols;
}

static trie_node_t* trie_node_init(const char *key) {
  trie_node_t *node = (trie_node_t*)malloc(sizeof(trie_node_t));
  if (NULL == node) return NULL;
  node->key = (char*)malloc(strlen(key));
  if (NULL == node->key) {
    free(node);
    return NULL;
  }
  strcpy(node->key, key);
  node->childs = NULL;
  return node;
}

/*
 * Creates new trie structure.
 *
 * divisor : function, that devides key to N-gramms and returns NULL-terminated
 *           list of them.
 *
 * ret : pointer to trie structure. Shall be destroyed by trie_destroy(ret).
 */
trie_t* trie_init(divisor_t divisor) {
  trie_t *trie;

  if (NULL == divisor) divisor = (divisor_t)default_divisor;

  trie = malloc(sizeof(trie_t));
  if (NULL == trie) return NULL;

  trie->divisor = divisor;
  if (NULL == (trie->root = trie_node_init(""))) {
    free(trie);
    return NULL;
  }

  return trie;
}


int trie_put(trie_t *trie, const char *key, const void *value,
    size_t val_length) {
  if (NULL == trie) return 1;
  if (NULL == key) return 1;

  char **splitted = trie->divisor(key);

  (void) splitted;
  (void) value;
  (void) val_length;
  return 1;
}


const void* trie_get(trie_t *trie, const char *key) {
  if (NULL == trie || NULL == key) return NULL;
  trie_node_t *cur = trie->root;
  char **splitted = trie->divisor(key);
  if (NULL == splitted) return NULL;
  void *ret = NULL;
  size_t i = 0;
  while (NULL != splitted[i]) {
    if (NULL == cur->childs) goto clean_splitted;
    trie_node_t *n;
    for (n = *cur->childs; NULL != n; ++n) {
      if (!strcmp(n->key, splitted[i])) {
        // We've found the part
        cur = n;
        ++i;
        break;
      }
    }
    if (NULL == n) goto clean_splitted;
  }
  if (NULL == cur->leaf) goto clean_splitted;
  ret = cur->leaf->value;

clean_splitted:
  for (char *i = splitted[0]; NULL != i; free(i), ++i);
  free(splitted);

  return ret;
}


/*
 * Destroys trie.
 *
 * trie : valid trie_t structure or NULL.
 */
void trie_destroy(trie_t *trie) {
  free(trie);
}
