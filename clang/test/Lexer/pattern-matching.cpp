// RUN: %clang_cc1 -fsyntax-only %s
// RUN: %clang_cc1 -fpattern-matching -DPATTERN_MATCHING -fsyntax-only %s

#ifdef PATTERN_MATCHING
#define PATTERN_MATCHING_KEYWORD(NAME) _Static_assert(!__is_identifier(NAME), #NAME)
#else
#define PATTERN_MATCHING_KEYWORD(NAME) _Static_assert(__is_identifier(NAME), #NAME)
#endif

PATTERN_MATCHING_KEYWORD(inspect);
