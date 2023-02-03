/******************************************************************************/
/** @file dk2mfile.c
 * Library for r/w of DK2 map files.
 * @par Purpose:
 *     Allows reading/writing DK2 dk2 map files (KWD/KLD).
 * @par Comment:
 *     Thanks to Mark Kinkead and the Evil One for info about file formats.
 * @author   Tomasz Lis
 * @date     15 Jul 2008 - 05 Aug 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#include "things.h"
#include "dk2mfile.h"
#include "../ADiKtEd/libadikted/adikted.h"


struct ActionPointFlag {
	char HERO_LAIR : 1;
	char none_1 : 1;
	char UNK_1 : 1;
	char UNK_2 : 1;
	char REVEAL_THROUGH_FOG_OF_WAR : 1;
	char TOOL_BOX : 1;
	char IGNORE_SOLID : 1;
    char none;
};

struct CreatureBehaviourFlag {
	char WILL_FIGHT : 1;
	char LEADER : 1;
	char FOLLOWER : 1;
	char WILL_BE_ATTACKED : 1;
	char RETURN_TO_HERO_LAIR : 1;
	char FREE_FRIENDS_ON_JAIL_BREAK : 1;
	char ACT_AS_DROPPED : 1;
	char START_AS_DYING : 1;
};

struct CreatureBehaviourFlag2 {
	char DESTROY_ROOMS : 1;
	char I_AM_A_TOOL : 1;
	char DIES_INSTANTLY : 1;
	char I_AM_A_MERCENARY : 1;
};

enum  DoorAppearanceFlag {
	NONE_DOOR,
	LOCKED_DOOR,
	BLUEPRINT_DOOR
};

enum ThingType{
	OBJECT_THING = 194,
	TRAP_THING = 195,
	DOOR_THING = 196,
	ACTIONPOINT_THING = 197,
	NEUTRAL_CREATURE_THING = 198,
	GOOD_CREATURE_THING = 199,
	CREATURE_THING = 200,
	HEROPARTY_THING = 201,
	DEAD_BODY_THING = 202,
	EFFECT_GENERATOR_THING = 203,
	ROOM_THING = 204,
	CAMERA_THING = 205
};

enum Objective {
	NO_OBJECTIVE,
	DESTROY_ROOMS_OBJECTIVE = 11,
	DESTROY_WALLS_OBJECTIVE = 12,
	STEAL_GOLD_OBJECTIVE = 13,
	STEAL_SPELLS_OBJECTIVE = 14,
    // 15
    // 16
	STEAL_MAFUFACTURE_CRATES = 17,
	KILL_CREATURES_OBJECTIVE = 18,
	KILL_PLAYER_OBJECTIVE = 19,
    // 20
    // 21
	WAIT_OBJECTIVE = 22,
	SEND_TO_ACTION_POINT_OBJECTIVE = 23,
    // 24
    // 25
    // 26
	JAIL_BREAK_OBJECTIVE = 27
};

enum RoomType {
	PORTAL = 12,
    // 13
	DUNGEON_HEART = 14,
    // 15
    // 16
    // 17
    // 18
    // 19
    // 20
    // 21
    // 22
    // 23
    // 24
    // 25
    // 26
    // 27
    // 28
    // 29
    // 30
    // 31
    // 32
	HERO_GATE_2X2 = 33,
	HERO_GATE_FRONT_END = 34,
    // 35
    // 36
	HERO_GATE_3X1 = 37,
    // 38
    // 39
	HERO_PORTAL = 40
};

struct CameraFlag {
	char DISABLE_YAW : 1; // angleXY ?
	char DISABLE_ROLL : 1; // angleYZ ?
	char DISABLE_PITCH : 1; // angleXZ ?
	char DISABLE_ZOOM : 1;
	char UNK_1 : 1; // ViewDistance, Lens
	char UNK_2 : 1; // ViewDistance, Lens
	char DISABLE_MOVE : 1; // Position
	char DISABLE_CHANGE : 1;
	char none[3];
};

struct GoodCreature {
	int posX;
	int posY;
	int posZ;
	unsigned short goldHeld;
	unsigned char level;
	struct CreatureBehaviourFlag creatureFlag;
	int objectiveTargetActionPointId;
	int initialHealth;
	unsigned short triggerId;
	unsigned char objectiveTargetPlayerId;
	enum Objective objective;
	unsigned char creatureId;
	unsigned char unk_1[2];
	struct CreatureBehaviourFlag2 creatureFlag2;
};

struct ThingData {
	enum ThingType type;
	unsigned int dataSize;
	union {
		struct {
			int posX;
			int posY;
			unsigned char unk_1[4];
			int keeperSpellId;
			int moneyAmount;
			unsigned short triggerId;
			unsigned char objectId;
			unsigned char playerId;
		} objectThing;
        
		struct {
			int posX;
			int posY;
			int unk_1;
			unsigned char numberOfShots;
			unsigned char trapId;
			unsigned char playerId;
			unsigned char unk_2;
		} trapThing;
		struct {
			int posX;
			int posY;
			int unk_1;
			unsigned short triggerId;
			unsigned char doorId;
			unsigned char playerId;
			enum DoorAppearanceFlag doorFlag;
			unsigned char unk_2[3];
		} doorThing;
		struct {
			int startX;
			int startY;
			int endX;
			int endY;
			unsigned short waitDelay;
			struct ActionPointFlag flags;
			unsigned short triggerId;
			unsigned char id;
			unsigned char nextWaypointId;
			char name[32];
		} actionPointThing;
		struct {
			int posX;
			int posY;
			int posZ;
			unsigned short goldHeld;
			unsigned char level;
			unsigned char creatureFlag;
			int initialHealth;
			unsigned short triggerId;
			unsigned char creatureId;
			unsigned char unk_1;
		} neutralCreatureThing;
		struct {
			int posX;
			int posY;
			int posZ;
			unsigned short goldHeld;
			unsigned char level;
			unsigned char creatureFlag;
			int initialHealth;
			int objectiveTargetPlayerId;
			unsigned short triggerId;
			unsigned char creatureId;
			unsigned char playerId;
		} keeperCreature;
		struct {
			char name[32];
			unsigned short triggerId;
			unsigned char id;
			int unk_1; // x23
			int unk_2; // x27
			struct GoodCreature partyMembers[16];
		} heroParty;
		struct {
			int posX;
			int posY;
			int posZ;
			unsigned short goldHeld;
			unsigned char creatureId;
			unsigned char playerId;
		} deadCreature;
		struct {
			int posX;
			int posY;
			int unk_1; // x08
			int unk_2; // x0c
			unsigned short unk_3; // x10
			unsigned short unk_4; // x12
			unsigned short effectIds[4];
			unsigned char frequency;
			unsigned char id;
			unsigned char pad[6];
		} effectGenerator;
		struct {
			int posX;
			int posY;
			int unk_1; // x08
			short unk_2; // x0c
		//	struct Direction dir;
			unsigned char unk_3; // x0f
			unsigned short initialHealth;
			enum RoomType type;
			unsigned char playerId;
		} room;
		struct {
		//	struct Vector3f position;
		//	struct Vector3f positionMinClipExtent;
		//	struct Vector3f positionMaxClipExtent;
			float viewDistanceValue;
			float viewDistanceMin;
			float viewDistanceMax;
			float zoomValue;
			float zoomValueMin;
			float zoomValueMax;
			float lensValue;
			float lensValueMin;
			float lensValueMax;
			struct CameraFlag flag;
			unsigned short angleYaw; // angleXY ?
			unsigned short angleRoll; // angleYZ ?
			unsigned short anglePitch; // angleXZ ?
			unsigned short id;
		} camera;
	};
};




/**********/


