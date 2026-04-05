#define _DEFAULT_SOURCE
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cachelab.h"

/* runtime helpers ***********************************************************/

void exit_message(char *s) {
  fprintf(stderr, "%s\n", s);
  exit(EXIT_FAILURE);
}

void exit_perror(char *s) {
  perror(s);
  exit(EXIT_FAILURE);
}

/* arg parsing helpers *******************************************************/

void exit_usage(char *cmd) {
  const char *usage = "[-hv] -s <num> -E <num> -b <num> -t <file>";
  fprintf(stderr, "Usage: %s %s\n", cmd, usage);
  exit(EXIT_FAILURE);
}

long optarg_to_long_or_exit_usage(char *cmd) {
  int tmp_errno = errno;
  errno = 0;
  auto val = strtol(optarg, NULL, 10);
  if (errno != 0)
    exit_usage(cmd);
  errno = tmp_errno;
  return val;
}

char *optarg_to_str_or_exit_usage(char *cmd) {
  if (optarg == NULL)
    exit_usage(cmd);
  auto len = strlen(optarg);
  if (len > 255) {
    fprintf(stderr, "optarg str too large\n");
    exit_usage(cmd);
  }
  char *str = malloc(len + 1);
  if (str == NULL) {
    fprintf(stderr, "unable to malloc optarg str\n");
    exit(EXIT_FAILURE);
  }
  strncpy(str, optarg, len);
  str[len] = '\0';
  // NOTE: str doesn't need to be freed because this is just to parse args
  return str;
}

typedef struct args {
  long set_bits;
  long lines_per_set;
  long block_bits;
  char *filename;
  bool verbose;
} args_t;

bool verbose = false;

args_t parse_args(int argc, char *argv[]) {
  args_t args = {
      .set_bits = -1,
      .lines_per_set = -1,
      .block_bits = -1,
      .filename = NULL,
      .verbose = false,
  };

  int opt;
  while ((opt = getopt(argc, argv, "s:E:b:t:v")) != -1) {
    switch (opt) {
    case 's':
      args.set_bits = optarg_to_long_or_exit_usage(argv[0]);
      break;
    case 'E':
      args.lines_per_set = optarg_to_long_or_exit_usage(argv[0]);
      break;
    case 'b':
      args.block_bits = optarg_to_long_or_exit_usage(argv[0]);
      break;
    case 't':
      args.filename = optarg_to_str_or_exit_usage(argv[0]);
      break;
    case 'v':
      args.verbose = true;
      break;
    default: /* '?' */
      exit_usage(argv[0]);
    }
  }

  if (args.set_bits < 0) {
    fprintf(stderr, "-s needs to be >= 0\n");
    exit_usage(argv[0]);
  }
  if (args.lines_per_set < 1) {
    fprintf(stderr, "-E needs to be > 0\n");
    exit_usage(argv[0]);
  }
  if (args.block_bits < 0) {
    fprintf(stderr, "-b needs to be >= 0\n");
    exit_usage(argv[0]);
  }
  if (args.filename == NULL) {
    fprintf(stderr, "-t needs to be set\n");
    exit_usage(argv[0]);
  }

  verbose = args.verbose;

  return args;
}

#define printf_verbose(...)                                                    \
  if (verbose) {                                                               \
    printf(__VA_ARGS__);                                                       \
  }

/* input parsing *************************************************************/

typedef enum in_action_type {
  line_type_unknown,
  line_type_load,
  line_type_store,
  line_type_mod,
  line_type_instruction,
} in_action_type_t;

struct in_action {
  in_action_type_t type;
  uint64_t addr;
  size_t size;
};

char in_action_type_char(in_action_type_t t) {
  switch (t) {
  case line_type_load:
    return 'L';
  case line_type_store:
    return 'S';
  case line_type_mod:
    return 'M';
  case line_type_instruction:
    return 'I';
  default:
    return 'U';
  }
}

