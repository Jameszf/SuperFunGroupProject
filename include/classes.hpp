
#include "funcs.hpp"
#include <memory>
#include <algorithm>

/* 
================ Base Entity Class =================
Constructor(hp, def, atk, spd)

Properties:
	uid (std::string) - unique id
	hp (int) - health points of entity
	def (int) - defense points for mitigating damage
	atk (int) - base attack for damage calculations
	spd (int) - how fast attack bar fills up

Damage Forumla:
	max(Base attack - reciever defense, 0) = damage dealt
*/

class Entity {
	public:
		std::string uid;
		int hp, def, atk, spd;

		Entity(int, int, int, int);
		void attack(std::unique_ptr<Entity>);
};
