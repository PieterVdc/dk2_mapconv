/******************************************************************************/
/** @file dk2mfile.h
 * Library for r/w of DK2 map files.
 * @par Purpose:
 *     Header file. Defines exported routines from dk2mfile.c.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     15 Jul 2008 - 05 Aug 2008
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/

#ifndef DK2MFILE_H
#define DK2MFILE_H

#include <stdio.h>

#define DK2MFLAG_VERBOSE        0x01
#define DK2MFLAG_IGNORE_BADHDR  0x40


struct DK2M_Chunk {
    unsigned int id;
    unsigned int dsize;
    unsigned char *data;
    };

enum DK2M_FileType {
        FTKWD_GLOBALS            = 0x0000,
        FTKWD_MAP                = 0x0064,
        FTKWD_TERRAIN            = 0x006e,
        FTKWD_ROOMS              = 0x0078,
        FTKWD_TRAPS              = 0x0082,
        FTKWD_DOORS              = 0x008c,
        FTKWD_KEEPRPELLS         = 0x0096,
        FTKWD_CRTRSPELLS         = 0x00a0,
        FTKWD_CREATURES          = 0x00aa,
        FTKWD_PLAYERS            = 0x00b4,
        FTKWD_THINGS             = 0x00be,
        FTKWD_TRIGGERS           = 0x00d2,
        FTKWD_VARIABLES          = 0x00e6,
        FTKWD_OBJECTS            = 0x00f0,
        FTKWD_SHOTS              = 0x0104,
};

#define SIZEOF_DK2M_Chunk_Header 8

enum DK2M_ChunkType {
        CTKWD_FILESIZE           = 0x00dc,
        CTKWD_FILEHEAD           = 0x00dd,
        CTKWD_FILEPATHS          = 0x00de,
        CTKWD_LEVPROPS           = 0x00df,
        CTMAP_FILESIZE           = 0x0064,
        CTMAP_LEVDIMENS          = 0x0065,
        CTMAP_LEVTILES           = 0x0066,
        CTTNG_FILESIZE           = 0x00be,
        CTTNG_FILEHEAD           = 0x00bf,
        CTTNG_LEVTHINGS          = 0x00c0,
        CTDOR_FILESIZE           = 0x008c,
        CTDOR_FILEHEAD           = 0x008d,
        CTDOR_DOORSDATA          = 0x008e,
        CTTRP_FILESIZE           = 0x0082,
        CTTRP_FILEHEAD           = 0x0083,
        CTTRP_TRAPSDATA          = 0x0084,
        CTCTR_FILESIZE           = 0x00aa,
        CTCTR_FILEHEAD           = 0x00ab,
        CTCTR_CRTRSDATA          = 0x00ac,
        CTROM_FILESIZE           = 0x0078,
        CTROM_FILEHEAD           = 0x0079,
        CTROM_ROOMSDATA          = 0x007a,
        CTPLR_FILESIZE           = 0x00b4,
        CTPLR_FILEHEAD           = 0x00b5,
        CTPLR_PLYRSDATA          = 0x00b6,
        CTSHT_FILESIZE           = 0x0104,
        CTSHT_FILEHEAD           = 0x0105,
        CTSHT_SHOTSDATA          = 0x0106,
        CTOBJ_FILESIZE           = 0x00f0,
        CTOBJ_FILEHEAD           = 0x00f1,
        CTOBJ_OBJCTDATA          = 0x00f2,
        CTCSP_FILESIZE           = 0x00a0,
        CTCSP_FILEHEAD           = 0x00a1,
        CTCSP_CTSPLDATA          = 0x00a2,
        CTKSP_FILESIZE           = 0x0096,
        CTKSP_FILEHEAD           = 0x0097,
        CTKSP_KPSPLDATA          = 0x0098,
        CTTRR_FILESIZE           = 0x006e,
        CTTRR_FILEHEAD           = 0x006f,
        CTTRR_TERRNDATA          = 0x0070,

        CTTRG_FILESIZE           = 0x00d2,
//        CTTRG_                   = 0x00d3,
        CTTRG_TRGRSDATA          = 0x0002,
};

/*
Global variables have 6 chunks!
Chunk ID: 00e6, size 4
Chunk ID: 00e7, size 28
Chunk ID: 00e8, size 9936
Chunk ID: 00be, size 4
Chunk ID: 00bf, size 28
Chunk ID: 00c0, size 3588

Triggers have at least 5 chunks!
Chunk ID: 00d2, size 4
Chunk ID: 00d3, size 32
Chunk ID: 00d4, size 32 (size is not really size)
*/

enum DK2M_LevFlags {
//        LFKWD_                   = 0x0004, // unknown; always on in maps
        LFKWD_ALWAYSIMPRISN      = 0x0008, // Always imprison enemies
        LFKWD_ONESHOTHORNY       = 0x0010, // Set if one shot Horny spell is available
        LFKWD_ISSECRETLVL        = 0x0020, // The map is Secret level
        LFKWD_ISSPECIALLVL       = 0x0040, // The map is Special level
        LFKWD_SHOWHEROKILLS      = 0x0080, // Display "Heroes killed" tally
        LFKWD_AUTOOBJECTVBX      = 0x0100, // Automatic show objective box
        LFKWD_HEARTMAKESGEM      = 0x0200, // Last heart generates Portal Gem
        LFKWD_ISMULTIPLRLVL      = 0x0400, // The map is Multiplayer level
        LFKWD_ISSKIRMISHLVL      = 0x0800, // The map is Skirmish level
        LFKWD_FREEZEOPTIONS      = 0x1000, // Freeze game options
        LFKWD_ISMPDLEVEL         = 0x2000, // The map is My Pet Dungeon level
};

