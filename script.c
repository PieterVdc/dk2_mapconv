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


enum ActionType {
    CREATE_CREATURE = 1,
    CREATE_PORTAL_GEM = 2, // text like [ X, Y ]
    SEND_TO_AP = 3, // or only sent
    DISPLAY_OBJECTIVE = 4, // text like X [ Zoom To AP Y ]
    INFORMATION = 5, // text like X [ Zoom To AP Y ]
    MAKE = 6,
    FLAG = 7,
    INITIALIZE_TIMER = 8,
    FLASH_BUTTON = 9,
    WIN_GAME = 10,
    LOSE_GAME = 11,
    WIN_SUBOBJECTIVE = 12,
    LOSE_SUBOBJECTIVE = 13,
    CREATE_HERO_PARTY = 14,
    SET_OBJECTIVE = 15,
    FLASH_ACTION_POINT = 16,
    REVEAL_ACTION_POINT = 17,
    SET_ALLIANCE = 18, // CREATE_ALLIANCE
    // 19
    ATTACH_PORTAL_GEM = 20,
    FORCE_FIRST_PERSON = 21,
    ALTER_TERRAIN_TYPE = 22,
    SET_TIME_LIMIT = 23, // text like : X Seconds
    PLAY_SPEECH = 24,
    DISPLAY_TEXT_STRING = 25,
    ZOOM_TO_ACTION_POINT = 26,
    ROTATE_AROUND_ACTION_POINT = 27,
    GENERATE_CREATURE = 28,
    MAKE_HUNGRY = 29,
    SHOW_HEALTH_FLOWER = 30,
    FOLLOW_CAMERA_PATH = 31,
    COLLAPSE_HERO_GATE = 32,
    ALTER_SPEED = 33,
    SET_MUSIC_LEVEL = 34, // text like X
    REMOVE_FROM_MAP = 35,
    SET_FIGHT_FLAG = 36,
    SET_PORTAL_STATUS = 37,
    SET_WIDESCREEN_MODE = 38,
    // 39
    // 40
    // 41
    MAKE_OBJECTIVE = 42,
    ZOOM_TO = 43,
    SET_CREATURE_MOODS = 44,
    SET_SYSTEM_MESSAGES = 45,
    DISPLAY_SLAB_OWNER = 46, // text like [ On | Off ] Of [ X, 1635980 ]
    DISPLAY_NEXT_ROOM_TYPE = 47,
    TOGGLE_EFFECT_GENERATOR = 48, // text like X [ On | Off ]
    CHANGE_ROOM_OWNER = 49,
    SET_SLAPS_LIMIT = 50,
    SET_TIMER_SPEECH = 51
};

