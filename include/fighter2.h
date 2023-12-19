#ifndef __HEIHACHI_H__
#define __HEIHACHI_H__

#include "entity.h"

/**
 * @brief Create a new heihachi entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an heihachi entity pointer on success
 */
// Update the declaration in the header file
Entity *fighter2_new(Vector3D position, Vector3D rotation, int playerNum);



#endif
