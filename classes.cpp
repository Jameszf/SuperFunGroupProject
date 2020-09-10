
#include "include/classes.hpp"


Entity::Entity(int hp, int def, int atk, int spd) {
	uid = uuid::generate_uuid_v4();
	hp = hp;
	def = def;
	atk = atk;
	spd = spd;
}

void Entity::attack(std::unique_ptr<Entity> enemy) {
	enemy->hp -= std::max(atk - enemy->def, 0);
}