#define SIZEOF_DK2M_KWDFileHead   28
#define SIZEOF_DK2M_KWDLevProps   25683
#define SIZEOF_DK2M_FilePath      72
#define SIZEOF_DK2M_TimeStamp     24

struct DK2_TimeStamp { // timestamp is written in UTC
       short year1;
       unsigned char day1;
       unsigned char month1;
       unsigned short u2;
       unsigned char hour1;
       unsigned char min1;
       unsigned char sec1;
       unsigned char u3;
       short year2;
       unsigned char day2;
       unsigned char month2;
       unsigned short u4;
       unsigned char hour2;
       unsigned char min2;
       unsigned char sec2;
       unsigned char u5;
       };

#define SIZEOF_DK2_FilePathStr 64

struct DK2_FilePath {
       unsigned int id;
       int u2;
       char str[SIZEOF_DK2_FilePathStr+1];
       };

struct DK2_LvlReward {
       int id;
       const char *name;
       unsigned short u5;
       unsigned short u6;
       short u7;
       };

#define SIZEOF_DK2_TinyStr     32
#define SIZEOF_DK2_ShortStr    64
#define SIZEOF_DK2_LongStr     1024
#define SIZEOF_DK2_MsgStr      512
#define SIZEOF_DK2_KWDRndBlock 520
#define DK2_MsgStr_count       20
#define DK2_MsgRewrd_count     5
#define DK2_MsgObjctv_count    3
#define DK2_MsgOverrd_count    8

struct DK2_LvlProps {
       short name[SIZEOF_DK2_ShortStr+1];
       short desc[SIZEOF_DK2_LongStr+1];
       short author[SIZEOF_DK2_ShortStr+1];
       short email[SIZEOF_DK2_ShortStr+1];
       short info[SIZEOF_DK2_LongStr+1];
       unsigned short u3;
       unsigned short ticks_per_sec;
       unsigned char randblock[SIZEOF_DK2_KWDRndBlock]; // This block contains random trash
       unsigned short msgs[DK2_MsgStr_count][SIZEOF_DK2_MsgStr];
       unsigned short lvflags; // flags from DK2M_LevFlags enumeration
       char speechstr[SIZEOF_DK2_TinyStr+1];
       unsigned char talisman_pieces;
       unsigned char reward_prev[DK2_MsgRewrd_count];
       unsigned char reward_next[DK2_MsgRewrd_count];
       unsigned char sound_track;
       unsigned char text_table_id;
       unsigned short text_title_id;
       unsigned short text_plot_id;
       unsigned short text_debrief_id;
       unsigned short text_objectv_id;
       unsigned short u10;//this may be first text_subobjctv_id - not sure
       unsigned short text_subobjctv_id[DK2_MsgObjctv_count];
       unsigned char speclvl_idx;
       unsigned char u11;
       unsigned char text_intrdc_overrd_obj[DK2_MsgOverrd_count];
       unsigned short text_intrdc_overrd_id[DK2_MsgOverrd_count];
       char terrain_path[SIZEOF_DK2_TinyStr+1];
       unsigned char one_shot_horny_lev;
       unsigned char u20;
       // Note: there are two reward entries in this place of KWD file
       unsigned short speech_horny_id;
       unsigned short speech_prelvl_id;
       unsigned short speech_postlvl_win;
       unsigned short speech_postlvl_lost;
       unsigned short speech_postlvl_news;
       unsigned short speech_prelvl_genr;
       short hero_name[SIZEOF_DK2_TinyStr+1];
       };

#define SIZEOF_DK2M_MAPDimnsn     8

struct DK2_LvlTile {
       unsigned char slab;
       unsigned char owner;
       unsigned char wlb;
       unsigned char u1;
       };

struct DK2_Level {
    // Data from KWD file
    struct DK2_LvlProps props;
    unsigned int u1;
    struct DK2_TimeStamp tmstamp;
    unsigned short unkarr01_count;
    unsigned short filepaths_count;
    struct DK2_FilePath **filepaths;
    unsigned short *unkarr01;
    // Data from MAP file
    int width;
    int height;
    struct DK2_LvlTile **tiles;
    };

// Routines

short dk2m_error(const char *msg);
short dk2m_ferror(const char *format, ...);
short dk2m_print_chunk(const struct DK2M_Chunk *chunk);

long dk2m_fread_chunk(struct DK2M_Chunk *chunk,FILE *fp,
    unsigned long offset,short flags);
short dk2m_read_kwdfile(struct DK2_Level *lvl,const char *fname,short flags);

short dk2m_lvl_create(struct DK2_Level **lvl,short flags);
short dk2m_lvl_free(struct DK2_Level **lvl,short flags);

short dk2m_read_mapfiles(struct DK2_Level *lvl,const char *name,short flags);

short dk2m_print_level(const struct DK2_Level *lvl);

#endif
