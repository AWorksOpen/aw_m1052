a = array_create()
assert(array_push(a,1,2,3) == 3)
assert(array_push(a,"a","b","c") == 3)
assert(array_pop(a), "c")
assert(array_pop(a), "b")
assert(array_pop(a), "a")
assert(array_pop(a), "3")
assert(array_pop(a), "2")
assert(array_pop(a), "1")
assert(!value_is_valid(array_pop(a)))
assert(array_clear(a))
assert(a.size == 0)

assert(array_insert(a, 0, 'a'))
assert(array_insert(a, 1, 'b'))
assert(array_insert(a, 2, 'c'))
assert(array_insert(a, 3, 'd'))
assert(a.size == 4)
assert(array_remove(a, 0))

assert(array_get(a, 0) == 'b')
assert(array_set(a, 0, 'bbb'))
assert(array_get(a, 0) == 'bbb')
assert(array_set(a, 0, 'b'))

assert(array_get(a, 1) == 'c')
assert(array_set(a, 1, 'ccc'))
assert(array_get(a, 1) == 'ccc')
assert(array_set(a, 1, 'c'))
assert(array_join(a, ',') == 'b,c,d')
print(a.size, a.capacity)
