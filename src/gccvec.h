#ifndef GCCVEC_H
#define GCCVEC_H 1

#define gvec(type, elems) type __attribute__((vector_size(sizeof(type)*elems)))

#endif /* GCCVEC_H */
