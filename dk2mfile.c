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

#include "dk2mfile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "lbfileio.h"

#include "../ADiKtEd/libadikted/adikted.h"

typedef short (*dk2m_read_chunk)(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags);

const char lvflg_alwaysimprisn_longtext[]="Always imprison enemies";
const char lvflg_oneshothorny_longtext[]= "One-shot Horny spell available";
const char lvflg_issecretlvl_longtext[]=  "Secret level";
const char lvflg_isspeciallvl_longtext[]= "Special level";
const char lvflg_showherokills_longtext[]="Display \"Heroes killed\" tally";
const char lvflg_autoobjectvbx_longtext[]="Automatic show objective box";
const char lvflg_heartmakesgem_longtext[]="Last heart generates Portal Gem";
const char lvflg_ismultiplrlvl_longtext[]="Multiplayer level";
const char lvflg_isskirmishlvl_longtext[]="Skirmish level";
const char lvflg_freezeoptions_longtext[]="Freeze game options";
const char lvflg_ismpdlevel_longtext[]=   "My Pet Dungeon level";

const char *lvflg_longtexts[]={ "Flag1","Flag2","Flag4",
    lvflg_alwaysimprisn_longtext,lvflg_oneshothorny_longtext,
    lvflg_issecretlvl_longtext,lvflg_isspeciallvl_longtext,
    lvflg_showherokills_longtext,lvflg_autoobjectvbx_longtext,
    lvflg_heartmakesgem_longtext,lvflg_ismultiplrlvl_longtext,
    lvflg_isskirmishlvl_longtext,lvflg_freezeoptions_longtext,
    lvflg_ismpdlevel_longtext,"Flag4000","Flag8000", };

const char lvflg_alwaysimprisn_shortext[]="AlwysImprisn";
const char lvflg_oneshothorny_shortext[]= "OneShotHorny";
const char lvflg_issecretlvl_shortext[]=  "IsSecret";
const char lvflg_isspeciallvl_shortext[]= "IsSpecial";
const char lvflg_showherokills_shortext[]="ShwHeroKills";
const char lvflg_autoobjectvbx_shortext[]="AutoObjctvBx";
const char lvflg_heartmakesgem_shortext[]="HeartMkesGem";
const char lvflg_ismultiplrlvl_shortext[]="IsMultiplyr";
const char lvflg_isskirmishlvl_shortext[]="IsSkirmish";
const char lvflg_freezeoptions_shortext[]="FreezeOptns";
const char lvflg_ismpdlevel_shortext[]=   "IsMPD";

const char *lvflg_shortexts[]={ "Flag1","Flag2","Flag4",
    lvflg_alwaysimprisn_shortext,lvflg_oneshothorny_shortext,
    lvflg_issecretlvl_shortext,lvflg_isspeciallvl_shortext,
    lvflg_showherokills_shortext,lvflg_autoobjectvbx_shortext,
    lvflg_heartmakesgem_shortext,lvflg_ismultiplrlvl_shortext,
    lvflg_isskirmishlvl_shortext,lvflg_freezeoptions_shortext,
    lvflg_ismpdlevel_shortext,"Flag4000","Flag8000", };

const char none_text[] = "None";
const char trap_alarm_text[] = "Alarm";
const char door_brricd_text[] = "Barricade";
const char trap_bouldr_text[] = "Boulder";
const char door_braced_text[] = "Braced";
const char room_bridgs_text[] = "Bridge_Stone";
const char room_bridgw_text[] = "Bridge_Wooden";
const char spell_cta_text[] = "Call_To_Arms";
const char room_casino_text[] = "Casino";
const char obj_chicken_text[] = "Chicken";
const char spell_ctgld_text[] = "Create_Gold";
const char spell_ctimp_text[] = "Create_Imp";
const char room_dlibry_text[] = "Dark_Library";
const char spell_erthq_text[] = "Earthquake";
const char trap_fear_text[] = "Fear";
const char trap_firebr_text[] = "Fireburst";
const char trap_freeze_text[] = "Freeze";
const char trap_gas_text[] = "Gas";
const char room_grvyrd_text[] = "Graveyard";
const char room_grdrom_text[] = "Guard_Room";
const char room_grdpst_text[] = "Guard_Post";
const char room_hatchr_text[] = "Hatchery";
const char spell_heal_text[] = "Heal";
const char trap_infrno_text[] = "Inferno";
const char room_lair_text[] = "Lair";
const char trap_lightn_text[] = "Lightning";
const char door_magic_text[] = "Magic_Door";
const char room_pit_text[] = "Pit";
const char spell_poses_text[] = "Possess";
const char room_prisn_text[] = "Prison";
const char talisman04_text[] = "Reaper_Talisman04";
const char rewrd_fight_text[] = "Fight";
const char door_secrt_text[] = "Secret_Door";
const char trap_sentry_text[] = "Sentry";
const char spell_sight_text[] = "Sight_Of_Evil";
const char trap_spike_text[] = "Spike";
const char door_steel_text[] = "Steel_Door";
const char spell_reapr_text[] = "Summon_Reaper";
const char room_temple_text[] = "Temple";
const char trap_thund_text[] = "Thunderbolt";
const char room_tortr_text[] = "Torture";
const char room_train_text[] = "Training";
const char room_treasr_text[] = "Treasury";
const char trap_triggr_text[] = "Trigger";
const char spell_trnct_text[] = "Turncoat";
const char notused_text[] = "<Not Used>";
const char door_wood_text[] = "Wood_Door";
const char workshop_text[] = "Workshop";
const char talisman01_text[] = "Reaper_Talisman01";
const char talisman02_text[] = "Reaper_Talisman02";
const char talisman03_text[] = "Reaper_Talisman03";

