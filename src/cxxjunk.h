#ifndef CXXJUNK_H
#define CXXJUNK_H 1

#ifdef __cplusplus
# define BEGIN_DECL extern "C" {
# define END_DECL }
#else
# define BEGIN_DECL
# define END_DECL
#endif /* __cplusplus */

#ifndef __GNUC__
# define __attribute__(__VA_ARGS__)
#endif /* __GNUC__ */

#endif /* CXXJUNK_H */