struct ThingData thingdatas[2048];


/**********/
int dk2_to_dk1creaturemodel(int model)
{
    switch (model)
    {
    case 1: //Imp
        return CREATR_SUBTP_IMP;
    case 7: //Goblin
        return CREATR_SUBTP_ORC;
    case 5: //Warlock
        return CREATR_SUBTP_WARLOCK;
    case 12: //FireFly
        return CREATR_SUBTP_FLY;
    case 10: //Troll
        return CREATR_SUBTP_TROLL;
    case 6: //DarkElf
        return CREATR_SUBTP_SPAWN;
    case 9: //Skeleton
        return CREATR_SUBTP_SKELETON;
    case 4: //Dark Mistress
        return CREATR_SUBTP_MISTRESS;
    case 11: //Salamander
        return CREATR_SUBTP_DRAGON;
    case 24: //Rogue
        return CREATR_SUBTP_BEETLE;
    case 3: //Bile demon
        return CREATR_SUBTP_BILEDEMN;
    case 8: //Vampire
        return CREATR_SUBTP_VAMPIRE;
    case 22: //Dark Knight
        return CREATR_SUBTP_GHOST;
    case 23: //Dark Angel
        return CREATR_SUBTP_HOUND;
    case 27: //Maiden
        return CREATR_SUBTP_SPIDER;
    case 45: //Horny
        return CREATR_SUBTP_HORNY;

    case 14: //Dwarf
        return CREATR_SUBTP_TUNELER;
    case 13: //Knight
        return CREATR_SUBTP_KNIGHT;
    case 18: //Thief
        return CREATR_SUBTP_THEFT;
    case 25: //Guard
        return CREATR_SUBTP_BARBARIN;
    case 16: //Wizard
        return CREATR_SUBTP_WIZRD;
    case 15: //Giant
        return CREATR_SUBTP_GIANT;
    case 17: //Elven Archer
        return CREATR_SUBTP_ARCHER;
    case 19: //Monk
        return CREATR_SUBTP_MONK;
    case 20: //Fairy
        return CREATR_SUBTP_FAIRY;
    case 30: //Royal Guard
        return CREATR_SUBTP_TENTCL;
    case 28: //Stone Knight
        return CREATR_SUBTP_WITCH;
    case 29: //Lord Of The Land
        return CREATR_SUBTP_DWAFT;
    case 21: //King Reginald
        return CREATR_SUBTP_AVATAR;
    case 2: //Prince Felix
    case 26: //Prince Balder
    case 31: //Prince Tristan
        return CREATR_SUBTP_SMURI;
    default:
        printf("unexpected creature %d", model);
        return 0;
    }
}

