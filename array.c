#include "compiler.h"
#include "helpers/vector.h"
#include <assert.h>

static struct compile_process *cp;

void set_compile_process_for_array(struct compile_process *compile_proc) {
  cp = compile_proc;
}

struct array_brackets *array_brackets_new() {
  struct array_brackets *brackets = calloc(1, sizeof(struct array_brackets));
  brackets->n_brackets = vector_create(sizeof(struct node *));
  return brackets;
}

void array_brackets_free(struct array_brackets *brackets) { free(brackets); }

void array_brackets_add(struct array_brackets *brackets,
                        struct node *bracket_node) {
  if (bracket_node->type != NODE_TYPE_BRACKET) {
    compiler_error(cp, "Array error: not a valid bracket node \n");
  }
  vector_push(brackets->n_brackets, &bracket_node);
}

struct vector *array_brackets_node_vector(struct array_brackets *brackets) {
  return brackets->n_brackets;
}

size_t array_brackets_calculate_size_from_index(struct datatype *dtype,
                                                struct array_brackets *brackets,
                                                int index) {
  struct vector *array_vec = array_brackets_node_vector(brackets);
  size_t size = dtype->size;
  if (index >= vector_count(array_vec)) {
    // char* abc;
    // return abc[0]; return abc[1];
    return size;
  }

  vector_set_peek_pointer(array_vec, index);
  struct node *array_bracket_node = vector_peek_ptr(array_vec);
  if (!array_bracket_node) {
    return 0;
  }

  while (array_bracket_node) {
    if (array_bracket_node->bracket.inner->type != NODE_TYPE_NUMBER) {
      compiler_error(
          cp, "array error: not a valid number node inside array brackets \n");
    }
    int number = array_bracket_node->bracket.inner->llnum;
    size *= number;
    array_bracket_node = vector_peek_ptr(array_vec);
  }

  return size;
}

size_t array_brackets_count(struct datatype *dtype) {
  return vector_count(dtype->array.brackets->n_brackets);
}

size_t array_brackets_calculate_size(struct datatype *dtype,
                                     struct array_brackets *brackets) {
  return array_brackets_calculate_size_from_index(dtype, brackets, 0);
}

int array_total_indexes(struct datatype *dtype) {
  if (!(dtype->flags & DATATYPE_FLAG_IS_ARRAY)) {
    compiler_error(cp, "array error: not an array datatype \n");
  }
  struct array_brackets *brackets = dtype->array.brackets;
  return vector_count(brackets->n_brackets);
}
