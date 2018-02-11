#include "RTT_2.h"
#include "doit_all.h"
#include <vector>
#include "RTT_patest_wire.h"
#include "RTTGDIXCB.h"
#include "OS_interface.h"
#include "main.h"
#include "bitmaps/cde_backdrops/ankh.bm"
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <xcb/xcb_keysyms.h>
#include "RTT_keysyms.h"

static keysym_to_ASCII keysym_translation_table[]={
  {0x00000020,' '},
  {0x00000021,'!'},
  {0x00000022,'\"'},
  {0x00000023,'#'},
  {0x00000024,'$'},
  {0x00000025,'%'},
  {0x00000026,'&'},
  {0x00000027,'\''},
  {0x00000028,'{'},
  {0x00000029,'}'},
  {0x0000002a,'*'},
  {0x0000002b,'+'},
  {0x0000002c,','},
  {0x0000002d,'-'},
  {0x0000002e,'.'},
  {0x0000002f,'/'},
  {0x00000030,'0'},
  {0x00000031,'1'},
  {0x00000032,'2'},
  {0x00000033,'3'},
  {0x00000034,'4'},
  {0x00000035,'5'},
  {0x00000036,'6'},
  {0x00000037,'7'},
  {0x00000038,'8'},
  {0x00000039,'9'},
  {0x0000003a,':'},
  {0x0000003b,';'},
  {0x0000003c,'<'},
  {0x0000003d,'='},
  {0x0000003e,'>'},
  {0x0000003f,'?'},
  {0x00000040,'@'},
  {0x00000041,'A'},
  {0x00000042,'B'},
  {0x00000043,'C'},
  {0x00000044,'D'},
  {0x00000045,'E'},
  {0x00000046,'F'},
  {0x00000047,'G'},
  {0x00000048,'H'},
  {0x00000049,'I'},
  {0x0000004a,'J'},
  {0x0000004b,'K'},
  {0x0000004c,'L'},
  {0x0000004d,'M'},
  {0x0000004e,'N'},
  {0x0000004f,'O'},
  {0x00000050,'P'},
  {0x00000051,'Q'},
  {0x00000052,'R'},
  {0x00000053,'S'},
  {0x00000054,'T'},
  {0x00000055,'U'},
  {0x00000056,'V'},
  {0x00000057,'W'},
  {0x00000058,'X'},
  {0x00000059,'Y'},
  {0x0000005a,'Z'},
  {0x0000005b,'['},
  {0x0000005c,'\\'},
  {0x0000005d,']'},
  {0x0000005e,'~'},
  {0x0000005f,'_'},
  {0x00000060,'^'},
  {0x00000061,'a'},
  {0x00000062,'b'},
  {0x00000063,'c'},
  {0x00000064,'d'},
  {0x00000065,'e'},
  {0x00000066,'f'},
  {0x00000067,'g'},
  {0x00000068,'h'},
  {0x00000069,'i'},
  {0x0000006a,'j'},
  {0x0000006b,'k'},
  {0x0000006c,'l'},
  {0x0000006d,'m'},
  {0x0000006e,'n'},
  {0x0000006f,'o'},
  {0x00000070,'p'},
  {0x00000071,'q'},
  {0x00000072,'r'},
  {0x00000073,'s'},
  {0x00000074,'t'},
  {0x00000075,'u'},
  {0x00000076,'v'},
  {0x00000077,'w'},
  {0x00000078,'x'},
  {0x00000079,'y'},
  {0x0000007a,'z'},
  {0x0000007b,'{'},
  {0x0000007c,'|'},
  {0x0000007d,'}'},
  {0x0000007e,'~'},
  {0x0000ffaa,'*'},
  {0x0000ffab,'+'},
  {0x0000ffac,','},
  {0x0000ffad,'-'},
  {0x0000ffae,'.'},
  {0x0000ffaf,'/'},
  {0x0000ffb0,'0'},
  {0x0000ffb1,'1'},
  {0x0000ffb2,'2'},
  {0x0000ffb3,'3'},
  {0x0000ffb4,'4'},
  {0x0000ffb5,'5'},
  {0x0000ffb6,'6'},
  {0x0000ffb7,'7'},
  {0x0000ffb8,'8'},
  {0x0000ffb9,'9'}
};





bool keysym_to_RTT(xcb_keysym_t keysym,u8 &ASCII){
  // translate X keysym to ASCII if that is useful, otherwise leave as keysym
  // true means its a valid ASCII char
  // false means its a functional keysym
  int n=sizeof(keysym_translation_table)/sizeof(*keysym_translation_table);
  u32 u32keysym=(u32)keysym;
  for(u32 i=0;i<n;i++){
    if (u32keysym==keysym_translation_table[i].X_keysym){
      ASCII=keysym_translation_table[i].ASCII;
      return true;
    }
  }
  return false;
}

