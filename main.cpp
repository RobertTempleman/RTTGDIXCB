#include <stdio.h>
#include "OS_interface.h"
#include "RTT_text_edit_box.h"
#include "main.h"
#include "fxprocessor.h"

#define USE_MLSSA_WINDOWS_ONLY 0

#define ON_THE_LAPTOP 0

#if ON_THE_LAPTOP==1
  u32 main_window_x=20;
  u32 main_window_y=10;
  //u32 main_window_w=1920;
  ////u32 main_window_w=3840;
  //u32 main_window_h=1080;
  u32 main_window_w=1880;
  //u32 main_window_w=3840;
  u32 main_window_h=1000;
#else
  u32 main_window_x=30;
  u32 main_window_y=10;
  //u32 main_window_w=1920;
  ////u32 main_window_w=3840;
  //u32 main_window_h=1080;
  u32 main_window_w=1880;
  //u32 main_window_w=3840;
  u32 main_window_h=1000;
#endif

RTTXCB *bon,*talos,*del,*newj,*rot,*snd,*iss;
RTTXCB *knob1=0;

void paint(rtt_window&w){
  // the function to redraw the window contents into the windows internal buffer
  switch(w.id){
    default:
    case 0:w.blit_bitmap_to_workarea_only(iss);   break;
    case 1:w.blit_bitmap_to_workarea_only(snd);   break;
    case 2:w.blit_bitmap_to_workarea_only(talos); break;
    case 3:w.blit_bitmap_to_workarea_only(newj);  break;
    case 4:w.blit_bitmap_to_workarea_only(rot);   break;
    case 5:w.blit_bitmap_to_workarea_only(del);   break;
    case 6:w.blit_bitmap_to_workarea_only(bon);   break;
  }
}


void pane_click_function(rtt_pane &p,s32 butt,s32 x_click,s32 y_click){
  p.circle(x_click,y_click,10,goldenrod2_c);
}


void pane_mouse_move_function(rtt_pane &p,s32 x_move_to,s32 y_move_to){
  u32 r=(u32)((255LL*rand())/RAND_MAX);
  u32 g=(u32)((255LL*rand())/RAND_MAX);
  u32 b=(u32)((255LL*rand())/RAND_MAX);
  u32 col=r|(g<<8)|(b<<16);
  p.setpixel(x_move_to, y_move_to, col);
  p.do_a_refresh=true;
}


void pane_paint_function(rtt_pane &p){
  printf("pane paint function called with RTTXCB id=%d name=%s\n",p.id,p.name);
  p.line(10,10,100,100,goldenrod_c);
  p.circle(100,100,20,darkred_c);
  for(int i=0;i<100;i+=4){
    s32 arclen=90;
    s32 x=123;
    s32 y=10;
    s32 w=40+(3*i)/2;
    s32 h=40+i*2;
    p.ellipse(x+10 ,y    ,w ,h ,darkmagenta_c ,0      ,arclen<<6);
    p.ellipse(x+10 ,y+10 ,w ,h ,darkkhaki_c   ,270<<6 ,arclen<<6);
    p.ellipse(x    ,y+10 ,w ,h ,darkblue_c    ,180<<6 ,arclen<<6);
    p.ellipse(x    ,y    ,w ,h ,darkcyan_c    ,90<<6  ,arclen<<6);
  }
}


void knob_paint_function(rtt_pane &p){
  //  printf("pane paint function called with RTTXCB id=%d name=%s\n",p.id,p.name);
  //  if (p.extra_bitmap){
  //    RTTXCB &knob=*p.extra_bitmap;
  //    p.blit_bitmap_to_self(knob,0,0,knob.w,knob.w, 0, 0,false);
  //  }
  //  p.line(10,10,100,100,goldenrod_c);
  //  p.circle(100,100,20,darkred_c);
}


#define MYBUT(button_index) [](RTTXCB &rttxcb){handle_button_press(button_index);}

void clicked(RTTXCB &rttxcb){
  cout<<"clicked from "<<rttxcb.name<<endl;
}


unique_ptr<fxprocessor> fxp;
    