int parse_line(const char *line, struct in_action *out) {
  if (!line || !out)
    return -1;

  // skip leading whitespace
  while (isspace((unsigned char)*line)) {
    line++;
  }

  // parse operation type
  char op = *line++;
  switch (op) {
  case 'L':
    out->type = line_type_load;
    break;
  case 'S':
    out->type = line_type_store;
    break;
  case 'M':
    out->type = line_type_mod;
    break;
  case 'I':
    out->type = line_type_instruction;
    break;
  default:
    out->type = line_type_unknown;
    return -1;
  }

  // skip whitespace between op and address
  while (isspace((unsigned char)*line)) {
    line++;
  }

  // parse address and size
  // format: <hex>,<decimal>
  char *endptr;

  out->addr = strtoull(line, &endptr, 16);
  if (endptr == line || *endptr != ',') {
    return -1;
  }

  line = endptr + 1;

  out->size = strtoull(line, &endptr, 10);
  if (endptr == line) {
    return -1;
  }

  return 0;
}

/* simuation *****************************************************************/

// TODO: Move to a single allocation and use char[] at the end of structs vs
// pointers.

typedef struct line {
  bool valid;
  uint64_t use; // use counter to determine lru
  uint64_t tag;
  // char block[];  // NOTE: block data is not needed for simulation
} line_t;

typedef struct set {
  uint64_t use; // use counter to determine lru
  long lines_per_set;
  line_t *lines; // array of lines
} set_t;

set_t *new_set(long lines_per_set) {
  set_t *s = calloc(1, sizeof(set_t));
  if (s == NULL)
    exit_message("unable to allocate set");

  s->lines_per_set = lines_per_set;

  s->lines = calloc(lines_per_set, sizeof(*s->lines));
  if (s->lines == NULL)
    exit_message("unable to allocate lines");

  return s;
}

void free_set(set_t *s) {
  free(s->lines);
  free(s);
}

typedef struct addr {
  uint64_t tag;
  uint64_t set;
  uint64_t block;
} addr_t;

typedef enum result {
  result_unknown,
  result_hit,
  result_miss,
  result_evicted,
} result_t;

result_t upsert_line(set_t *s, addr_t addr) {
  int valid_count = 0;
  int last_invalid = -1;
  int lru_idx = -1; // index of the least reacently used line
  int lru_use = 0;  // value of the lru line's use

  for (auto i = 0; i < s->lines_per_set; i++) {
    auto line = &s->lines[i];
    if (line->valid) {
      valid_count++;
      if (line->tag == addr.tag) {
        line->use = ++s->use;
        return result_hit;
      }
    } else
      last_invalid = i;

    if (lru_idx < 0 || lru_use > line->use) {
      lru_idx = i;
      lru_use = line->use;
    }
  }

  line_t line = {
      .valid = true,
      .use = ++s->use,
      .tag = addr.tag,
  };

  if (valid_count < s->lines_per_set) {
    assert(last_invalid >= 0);
    s->lines[last_invalid] = line;
    return result_miss;
  }

  // printf_verbose("evicting\n");
  // printf_verbose("  lru_idx: %d\n", lru_idx);
  // printf_verbose("  lru_use: %d\n", lru_use);
  // printf_verbose("  lines[lru_idx].tag: %lx\n", s->lines[lru_idx].tag);
  // for (auto i = 0; i < s->lines_per_set; i++) {
  //   printf_verbose("  lines[%d]:\n", i);
  //   printf_verbose("    lines[%d].use: %ld\n", i, s->lines[i].use);
  // }

  assert(lru_idx >= 0);
  s->lines[lru_idx] = line;
  return result_evicted;
}

typedef struct cache {
  long set_bits;
  long number_of_sets;
  long block_bits;
  set_t **sets;
} cache_t;

cache_t *new_cache(long set_bits, long lines_per_set, long block_bits) {
  cache_t *c = calloc(1, sizeof(cache_t));
  if (c == NULL)
    exit_message("unable to allocate cache");

  auto num_sets = 1 << set_bits;
  c->set_bits = set_bits;
  c->number_of_sets = num_sets;
  c->block_bits = block_bits;

  c->sets = calloc(num_sets, sizeof(*c->sets));
  if (c->sets == NULL)
    exit_message("unable to allocate sets");

  for (auto i = 0; i < num_sets; i++) {
    c->sets[i] = new_set(lines_per_set);
  }

  return c;
}

void free_cache(cache_t *c) {
  for (auto i = 0; i < c->number_of_sets; i++) {
    free_set(c->sets[i]);
  }
  free(c->sets);
  free(c);
}