enum TargetType { //<unsigned char>
    NONE_ = 0,
    FLAG_ = 1,
    TIMER = 2,
    CREATURE_CREATED = 3, // Event, This creature is
    CREATURE_KILLED = 4, // Event, This creature is
    CREATURE_SLAPPED = 5, // Event, This creature is
    CREATURE_ATTACKED = 6, // Event, This creature is
    CREATURE_IMPRISONED = 7, // Event, This creature is
    CREATURE_TORTURED = 8, // Event, This creature is
    CREATURE_CONVERTED = 9, // Event, This creature is
    CREATURE_CLAIMED = 10, // Event, This creature is
    CREATURE_ANGRY = 11, // Event, This creature is
    CREATURE_AFRAID = 12, // Event, This creature is
    CREATURE_STEALS = 13, // Event, This creature is
    CREATURE_LEAVES = 14, // Event, This creature is
    CREATURE_STUNNED = 15, // Event, This creature is
    CREATURE_DYING = 16, // Event, This creature is
    DOOR_DESTROYED = 17, // Event, This door is
    OBJECT_CLAIMED = 18, // Event, This object is
    PLAYER_CREATURES = 19, // Player
    PLAYER_HAPPY_CREATURES = 20, // Player
    PLAYER_ANGRY_CREATURES = 21, // Player
    PLAYER_CREATURES_KILLED = 22, // Player
    PLAYER_KILLS_CREATURES = 23, // Player
    PLAYER_ROOM_SLABS = 24, // Player
    PLAYER_ROOMS = 25, // Player
    PLAYER_ROOM_SIZE = 26, // Player
    PLAYER_DOORS = 27, // Player
    PLAYER_TRAPS = 28, // Player
    PLAYER_KEEPER_SPELL = 29, // Player
    PLAYER_GOLD = 30, // Player
    PLAYER_GOLD_MINED = 31, // Player
    PLAYER_MANA = 32, // Player
    PLAYER_DESTROYS = 33, // Player
    LEVEL_TIME = 34, // Attribute, This level's...
    LEVEL_CREATURES = 35, // Attribute, This level's...
    CREATURE_HEALTH = 36, // Attribute, This creature's...
    CREATURE_GOLD_HELD = 37, // Attribute, This creature's...
    // 38
    AP_CONGREGATE_IN = 39, // Action point
    AP_CLAIM_PART_OF = 40, // Action point
    AP_CLAIM_ALL_OF = 41, // Action point
    AP_SLAB_TYPES = 42, // Action point
    PARTY_CREATED = 43, // Party
    PARTY_MEMBERS_KILLED = 44, // Party
    PARTY_MEMBERS_CAPTURED = 45, // Party
    LEVEL_PAY_DAY = 46, // Event, This level's...
    PLAYER_KILLED = 47, // Event, This player's...
    // 48
    CREATURE_EXPERIENCE_LEVEL = 49, // Attribute, This creature's...
    // 50
    // 51
    PLAYER_CREATURES_AT_LEVEL = 52, // Player
    GUI_BUTTON_PRESSED = 53, // GUI
    CREATURE_HUNGER_SATED = 54, // Event, This creature is
    CREATURE_PICKS_UP_PORTAL_GEM = 55, // Event, This creature is
    PLAYER_DUNGEON_BREACHED = 56, // Event, This player's...
    PLAYER_ENEMY_BREACHED = 57, // Event, This player's...
    PLAYER_CREATURE_PICKED_UP = 58, // Event, This player's...
    PLAYER_CREATURE_DROPPED = 59, // Event, This player's...
    PLAYER_CREATURE_SLAPPED = 60, // Event, This player's...
    PLAYER_CREATURE_SACKED = 61, // Event, This player's...
    AP_TAG_PART_OF = 62, // Action point
    CREATURE_SACKED = 63, // Event, This creature is
    PARTY_MEMBERS_INCAPACITATED = 64, // Party
    CREATURE_PICKED_UP = 65, // Event, This creature is
    // 66
    LEVEL_PLAYED = 67, // Attribute, This level's...
    PLAYER_ROOM_FURNITURE = 68, // Player
    AP_TAG_ALL_OF = 69, // Action point
    AP_POSESSED_CREATURE_ENTERS = 70, // Action point
    PLAYER_SLAPS = 71, // Player
    GUI_TRANSITION_ENDS = 72, // GUI
    PLAYER_CREATURES_GROUPED = 73, // Player
    PLAYER_CREATURES_DYING = 74
} ;

enum  TriggerType{ //<unsigned int>
	TriggerGeneric = 213,
	TriggerAction = 214
} ;

struct TriggerData {
    enum TriggerType tag; 
    unsigned int size;
    unsigned char tdata[8]; // very complex and tag depended
    unsigned short id;
    unsigned short idNext;
    unsigned short idChild;
    unsigned char type;
    unsigned char repeatTimes;
};


struct TriggerData triggerdatas[4096];

