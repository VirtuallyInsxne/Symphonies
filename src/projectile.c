#include "simple_logger.h"
#include "projectile.h"


void projectile_think(Entity *self);
void projectile_update(Entity *self);


Entity *projectile_new_from_config(Entity *parent,Vector3D position, Vector3D dir, const char *configFile)
{
    SJson *config;
    float speed = 0;
    float damage = 0;

    config = sj_load(configFile);
    if (!config)
    {
        slog("failed to load projectile config file %s",configFile);
        return NULL;
    }
    // PARSE THE FILE
    sj_object_get_value_as_float(config,"speed",&speed);
    sj_object_get_value_as_float(config,"damage",&damage);
    sj_free(config);// clean up

    return projectile_new(parent,position, dir, speed,damage);
}

Entity *projectile_new(Entity *parent,Vector3D position, Vector3D dir, float speed,float damage)
{
    Entity *self;

    self = entity_new();
    if (!self)
    {
        slog("failed to make a new projectile!");
        return NULL;
    }

    self->parent = parent;
    vector3d_copy(self->position,position);
    vector3d_normalize(&dir);
    vector3d_scale(self->velocity,dir,speed);
    self->think = projectile_think;
    self->update = projectile_update;
    self->damage = damage;
    self->health = 1000;
    self->model = gf3d_model_load("models/dino.model");

    return self;
}

Entity *short_projectile_new(Entity *parent,Vector3D position, Vector3D dir, float speed,float damage)
{
    Entity *self;

    self = entity_new();
    if (!self)
    {
        slog("failed to make a new projectile!");
        return NULL;
    }

    self->parent = parent;
    vector3d_copy(self->position,position);
    vector3d_normalize(&dir);
    vector3d_scale(self->velocity,dir,speed);
    self->think = projectile_think;
    self->update = projectile_update;
    self->damage = damage;
    self->health = 50;
    self->model = gf3d_model_load("models/dino.model");

    return self;
}

Entity *shield_new(Entity *parent,Vector3D position, Vector3D dir)
{
    Entity *self;

    self = entity_new();
    if (!self)
    {
        slog("failed to make a new projectile!");
        return NULL;
    }

    self->parent = parent;
    vector3d_copy(self->position,position);
    vector3d_normalize(&dir);
    vector3d_scale(self->velocity,dir,0);
    self->think = projectile_think;
    self->update = projectile_update;
    self->health = 10;
    self->health = 10000;
    self->model = gf3d_model_load("models/dino.model");

    return self;
}

void projectile_think(Entity *self)
{
    Entity *other;
    if (!self)return;

    other = entity_get_collision_entity(self);

    if (other)
    {
        if (other->onDamage && other->blocking != 1)
        {
            other->onDamage(other, self->damage, self->parent?self->parent:self);
        }
        if (other->blocking)
        {
            other->blocking = 0;
            entity_free(self);
        }

        entity_free(self);
    }
}

void projectile_update(Entity *self)
{
    if (!self)return;
    if (!self->blocking)return;
    if (self->health < 10) entity_free(self);
    self->health -= 10;
    slog("projectile health: %i",self->health);
    if (self->health <= 0)entity_free(self);
    //run physics, update position, check for collision, whatever else I gotta do
}