addr_t parse_addr(cache_t *c, uint64_t addr) {
  // [tag bits] [set bits] [block bits]

  auto tag_bits = sizeof(addr) * 8 - c->set_bits - c->block_bits;

  addr_t a = {
      .tag = addr >> (c->set_bits + c->block_bits),
      .set = addr << tag_bits >> tag_bits >> c->block_bits,
      .block = addr << (tag_bits + c->set_bits) >> (tag_bits + c->set_bits),
  };

  // printf_verbose("parse_addr:\n");
  // printf_verbose("  .tag: %ld\n", a.tag);
  // printf_verbose("  .set: %ld\n", a.set);
  // printf_verbose("  .block: %ld\n", a.block);

  return a;
}

set_t *select_set(cache_t *c, addr_t addr) { return c->sets[addr.set]; }

typedef struct sim {
  cache_t *cache;
  uint64_t hits;
  uint64_t misses;
  uint64_t evictions;
} sim_t;

sim_t *new_sim(long set_bits, long lines_per_set, long block_bits) {
  sim_t *s = calloc(1, sizeof(sim_t));
  if (s == NULL)
    exit_message("unable to allocate sim");

  s->cache = new_cache(set_bits, lines_per_set, block_bits);

  return s;
}

void free_sim(sim_t *s) {
  free_cache(s->cache);
  free(s);
}

void record_result(sim_t *s, result_t r) {
  switch (r) {
  case result_hit:
    s->hits++;
    printf_verbose("hit ");
    break;
  case result_miss:
    s->misses++;
    printf_verbose("miss ");
    break;
  case result_evicted:
    s->misses++;
    s->evictions++;
    printf_verbose("miss eviction ");
    break;
  default:
    exit_message("unknown result");
  }
}

void load(sim_t *s, uint64_t a, size_t size) {
  // NOTE: size can be ignored because the lab said all memory accesses are
  // aligned and never cross block boundaries
  auto addr = parse_addr(s->cache, a);
  auto set = select_set(s->cache, addr);
  auto result = upsert_line(set, addr);

  // printf_verbose("set state:\n");
  // printf_verbose("  use: %ld\n", set->use);
  // for (auto i = 0; i < set->lines_per_set; i++) {
  //   printf_verbose("  lines[%d].use: %ld\n", i, set->lines[i].use);
  //   printf_verbose("  lines[%d].tag: %lx\n", i, set->lines[i].tag);
  //   printf_verbose("  lines[%d].valid: %b\n", i, set->lines[i].valid);
  // }

  record_result(s, result);
};

void store(sim_t *s, uint64_t addr, size_t size) {
  // NOTE: since data is not actually stored, store can be the same as a load
  load(s, addr, size);
};

void print_summary(sim_t *s) { printSummary(s->hits, s->misses, s->evictions); }

void run_sim(sim_t *s, args_t args) {
  auto file = fopen(args.filename, "r");
  if (file == NULL)
    exit_perror("fopen");

  char *line = NULL;
  size_t len = 0;

  while (getline(&line, &len, file) != -1) {
    struct in_action act;
    if (parse_line(line, &act) != 0) {
      exit_message("parse error");
    }

    printf_verbose("%c %llx,%zu ", in_action_type_char(act.type),
                   (unsigned long long)act.addr, act.size);

    switch (act.type) {
    case line_type_load:
      load(s, act.addr, act.size);
      break;
    case line_type_store:
      store(s, act.addr, act.size);
      break;
    case line_type_mod:
      load(s, act.addr, act.size);
      store(s, act.addr, act.size);
      break;
    case line_type_instruction:
      // NOTE: lab instructions said to ignore this
      break;
    default:
      exit_message("invalid action");
    }

    printf_verbose("\n");
  }

  free(line);
  fclose(file);
}

/* main **********************************************************************/

int main(int argc, char *argv[]) {
  auto args = parse_args(argc, argv);

  // print parameters
  // printf_verbose("running simulation with these parameters:\n");
  // printf_verbose("  set_bits: %ld\n", args.set_bits);
  // printf_verbose("  lines_per_set: %ld\n", args.lines_per_set);
  // printf_verbose("  block_bits: %ld\n", args.block_bits);
  // printf_verbose("  filename: %s\n\n", args.filename);

  // start sim
  auto s = new_sim(args.set_bits, args.lines_per_set, args.block_bits);

  // run the simulation from input
  run_sim(s, args);

  // report and free
  print_summary(s);
  free_sim(s);

  return 0;
}
