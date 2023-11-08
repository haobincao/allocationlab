# MallocLab

NOTE: You are explicitly forbidden to publish your solution or any part of this repository.

## Compiling `mtest`

There are two ways of compiling the executable program `mtest`:

1. `make` (or `make debug`) builds an executable
  - with debugging symbols (`-g`)
  - optimized for "debugging experience" (`-Og`)
  - with the macro `DEBUG` defined (so, it will run code inside `#ifdef DEBUG`)
  - with assertions enabled (so, `assert(condition)` is evaluated)

2. `make release` builds an executable
  - without debugging symbols
  - with the highest optimization level (`-O3`)
  - without the macro `DEBUG` not defined (so, it will skip code inside `#ifdef DEBUG`)
  - with assertions disabled (by defining the macro `NDEBUG`, so that they incur no overhead)

Since `make release` produces a faster executable, that's used to calculate your grade.

Instead, `make` is used to compile the tests, so that you can easily debug them.


## Running Tests

To run all the tests (recompiling if necessary, in debug mode): `make test`

To compile and run a specific test, for example `test/test_mm_list.c`, you can use
```
$ make bin/test_mm_list
$ ./bin/test_mm_list
```

If the test fails, you can CTRL-click in the VS Code terminal to go to the assertion that wasn't satisfied:

<img src="https://i.imgur.com/fu5igWB.png">

To quickly debug a test, you can use `gdb` in VS Code's terminal (since this is C, you'll use `n` instead of `ni`):

```
$ gdb bin/test_mm_list
(gdb) layout src
(gdb) b test_prepend_empty
(gdb) b run
```

<img src="https://i.imgur.com/b8gR4FM.png">

You can also use the VS Code debugger, but to do that you need to manually edit `.vscode/launch.json` to change the line:

```
            "program": "${workspaceFolder}/bin/mtest",
```

to

```
            "program": "${workspaceFolder}/bin/test_mm_list",
```

and then you can set breakpoints and start the VS Code debugger with the "play button":

<img src="https://i.imgur.com/BQds8tv.png">


## Checking Your Grade

To check your grade on all traces (recompiling if necessary, in release mode): `./grade`

To run only one trace, once: `./bin/mtest -r 1 -f traces/short1-bal.rep`

## Where to Start

Writing an explicit list (or segregated list) implementation of `malloc` may feel overwhelming... So, we've split the functions that you should implement into three compilation units: `mm_block.c`, `mm_list.c` and `mm.c` (and their headers). We recommend that you implement and test your functions in this order (each unit has a corresponding set of unit tests).

### `mm_block.c`

This unit contains utility functions to manipulate blocks stored on the heap. In particular, it contains:
- the global variable `heap_blocks` pointing to the first block;
- functions to read/write header and footer information of blocks;
- functions to find the next/previous adjacent block on the heap.

You can change the API of these functions as you wish. If you do so, you also need to update the tests.

### `mm_list.c`

This unit contains utility functions to manage a linked list of blocks stored on the heap. In particular, it contains:
- global variables `mm_list_headp` and `mm_list_tailp` pointing to the head/tail blocks of the free list;
- functions to append/prepend/remove a block from the free list.

Note that blocks are always stored on the heap; the linked list implementation simply updates pointers in their payloads.

You can change the API of these functions as you wish (and update the tests).

More importantly, you can decide to use a completely different data structure (e.g., segregated free lists, balanced trees, and so on).

### `mm.c`

This unit contains the implementation of the public API of your malloc: `mm_init`, `mm_malloc`, `mm_realloc`, `mm_free` (declared in `mm.h`). It uses the functions declared in `mm_block.h` to manage blocks, and the functions declared in `mm_list.h` to manage the explicit free list; it also defines some private (`static`) helper functions such as `find_fit`, `place`, `free_coalesce`, `extend_heap`, `required_block_size`.

You can change the API of the helper functions, but **not** the public API defined in `mm.h`:

```
int   mm_init(void);
void *mm_malloc(size_t size);
void *mm_realloc(void *ptr, size_t size);
void  mm_free(void *ptr);
```
