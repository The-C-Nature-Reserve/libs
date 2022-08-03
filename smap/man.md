# smap

A `cstr (char*)` to `void*` map. Uses fnv-1 hash function. And is managed
internally. Will rehash on a loadfactor of `0.5` down to `0.25` and grow
exponentially with base `2`

Note: This implementation is **not thread safe!**

## smap_create
```smap_t* smap_create(uint32_t size);```

### Definition
Will create a heap allocated `smap_t` and return a pointer to it.

### Return
- On **Success**, returning a valid pointer to a `smap_t` struct. 
- On **Failure**, returning `NULL`. (_bad_alloc_)

### Caution
- `size` should not get bigger than `((uint32_t)-1) >> 2`. Otherwise overflows
  might occur.
- `size` should be greater than `0`
- created struct must later be freed via `smap_free`

## smap_free
```void smap_free(smap_t* m);```

### Definition
Will free the with `m` associated memory. Will not perform any action on `m ==
NULL`

### Caution
- will not free memory associated with keys and values stored in the map. If the
  user uses heap allocations to store his stuff, it is his responsibility to
  clean it up. This can be most easily achieved with the `map_iter` utilities
  or `map_move_pair`

## smap_insert
```void* smap_insert(smap_t* m, char* key, void* value);```

### Definition
Will create a mapping between `key` and `value`. `value` can later be retrieved
by referencing it with `key` (Note: the string `key` points to not the pointer
itsself).

### Return
- On **Success**, returning `value`.
- On **Failure**, returning `NULL`. (_bad_alloc_)

### Caution
- If `key` is already present it will not update the stored `key` pointer. Use
  `smap_insert_key` for that.
- It returns a raw pointer be aware of that.

## smap_get
```void* smap_get(smap_t* m, char* key);```

### Definition
Will return the with `key` associated `value`.

### Return
- returns `NULL` if `key` is not present
- returns `void* value` if `key` is present
 
### Caution
- It returns a raw pointer be aware of that.

## smap_size
```uint32_t smap_size(smap_t* m);```

### Definition
Returns the current size of the map

## smap_move_pair
```void smap_move_pair(smap_t* m, char* key, char** dest_key, void** dest_value);```

### Definition
If `key` is present in the map it moves the `key` pointer stored into `dest_key`
and the `value` pointer stored into `dest_value`. If `key` is not present it
will move `NULL` into both. And finnally delete the entry.

You may pass `NULL` as `dest_key` or `dest_value` on indicate you are not
interested in the associated pointer.

### Caution
- It will delete the entry: all pointers that point to heap blocks should be
  retrieved

## smap_copy_pair
```void smap_copy_pair(smap_t* m, char* key, char** dest_key, void** dest_value);```

### Definition
If `key` is present in the map it will copy the stored `key` pointer and `value`
pointer to `dest_key` and `dest_value`

### Caution
- Since the stored `key` is still in the map, altering the memory it points to
  will lead to **undefined behavior**

## smap_contains
```bool smap_contains(smap_t* m, char* key);```

### Definition
Checks if `key` is present in the table

### Return
- returns `0` / `false` if `key` is not present
- returns 1 / `true` if `key` is present

## smap_iter_next
```smap_iter_t smap_iter_next(smap_t* m, smap_iter_t i);```

### Definition
Can be used to iterate over `m`. If passed a `SMAP_NULL` as `i` it returns first element
in `m` if `i` is last element in `m` it returns `SMAP_NULL`.

### Return
- returns first element in `m` if `i == SMAP_NULL`
- returns next element in `m` if `i != SMAP_NULL && i in m`
- returns `SMAP_NULL` if `i` is last element in `m`

### Caution
- Don't `delete` the current iterator in any way. This will result in
  **undifined behavior** for the next `smap_iter_next` call

## smap_iter_key
```char* smap_iter_key(smap_t* m, smap_iter_t i);```

### Definition
If given a valid `i` it returns the associated `key` pointer

### Returns
- Always returns pointer stored as `key`

### Caution
- Expects valid `i`, does not perform any check at all
- Don't alter memory `key` points to. Will result in **undifined behavior**

## smap_iter_value
```void* smap_iter_value(smap_t* m, smap_iter_t i);```

### Definition
If given a valid `i` it returns the associated `value` pointer

### Returns
- Always returns pointer stored as `value`

### Caution
- Expects valid `i`, does not perform any check at all

