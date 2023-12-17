#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "simple_logger.h"

#include "entity.h"

Entity *projectile_new_from_config(Entity *parent,Vector3D position, Vector3D dir, const char *configFile);

Entity *projectile_new(Entity *parent,Vector3D position, Vector3D dir, float speed,float damage);

Entity *short_projectile_new(Entity *parent,Vector3D position, Vector3D dir, float speed,float damage);

Entity *shield_new(Entity *parent,Vector3D position, Vector3D dir);

#endif