char* creaturemodel_to_string(int model)
{
    switch (model)
    {
    case 1: //Imp
        return "imp";
    case 7: //Goblin
        return "Goblin";
    case 5: //Warlock
        return "Warlock";
    case 12: //FireFly
        return "FireFly";
    case 10: //Troll
        return "Troll";
    case 6: //DarkElf
        return "DarkElf";
    case 9: //Skeleton
        return "Skeleton";
    case 4: //Dark Mistress
        return "Dark Mistress";
    case 11: //Salamander
        return "Salamander";
    case 24: //Rogue
        return "Rogue";
    case 3: //Bile demon
        return "Bile demon";
    case 8: //Vampire
        return "Vampire";
    case 22: //Dark Knight
        return "Dark Knight";
    case 23: //Dark Angel
        return "Dark Angel";
    case 27: //Maiden
        return "Maiden";
    case 45: //Horny
        return "Horny";

    case 14: //Dwarf
        return "Dwarf";
    case 13: //Knight
        return "Knight";
    case 18: //Thief
        return "Thief";
    case 25: //Guard
        return "Guard";
    case 16: //Wizard
        return "Wizard";
    case 15: //Giant
        return "Giant";
    case 17: //Elven Archer
        return "Elven Archer";
    case 19: //Monk
        return "Monk";
    case 20: //Fairy
        return "Fairy";
    case 30: //Royal Guard
        return "Royal Guard";
    case 28: //Stone Knight
        return "Stone Knight";
    case 29: //Lord Of The Land
        return "Lord Of The Land";
    case 21: //King Reginald
        return "King Reginald";
    case 2: //Prince Felix
        return "Prince Felix";
    case 26: //Prince Balder
        return "Prince Balder";
    case 31: //Prince Tristan
        return "Prince Tristan";
    default:
        return "Unknown creature";
    }
}

void print_thingtype(int type)
{
    switch (type)
    {
    case OBJECT_THING:
        printf("OBJECT_THING\n");
        break;
        case TRAP_THING:
        printf("TRAP_THING\n");
        break;
    case DOOR_THING:
        printf("DOOR_THING\n");
        break;
    case ACTIONPOINT_THING:
        printf("ACTIONPOINT_THING\n");
        break;
    case NEUTRAL_CREATURE_THING:
        printf("NEUTRAL_CREATURE_THING\n");
        break;
    case CREATURE_THING:
        printf("CREATURE_THING\n");
        break;
    case HEROPARTY_THING:
        printf("HEROPARTY_THING\n");
        break;
    case DEAD_BODY_THING:
        printf("DEAD_BODY_THING\n");
        break;
    case EFFECT_GENERATOR_THING:
        printf("EFFECT_GENERATOR_THING\n");
        break;
    case ROOM_THING:
        printf("ROOM_THING\n");
        break;
    case CAMERA_THING:
        printf("CAMERA_THING\n");
        break;
    default:
        break;
    }
}



int count;

