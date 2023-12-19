#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "entity.h"

Entity *projectile_new_low(Vector3D position, Vector3D velocity, EntityState state);

Entity *projectile_new_mid(Vector3D position, Vector3D velocity, EntityState state);

Entity *projectile_new_high(Vector3D position, Vector3D velocity, EntityState state);

void projectile_think(Entity *self);

void projectile_update(Entity *self);

// Example function to spawn a projectile (you can call this when a player triggers an attack)
void spawn_projectile(Vector3D position, Vector3D velocity, EntityState state);

#endif
