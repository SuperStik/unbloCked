#ifndef HITRESULT_H
#define HITRESULT_H 1

struct UBLC_hitresult {
	unsigned x;
	unsigned y;
	unsigned z;
	unsigned o;
	unsigned f;
	unsigned char hit:1;
};

#endif /* HITRESULT_H */
