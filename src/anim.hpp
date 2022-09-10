/* the an namespace contains all technology related to
 * animations, be it in-grid character animation or full-grid
 * animation
 */
#pragma once
#include "util.hpp"
#include <unordered_map>
#include <bitset>

 
namespace an
{
	/* animation technology concerning in-grid animation */
	namespace g
	{
		/* data structure for the base representation of an object
		 * and all possible animations
		 */
		struct Anim
		{
			wchar_t ch;
			Vec3 col = Vec3(255, 255, 255);
			Vec3 bg = Vec3(0, 0, 0);
		};
		/* Types that link specific integers to specific animations
		 * defined in anim_types
		 */
		enum AId { BURNING, FROZEN, ASLEEP };
		/* Define no animations with an ID greater than MAX_ANIMS */
		const int MAX_ANIMS = AId::ASLEEP+1;
		/* data structure the predefines all possible
		 * animations a character can have
		 * - If set to 0x0, the character displayed is to be the same
		 *   as the main character
		 */
		const std::unordered_map<int, Anim> anim_types =
		{
			{BURNING, {L'f', Vec3(235, 134, 25), Vec3(255,0,0)}},
			{FROZEN, {0x0, Vec3(30, 78, 97), Vec3(255,255,255)}},
			{ASLEEP, {L'Z', Vec3(128, 128, 128), Vec3(0,0,0)}}
		};
	}
}
