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
/*
typedef struct ThingData {
	enum ThingType type;
	unsigned int dataSize;
	switch (type) {
		case OBJECT_THING:
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
			break;
		case TRAP_THING:
			struct {
				int posX;
				int posY;
				int unk_1;
				unsigned char numberOfShots;
				unsigned char trapId;
				unsigned char playerId;
				unsigned char unk_2;
			} trapThing;
			break;
		case DOOR_THING:
			struct {
				int posX;
				int posY;
				int unk_1;
				unsigned short triggerId;
				unsigned char doorId;
				unsigned char playerId;
				DoorAppearanceFlag doorFlag;
				unsigned char unk_2[3];
			} trapThing;
			break;
		case ACTIONPOINT_THING:
			struct {
				int startX;
				int startY;
				int endX;
				int endY;
				unsigned short waitDelay;
				ActionPointFlag flags;
				unsigned short triggerId;
				unsigned char id;
				unsigned char nextWaypointId;
				char name[32];
			} actionPointThing;
			break;
		case NEUTRAL_CREATURE_THING:
			struct {
				int posX;
				int posY;
				int posZ;
				unsigned short goldHeld;
				unsigned char level;
				CreatureBehaviourFlag creatureFlag;
				int initialHealth;
				unsigned short triggerId;
				unsigned char creatureId;
				unsigned char unk_1;
			} neutralCreatureThing;
			break;
		case GOOD_CREATURE_THING:
			GoodCreature goodCreature;
			break;
		case CREATURE_THING:
			struct {
				int posX;
				int posY;
				int posZ;
				unsigned short goldHeld;
				unsigned char level;
				CreatureBehaviourFlag creatureFlag;
				int initialHealth;
				int objectiveTargetPlayerId;
				unsigned short triggerId;
				unsigned char creatureId;
				unsigned char playerId;
			} keeperCreature;
			break;
		case HEROPARTY_THING:
			struct {
				char name[32];
				unsigned short triggerId;
				unsigned char id;
				int unk_1; // x23
				int unk_2; // x27
				GoodCreature partyMembers[16];
			} heroParty;
			break;
		case DEAD_BODY_THING:
			struct {
				int posX;
				int posY;
				int posZ;
				unsigned short goldHeld;
				unsigned char creatureId;
				unsigned char playerId;
			} deadCreature;
			break;
		case EFFECT_GENERATOR_THING:
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
			break;
		case ROOM_THING:
			struct {
				int posX;
				int posY;
				int unk_1; // x08
				short unk_2; // x0c
				Direction dir;
				unsigned char unk_3; // x0f
				unsigned short initialHealth;
				RoomType type;
				unsigned char playerId;
			} room;
			break;
		case CAMERA_THING:
			struct {
				Vector3f position;
				Vector3f positionMinClipExtent;
				Vector3f positionMaxClipExtent;
				float32 viewDistanceValue;
				float32 viewDistanceMin;
				float32 viewDistanceMax;
				float32 zoomValue;
				float32 zoomValueMin;
				float32 zoomValueMax;
				float32 lensValue;
				float32 lensValueMin;
				float32 lensValueMax;
				CameraFlag flag;
				unsigned short angleYaw; // angleXY ?
				unsigned short angleRoll; // angleYZ ?
				unsigned short anglePitch; // angleXZ ?
				unsigned short id;
			} camera;
			break;
		default:
            showError("ThingType", type, type);
			unsigned char unknownData[dataSize]<optimize=false>;
	}
	
};
*/

struct keeperCreature {
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
} ;

int dk2creaturemodel_to_dk1creaturemodel(int model)
{
    switch (model)
    {
    case 1: //Imp
        return CREATR_SUBTP_IMP;
    case 2: //Goblin
        return CREATR_SUBTP_ORC;
    case 3: //Warlock
        return CREATR_SUBTP_WARLOCK;
    case 4: //FireFly
        return CREATR_SUBTP_FLY;
    case 5: //Troll
        return CREATR_SUBTP_TROLL;
    case 6: //DarkElf
        return CREATR_SUBTP_SPAWN;
    case 7: //Skeleton
        return CREATR_SUBTP_SKELETON;
    case 8: //Dark Mistress
        return CREATR_SUBTP_MISTRESS;
    case 9: //Salamander
        return CREATR_SUBTP_DRAGON;
    case 10: //Rogue
        return CREATR_SUBTP_BEETLE;
    case 11: //Bile demon
        return CREATR_SUBTP_BILEDEMN;
    case 12: //Vampire
        return CREATR_SUBTP_VAMPIRE;
    case 13: //Dark Knight
        return CREATR_SUBTP_GHOST;
    case 14: //Dark Angel
        return CREATR_SUBTP_HOUND;
    case 15: //Maiden
        return CREATR_SUBTP_SPIDER;
    case 16: //Horny
        return CREATR_SUBTP_HORNY;

    case 17: //Dwarf
        return CREATR_SUBTP_TUNELER;
    case 18: //Knight
        return CREATR_SUBTP_KNIGHT;
    case 19: //Thief
        return CREATR_SUBTP_THEFT;
    case 20: //Guard
        return CREATR_SUBTP_BARBARIN;
    case 21: //Wizard
        return CREATR_SUBTP_WIZRD;
    case 22: //Giant
        return CREATR_SUBTP_GIANT;
    case 23: //Elven Archer
        return CREATR_SUBTP_ARCHER;
    case 24: //Monk
        return CREATR_SUBTP_MONK;
    case 25: //Fairy
        return CREATR_SUBTP_FAIRY;
    case 26: //King Reginald
        return CREATR_SUBTP_AVATAR;
    case 27: //Prince Felix
        return CREATR_SUBTP_DWAFT;
    case 28: //Prince Balder
        return CREATR_SUBTP_WITCH;
    case 29: //Prince Tristan
        return CREATR_SUBTP_SMURI;
    default:
        break;
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
  
    unsigned int type;
    unsigned int dataSize;
    
  for (int i=0;i<count;i++)
  {
      type     = read_int32_le_buf(chunk->data+offs+0);
      dataSize = read_int32_le_buf(chunk->data+offs+4);

      print_thingtype(type);

      if (type == CREATURE_THING)
      {
        struct keeperCreature crt;
        crt.posX = read_int32_le_buf(chunk->data+offs+8);
        crt.posY = read_int32_le_buf(chunk->data+offs+12);
        crt.posZ = read_int32_le_buf(chunk->data+offs+16);
        crt.goldHeld = read_int16_le_buf(chunk->data+offs+20);
        crt.creatureFlag = read_int8_buf(chunk->data+offs+22);
        crt.initialHealth = read_int16_le_buf(chunk->data+offs+23);
        crt.objectiveTargetPlayerId = read_int32_le_buf(chunk->data+offs+27);
        crt.triggerId = read_int16_le_buf(chunk->data+offs+31);
        crt.creatureId = read_int8_buf(chunk->data+offs+33);
        crt.playerId = read_int8_buf(chunk->data+offs+34);

        printf("    %d %d %d Id:%d plr:%d \n",crt.posX,crt.posY,crt.posY,crt.creatureId,crt.playerId);


      }

      offs+=8;
      offs+=dataSize;
      
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

