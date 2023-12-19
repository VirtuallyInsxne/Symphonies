#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_types.h"
#include "gfc_config.h"

#include "world.h"
#include "agumon.h"
#include "fighter2.h"


/*typedef struct
{

    Model *worldModel;
    List *spawnList;        //entities to spawn
    List *entityList;       //entities that exist in the world
}World;*/

Entity *spawn_entity_from_config(World *self, SJson *spawn)
{
    const char *name = sj_get_string_value(sj_object_get_value(spawn,"name"));
    //slog(name);
    Entity *ent = NULL;
    Vector3D pos;
    Uint32 health;
    if(strcmp(name,"player1") == 0)
    {
        slog("Spawned player 1");
        sj_value_as_vector3d(sj_object_get_value(spawn,"position"),&pos);
        sj_get_uint32_value(sj_object_get_value(spawn, "health"), &health);
        ent = agumon_new(pos,vector3d(0,0,-GFC_HALF_PI), 1);
        if (ent)ent->selected = 1;
        ent->maxHealth = health;
        ent->currHealth = health;
        gfc_list_append(self->entityList,ent);
        return ent;
    }
    else if (strcmp(name,"player2") == 0)
    {
        slog("Spawned player 2");
        sj_value_as_vector3d(sj_object_get_value(spawn,"position"),&pos);
        sj_get_uint32_value(sj_object_get_value(spawn, "health"), &health);
        ent = fighter2_new(pos, vector3d(0,0,GFC_HALF_PI), 2);
        if (ent)ent->selected = 1;
        ent->maxHealth = health;
        ent->currHealth = health;
        gfc_list_append(self->entityList,ent);
        return ent;
    }

    return NULL;
}

World *world_load(char *filename)
{
    SJson *json,*wjson,*spawns;
    World *w = NULL;
    const char *modelName = NULL;
    w = gfc_allocate_array(sizeof(World),1);
    if (w == NULL)
    {
        slog("failed to allocate data for the world");
        return NULL;
    }
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to load json file (%s) for the world data",filename);
        free(w);
        return NULL;
    }
    wjson = sj_object_get_value(json,"world");
    if (!wjson)
    {
        slog("failed to find world object in %s world condig",filename);
        free(w);
        sj_free(json);
        return NULL;
    }
    modelName = sj_get_string_value(sj_object_get_value(wjson,"model"));
    if (!modelName)
    {
        slog("world data (%s) has no model",filename);
        sj_free(json);
        return w;
    }
    w->model = gf3d_model_load(modelName);

    spawns = sj_object_get_value(json, "spawns");
    slog("Made the spawns object");
    if(!spawns)
    {
        slog("didn't work");
    }
    int count,i;
    w->entityList = gfc_list_new();
    count = sj_array_get_count(spawns);
    for(i = 0; i < count; i++)
    {
        spawn_entity_from_config(w, sj_array_get_nth(spawns,i));
    }

    sj_value_as_vector3d(sj_object_get_value(wjson,"scale"),&w->scale);
    sj_value_as_vector3d(sj_object_get_value(wjson,"position"),&w->position);
    sj_value_as_vector3d(sj_object_get_value(wjson,"rotation"),&w->rotation);

    sj_free(json);
    w->color = gfc_color(1,1,1,1);
    return w;
}

void world_draw(World *world)
{
    if (!world)return;
    if (!world->model)return;// no model to draw, do nothing
    gf3d_model_draw(world->model,world->modelMat,gfc_color_to_vector4f(world->color),vector4d(0.5f, 0.5f, 0.5f,0.25f));
    //gf3d_model_draw_highlight(world->worldModel,world->modelMat,vector4d(1,.5,.1,1));
}

void world_delete(World *world)
{
    if (!world)return;
    gf3d_model_free(world->model);
    free(world);
}

void world_run_updates(World *self)
{
    //self->rotation.z += 0.0001;
    gfc_matrix_identity(self->modelMat);
    self->position = vector3d(0, 0, 0);

    gfc_matrix_scale(self->modelMat,self->scale);
    gfc_matrix_rotate_by_vector(self->modelMat,self->modelMat,self->rotation);
    gfc_matrix_translate(self->modelMat,self->position);

}

void world_add_entity(World *world,Entity *entity);


/*eol@eof*/
