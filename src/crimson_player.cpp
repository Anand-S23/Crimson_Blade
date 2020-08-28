internal void PlayerInit(Entity *player, v2 pos)
{
    player->pos = { pos.x, pos.y };
    player->initialized = true;
}
