// vim: sw=2 ts=2 et :

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef TRIE_DEBUG

#define DBGF(fmt, ...) fprintf(stderr, "\nTRIE: " fmt "\n", __VA_ARGS__)
#define DBG(s) DBGF("%s", s)

#else /* TRIE_DEBUG */

#define DBGF(fmt, ...) (void)fmt;
#define DBG(s) (void)s;

#endif /* TRIE_DEBUG */

typedef char** (*divisor_t)(const char *);

// Structure for containing data, inserted by user
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

#ifdef TRIE_DEBUG
  size_t depth;      // Current depth of trie
  size_t max_key_len;// Maximum length of inserted key part
#endif /* TRIE_DEBUG */
} trie_t;

#define TRIE_IMPL
#include "trie.h"

/*
 * Divides s into symbols
 */
static char** default_divisor(char *s) {
  if (NULL == s) return NULL;

  char **symbols = (char**)malloc(sizeof(char*) * (strlen(s) + 1));
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
  node->key = (char*)malloc(strlen(key) + 1);
  if (NULL == node->key) {
    free(node);
    return NULL;
  }
  strcpy(node->key, key);
  node->childs = NULL;
  node->leaf = NULL;
  return node;
}

/*
 * Creates new trie structure.
 *
 * divisor : function, that devides key to N-gramms and returns NULL-terminated
 *           list of them. If isn't specified, uses default divisor, which
 *           devides key into characters.
 *
 * ret : pointer to trie structure or NULL. Shall be destroyed by
 *       trie_destroy(ret).
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


/*
 * Puts pair of key and value into trie.
 *
 * trie       : trie created by trie_init()
 * key        : key in trie, will be splitted by divisor
 * value      : pointer to value, strored in the trie. Will be copied
 * val_length : sizeof(value), length in bytes
 *
 * ret : 0 if successed
 */
int trie_put(trie_t *trie, const char *key, const void *value,
    size_t val_length) {
  if (NULL == trie) return 1;
  if (NULL == key) return 1;

#if defined(TRIE_PUT_DEBUG) || defined(TRIE_FULL_DEBUG)
  DBGF("trie_put, key=%s", key);
#endif /* TRIE_PUT_DEBUG || TRIE_FULL_DEBUG */

  int ret = 1;
  // Use given (or default) splitting function to divide key into pieces
  char **splitted = trie->divisor(key);

#if defined(TRIE_PUT_DEBUG) || defined(TRIE_FULL_DEBUG)
  DBG("Key was splitted into:");
  for (size_t i = 0; splitted[i]; ++i) {
    DBGF("\t%s", splitted[i]);
  }
#endif /* TRIE_PUT_DEBUG || TRIE_FULL_DEBUG */

  // Going down by the tree. It could be done recursively, but makes lack of
  // performance.
  trie_node_t *cur = trie->root;
  size_t i = 0;
  // Iterate by parts of key until all parts put onto their places
  while (NULL != splitted[i]) {
#ifdef TRIE_DEBUG
    size_t new_max_len = strlen(splitted[i]);
    if (new_max_len > trie->max_key_len) trie->max_key_len = new_max_len;
#endif

    // If this node wasn't used as an intermediate node in a path to values,
    // trying to create new path.
    if (NULL == cur->childs) goto create_suffix;
    // Otherwise, look for suitable child.
    trie_node_t **n;
    for (n = cur->childs; NULL != *n; ++n) {
      // TODO: implement binary search.
      if (!strcmp((*n)->key, splitted[i])) {
        // We've found the child, lets make *cur to point onto it, thus we will
        // work with cur->childs[n]->childs during next iteration of outer
        // loop.
        cur = *n;
        ++i;
        break;
      }
    }
    // "n" was initialized by one of children of current node, thus if it is
    // NULL, we've iterated out of the bound and we shall to create new child.
    if (NULL == *n) {
      // Or we just jumped here because of lack of children.
create_suffix:
      // Node wasn't used as an intermediate node in pathways to values.
      if (NULL == cur->childs) {
        if (NULL ==
            (cur->childs = (trie_node_t**)(malloc(2*sizeof(trie_node_t*))))) {
          goto clean_splitted;
        }
        // It is *NULL-terminated* array of pointers to trie_node.
        cur->childs[1] = NULL;
        if (NULL == (cur->childs[0] = trie_node_init(splitted[i]))) {
          goto clean_childs;
        }
        cur = cur->childs[0];
      } else { // Node is intermediate, just make new child.
        size_t cnt = 0;
        // TODO: make it sorted to get opportunity use binary search in the
        // code above.
        for (cnt = 0; cur->childs[++cnt];);
        if (NULL == (cur->childs[cnt] = trie_node_init(splitted[i]))) {
          goto clean_splitted;
        }
        trie_node_t **new_childs;
        if (NULL == (new_childs = (trie_node_t**)realloc(cur->childs,
                sizeof(trie_node_t*) * (cnt + 2)))) {
          goto clean_splitted;
        }
        cur->childs = new_childs;
        cur->childs[cnt+1] = NULL;
        cur = cur->childs[cnt];
      }
      ++i;
    }
  }
  // Node is found and prepared for new child insertion.
  if (NULL == cur->leaf) { // New child
    if (NULL == (cur->leaf = (trie_leaf_t*)malloc(sizeof(trie_leaf_t)))) {
      goto clean_splitted;
    }
  } else { // Node is already in use, overwrite.
    free(cur->leaf->value);
  }

  if (NULL == (cur->leaf->value = (char*)malloc(val_length))) {
    free(cur->leaf);
    goto clean_splitted;
  }
  memcpy(cur->leaf->value, value, val_length);
  cur->leaf->val_length = val_length;
  ret = 0;
  goto clean_splitted;

clean_childs:
  free(cur->childs);
  cur->childs = NULL;

clean_splitted:
  for (size_t it = 0; NULL != splitted[it]; free(splitted[it]), ++it);
  free(splitted);

  return ret;
}