const struct DK2_LvlReward lvl_rewards[] = {
 {0,  none_text, 0, 0, 1},
 {1,  trap_alarm_text,  295, 1369, 1},
 {2,  door_brricd_text, 314, 1366, 1},
 {3,  trap_bouldr_text, 294, 1377, 1},
 {4,  door_braced_text, 306, 1373, 1},
 {5,  room_bridgs_text, 242, 1370, 1},
 {6,  room_bridgw_text, 236, 1360, 1},
 {7,  spell_cta_text,   283, 1368, 1},
 {8,  room_casino_text, 239, 1371, 1},
 {9,  obj_chicken_text, 292, 1392, 0},
 {10, spell_ctgld_text, 290, 1385, 0},
 {11, spell_ctimp_text, 284, 1352, 0},
 {12, room_dlibry_text, 232, 1350, 1},
 {13, spell_erthq_text, 287, 1374, 0},
 {14, trap_fear_text,   299, 1363, 1},
 {15, trap_firebr_text, 296, 1390, 1},
 {16, trap_freeze_text, 298, 1381, 1},
 {17, trap_gas_text,    293, 1372, 1},
 {18, room_grvyrd_text, 240, 1379, 1},
 {19, room_grdrom_text, 238, 1358, 1},
 {20, room_grdpst_text, 0, 1362, 1},
 {21, room_hatchr_text, 230, 1394, 1},
 {22, spell_heal_text,  289, 1365, 0},
 {23, trap_infrno_text, 288, 1389, 0},
 {24, room_lair_text,   229, 1395, 1},
 {25, trap_lightn_text, 297, 1386, 1},
 {26, door_magic_text,  310, 1383, 1},
 {27, room_pit_text,    235, 1384, 1},
 {28, spell_poses_text, 282, 1359, 0},
 {29, room_prisn_text,  234, 1361, 1},
 {30, talisman04_text,  0, 0, 1},
 {31, rewrd_fight_text, 0, 0, 0},
 {32, door_secrt_text,  312, 1387, 1},
 {33, trap_sentry_text, 301, 1356, 1},
 {34, spell_sight_text, 281, 1367, 0},
 {35, trap_spike_text,  302, 1375, 1},
 {36, door_steel_text,  308, 1378, 1},
 {37, spell_reapr_text, 285, 1382, 0},
 {38, notused_text, 0, 0, 0},
 {39, room_temple_text, 241, 1388, 1},
 {40, trap_thund_text,  286, 1353, 0},
 {41, room_tortr_text,  237, 1364, 1},
 {42, room_train_text,  231, 1351, 1},
 {43, room_treasr_text, 228, 1354, 1},
 {44, trap_triggr_text, 300, 1376, 1},
 {45, spell_trnct_text, 291, 1380, 0},
 {46, notused_text, 0, 0, 0},
 {47, door_wood_text,   304, 1357, 1},
 {48, workshop_text,    233, 1355, 1},
 {49, talisman01_text, 0, 0, 1},
 {50, talisman02_text, 0, 0, 1},
 {51, talisman03_text, 0, 0, 1},
 {-1, NULL, 0, 0, 0} };

/*
 * Displays simple error message.
 */
short dk2m_error(const char *msg)
{
    printf("dk2map Support: %s\n",msg);
    return ERR_NONE;
}

/*
 * Displays formatted error message.
 */
short dk2m_ferror(const char *format, ...)
{
    static char errmessage[255];
    va_list val;
    va_start(val, format);
    vsprintf(errmessage,format,val);
    va_end(val);
    short result;
    result=dk2m_error(errmessage);
    return result;
}

short dk2m_wtos(char *dst,const short *src)
{
    unsigned int i=0;
    short chr=1;
    while (chr!=0)
    {
        chr=src[i];
        if (chr<256)
          dst[i]=chr;
        else
          dst[i]='_';
        i++;
    }
    return ERR_NONE;
}

/**
 * Returns file name pointer from given filename with path.
 * @param pathname The source filename, possibly with path.
 * @return Pointer to the name in pathname string, with path skipped.
 */
char *filename_from_path2(const char *pathname)
{
    const char *fname = NULL;
    if (pathname)
    {
        fname = strrchr (pathname, '/') + 1;
        char *fname2 = strrchr (pathname, '\\') + 1;
        if ((!fname)||(fname2>fname))
            fname = fname2;
    }
    if (!fname)
        fname=pathname;
    return (char *)fname;
}

void path_set_to_higher_level(char *fpath)
{
    int fplen=strlen(fpath);
    char chr;
    if (fplen>1)
    {
      chr=fpath[fplen-1];
      if ((chr=='\\')||(chr=='/'))
      {
        if (fplen>2)
          if ((fpath[fplen-2]=='.')&&(fpath[fplen-3]=='.'))
          {
              strcpy(fpath+fplen,"..\\");
              return;
          }
        fplen--;
      }
      while (fplen>0)
      {
        chr=fpath[fplen-1];
        if ((chr=='\\')||(chr=='/'))
          break;
        fplen--;
      }
      fpath[fplen]='\0';
      return;
    }
    // So input string is not larger than 1
    if (fplen>0)
    {
      char chr=fpath[fplen-1];
      if ((chr=='\\')||(chr=='/'))
        return;
      fpath[0]='\0';
      return;
    }
    // So input string is empty
    strcpy(fpath,"..\\");
}

unsigned int str_chrnum(char *str,char chr)
{
    unsigned int i;
    i=0;
    char *s=strchr(str,chr);
    while (s!=NULL)
    {
      i++;
      s++;
      s=strchr(s,chr);
    }
    return i;
}

char dk2_slab2char(unsigned short slab)
{
  if (slab<32)
       return 'A'+slab;
  else
       return slab;
}
/*
#define PLAYER0                0x00
#define PLAYER1                0x01
#define PLAYER2                0x02
#define PLAYER3                0x03
#define PLAYER_GOOD            0x04
#define PLAYER_UNSET           0x05
*/
char dk2_owner2DK1owner(unsigned short dk2owner)
{
  switch (dk2owner)
  {
  case 1:
    return PLAYER_GOOD;
  case 2:
    return PLAYER_UNSET;
    break;
  case 3:
    return PLAYER0;
    break;
  case 4:
    return PLAYER1;
    break;
  case 5:
    return PLAYER2;
    break;
  case 6:
    return PLAYER3;
    break;

  default:
  printf("invalid owner: %d\n",dk2owner);
    return PLAYER_UNSET;

  }
}