char* type_to_string(int tag,int type)
{
    if (tag == TriggerAction)
    {
             if(type == CREATE_CREATURE             ) return "CREATE_CREATURE        ";
        else if(type == CREATE_PORTAL_GEM           ) return "CREATE_PORTAL_GEM      ";
        else if(type == SEND_TO_AP                  ) return "SEND_TO_AP             ";
        else if(type == DISPLAY_OBJECTIVE           ) return "DISPLAY_OBJECTIVE      ";
        else if(type == INFORMATION                 ) return "INFORMATION            ";
        else if(type == MAKE                        ) return "MAKE                   ";
        else if(type == FLAG                        ) return "FLAG                   ";
        else if(type == INITIALIZE_TIMER            ) return "INITIALIZE_TIMER       ";
        else if(type == FLASH_BUTTON                ) return "FLASH_BUTTON           ";
        else if(type == WIN_GAME                    ) return "WIN_GAME               ";
        else if(type == LOSE_GAME                   ) return "LOSE_GAME              ";
        else if(type == WIN_SUBOBJECTIVE            ) return "WIN_SUBOBJECTIVE       ";
        else if(type == LOSE_SUBOBJECTIVE           ) return "LOSE_SUBOBJECTIVE      ";
        else if(type == CREATE_HERO_PARTY           ) return "CREATE_HERO_PARTY      ";
        else if(type == SET_OBJECTIVE               ) return "SET_OBJECTIVE          ";
        else if(type == FLASH_ACTION_POINT          ) return "FLASH_ACTION_POINT     ";
        else if(type == REVEAL_ACTION_POINT         ) return "REVEAL_ACTION_POINT    ";
        else if(type == SET_ALLIANCE                ) return "SET_ALLIANCE           ";
        else if(type == ATTACH_PORTAL_GEM           ) return "ATTACH_PORTAL_GEM      ";
        else if(type == FORCE_FIRST_PERSON          ) return "FORCE_FIRST_PERSON     ";
        else if(type == ALTER_TERRAIN_TYPE          ) return "ALTER_TERRAIN_TYPE     ";
        else if(type == SET_TIME_LIMIT              ) return "SET_TIME_LIMIT         ";
        else if(type == PLAY_SPEECH                 ) return "PLAY_SPEECH            ";
        else if(type == DISPLAY_TEXT_STRING         ) return "DISPLAY_TEXT_STRING    ";
        else if(type == ZOOM_TO_ACTION_POINT        ) return "ZOOM_TO_ACTION_POINT   ";
        else if(type == ROTATE_AROUND_ACTION_POINT  ) return "ROTATE_AROUND_ACTION_PO";
        else if(type == GENERATE_CREATURE           ) return "GENERATE_CREATURE      ";
        else if(type == MAKE_HUNGRY                 ) return "MAKE_HUNGRY            ";
        else if(type == SHOW_HEALTH_FLOWER          ) return "SHOW_HEALTH_FLOWER     ";
        else if(type == FOLLOW_CAMERA_PATH          ) return "FOLLOW_CAMERA_PATH     ";
        else if(type == COLLAPSE_HERO_GATE          ) return "COLLAPSE_HERO_GATE     ";
        else if(type == ALTER_SPEED                 ) return "ALTER_SPEED            ";
        else if(type == SET_MUSIC_LEVEL             ) return "SET_MUSIC_LEVEL        ";
        else if(type == REMOVE_FROM_MAP             ) return "REMOVE_FROM_MAP        ";
        else if(type == SET_FIGHT_FLAG              ) return "SET_FIGHT_FLAG         ";
        else if(type == SET_PORTAL_STATUS           ) return "SET_PORTAL_STATUS      ";
        else if(type == SET_WIDESCREEN_MODE         ) return "SET_WIDESCREEN_MODE    ";
        else if(type == MAKE_OBJECTIVE              ) return "MAKE_OBJECTIVE         ";
        else if(type == ZOOM_TO                     ) return "ZOOM_TO                ";
        else if(type == SET_CREATURE_MOODS          ) return "SET_CREATURE_MOODS     ";
        else if(type == SET_SYSTEM_MESSAGES         ) return "SET_SYSTEM_MESSAGES    ";
        else if(type == DISPLAY_SLAB_OWNER          ) return "DISPLAY_SLAB_OWNER     ";
        else if(type == DISPLAY_NEXT_ROOM_TYPE      ) return "DISPLAY_NEXT_ROOM_TYPE ";
        else if(type == TOGGLE_EFFECT_GENERATOR     ) return "TOGGLE_EFFECT_GENERATOR";
        else if(type == CHANGE_ROOM_OWNER           ) return "CHANGE_ROOM_OWNER      ";
        else if(type == SET_SLAPS_LIMIT             ) return "SET_SLAPS_LIMIT        ";
        else if(type == SET_TIMER_SPEECH            ) return "SET_TIMER_SPEECH       ";
        return "UNKNOWN ACTION              ";
    }
    else if (tag == TriggerGeneric)
    {
             if(type == NONE_                       ) return "NONE_                       ";
        else if(type == FLAG_                       ) return "FLAG_                       ";
        else if(type == TIMER                       ) return "TIMER                       ";
        else if(type == CREATURE_CREATED            ) return "CREATURE_CREATED            ";
        else if(type == CREATURE_KILLED             ) return "CREATURE_KILLED             ";
        else if(type == CREATURE_SLAPPED            ) return "CREATURE_SLAPPED            ";
        else if(type == CREATURE_ATTACKED           ) return "CREATURE_ATTACKED           ";
        else if(type == CREATURE_IMPRISONED         ) return "CREATURE_IMPRISONED         ";
        else if(type == CREATURE_TORTURED           ) return "CREATURE_TORTURED           ";
        else if(type == CREATURE_CONVERTED          ) return "CREATURE_CONVERTED          ";
        else if(type == CREATURE_CLAIMED            ) return "CREATURE_CLAIMED            ";
        else if(type == CREATURE_ANGRY              ) return "CREATURE_ANGRY              ";
        else if(type == CREATURE_AFRAID             ) return "CREATURE_AFRAID             ";
        else if(type == CREATURE_STEALS             ) return "CREATURE_STEALS             ";
        else if(type == CREATURE_LEAVES             ) return "CREATURE_LEAVES             ";
        else if(type == CREATURE_STUNNED            ) return "CREATURE_STUNNED            ";
        else if(type == CREATURE_DYING              ) return "CREATURE_DYING              ";
        else if(type == DOOR_DESTROYED              ) return "DOOR_DESTROYED              ";
        else if(type == OBJECT_CLAIMED              ) return "OBJECT_CLAIMED              ";
        else if(type == PLAYER_CREATURES            ) return "PLAYER_CREATURES            ";
        else if(type == PLAYER_HAPPY_CREATURES      ) return "PLAYER_HAPPY_CREATURES      ";
        else if(type == PLAYER_ANGRY_CREATURES      ) return "PLAYER_ANGRY_CREATURES      ";
        else if(type == PLAYER_CREATURES_KILLED     ) return "PLAYER_CREATURES_KILLED     ";
        else if(type == PLAYER_KILLS_CREATURES      ) return "PLAYER_KILLS_CREATURES      ";
        else if(type == PLAYER_ROOM_SLABS           ) return "PLAYER_ROOM_SLABS           ";
        else if(type == PLAYER_ROOMS                ) return "PLAYER_ROOMS                ";
        else if(type == PLAYER_ROOM_SIZE            ) return "PLAYER_ROOM_SIZE            ";
        else if(type == PLAYER_DOORS                ) return "PLAYER_DOORS                ";
        else if(type == PLAYER_TRAPS                ) return "PLAYER_TRAPS                ";
        else if(type == PLAYER_KEEPER_SPELL         ) return "PLAYER_KEEPER_SPELL         ";
        else if(type == PLAYER_GOLD                 ) return "PLAYER_GOLD                 ";
        else if(type == PLAYER_GOLD_MINED           ) return "PLAYER_GOLD_MINED           ";
        else if(type == PLAYER_MANA                 ) return "PLAYER_MANA                 ";
        else if(type == PLAYER_DESTROYS             ) return "PLAYER_DESTROYS             ";
        else if(type == LEVEL_TIME                  ) return "LEVEL_TIME                  ";
        else if(type == LEVEL_CREATURES             ) return "LEVEL_CREATURES             ";
        else if(type == CREATURE_HEALTH             ) return "CREATURE_HEALTH             ";
        else if(type == CREATURE_GOLD_HELD          ) return "CREATURE_GOLD_HELD          ";
        else if(type == AP_CONGREGATE_IN            ) return "AP_CONGREGATE_IN            ";
        else if(type == AP_CLAIM_PART_OF            ) return "AP_CLAIM_PART_OF            ";
        else if(type == AP_CLAIM_ALL_OF             ) return "AP_CLAIM_ALL_OF             ";
        else if(type == AP_SLAB_TYPES               ) return "AP_SLAB_TYPES               ";
        else if(type == PARTY_CREATED               ) return "PARTY_CREATED               ";
        else if(type == PARTY_MEMBERS_KILLED        ) return "PARTY_MEMBERS_KILLED        ";
        else if(type == PARTY_MEMBERS_CAPTURED      ) return "PARTY_MEMBERS_CAPTURED      ";
        else if(type == LEVEL_PAY_DAY               ) return "LEVEL_PAY_DAY               ";
        else if(type == PLAYER_KILLED               ) return "PLAYER_KILLED               ";
        else if(type == CREATURE_EXPERIENCE_LEVEL   ) return "CREATURE_EXPERIENCE_LEVEL   ";
        else if(type == PLAYER_CREATURES_AT_LEVEL   ) return "PLAYER_CREATURES_AT_LEVEL   ";
        else if(type == GUI_BUTTON_PRESSED          ) return "GUI_BUTTON_PRESSED          ";
        else if(type == CREATURE_HUNGER_SATED       ) return "CREATURE_HUNGER_SATED       ";
        else if(type == CREATURE_PICKS_UP_PORTAL_GEM) return "CREATURE_PICKS_UP_PORTAL_GEM";
        else if(type == PLAYER_DUNGEON_BREACHED     ) return "PLAYER_DUNGEON_BREACHED     ";
        else if(type == PLAYER_ENEMY_BREACHED       ) return "PLAYER_ENEMY_BREACHED       ";
        else if(type == PLAYER_CREATURE_PICKED_UP   ) return "PLAYER_CREATURE_PICKED_UP   ";
        else if(type == PLAYER_CREATURE_DROPPED     ) return "PLAYER_CREATURE_DROPPED     ";
        else if(type == PLAYER_CREATURE_SLAPPED     ) return "PLAYER_CREATURE_SLAPPED     ";
        else if(type == PLAYER_CREATURE_SACKED      ) return "PLAYER_CREATURE_SACKED      ";
        else if(type == AP_TAG_PART_OF              ) return "AP_TAG_PART_OF              ";
        else if(type == CREATURE_SACKED             ) return "CREATURE_SACKED             ";
        else if(type == PARTY_MEMBERS_INCAPACITATED ) return "PARTY_MEMBERS_INCAPACITATED ";
        else if(type == CREATURE_PICKED_UP          ) return "CREATURE_PICKED_UP          ";
        else if(type == LEVEL_PLAYED                ) return "LEVEL_PLAYED                ";
        else if(type == PLAYER_ROOM_FURNITURE       ) return "PLAYER_ROOM_FURNITURE       ";
        else if(type == AP_TAG_ALL_OF               ) return "AP_TAG_ALL_OF               ";
        else if(type == AP_POSESSED_CREATURE_ENTERS ) return "AP_POSESSED_CREATURE_ENTERS ";
        else if(type == PLAYER_SLAPS                ) return "PLAYER_SLAPS                ";
        else if(type == GUI_TRANSITION_ENDS         ) return "GUI_TRANSITION_ENDS         ";
        else if(type == PLAYER_CREATURES_GROUPED    ) return "PLAYER_CREATURES_GROUPED    ";
        else if(type == PLAYER_CREATURES_DYING      ) return "PLAYER_CREATURES_DYING      ";
        return "UNKNOWN TRIGGER             ";
    }
    else
        return "INVALID                     ";

}

