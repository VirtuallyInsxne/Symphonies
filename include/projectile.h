#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "simple_logger.h"

#include "entity.h"

Entity *projectile_new(Entity *parent, Entity *target, Vector3D position, Vector3D dir, float speed, float damage);

#endif