char dk2_slab2DK1Slab(unsigned short slab)
{

  
/*
unused slabs
#define SLAB_TYPE_TORCHDIRT    0x03
#define SLAB_TYPE_WALLDRAPE    0x04
#define SLAB_TYPE_WALLTORCH    0x05
#define SLAB_TYPE_WALLWTWINS   0x06
#define SLAB_TYPE_WALLWWOMAN   0x07
#define SLAB_TYPE_WALLPAIRSHR  0x08
#define SLAB_TYPE_SCAVENGER    0x1e
#define SLAB_TYPE_BARRACKS     0x28
#define SLAB_TYPE_DOORWOOD1    0x2a
#define SLAB_TYPE_DOORWOOD2    0x2b
#define SLAB_TYPE_DOORBRACE1   0x2c
#define SLAB_TYPE_DOORBRACE2   0x2d
#define SLAB_TYPE_DOORIRON1    0x2e
#define SLAB_TYPE_DOORIRON2    0x2f
#define SLAB_TYPE_DOORMAGIC1   0x30
#define SLAB_TYPE_DOORMAGIC2   0x31
*/


#define SLAB_TYPE_PURPLE_PATH 0x36
switch (slab)
{
case 30:
case 1:
  return SLAB_TYPE_ROCK;
  break;
case 6:
  return SLAB_TYPE_GOLD;
  break;
case 2:
  return SLAB_TYPE_EARTH;
  break;
case 9:
  return SLAB_TYPE_WALLDRAPE;
  break;
case 3:
  return SLAB_TYPE_PATH;
  break;
case 8:
  return SLAB_TYPE_CLAIMED;
  break;
case 4:
  return SLAB_TYPE_LAVA;
  break;
case 5:
  return SLAB_TYPE_WATER;
  break;
case 12:
  return SLAB_TYPE_PORTAL;
  break;
case 10:
  return SLAB_TYPE_TREASURE;
  break;
case 15:
  return SLAB_TYPE_LIBRARY;
  break;
case 20:
  return SLAB_TYPE_PRISONCASE;
  break;
case 21:
  return SLAB_TYPE_TORTURE;
  break;
case 16:
  return SLAB_TYPE_TRAINING;
  break;
case 14:
  return SLAB_TYPE_DUNGHEART;
  break;
case 19:
  return SLAB_TYPE_WORKSHOP;
  break;
case 22:
  return SLAB_TYPE_TEMPLE;
  break;
case 23:
  return SLAB_TYPE_GRAVEYARD;
  break;
case 17://wood bridge
case 26://stone bridge
  return SLAB_TYPE_BRIDGE;
  break;
case 7:
  return SLAB_TYPE_GEMS;
  break;
case 18:
  return SLAB_TYPE_GUARDPOST;
  break;
  /*  TODO figure out dk2 number for lair and hatchery as they can't be placed in editor
case 'O':
  return SLAB_TYPE_HATCHERY;
  break;
case 'O':
  return SLAB_TYPE_LAIR;
  break;
*/

case 24://casino
case 25://arena
case 35://hero lair
case 28://hero gate loose
case 33://hero gate 2*2
case 37://hero gate 1*3
case 40://hero gate 3*3
default:
  return SLAB_TYPE_PURPLE_PATH;
  break;
}








}

/*
 * Prints chunk information.
 */
short dk2m_print_chunk(const struct DK2M_Chunk *chunk)
{
    printf("Chunk ID: %04x, size %u\n",chunk->id,chunk->dsize);
    printf("Data: ");
    // Hack to disable huge chunks dumping
    if (chunk->dsize>65535) { printf("(large block)\n"); return ERR_NONE; }
    int dsize = (chunk->dsize >> 2);
    int i;
    printf("         ");
    for (i=0;i<dsize;i++)
      printf("%08x ",*((int *)(&chunk->data[i<<2])));
    printf("\n");
    return ERR_NONE;
}

/*
 * Loads single, RAW dk2m data block from disk file.
 * No processing is performed to the returned block - RAW read only.
 * Note that this is low-level function; use dk2m_fread_entry_body instead.
 * Returns amount of bytes readed, or negative value on error.
 */
long dk2m_fread_chunk(struct DK2M_Chunk *chunk,FILE *fp,unsigned long offset,short flags)
{
  char buf[SIZEOF_DK2M_Chunk_Header+1];
  if ((fp==NULL)||(chunk==NULL))
  {
      if (flags&DK2MFLAG_VERBOSE)
        dk2m_ferror("Bad initial variables for chunk reading");
      return -1;
  }
  if (fseek(fp,offset,SEEK_SET) != 0)
  {
      if (flags&DK2MFLAG_VERBOSE)
        dk2m_ferror("%s while seeking for chunk",strerror(errno));
      return -1;
  }
  //printf("reading file at %d, wanted to read %d\n",ftell(fp),offset);
  long nread;
  nread=fread(buf,1,SIZEOF_DK2M_Chunk_Header,fp);
  if (nread != SIZEOF_DK2M_Chunk_Header)
  {
      if (nread==0)
        return 0;
      if (flags&DK2MFLAG_VERBOSE)
        dk2m_ferror("%s while reading chunk header",strerror(errno));
      return -1;
  }
  chunk->id=read_int32_le_buf(buf+0);
  // Special chunks
  if (chunk->id==212)
  {
      chunk->dsize=0;
  } else
  {
      chunk->dsize=read_int32_le_buf(buf+4);
  }
  //printf("chunk id %d, size %d\n",chunk->id,chunk->dsize);
  if (chunk->dsize==0)
  {
      chunk->data=NULL;
      return SIZEOF_DK2M_Chunk_Header;
  }
  chunk->data=malloc(chunk->dsize);
  if (chunk->data==NULL)
  {
      if (flags&DK2MFLAG_VERBOSE)
        dk2m_ferror("Can't allocate memory to store chunk data");
      return -1;
  }
  nread=fread(chunk->data,1,chunk->dsize,fp);
  if (nread != chunk->dsize)
  {
      //printf("readed %d, wanted %d\n",nread,chunk->dsize);
      if (flags&DK2MFLAG_VERBOSE)
        dk2m_ferror("%s while reading data",strerror(errno));
      if (nread<chunk->dsize)
          memset(chunk->data+nread,0,chunk->dsize-nread);
  }
  return nread+SIZEOF_DK2M_Chunk_Header;
}

/*
 * Returns level flags as human-readable string.
 */
char *dk2m_get_lvflags_shorttext(const unsigned int lvflags)
{
    static char lvflags_shorttext[16*14];
    char *flagstext=lvflags_shorttext;
    int i;
    for (i=0;i<16;i++)
    {
      unsigned int mask=1<<i;
      if (lvflags&mask)
      {
          strcpy(flagstext,lvflg_shortexts[i]);
          flagstext+=strlen(flagstext);
          flagstext[0]=' ';
          flagstext++;
      }
    }
    flagstext[0]='\0';
    return lvflags_shorttext;
}

/*
 * Returns level reward name string.
 */
const char *dk2m_get_reward_text(const unsigned int lvrewrd)
{
     int i;
     for (i=0;lvl_rewards[i].id>=0;i++)
     {
         if (lvl_rewards[i].id==lvrewrd)
           return lvl_rewards[i].name;
     }
     return "Unknown";
}

short dk2m_set_filepaths_count(struct DK2_Level *lvl,const unsigned short count,short flags)
{
  unsigned int prev_count;
  prev_count=lvl->filepaths_count;
  int i;
  // Free any old pointers
  if (lvl->filepaths!=NULL)
  {
    i=prev_count-1;
    while (i>=count)
    {
      free(lvl->filepaths[i]);
      lvl->filepaths[i]=NULL;
      i--;
    }
  }
  // Change length of the array
  lvl->filepaths_count=count;
  lvl->filepaths=realloc(lvl->filepaths,count*sizeof(struct DK2_FilePath *));
  if ((lvl->filepaths==NULL)&&(count>0))
      return -1;
  // Clear new entries
  for (i=prev_count;i<count;i++)
  {
      lvl->filepaths[i]=NULL;
  }
  return ERR_NONE;
}

