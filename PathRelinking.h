#ifndef _PATH_RELINKING_
#define _PATH_RELINKING_

#include "Solution.h"
#include "Instance.h"

inline int pool_size = 7;
inline int similarity_threshold = 4;
inline vector< Solution > eliteSet;

void addToElitePool(Solution s);
void getBestPathRelinking(Solution& s);

#endif // _PATH_RELINKING_
