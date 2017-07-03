// vim: sw=2 ts=2 et :

#ifndef TRIE_H_
#define TRIE_H_

#include <stdlib.h>

#ifndef TRIE_IMPL
struct trie;
typedef struct trie trie_t;
typedef char** (*divisor_t)(const char *);
#endif


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
trie_t* trie_init(divisor_t divisor);

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
int trie_put(trie_t *trie, const char *key, const void* value,
    size_t val_length);

/*
 * Retrieves pointer to value from the trie.
 *
 * trie : trie created by trie_init()
 * key  : key of value, used in trie_put()
 *
 * ret : constant pointer to value. Recommended to avoid something like
 *       const_cast<>(ret), copy it instad of modifying.
 */
const void* trie_get(trie_t *trie, const char *key);

/*
 * Destroys (deallocates) trie.
 *
 * trie : trie created by trie_init()
 */
void trie_destroy(trie_t *trie);

#endif /* TRIE_H_ */