short dk2m_read_variableschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
/*
//TODO: make cases for all chunks
  case CTxxx_:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
  case CTxxx_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
*/
  default:
      return ERR_NONE; //no error message - not all chunks supported
      dk2m_ferror("Unknown VARIABLES chunk type %04x",chunk->id);
      return -1;
  }
}

int no_of_triggers = 0;
short dk2m_read_script_header(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{

    no_of_triggers = read_int32_le_buf(chunk->data + 0) + read_int32_le_buf(chunk->data + 4);

    return ERR_NONE;

}
int i = 0;
short dk2m_read_script(const struct DK2M_Chunk *chunk)
{
    if(chunk->dsize == 0)
        return;
    

     unsigned long offs = 0;

   // for (int i = 0; i < no_of_triggers; i++)
   
    {
        triggerdatas[i].tag = chunk->id;
        triggerdatas[i].tdata[0]    = read_int8_buf(chunk->data + offs + 0);
        triggerdatas[i].tdata[1]    = read_int8_buf(chunk->data + offs + 1);
        triggerdatas[i].tdata[2]    = read_int8_buf(chunk->data + offs + 2);
        triggerdatas[i].tdata[3]    = read_int8_buf(chunk->data + offs + 3);
        triggerdatas[i].tdata[4]    = read_int8_buf(chunk->data + offs + 4);
        triggerdatas[i].tdata[5]    = read_int8_buf(chunk->data + offs + 5);
        triggerdatas[i].tdata[6]    = read_int8_buf(chunk->data + offs + 6);
        triggerdatas[i].tdata[7]    = read_int8_buf(chunk->data + offs + 7);
        triggerdatas[i].id          = read_int16_le_buf(chunk->data + offs + 8);
        triggerdatas[i].idNext      = read_int16_le_buf(chunk->data + offs + 10);
        triggerdatas[i].idChild     = read_int16_le_buf(chunk->data + offs + 12);
        triggerdatas[i].type        = read_int8_buf(chunk->data + offs + 14);
        triggerdatas[i].repeatTimes = read_int8_buf(chunk->data + offs + 15);

        offs += 24;
    }
i++;
  //if (chunk->dsize != (total_tiles<<2))
  //    return -1;
  return ERR_NONE;

}


short dk2m_read_triggerschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
    switch (chunk->id)
    {  
        case CTTRG_TRGRSHEADER:
            dk2m_read_script_header(lvl,chunk,flags);
            return ERR_NONE;

        case CTTRG_TRGRSDATA:
            dk2m_read_script(chunk);
            return ERR_NONE;

        case CTTRG_FILESIZE:
            // Skip the file size chunk - we don't need it
            return ERR_NONE;
        default:
            dk2m_read_script(chunk);
            //dk2m_ferror("Unknown TRIGGERS chunk type %d",chunk->id);
            return ERR_NONE;
    }
}




