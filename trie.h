// vim: sw=2 ts=2 et :

#ifndef _TRIE_H_
#define _TRIE_H_

#include <stdlib.h>

#ifndef TRIE_IMPL
struct trie;
typedef struct trie trie_t;
typedef char** (*divisor_t)(const char *);
#endif


trie_t* trie_init(divisor_t divisor);
int trie_put(trie_t *trie, const char *key, const void* value,
    size_t value_size);
const void* trie_get(trie_t *trie, const char *key);
void trie_destroy(trie_t *trie);

#endif /* _TRIE_H_ */
