#include <stdio.h>

typedef struct
{
    int health;
    int score;
} Player;

/**
 * @brief Add damage to the health of the Player
 * @param Player pointer
 * @int damage
 * return void
 */
void take_damage(Player *p, int damage)
{
    p->health -= damage;
}

int main(void)
{
    Player p1;
    p1.health = 100;
    p1.score = 0;

    take_damage(&p1, 25);
    printf("Current player's health = %d\n", p1.health);

    return 0;
}