void RTT_init(){
  vector<string> entries,entries0,entries2;
  char rtt[64];
  for(int i=0;i<140;i++){
    sprintf(rtt,"entry %d",i);
    string srtt2(rtt);
    entries.push_back(srtt2);
    if (i<40) entries2.push_back(rtt);
  }
  
  wm.set_main_window_geometry(main_window_x,main_window_y,main_window_w,main_window_h,false);
  wm.set_backdrop(wm.load_bitmap("bitmaps/backdrops/newj1920.bmp"));

  rot=wm.load_bitmap("bitmaps/backdrops/rot.bmp");
  rtt_window &MLS   =wm.new_window(4 ,"MLS window"        ,70   ,800 ,450 ,200 ,darkkhaki_cr ,0 ,paint ,WINDOW_DISPLAY_TYPE_NORMAL_CLIENT           ,rot->w   ,rot->h  );
  MLS.add_button("Generate MLS sequence"            ,"MLS button 1" ,MYBUT(INDEX_START_BUTTON)                   ,20  ,24  ,150 ,20);
  MLS.add_button("Initialize audio"                 ,"MLS button 2" ,MYBUT(INDEX_SINGLE_PLAY_BUTTON)             ,210 ,24  ,200 ,20);
  MLS.add_button("Shutdown audio"                   ,"MLS button 3" ,MYBUT(INDEX_SHUTDOWN_AUDIO_BUTTON)          ,210 ,44  ,200 ,20);
  MLS.add_button("Toggle MLS playback"              ,"MLS button 4" ,MYBUT(INDEX_STOP_MLS_PLAYBACK)              ,210 ,64  ,200 ,20);
  MLS.add_button("Determine system audio latency"   ,"MLS button 5" ,MYBUT(INDEX_DETERMINE_SYSTEM_AUDIO_LATENCY) ,210 ,84  ,200 ,20);
  MLS.add_button("Start single MLS sample run"      ,"MLS button 6" ,MYBUT(INDEX_SINGLE_MLS_SAMPLE_RUN)          ,210 ,104 ,200 ,20);
  MLS.add_button("Start multiple MLS sample runs"   ,"MLS button 7" ,MYBUT(INDEX_MULTIPLE_MLS_SAMPLE_RUN)        ,210 ,124 ,200 ,20);


  if (USE_MLSSA_WINDOWS_ONLY==0){
    iss=  wm.load_bitmap("bitmaps/backdrops/iss.bmp"   );
    snd=  wm.load_bitmap("bitmaps/backdrops/snd.bmp"   );
    newj= wm.load_bitmap("bitmaps/backdrops/newj.bmp"  );
    del=  wm.load_bitmap("bitmaps/backdrops/iss.bmp"   );
    talos=wm.load_bitmap("bitmaps/backdrops/iss.bmp"   );
    bon=  wm.load_bitmap("bitmaps/backdrops/bweeon.bmp");
  
                       wm.new_window(0 ,"RTT window manager",200  ,400 ,300 ,500 ,black_cr     ,0 ,paint ,WINDOW_DISPLAY_TYPE_NORMAL_CLIENT           ,iss->w   ,iss->h  );
    rtt_window &nobwin=wm.new_window(1 ,"RTT window 2"      ,600  ,500 ,1050 ,480 ,grey20_cr    ,0 ,paint ,WINDOW_DISPLAY_TYPE_NORMAL_CLIENT           ,snd->w   ,snd->h  );
    rtt_window &fxpwin=wm.new_window(2 ,"RTT window 3"      ,1200 ,100 ,250 ,450 ,0x200808     ,0 ,paint ,WINDOW_DISPLAY_TYPE_NORMAL_CLIENT           ,talos->w ,talos->h);
                       wm.new_window(3 ,"RTT window 4"      ,50   ,100 ,550 ,300 ,0x090828     ,0 ,paint ,WINDOW_DISPLAY_TYPE_NORMAL_CLIENT           ,newj->w  ,newj->h );
                       wm.new_window(5 ,"RTT window 6"      ,350  ,1000 ,350 ,500 ,darkkhaki_cr ,0 ,paint ,WINDOW_DISPLAY_TYPE_NORMAL_CLIENT           ,del->w   ,del->h  );
    rtt_window &win6  =wm.new_window(6 ,"RTT bouillon"      ,550  ,1000 ,700 ,480 ,darkkhaki_cr ,0 ,paint ,WINDOW_DISPLAY_TYPE_NORMAL_CLIENT           ,bon->w   ,bon->h  );
    rtt_window &combo =wm.new_window(7 ,"combo0"            ,150  ,800 ,150 ,80  ,darkkhaki_cr ,0 ,paint ,WINDOW_DISPLAY_TYPE_COMBO_BOX               ,del->w   ,del->h  );
    rtt_window &combo2=wm.new_window(8 ,"combo1"            ,1450 ,800 ,150 ,480 ,grey20_cr    ,0 ,paint ,WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST ,del->w   ,del->h  );
    combo.add_combo_items(entries);
    combo2.add_combo_items(entries);

    win6.add_text_edit_box("tb1"                                                            ,"editbox1"   ,10, 10, 50,  20, entries  ,0);
    win6.add_text_edit_box("window(1,\"RTT window 2\",50,300,250,150,grey20_cr,0,paint,WIN" ,"editbox2.5" ,10, 32, 100, 20, entries0 ,0);
    win6.add_text_edit_box("text_box3 back(new RTT_text_edit_box(win6,\"text_box3 back"     ,"editbox3"   ,10, 54, 300, 20, entries  ,0);
    win6.add_multiple_select_box(350,10,90,600,entries2);
    win6.add_multiple_select_box(450,10,90,400,entries);
    win6.add_multiple_select_box(550,60,90,600,entries);
    win6.add_button("RTT  is kuhl1","button1",clicked,100,300,140,20);
    win6.add_button("RTT  is kuhl2","button2",clicked,100,324,100,20);
    win6.add_button("RTT  is kuhl3","button3",clicked,100,348,50,20);
    win6.add_pane("pane1", pane_paint_function, pane_click_function, pane_mouse_move_function, 10 ,80, 320, 256);

    fxp=unique_ptr<fxprocessor>(new fxprocessor(nobwin,10,10));
    //    fxp=unique_ptr<fxprocessor>(new fxprocessor(fxpwin,10,10));
  }
}


void RTT_update(){
}


void RTT_key(int dir,int key){
}