short dk2m_set_unkarr01_count(struct DK2_Level *lvl,const unsigned short count,short flags)
{
  unsigned int prev_count;
  prev_count=lvl->unkarr01_count;
  int i;
  // Change length of the array
  lvl->unkarr01_count=count;
  lvl->unkarr01=realloc(lvl->unkarr01,count*sizeof(unsigned short));
  if ((lvl->unkarr01==NULL)&&(count>0))
      return -1;
  // Clear new entries
  for (i=prev_count;i<count;i++)
  {
      lvl->unkarr01[i]=0;
  }
  return ERR_NONE;
}

short dk2m_grow_filepaths_count(struct DK2_Level *lvl,const unsigned short grow,short flags)
{
  dk2m_set_filepaths_count(lvl,lvl->filepaths_count+grow,flags);
}

struct DK2_FilePath *dk2m_make_filepaths_item_at(struct DK2_Level *lvl,unsigned int idx,short flags)
{
  short result;
  if (lvl->filepaths[idx]==NULL)
  {
    lvl->filepaths[idx]=malloc(sizeof(struct DK2_FilePath));
    if (lvl->filepaths[idx]==NULL)
      return NULL;
  }
  memset(lvl->filepaths[idx],0,sizeof(struct DK2_FilePath));
  return lvl->filepaths[idx];
}

struct DK2_FilePath *dk2m_new_filepaths_item(struct DK2_Level *lvl,short flags)
{
  short result;
  unsigned int new_idx=lvl->filepaths_count;
  result=dk2m_set_filepaths_count(lvl,lvl->filepaths_count+1,flags);
  if (result!=ERR_NONE)
      return NULL;
  return dk2m_make_filepaths_item_at(lvl,new_idx,flags);
}

short dk2m_print_timestamp(const struct DK2_TimeStamp *tmstamp)
{
    printf("DK2_TimeStamp\n");
    int yy,nn,dd,hh,mm,ss;
    yy=tmstamp->year1;
    nn=tmstamp->month1;
    dd=tmstamp->day1;
    hh=tmstamp->hour1;
    mm=tmstamp->min1;
    ss=tmstamp->sec1;
    printf(" Date1: %02d.%02d.%d %02d:%02d:%02d\n",dd,nn,yy,hh,mm,ss);
    yy=tmstamp->year2;
    nn=tmstamp->month2;
    dd=tmstamp->day2;
    hh=tmstamp->hour2;
    mm=tmstamp->min2;
    ss=tmstamp->sec2;
    printf(" Date2: %02d.%02d.%d %02d:%02d:%02d\n",dd,nn,yy,hh,mm,ss);
    yy=tmstamp->u2;
    nn=tmstamp->u3;
    dd=tmstamp->u4;
    hh=tmstamp->u5;
    printf(" Unknowns: %04x %02x   %04x %02x\n",yy,nn,dd,hh);
}

/*
 * Prints the level information normally stored in KWD file.
 */
short dk2m_print_lvlkwd(const struct DK2_Level *lvl)
{
    char str[SIZEOF_DK2_LongStr+1];
    int i,k;
    printf("DK2_LvlProps\n");
    dk2m_wtos(str,lvl->props.name);
    printf(" Name: %s\n",str);
    dk2m_wtos(str,lvl->props.desc);
    printf(" Desc: %s\n",str);
    dk2m_wtos(str,lvl->props.author);
    printf(" Author: %s\n",str);
    dk2m_wtos(str,lvl->props.email);
    printf(" E-mail: %s\n",str);
    dk2m_wtos(str,lvl->props.info);
    printf(" Info: %s\n",str);
    printf(" U3: %04x\n",(int)lvl->props.u3);
    printf(" Ticks Per Second: %u\n",(int)lvl->props.ticks_per_sec);
    printf(" RandBlock:");
    k=0;
    for (i=0;i<SIZEOF_DK2_KWDRndBlock;i++)
    {
        unsigned int chr=lvl->props.randblock[i];
        if (chr!=0) k=1;
    }
    if (k!=0)
    {
      for (i=0;i<SIZEOF_DK2_KWDRndBlock;i++)
      {
        if ((i%40)==0)
          printf("\n ");
        unsigned int chr=lvl->props.randblock[i];
        printf("%02x",chr);
      }
      printf("\n");
    } else
    {
        printf(" (zeros)\n");
    }
    for (i=0;i<20;i++)
    {
        unsigned int chr=lvl->props.msgs[i][0];
        if (chr!=0)
        {
          dk2m_wtos(str,lvl->props.msgs[i]);
          printf(" Message %d: %s\n",i,str);
        }
    }
    printf(" LvFlags: %04x, %s\n",lvl->props.lvflags,dk2m_get_lvflags_shorttext(lvl->props.lvflags));
    printf(" SpeechStr: %s\n",lvl->props.speechstr);
    printf(" Horny talisman pieces: %u, sound track %d\n",
        (int)lvl->props.talisman_pieces,(int)lvl->props.sound_track);
    k=0;
    printf(" Previous rewards:");
    for (i=0;i<DK2_MsgRewrd_count;i++)
    {
        unsigned int itm=lvl->props.reward_prev[i];
        if (itm!=0)
        {
          printf(" %s",dk2m_get_reward_text(itm));
          k++;
        }
    }
    if (k==0)
      printf(" (none)\n");
    else
      printf("\n");
    k=0;
    printf(" Next rewards:");
    for (i=0;i<DK2_MsgRewrd_count;i++)
    {
        unsigned int itm=lvl->props.reward_next[i];
        if (itm!=0)
        {
          printf(" %s",dk2m_get_reward_text(itm));
          k++;
        }
    }
    if (k==0)
      printf(" (none)\n");
    else
      printf("\n");
    printf(" Text IDs: table %u, tile %u, plot %u, debrief %u, objective %u\n",
        (int)lvl->props.text_table_id,(int)lvl->props.text_title_id,
        (int)lvl->props.text_plot_id,(int)lvl->props.text_debrief_id,
        (int)lvl->props.text_objectv_id);
    printf(" U10: %4d\n",(int)lvl->props.u10);
    k=0;
    printf(" Text IDs of sub-objectives:");
    for (i=0;i<DK2_MsgObjctv_count;i++)
    {
        unsigned int itm=lvl->props.text_subobjctv_id[i];
        if (itm!=0)
        {
          printf(" %d",itm);
          k++;
        }
    }
    if (k==0)
      printf(" (none)\n");
    else
      printf("\n");
    printf(" Secret/special lvl idx: %d, U11: %4d\n",
        (int)lvl->props.speclvl_idx,(int)lvl->props.u11);
    k=0;
    printf(" Introduction overrides:");
    for (i=0;i<DK2_MsgOverrd_count;i++)
    {
        unsigned int itm=lvl->props.text_intrdc_overrd_obj[i];
        if (itm!=0)
        {
          printf(" %d->%d,",itm,(int)lvl->props.text_intrdc_overrd_id[i]);
          k++;
        }
    }
    if (k==0)
      printf(" (none)\n");
    else
      printf("\n");
    printf(" Alt. terrain path: %s\n",lvl->props.terrain_path);
    printf(" One shot horny level: %d\n",lvl->props.one_shot_horny_lev);
    printf(" U20: %02x\n",(int)lvl->props.u20);
    printf(" Horny speech ID: %u\n",(int)lvl->props.speech_horny_id);
    printf(" Pre-level speech: briefing %u, generic %u\n",
        (int)lvl->props.speech_prelvl_id,(int)lvl->props.speech_prelvl_genr);
    printf(" Post-level speech: win %u, lost %u, news %u\n",
        (int)lvl->props.speech_postlvl_win,(int)lvl->props.speech_postlvl_lost,
        (int)lvl->props.speech_postlvl_news);

    dk2m_wtos(str,lvl->props.hero_name);
    printf(" Hero name: %s\n",str);
    dk2m_print_timestamp(&lvl->tmstamp);
    printf(" U1: %06x\n",lvl->u1);
    printf("DK2_FilePath, %d items\n",lvl->filepaths_count);
    for (i=0;i<lvl->filepaths_count;i++)
    {
        struct DK2_FilePath *itm=lvl->filepaths[i];
        if (itm!=NULL)
          printf(" ID: %04x U2: %04x Str: %s\n",itm->id,itm->u2,itm->str);
    }
    printf("DK2_unkarr01, %d items",lvl->unkarr01_count);
    for (i=0;i<lvl->unkarr01_count;i++)
    {
        if ((i%32)==0)
          printf("\n");
        unsigned int val=lvl->unkarr01[i];
        printf(" %02u",val);
    }
    printf("\n");
    return ERR_NONE;
}