short dk2m_read_things_header(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
    count = read_int32_le_buf(chunk->data);
    return ERR_NONE;
}

short dk2m_read_things(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  unsigned long offs=0;
      
  for (int i=0;i<count;i++)
  {
      thingdatas[i].type     = read_int32_le_buf(chunk->data+offs+0);
      thingdatas[i].dataSize = read_int32_le_buf(chunk->data+offs+4);

    switch (thingdatas[i].type)
    {
    case OBJECT_THING:
    case TRAP_THING:
    case DOOR_THING:
    case ACTIONPOINT_THING:
        break;
    case NEUTRAL_CREATURE_THING:
    case GOOD_CREATURE_THING:
    case CREATURE_THING:
        thingdatas[i].keeperCreature.posX = read_int32_le_buf(chunk->data+offs+8);
        thingdatas[i].keeperCreature.posY = read_int32_le_buf(chunk->data+offs+12);
        thingdatas[i].keeperCreature.posZ = read_int32_le_buf(chunk->data+offs+16);
        thingdatas[i].keeperCreature.goldHeld = read_int16_le_buf(chunk->data+offs+20);
        thingdatas[i].keeperCreature.level = read_int8_buf(chunk->data+offs+22);
        thingdatas[i].keeperCreature.creatureFlag = read_int8_buf(chunk->data+offs+23);
        thingdatas[i].keeperCreature.initialHealth = read_int32_le_buf(chunk->data+offs+24);
        thingdatas[i].keeperCreature.objectiveTargetPlayerId = read_int32_le_buf(chunk->data+offs+28);
        thingdatas[i].keeperCreature.triggerId = read_int16_le_buf(chunk->data+offs+32);
        thingdatas[i].keeperCreature.creatureId = read_int8_buf(chunk->data+offs+34);
        thingdatas[i].keeperCreature.playerId = read_int8_buf(chunk->data+offs+35);
        break;
    case HEROPARTY_THING:
    case DEAD_BODY_THING:
    case EFFECT_GENERATOR_THING:
    case ROOM_THING:
    case CAMERA_THING:
        break;
    
    default:
        printf("unknown ThingType %d", thingdatas[i].type);
        break;
    }
      if (thingdatas[i].type == CREATURE_THING)
      {
        


        //printf("    %d %d %d Id:%d plr:%d \n",crt.posX,crt.posY,crt.posZ,crt.creatureId,crt.playerId);

      }

      offs+=8;
      offs+=thingdatas[i].dataSize;
      
  }

  //if (chunk->dsize != (total_tiles<<2))
  //    return -1;
  return ERR_NONE;


}

short dk2m_read_thingschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
  case CTTNG_FILEHEAD:
      return dk2m_read_things_header(lvl,chunk,flags);
  case CTTNG_LEVTHINGS:
      return dk2m_read_things(lvl,chunk,flags);
  case CTTNG_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown THINGS chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_print_things(const struct DK2_Level *lvlDk2,struct LEVEL *lvl)
{
    for (size_t i = 0; i < count; i++)
    {
        unsigned char *thing;
        switch (thingdatas[i].type)
        {
        case OBJECT_THING:
        case TRAP_THING:
        case DOOR_THING:
        case ACTIONPOINT_THING:
            break;
        case NEUTRAL_CREATURE_THING:
        case GOOD_CREATURE_THING:
        case CREATURE_THING:
            thing = create_thing(thingdatas[i].keeperCreature.posX * 3 + 1,thingdatas[i].keeperCreature.posY * 3 + 1);
            set_thing_type(thing,THING_TYPE_CREATURE);
            set_thing_subtype(thing,dk2_to_dk1creaturemodel(thingdatas[i].keeperCreature.creatureId));
            set_thing_owner(thing,dk2_owner2DK1owner(thingdatas[i].keeperCreature.playerId));
            set_thing_level(thing,thingdatas[i].keeperCreature.level);
            thing_add(lvl,thing);
            printf(" lvl:%d %s owner:%d\n",
                   thingdatas[i].keeperCreature.level,
                   creaturemodel_to_string(thingdatas[i].keeperCreature.creatureId),
                   thingdatas[i].keeperCreature.playerId);

            break;
        case HEROPARTY_THING:
        case DEAD_BODY_THING:
        case EFFECT_GENERATOR_THING:
        case ROOM_THING:
        case CAMERA_THING:
            break;
        
        default:
            printf("unknown ThingType %d\n", thingdatas[i].type);
            break;
        }
    }
    
 


   
}