short dk2m_print_script(const struct DK2_Level *lvlDk2,struct LEVEL *lvl)
{
    printf("--script--\n");
    for (int i = 0; i < no_of_triggers; i++)
    {
        if      (triggerdatas[i].tag == TriggerGeneric) printf("trigger ");
        else if (triggerdatas[i].tag == TriggerAction)  printf("action  ");
        else                                            printf("unknown ");
        printf("%s ",type_to_string(triggerdatas[i].tag,triggerdatas[i].type));
        
        printf("%d ",triggerdatas[i].id         );
        printf("%d ",triggerdatas[i].idNext     );
        printf("%d ",triggerdatas[i].idChild    );
        printf("%d  ",triggerdatas[i].repeatTimes);
        printf("%d ",triggerdatas[i].tdata[0]   );
        printf("%d ",triggerdatas[i].tdata[1]   );
        printf("%d ",triggerdatas[i].tdata[2]   );
        printf("%d ",triggerdatas[i].tdata[3]   );
        printf("%d ",triggerdatas[i].tdata[4]   );
        printf("%d ",triggerdatas[i].tdata[5]   );
        printf("%d ",triggerdatas[i].tdata[6]   );
        printf("%d ",triggerdatas[i].tdata[7]   );
        printf("\n");
    }



}