/*
 * Prints the level information normally stored in MAP file.
 */
short dk2m_print_lvlmap(const struct DK2_Level *lvlDk2)
{



    struct LEVEL *lvl;

  // create object for storing map

    init_messages();
    set_msglog_fname("aaa.log");
    //there's a bug in adikted when using non square maps, so for now make it a square
    int size = max(lvlDk2->width,lvlDk2->height);
    struct UPOINT_3D mapsize={size,size,1};
    level_init(&(lvl),MFV_DKXPAND,&mapsize);
  //level_init(&lvl,MFV_DKGOLD,NULL);
    

  // Setting file name of the map to load
  


  printf("Free\n");
  //free_map(lvl);
  printf("starting new map file\n");

  start_new_map(lvl);

  format_lvl_fname(lvl,"levels/map00001");

  // Making modifications

    lvl->savfname="levels/map00001";

    char str[SIZEOF_DK2_LongStr+1];
    dk2m_wtos(str,lvlDk2->props.name);
    set_lif_name_text(lvl,str);
    lvl->info.author_text = lvlDk2->props.author;
    lvl->info.desc_text = lvlDk2->props.desc;


  printf("changing slab\n");

  printf("DK2_LvlTiles, %dx%d\n",lvlDk2->width,lvlDk2->height);
  int tile_y,tile_x;
  for (tile_x=0;tile_x<lvlDk2->height;tile_x++)
  {
    printf(" ");
    for (tile_y=0;tile_y<lvlDk2->width;tile_y++)
    {
        struct DK2_LvlTile *tile;
        tile=&(lvlDk2->tiles[tile_x][tile_y]);
        set_tile_owner(lvl,tile_y,tile_x,dk2_owner2DK1owner(tile->owner));
            
        user_set_slab(lvl,tile_y,tile_x,dk2_slab2DK1Slab(tile->slab));
        printf("%*d ",2,tile->owner);
        //printf("%c ",dk2_slab2char(tile->slab));
    }
    printf("\n");
  }





  // Writing the map on same file name
  short result=user_save_map(lvl,0);
  if (result!=ERR_NONE)
  {
    printf("cannot save map\n");
    printf("example1 finished with error\n");
    system("pause");	

    // The following two commands should be used to free memory
    // allocated for level
    level_free(lvl);
    level_deinit(&lvl);

    // This command should be always last function used from library
    free_messages();
    return 1;
  }
  printf("map \"%s\" saved\n", get_lvl_savfname(lvl));
  printf("example1 finished successfully\n");
  system("pause");	

  // The following two commands should be used to free memory
  // allocated for level
  level_free(lvl);
  level_deinit(&lvl);

  // This command should be always last function used from library
  free_messages();
  return 0;




}



/*
 * Prints all the level information.
 */
short dk2m_print_level(const struct DK2_Level *lvl)
{
  short result=ERR_NONE;
  if (result==ERR_NONE)
      result=dk2m_print_lvlkwd(lvl);
  if (result==ERR_NONE)
      result=dk2m_print_lvlmap(lvl);
  return result;
}

short dk2m_read_timestamp(struct DK2_TimeStamp *tmstamp,
    const struct DK2M_Chunk *chunk,unsigned long *offs,short flags)
{
    tmstamp->year1=read_int16_le_buf(chunk->data+(*offs));
    (*offs)+=2;
    tmstamp->day1=read_int8_buf(chunk->data+(*offs));
    (*offs)++;
    tmstamp->month1=read_int8_buf(chunk->data+(*offs));
    (*offs)++;
    tmstamp->u2=read_int16_le_buf(chunk->data+(*offs));
    (*offs)+=2;
    tmstamp->hour1=read_int8_buf(chunk->data+(*offs));
    (*offs)++;
    tmstamp->min1=read_int8_buf(chunk->data+(*offs));
    (*offs)++;
    tmstamp->sec1=read_int8_buf(chunk->data+(*offs));
    (*offs)++;
    tmstamp->u3=read_int8_buf(chunk->data+(*offs));
    (*offs)++;
    tmstamp->year2=read_int16_le_buf(chunk->data+(*offs));
    (*offs)+=2;
    tmstamp->day2=read_int8_buf(chunk->data+(*offs));
    (*offs)++;
    tmstamp->month2=read_int8_buf(chunk->data+(*offs));
    (*offs)++;
    tmstamp->u4=read_int16_le_buf(chunk->data+(*offs));
    (*offs)+=2;
    tmstamp->hour2=read_int8_buf(chunk->data+(*offs));
    (*offs)++;
    tmstamp->min2=read_int8_buf(chunk->data+(*offs));
    (*offs)++;
    tmstamp->sec2=read_int8_buf(chunk->data+(*offs));
    (*offs)++;
    tmstamp->u5=read_int8_buf(chunk->data+(*offs));
    (*offs)++;
    return ERR_NONE;
}