/*
 * Retrieves pointer to value from the trie.
 *
 * trie : trie created by trie_init()
 * key  : key of value, used in trie_put()
 *
 * ret : constant pointer to value. Recommended to avoid something like
 *       const_cast<>(ret), copy it instad of modifying.
 */
const void* trie_get(const trie_t *trie, const char *key) {
  // Tree traversal algorithm is very similar to algorithm in trie_put() above.
  // read comments to that function to get the awareness.
  if (NULL == trie || NULL == key) return NULL;
  trie_node_t *cur = trie->root;
  char **splitted = trie->divisor(key);
  if (NULL == splitted) return NULL;
  void *ret = NULL;
  size_t i = 0;
  while (NULL != splitted[i]) {
    if (NULL == cur->childs) goto clean_splitted;
    trie_node_t **n;
    for (n = cur->childs; NULL != *n; ++n) {
      if (!strcmp((*n)->key, splitted[i])) {
        // We've found the part
        cur = *n;
        ++i;
        break;
      }
    }
    if (NULL == *n) goto clean_splitted;
  }
  if (NULL == cur->leaf) goto clean_splitted;
  ret = cur->leaf->value;

clean_splitted:
  for (size_t it = 0; NULL != splitted[it]; free(splitted[it]), ++it);
  free(splitted);

  return ret;
}


static void trie_node_destroy(trie_node_t *n) {
  if (NULL == n) return;
  if (NULL == n->childs) goto clean_leaf;

  for (trie_node_t **cur = n->childs; NULL != *cur; ++cur) {
    trie_node_destroy(*cur);
  }
  free(n->childs);

clean_leaf:
  if (NULL == n->leaf) goto clean_n;
  if (NULL != n->leaf->value)
    free(n->leaf->value);
  free(n->leaf);

clean_n:
  free(n);
}


/*
 * Destroys (deallocates) trie.
 *
 * trie : trie created by trie_init()
 */
void trie_destroy(trie_t *trie) {
  if (NULL == trie) return;

  trie_node_destroy(trie->root);

  free(trie);
}

#ifdef TRIE_DEBUG

static int nl = 1;

static void trie_explore(const trie_t *t, const trie_node_t *n, int depth) {
  if (NULL == n) return;
  if (NULL == n->key) {
    DBG("key is corrupted here");
    return;
  }
  if (nl) {
    for (size_t i = 0; i < depth; ++i) {
      for (size_t j = 0; j < t->max_key_len + 10; ++j) {
        fputs(" ", stderr);
      }
    }
  }
  nl = 0;
  fprintf(stderr, " : %s ", n->key);
  fputs(n->leaf ? "[leaf]" : "[empt]", stderr);
  for (size_t i = 0; i < t->max_key_len - strlen(n->key); ++i) {
    fputs(" ", stderr);
  }
  if (NULL == n->childs) {
    nl = 1;
    fputs("\n", stderr);
    return;
  }
  for (size_t i = 0; n->childs[i]; ++i) {
    trie_explore(t, n->childs[i], depth + 1);
  }
}


/*
 * Outputs dump of trie data.
 *
 * trie : trie to dump
 */
void trie_dump(const trie_t *trie) {
  fputs("\n", stderr);
  if (NULL == trie) {
    DBG("[NULL]");
    return;
  }
  DBG("Not null, printing all pathways");
  trie_explore(trie, trie->root, 1);
}

#endif /* TRIE_DEBUG */
