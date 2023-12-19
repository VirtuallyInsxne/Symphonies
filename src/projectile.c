#include "projectile.h"
#include "simple_logger.h"

Entity *projectile_new_low(Vector3D position, Vector3D velocity, EntityState state)
{
    Entity *proj = entity_new();
    if (!proj)
    {
        slog("Failed to create projectile!");
        return NULL;
    }

    proj->type = ENT_PROJ_LOW;  // Set the default type to low, adjust as needed
    proj->model = gf3d_model_load("models/kiryu.model");  // Load your projectile model

    // Set other projectile properties
    proj->think = projectile_think;
    proj->update = projectile_update;
    proj->draw = entity_draw;  // Use the same draw function as entities for simplicity

    vector3d_copy(proj->position, position);
    vector3d_copy(proj->velocity, velocity);

    proj->state = state;  // Set the projectile state (low, mid, high)

    return proj;
}

Entity *projectile_new_mid(Vector3D position, Vector3D velocity, EntityState state)
{
    Entity *proj = entity_new();
    if (!proj)
    {
        slog("Failed to create projectile!");
        return NULL;
    }

    proj->type = ENT_PROJ_MID;  // Set the default type to low, adjust as needed
    proj->model = gf3d_model_load("models/kiryu.model");  // Load your projectile model

    // Set other projectile properties
    proj->think = projectile_think;
    proj->update = projectile_update;
    proj->draw = entity_draw;  // Use the same draw function as entities for simplicity

    vector3d_copy(proj->position, position);
    vector3d_copy(proj->velocity, velocity);

    proj->state = state;  // Set the projectile state (low, mid, high)

    return proj;
}

Entity *projectile_new_high(Vector3D position, Vector3D velocity, EntityState state)
{
    Entity *proj = entity_new();
    if (!proj)
    {
        slog("Failed to create projectile!");
        return NULL;
    }

    proj->type = ENT_PROJ_HIGH;  // Set the default type to low, adjust as needed
    proj->model = gf3d_model_load("models/kiryu.model");  // Load your projectile model

    // Set other projectile properties
    proj->think = projectile_think;
    proj->update = projectile_update;
    proj->draw = entity_draw;  // Use the same draw function as entities for simplicity

    vector3d_copy(proj->position, position);
    vector3d_copy(proj->velocity, velocity);

    proj->state = state;  // Set the projectile state (low, mid, high)

    return proj;
}

void projectile_think(Entity *self)
{
    Entity *other = entity_get_collision(self);

    if (entity_check_collision(self, other))
    {
        // Handle collision (e.g., deal damage, play an effect, etc.)
        slog("Projectile collided!");

        // Free the projectile
        entity_free(self);
        return; // Don't execute further logic if the projectile is freed
    }
}

// Update the projectile_update function
void projectile_update(Entity *self)
{
    // Add specific logic for projectile updating if needed
    // For example, move the projectile based on velocity
    vector3d_add(self->position, self->position, self->velocity);

    // Think function handles collision and other logic
    self->think(self);
}

// Example function to spawn a projectile (you can call this when a player triggers an attack)
void spawn_projectile(Vector3D position, Vector3D velocity, EntityState state)
{
    if (state == ES_attack_low) {
        Entity *proj = projectile_new_low(position, velocity, state);
    }
    else if (state == ES_attack_mid) {
        Entity *proj = projectile_new_mid(position, velocity, state);
    }
    else if (state == ES_attack_high) {
        Entity *proj = projectile_new_high(position, velocity, state);
    }
}