short dk2m_read_kwd_filehead(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  unsigned long offs;
  //dk2m_print_chunk(chunk);
  if (chunk->dsize<SIZEOF_DK2M_KWDFileHead)
      return -1;
  offs=0;
  dk2m_set_filepaths_count(lvl,read_int16_le_buf(chunk->data+offs),flags);
  offs += 2;
  dk2m_set_unkarr01_count(lvl,read_int16_le_buf(chunk->data+offs),flags);
  offs += 2;
  lvl->u1=read_int32_le_buf(chunk->data+offs);
  offs+=4;
  dk2m_read_timestamp(&(lvl->tmstamp),chunk,&offs,flags);
  if (chunk->dsize!=SIZEOF_DK2M_KWDFileHead)
      return -1;
  return ERR_NONE;
}

short dk2m_read_kwd_filepaths(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  unsigned int i,k;
  unsigned long offs;
  offs=0;
  for (k=0;k<lvl->filepaths_count;k++)
  {
      struct DK2_FilePath *itm=dk2m_make_filepaths_item_at(lvl,k,flags);
      if (itm==NULL) return -1;
      if (chunk->dsize < offs+SIZEOF_DK2M_FilePath)
          return -1;
      itm->id=read_int32_le_buf(chunk->data+offs+0);
      itm->u2=read_int32_le_buf(chunk->data+offs+4);
      for (i=0;i<SIZEOF_DK2_FilePathStr;i++)
          itm->str[i]=read_int8_buf(chunk->data+offs+8+i);
      itm->str[SIZEOF_DK2_FilePathStr]='\0';
      offs+=SIZEOF_DK2M_FilePath;
  }
  if (chunk->dsize < offs+(lvl->unkarr01_count<<1))
      return -1;
  for (k=0;k<lvl->unkarr01_count;k++)
  {
      lvl->unkarr01[k]=read_int32_le_buf(chunk->data+offs+(k<<1));
  }
  offs+=(lvl->unkarr01_count<<1);
  return ERR_NONE;
}

short dk2m_read_kwd_lvlprops(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  if (chunk->dsize<SIZEOF_DK2M_KWDLevProps)
      return -1;
  unsigned int i,k;
  unsigned long offs;
  offs=0;
  for (i=0;i<SIZEOF_DK2_ShortStr;i++)
      lvl->props.name[i]=read_int16_le_buf(chunk->data+offs+(i<<1));
  lvl->props.name[SIZEOF_DK2_ShortStr]=0;
  offs += (SIZEOF_DK2_ShortStr<<1);
  for (i=0;i<SIZEOF_DK2_LongStr;i++)
      lvl->props.desc[i]=read_int16_le_buf(chunk->data+offs+(i<<1));
  lvl->props.desc[SIZEOF_DK2_LongStr]=0;
  offs += (SIZEOF_DK2_LongStr<<1);
  for (i=0;i<SIZEOF_DK2_ShortStr;i++)
      lvl->props.author[i]=read_int16_le_buf(chunk->data+offs+(i<<1));
  lvl->props.author[SIZEOF_DK2_ShortStr]=0;
  offs += (SIZEOF_DK2_ShortStr<<1);
  for (i=0;i<SIZEOF_DK2_ShortStr;i++)
      lvl->props.email[i]=read_int16_le_buf(chunk->data+offs+(i<<1));
  lvl->props.email[SIZEOF_DK2_ShortStr]=0;
  offs += (SIZEOF_DK2_ShortStr<<1);
  for (i=0;i<SIZEOF_DK2_LongStr;i++)
      lvl->props.info[i]=read_int16_le_buf(chunk->data+offs+(i<<1));
  lvl->props.info[SIZEOF_DK2_LongStr]=0;
  offs += (SIZEOF_DK2_LongStr<<1);
  lvl->props.u3=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  lvl->props.ticks_per_sec=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  //TODO - read this properly!
  memcpy(lvl->props.randblock,chunk->data+offs,SIZEOF_DK2_KWDRndBlock);
  offs += SIZEOF_DK2_KWDRndBlock;
  for (k=0;k<DK2_MsgStr_count;k++)
  {
      for (i=0;i<SIZEOF_DK2_MsgStr;i++)
          lvl->props.msgs[k][i]=read_int16_le_buf(chunk->data+offs+(i<<1));
      offs += (SIZEOF_DK2_MsgStr<<1);
  }
  lvl->props.lvflags=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  for (i=0;i<SIZEOF_DK2_TinyStr;i++)
      lvl->props.speechstr[i]=read_int8_buf(chunk->data+offs+i);
  lvl->props.speechstr[SIZEOF_DK2_TinyStr]='\0';
  offs += SIZEOF_DK2_TinyStr;
  lvl->props.talisman_pieces=read_int8_buf(chunk->data+offs);
  offs ++;
  for (i=0;i<4;i++)
      lvl->props.reward_prev[i]=read_int8_buf(chunk->data+offs+i);
  offs += 4;
  for (i=0;i<4;i++)
      lvl->props.reward_next[i]=read_int8_buf(chunk->data+offs+i);
  offs += 4;
  lvl->props.sound_track=read_int8_buf(chunk->data+offs);
  offs ++;
  lvl->props.text_table_id=read_int8_buf(chunk->data+offs);
  offs ++;
  lvl->props.text_title_id=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  lvl->props.text_plot_id=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  lvl->props.text_debrief_id=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  lvl->props.text_objectv_id=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  lvl->props.u10=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  for (i=0;i<DK2_MsgObjctv_count;i++)
      lvl->props.text_subobjctv_id[i]=read_int16_le_buf(chunk->data+offs+(i<<1));
  offs += (DK2_MsgObjctv_count<<1);
  lvl->props.speclvl_idx=read_int8_buf(chunk->data+offs);
  offs ++;
  lvl->props.u11=read_int8_buf(chunk->data+offs);
  offs ++;
  for (i=0;i<DK2_MsgOverrd_count;i++)
      lvl->props.text_intrdc_overrd_obj[i]=read_int8_buf(chunk->data+offs+i);
  offs += DK2_MsgOverrd_count;
  for (i=0;i<DK2_MsgOverrd_count;i++)
      lvl->props.text_intrdc_overrd_id[i]=read_int16_le_buf(chunk->data+offs+(i<<1));
  offs += (DK2_MsgOverrd_count<<1);
  for (i=0;i<SIZEOF_DK2_TinyStr;i++)
      lvl->props.terrain_path[i]=read_int8_buf(chunk->data+offs+i);
  lvl->props.terrain_path[SIZEOF_DK2_TinyStr]='\0';
  offs += SIZEOF_DK2_TinyStr;
  lvl->props.one_shot_horny_lev=read_int8_buf(chunk->data+offs);
  offs ++;
  lvl->props.u20=read_int8_buf(chunk->data+offs);
  offs ++;
  lvl->props.reward_prev[4]=read_int8_buf(chunk->data+offs);
  offs ++;
  lvl->props.reward_next[4]=read_int8_buf(chunk->data+offs);
  offs ++;
  lvl->props.speech_horny_id=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  lvl->props.speech_prelvl_id=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  lvl->props.speech_postlvl_win=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  lvl->props.speech_postlvl_lost=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  lvl->props.speech_postlvl_news=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  lvl->props.speech_prelvl_genr=read_int16_le_buf(chunk->data+offs);
  offs += 2;
  for (i=0;i<SIZEOF_DK2_TinyStr;i++)
      lvl->props.hero_name[i]=read_int16_le_buf(chunk->data+offs+(i<<1));
  lvl->props.hero_name[SIZEOF_DK2_TinyStr]=0;
  offs += (SIZEOF_DK2_TinyStr<<1);
  //printf("Finishing read at chunk pos %d, chunk size %d\n",offs,chunk->dsize);
  if (chunk->dsize!=SIZEOF_DK2M_KWDLevProps)
      return -1;
  return ERR_NONE;
}

