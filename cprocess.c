#include "compiler.h"
#include "helpers/vector.h"
#include <stdio.h>
#include <stdlib.h>
struct compile_process *compile_process_create(const char *filename,
                                               const char *filename_out,
                                               int flags) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    return NULL;
  }

  FILE *out_file = NULL;
  if (filename_out) {
    out_file = fopen(filename_out, "w");
    if (!out_file) {
      return NULL;
    }
  }

  struct compile_process *process = calloc(1, sizeof(struct compile_process));
  process->node_vec = vector_create(sizeof(struct node *));
  process->node_tree_vec = vector_create(sizeof(struct node *));

  process->flags = flags;
  process->cfile.fp = file;
  process->ofile = out_file;
  process->generator = codegenerator_new(process);
  process->resolver = resolver_default_new_process(process);
  process->nodeGarbageVec = vector_create(sizeof(struct node *));
  process->gb = vector_create(sizeof(void *));
  process->gbForVectors = vector_create(sizeof(struct vector *));
  process->gbVectorForCustonResolverEntities =
      vector_create(sizeof(struct resolver_entity *));

  symresolver_initialize(process);
  symresolver_new_table(process);

  return process;
}

char compile_process_next_char(struct lex_process *lex_process) {
  struct compile_process *compiler = lex_process->compiler;
  compiler->pos.col += 1;
  char c = getc(compiler->cfile.fp);
  if (c == '\n') {
    compiler->pos.line += 1;
    compiler->pos.col = 1;
  }

  return c;
}
void freeVectorContentsVectors(struct vector *vecToFree) {
  struct vector **data = (struct vector **)vector_peek(vecToFree);
  while (data) {
    if (*data) {
      vector_free(*data);
    }
    data = (struct vector **)vector_peek(vecToFree);
  }
}

void freeVectorContents(struct vector *vecToFree) {
  vector_set_peek_pointer(vecToFree, 0);
  void **data = (void **)vector_peek(vecToFree);
  while (data) {
    if (*data) {
      free(*data);
    }
    data = (void **)vector_peek(vecToFree);
  }
}

void free_compile_process(struct compile_process *cp) {
  if (cp->cfile.fp) {
    fclose(cp->cfile.fp);
  }
  if (cp->ofile) {
    fclose(cp->ofile);
  }
  freeVectorContents(cp->nodeGarbageVec);
  vector_free(cp->node_tree_vec);
  vector_free(cp->node_vec);
  vector_free(cp->nodeGarbageVec);
  freeVectorContentsVectors(cp->gbForVectors);
  vector_free(cp->gbForVectors);
  freeVectorContents(cp->gb);
  vector_free(cp->gbVectorForCustonResolverEntities);
  vector_free(cp->gb);
}
char compile_process_peek_char(struct lex_process *lex_process) {
  struct compile_process *compiler = lex_process->compiler;
  char c = getc(compiler->cfile.fp);
  ungetc(c, compiler->cfile.fp);
  return c;
}

void compile_process_push_char(struct lex_process *lex_process, char c) {
  struct compile_process *compiler = lex_process->compiler;
  ungetc(c, compiler->cfile.fp);
}