short dk2m_read_kwdchunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  switch (chunk->id)
  {
  case CTKWD_FILEHEAD:
      return dk2m_read_kwd_filehead(lvl,chunk,flags);
  case CTKWD_FILEPATHS:
      return dk2m_read_kwd_filepaths(lvl,chunk,flags);
  case CTKWD_LEVPROPS:
      return dk2m_read_kwd_lvlprops(lvl,chunk,flags);
  case CTKWD_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown KWD chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_free_map_dimensions(struct DK2_Level *lvl,short flags)
{
  if (lvl==NULL) return -1;
  int i;
  if ((lvl->tiles==NULL)&&(lvl->height>0)) return -1;
  for (i=lvl->height-1;i>=0;i--)
    free(lvl->tiles[i]);
  free(lvl->tiles);
  lvl->tiles=NULL;
  lvl->width=0;
  lvl->height=0;
  return ERR_NONE;
}

short dk2m_alloc_map_dimensions(struct DK2_Level *lvl,short flags)
{
  if (lvl==NULL) return -1;
  int i;
  lvl->tiles=malloc(lvl->height*sizeof(struct DK2_LvlTile *));
  if (lvl->tiles==NULL)
    return -1;
  for (i=0;i<lvl->height;i++)
  {
    lvl->tiles[i]=malloc(lvl->width*sizeof(struct DK2_LvlTile));
    if (lvl->tiles[i]==NULL)
    {
      free(lvl->tiles);
      lvl->tiles=NULL;
      return -1;
    }
  }
  return ERR_NONE;
}

short dk2m_set_map_dimensions(struct DK2_Level *lvl,unsigned int w,unsigned int h,short flags)
{
  dk2m_free_map_dimensions(lvl,flags);
  lvl->width=w;
  lvl->height=h;
  return dk2m_alloc_map_dimensions(lvl,flags);
}

short dk2m_read_map_dimensions(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  unsigned long offs;
  //dk2m_print_chunk(chunk);
  if (chunk->dsize<SIZEOF_DK2M_MAPDimnsn)
      return -1;
  offs=0;
  unsigned int w,h;
  w=read_int32_le_buf(chunk->data+offs);
  offs+=4;
  h=read_int32_le_buf(chunk->data+offs);
  offs+=4;
  short result;
  result=dk2m_set_map_dimensions(lvl,w,h,flags);
  if (result!=ERR_NONE)
      return result;
  if (chunk->dsize!=SIZEOF_DK2M_MAPDimnsn)
      return -1;
  return ERR_NONE;
}

short dk2m_read_kwd_lvltiles(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  unsigned long offs;
  unsigned long total_tiles;
  total_tiles = lvl->width*lvl->height;
  //dk2m_print_chunk(chunk);
  if (chunk->dsize < (total_tiles<<2))
      return -1;
  offs=0;
  struct DK2_LvlTile *tile;
  int i,k;
  for (k=0;k<lvl->height;k++)
    for (i=0;i<lvl->width;i++)
    {
        tile=&(lvl->tiles[k][i]);
        tile->slab=read_int8_buf(chunk->data+offs+0);
        tile->owner=read_int8_buf(chunk->data+offs+1);
        tile->wlb=read_int8_buf(chunk->data+offs+2);
        tile->u1=read_int8_buf(chunk->data+offs+3);
        offs+=4;
    }
  if (chunk->dsize != (total_tiles<<2))
      return -1;
  return ERR_NONE;
}

short dk2m_read_mapchunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  switch (chunk->id)
  {
  case CTMAP_LEVDIMENS:
      return dk2m_read_map_dimensions(lvl,chunk,flags);
  case CTMAP_LEVTILES:
      return dk2m_read_kwd_lvltiles(lvl,chunk,flags);
  case CTMAP_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown MAP chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_read_thingschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
  case CTTNG_FILEHEAD:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
  case CTTNG_LEVTHINGS:
      return ERR_NONE;
  case CTTNG_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown THINGS chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_read_doorschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
  case CTDOR_FILEHEAD:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
  case CTDOR_DOORSDATA:
      return ERR_NONE;
  case CTDOR_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown DOORS chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_read_trapschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
  case CTTRP_FILEHEAD:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
  case CTTRP_TRAPSDATA:
      return ERR_NONE;
  case CTTRP_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown TRAPS chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_read_creatureschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
  case CTCTR_FILEHEAD:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
  case CTCTR_CRTRSDATA:
      return ERR_NONE;
  case CTCTR_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown CREATURES chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_read_roomschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
  case CTROM_FILEHEAD:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
  case CTROM_ROOMSDATA:
      return ERR_NONE;
  case CTROM_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown ROOMS chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_read_playerschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
  case CTPLR_FILEHEAD:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
  case CTPLR_PLYRSDATA:
      return ERR_NONE;
  case CTPLR_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown PLAYERS chunk type %04x",chunk->id);
      return -1;
  }
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

short dk2m_read_triggerschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
  case CTTRG_TRGRSDATA:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
//TODO: make cases for all chunks
  case CTTRG_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      return ERR_NONE; //no error message - not all chunks supported
      dk2m_ferror("Unknown TRIGGERS chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_read_shotschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
  case CTSHT_FILEHEAD:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
  case CTSHT_SHOTSDATA:
      return ERR_NONE;
  case CTSHT_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown SHOTS chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_read_objectschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
  case CTOBJ_FILEHEAD:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
  case CTOBJ_OBJCTDATA:
      return ERR_NONE;
  case CTOBJ_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown OBJECTS chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_read_ctspellschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
  case CTCSP_FILEHEAD:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
  case CTCSP_CTSPLDATA:
      return ERR_NONE;
  case CTCSP_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown CREATURESPELLS chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_read_kpspellschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
  case CTKSP_FILEHEAD:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
  case CTKSP_KPSPLDATA:
      return ERR_NONE;
  case CTKSP_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown KEEPERSPELLS chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_read_globalschunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
/*
//TODO: make cases for all chunks
  case CTxxx_:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
  case CTxxx_:
      return ERR_NONE;
  case CTxxx_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
*/
  default:
      return ERR_NONE; //no error message - not all chunks supported
      dk2m_ferror("Unknown GLOBALS chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_read_terrainchunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
  //dk2m_print_chunk(chunk);
  switch (chunk->id)
  {
  case CTTRR_FILEHEAD:
      return ERR_NONE;
//      return dk2m_read_xxx(lvl,chunk,flags);
  case CTTRR_TERRNDATA:
      return ERR_NONE;
  case CTTRR_FILESIZE:
      // Skip the file size chunk - we don't need it
      return ERR_NONE;
  default:
      dk2m_ferror("Unknown TERRAIN chunk type %04x",chunk->id);
      return -1;
  }
}

short dk2m_read_anychunk(struct DK2_Level *lvl,const struct DK2M_Chunk *chunk,short flags)
{
    dk2m_print_chunk(chunk);
}

short dk2m_free_chunk_data(struct DK2M_Chunk *chunk)
{
  if (chunk==NULL) return -1;
  free(chunk->data);
  chunk->id=0;
  chunk->dsize=0;
  chunk->data=NULL;
  return ERR_NONE;
}

short dk2m_lvl_clear(struct DK2_Level *lvl,short flags)
{
  if (lvl==NULL)
  {
    if (flags&DK2MFLAG_VERBOSE)
      dk2m_error("Bad DK2_Level pointer for clearing");
    return -1;
  }
  memset(lvl,0,sizeof(struct DK2_Level));
  return ERR_NONE;
}

short dk2m_lvl_create(struct DK2_Level **lvl,short flags)
{
  (*lvl)=malloc(sizeof(struct DK2_Level));
  if ((*lvl)==NULL)
  {
    if (flags&DK2MFLAG_VERBOSE)
      dk2m_error("Cannot allocate DK2_Level memory");
    return -1;
  }
  return dk2m_lvl_clear(*lvl,flags);
}

short dk2m_lvl_free(struct DK2_Level **lvl,short flags)
{
  if ((lvl==NULL)||((*lvl)==NULL))
  {
    if (flags&DK2MFLAG_VERBOSE)
      dk2m_error("Cannot free DK2_Level");
    return -1;
  }
  free(*lvl);
  (*lvl)=NULL;
  return ERR_NONE;
}

/*
 * Loads any chunked file from given filename.
 */
short dk2m_read_chunkedfile(struct DK2_Level *lvl,const char *fname,dk2m_read_chunk read_chunk,short flags)
{
  struct DK2M_Chunk chunk;
  FILE *fp;
  fp=fopen(fname,"rb");
  if (fp==NULL)
  {
    if (flags&DK2MFLAG_VERBOSE)
      dk2m_ferror("%s - %s",strerror(errno),fname);
    return -1;
  }
  unsigned long offset;
  long nread;
  offset=0;
  while (!feof(fp))
  {
    nread=dk2m_fread_chunk(&chunk,fp,offset,flags);
    if (nread < SIZEOF_DK2M_Chunk_Header)
    {
      fclose(fp);
      if (nread==0)
        return ERR_NONE;
      return -1;
    }
    read_chunk(lvl,&chunk,flags);
    dk2m_free_chunk_data(&chunk);
    offset+=nread;
  }
  fclose(fp);
  return ERR_NONE;
}

/*
 * Loads the whole DK2 map.
 */
short dk2m_read_mapfiles(struct DK2_Level *lvl,const char *name,short flags)
{
  char *fname;
  short result;
  result=ERR_NONE;
  fname=malloc(strlen(name)+SIZEOF_DK2_FilePathStr);
  if (fname==NULL)
  {
      if (flags&DK2MFLAG_VERBOSE)
        dk2m_ferror("Can't allocate memory to store file name");
      return -1;
  }
  if (result==ERR_NONE)
  {
      sprintf(fname,"%s.kwd",name);
      result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_kwdchunk,flags);
  }
  int i;
  int currpath_len = filename_from_path2(name)-name;
  if ((currpath_len<0)||(currpath_len>strlen(name)))
    currpath_len=0;
  for (i=0;i<lvl->filepaths_count;i++)
  {
      if (result!=ERR_NONE) break;
      // Preparing file name with correct path
      strncpy(fname,name,currpath_len);
      fname[currpath_len]=0;
      char *origname=lvl->filepaths[i]->str;
      char *onlyname=filename_from_path2(origname);
      unsigned int dirlevel=str_chrnum(origname,'\\')+str_chrnum(origname,'/');
      while (dirlevel<3)
      {
          path_set_to_higher_level(fname);
          dirlevel++;
      }
      strcat(fname,onlyname);
      if (strchr(onlyname,'.')==NULL)
          strcat(fname,".kwd");
      printf("Prepared file name \"%s\"\n",fname);
      switch (lvl->filepaths[i]->id)
      {
      case FTKWD_GLOBALS:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_globalschunk,flags);
          break;
      case FTKWD_MAP:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_mapchunk,flags);
          break;
      case FTKWD_TERRAIN:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_terrainchunk,flags);
          break;
      case FTKWD_ROOMS:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_roomschunk,flags);
          break;
      case FTKWD_TRAPS:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_trapschunk,flags);
          break;
      case FTKWD_DOORS:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_doorschunk,flags);
          break;
      case FTKWD_KEEPRPELLS:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_kpspellschunk,flags);
          break;
      case FTKWD_CRTRSPELLS:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_ctspellschunk,flags);
          break;
      case FTKWD_CREATURES:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_creatureschunk,flags);
          break;
      case FTKWD_PLAYERS:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_playerschunk,flags);
          break;
      case FTKWD_THINGS:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_thingschunk,flags);
          break;
      case FTKWD_TRIGGERS:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_triggerschunk,flags);
          break;
      case FTKWD_VARIABLES:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_variableschunk,flags);
          break;
      case FTKWD_OBJECTS:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_objectschunk,flags);
          break;
      case FTKWD_SHOTS:
          result=dk2m_read_chunkedfile(lvl,fname,dk2m_read_shotschunk,flags);
          break;
      default:
          if (flags&DK2MFLAG_VERBOSE)
            dk2m_ferror("Unidentified file type %04x for \"%s\"",lvl->filepaths[i]->id,fname);
          result=-1;
          break;
      }
  }
  free(fname);
  return result;
}
