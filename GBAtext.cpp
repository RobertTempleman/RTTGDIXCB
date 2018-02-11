#include "OS_interface.h"

typedef unsigned int u32;
typedef int i32;
typedef unsigned char u8;

static u8 colour_index[96]={
  0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
  0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
  2  , 2  , 2  , 2  , 2  , 2  , 2  , 2  ,
  2  , 2  , 0  , 0  , 0  , 0  , 0  , 0  ,
  0  , 1  , 1  , 1  , 1  , 1  , 1  , 1  ,
  1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  ,
  1  , 1  , 1  , 1  , 1  , 1  , 1  , 1  ,
  1  , 1  , 1  , 0  , 0  , 0  , 0  , 1  ,
  0  , 1  , 1  , 1  , 1  , 1  , 1  , 1+4,
  1  , 1  , 1+4, 1  , 1  , 1  , 1  , 1  ,
  1+4, 1+4, 1  , 1  , 1  , 1  , 1  , 1  ,
  1  , 1+4, 1  , 0  , 0  , 0  , 0  , 0  
};


static u8 debugfont[1440]={
  0 , 0 , 0 ,  0 ,255, 0 , 255, 0 ,255, 255, 0 ,255, 255,255,255, 255, 0 ,255,  0 ,255, 0 ,  0 , 0 ,255,
  0 , 0 , 0 ,  0 ,255, 0 , 255, 0 ,255, 255,255,255, 255,255, 0 ,  0 , 0 ,255, 255, 0 ,255,  0 ,255, 0 ,
  0 , 0 , 0 ,  0 ,255, 0 ,  0 , 0 , 0 , 255, 0 ,255, 255,255,255,  0 ,255, 0 ,  0 ,255, 0 ,  0 , 0 , 0 ,
  0 , 0 , 0 ,  0 , 0 , 0 ,  0 , 0 , 0 , 255,255,255,  0 ,255,255, 255, 0 , 0 , 255, 0 ,255,  0 , 0 , 0 ,
  0 , 0 , 0 ,  0 ,255, 0 ,  0 , 0 , 0 , 255, 0 ,255, 255,255,255, 255, 0 ,255,  0 ,255,255,  0 , 0 , 0 ,

  0 , 0 ,255,  0 ,255, 0 , 255, 0 ,255,  0 , 0 , 0 ,  0 , 0 , 0 ,  0 , 0 , 0 ,  0 , 0 , 0 ,  0 , 0 ,255,
  0 ,255, 0 ,  0 , 0 ,255,  0 ,255, 0 ,  0 ,255, 0 ,  0 , 0 , 0 ,  0 , 0 , 0 ,  0 , 0 , 0 ,  0 , 0 ,255,
  0 ,255, 0 ,  0 , 0 ,255, 255,255,255, 255,255,255,  0 , 0 , 0 ,  0 ,255,255,  0 , 0 , 0 ,  0 ,255, 0 ,
  0 ,255, 0 ,  0 , 0 ,255,  0 ,255, 0 ,  0 ,255, 0 ,  0 , 0 ,255,  0 , 0 , 0 ,  0 , 0 , 0 , 255, 0 , 0 ,
  0 , 0 ,255,  0 ,255, 0 , 255, 0 ,255,  0 , 0 , 0 ,  0 ,255, 0 ,  0 , 0 , 0 ,  0 ,255, 0 , 255, 0 , 0 ,

  255,255,255,  0 ,255, 0 , 255,255,255, 255,255,255, 255, 0 ,255, 255,255,255, 255,255,255, 255,255,255,
  255, 0 ,255, 255,255, 0 ,  0 , 0 ,255,  0 , 0 ,255, 255, 0 ,255, 255, 0 , 0 , 255, 0 , 0 ,  0 , 0 ,255,
  255, 0 ,255,  0 ,255, 0 , 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255,  0 , 0 ,255,
  255, 0 ,255,  0 ,255, 0 , 255, 0 , 0 ,  0 , 0 ,255,  0 , 0 ,255,  0 , 0 ,255, 255, 0 ,255,  0 , 0 ,255,
  255,255,255,  0 ,255, 0 , 255,255,255, 255,255,255,  0 , 0 ,255, 255,255,255, 255,255,255,  0 , 0 ,255,

  255,255,255, 255,255,255,  0 , 0 , 0 ,  0 , 0 , 0 ,  0 , 0 ,255,  0 , 0 , 0 , 255, 0 , 0 , 255,255, 0 ,
  255, 0 ,255, 255, 0 ,255,  0 ,255, 0 ,  0 , 0 ,255,  0 ,255, 0 , 255,255,255,  0 ,255, 0 ,  0 , 0 ,255,
  255,255,255, 255,255,255,  0 , 0 , 0 ,  0 , 0 , 0 , 255, 0 , 0 ,  0 , 0 , 0 ,  0 , 0 ,255,  0 ,255, 0 ,
  255, 0 ,255,  0 , 0 ,255,  0 ,255, 0 ,  0 , 0 ,255,  0 ,255, 0 , 255,255,255,  0 ,255, 0 ,  0 , 0 , 0 ,
  255,255,255, 255,255,255,  0 , 0 , 0 ,  0 ,255, 0 ,  0 , 0 ,255,  0 , 0 , 0 , 255, 0 , 0 ,  0 ,255, 0 ,

  255,255,255,  0 ,255, 0 , 255,255, 0 ,  0 ,255,255, 255,255, 0 , 255,255,255, 255,255,255,  0 ,255,255,
  255, 0 ,255, 255, 0 ,255, 255, 0 ,255, 255, 0 , 0 , 255, 0 ,255, 255, 0 , 0 , 255, 0 , 0 , 255, 0 , 0 ,
  255,255,255, 255,255,255, 255,255, 0 , 255, 0 , 0 , 255, 0 ,255, 255,255, 0 , 255,255, 0 , 255, 0 ,255,
  255, 0 , 0 , 255, 0 ,255, 255, 0 ,255, 255, 0 , 0 , 255, 0 ,255, 255, 0 , 0 , 255, 0 , 0 , 255, 0 ,255,
  255,255,255, 255, 0 ,255, 255,255, 0 ,  0 ,255,255, 255,255, 0 , 255,255,255, 255, 0 , 0 ,  0 ,255,255,

  255, 0 ,255, 255,255,255,  0 , 0 ,255, 255, 0 ,255, 255, 0 , 0 , 255, 0 ,255, 255, 0 ,255,  0 ,255, 0 ,
  255, 0 ,255,  0 ,255, 0 ,  0 , 0 ,255, 255, 0 ,255, 255, 0 , 0 , 255,255,255, 255,255,255, 255, 0 ,255,
  255,255,255,  0 ,255, 0 ,  0 , 0 ,255, 255,255, 0 , 255, 0 , 0 , 255,255,255, 255,255,255, 255, 0 ,255,
  255, 0 ,255,  0 ,255, 0 , 255, 0 ,255, 255, 0 ,255, 255, 0 , 0 , 255, 0 ,255, 255,255,255, 255, 0 ,255,
  255, 0 ,255, 255,255,255,  0 ,255, 0 , 255, 0 ,255, 255,255,255, 255, 0 ,255, 255, 0 ,255,  0 ,255, 0 ,

  255,255, 0 ,  0 ,255, 0 , 255,255, 0 ,  0 ,255,255, 255,255,255, 255, 0 ,255, 255, 0 ,255, 255, 0 ,255,
  255, 0 ,255, 255, 0 ,255, 255, 0 ,255, 255, 0 , 0 ,  0 ,255, 0 , 255, 0 ,255, 255, 0 ,255, 255, 0 ,255,
  255,255, 0 , 255, 0 ,255, 255,255, 0 ,  0 ,255, 0 ,  0 ,255, 0 , 255, 0 ,255, 255, 0 ,255, 255,255,255,
  255, 0 , 0 , 255,255, 0 , 255, 0 ,255,  0 , 0 ,255,  0 ,255, 0 , 255, 0 ,255, 255, 0 ,255, 255,255,255,
  255, 0 , 0 ,  0 ,255,255, 255, 0 ,255, 255,255, 0 ,  0 ,255, 0 , 255,255,255,  0 ,255, 0 , 255, 0 ,255,

  255, 0 ,255, 255, 0 ,255, 255,255,255,  0 ,255,255, 255, 0 , 0 ,  0 ,255,255,  0 ,255, 0 ,  0 , 0 , 0 ,
  255, 0 ,255, 255, 0 ,255,  0 , 0 ,255,  0 ,255, 0 , 255, 0 , 0 ,  0 , 0 ,255, 255, 0 ,255,  0 , 0 , 0 ,
   0 ,255, 0 ,  0 ,255, 0 ,  0 ,255, 0 ,  0 ,255, 0 ,  0 ,255, 0 ,  0 , 0 ,255,  0 , 0 , 0 ,  0 , 0 , 0 ,
  255, 0 ,255,  0 ,255, 0 , 255, 0 , 0 ,  0 ,255, 0 ,  0 , 0 ,255,  0 , 0 ,255,  0 , 0 , 0 ,  0 , 0 , 0 ,
  255, 0 ,255,  0 ,255, 0 , 255,255,255,  0 ,255,255,  0 , 0 ,255,  0 ,255,255,  0 , 0 , 0 , 255,255,255,

   0 , 0 , 0 ,  0 , 0 , 0 , 255, 0 , 0 ,  0 , 0 , 0 ,  0 , 0 ,255,  0 , 0 , 0 ,  0 ,255,255, 255,255,255, 
   0 , 0 , 0 ,  0 ,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255, 0 , 0 , 255, 0 ,255, 
   0 ,255,255, 255, 0 ,255, 255, 0 ,255, 255, 0 , 0 , 255, 0 ,255, 255, 0 ,255, 255,255,255, 255,255,255, 
   0 , 0 , 0 , 255, 0 ,255, 255, 0 ,255, 255, 0 , 0 , 255, 0 ,255, 255,255, 0 , 255, 0 , 0 ,  0 , 0 ,255, 
   0 , 0 , 0 ,  0 ,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255, 0 , 0 , 255,255, 0 , 
  
  255, 0 , 0 ,  0 , 0 , 0 ,  0 ,255, 0 , 255, 0 , 0 ,  0 ,255, 0 ,  0 , 0 , 0 ,  0 , 0 , 0 ,  0 , 0 , 0 , 
  255, 0 , 0 ,  0 ,255, 0 ,  0 , 0 , 0 , 255, 0 , 0 ,  0 ,255, 0 , 255, 0 ,255, 255,255, 0 , 255,255,255, 
  255,255,255,  0 , 0 , 0 ,  0 ,255, 0 , 255, 0 ,255,  0 ,255, 0 , 255,255,255, 255, 0 ,255, 255, 0 ,255, 
  255, 0 ,255,  0 ,255, 0 ,  0 ,255, 0 , 255,255, 0 ,  0 ,255, 0 , 255, 0 ,255, 255, 0 ,255, 255, 0 ,255, 
  255, 0 ,255,  0 ,255, 0 , 255,255, 0 , 255, 0 ,255,  0 ,255, 0 , 255, 0 ,255, 255, 0 ,255, 255,255,255, 
  
  255,255,255, 255,255,255,  0 , 0 , 0 ,  0 , 0 , 0 ,  255, 0 , 0 ,  0 , 0 , 0 ,  0 , 0 , 0 ,  0 , 0 , 0 , 
  255, 0 ,255, 255, 0 ,255,  0 ,255,255, 255,255,255,  255,255,255, 255, 0 ,255, 255, 0 ,255, 255, 0 ,255, 
  255,255,255, 255,255,255,  0 ,255, 0 , 255,255, 0 ,  255, 0 , 0 , 255, 0 ,255, 255, 0 ,255, 255, 0 ,255, 
  255, 0 , 0 ,  0 , 0 ,255,  0 ,255, 0 ,  0 , 0 ,255,  255, 0 , 0 , 255, 0 ,255, 255, 0 ,255, 255,255,255, 
  255, 0 , 0 ,  0 , 0 ,255,  0 ,255, 0 , 255,255,255,   0 ,255,255, 255,255,255,  0 ,255, 0 , 255, 0 ,255, 
  
   0 , 0 , 0 , 255, 0 ,255,  0 , 0 , 0 ,  0 ,255,255,  0 ,255, 0 , 255,255, 0 ,  0 , 0 , 0 , 255,255,255, 
  255, 0 ,255, 255, 0 ,255, 255,255,255,  0 ,255, 0 ,  0 ,255, 0 ,  0 ,255, 0 ,  0 , 0 , 0 ,  0 ,255, 0 , 
   0 ,255, 0 , 255,255,255,  0 , 0 ,255, 255,255, 0 ,  0 ,255, 0 ,  0 ,255,255, 255,255,255, 255,255,255, 
   0 ,255, 0 ,  0 , 0 ,255,  0 ,255, 0 ,  0 ,255, 0 ,  0 ,255, 0 ,  0 ,255, 0 ,  0 , 0 , 0 ,  0 ,255, 0 , 
  255, 0 ,255,  0 ,255,255, 255,255,255,  0 ,255,255,  0 ,255, 0 , 255,255, 0 ,  0 , 0 , 0 , 255,255,255, 
};


#define FONT_CHAR_WIDTH 3
#define FONT_CHAR_HEIGHT 5
#define CHARS_PER_LINE 8
#define POW_CHARS_PER_LINE 3
#define FONT_LINE_WIDTH (CHARS_PER_LINE*FONT_CHAR_WIDTH)


int last_width;
void print_colour(RTTXCB *rttxcb,int screen_x,int screen_y,const char *string, COLORREF colour,bool auto_kern,bool no_output){
  int pos=0;
  int start_x=screen_x;
  rttxcb->setup_font_plotting(colour);
  do{
    int c=*string-32;
    int fontx=c&(CHARS_PER_LINE-1);
    int fonty=c>>POW_CHARS_PER_LINE;
    u8 *debugfontp_base=debugfont+fontx*FONT_CHAR_WIDTH+fonty*FONT_LINE_WIDTH*FONT_CHAR_HEIGHT;
    u8 *debugfontp=debugfontp_base;
    int index=(int)colour_index[c];
    int screen_y_use=screen_y;
    if (index&4){
      screen_y_use++;
    }
    bool empty_last_column=true;
    if (auto_kern){
      bool empty_first_column=true;
      for(int y=0;y<FONT_CHAR_HEIGHT;y++){
        int ys=y+screen_y_use;
        if (debugfontp[0]!=0){
          empty_first_column=false;
        }
        if (debugfontp[2]!=0){
          empty_last_column=false;
        }
        debugfontp+=FONT_LINE_WIDTH;
      }
      if (empty_first_column){
        screen_x-=1;
      }
    }
    for(int y=0;y<FONT_CHAR_HEIGHT;y++){
      int ys=y+screen_y_use;
      if (!no_output){
        if (debugfontp_base[0]){
          rttxcb->add_font_pixel(screen_x+2,ys);
        }
        if (debugfontp_base[1]){
          rttxcb->add_font_pixel(screen_x+3,ys);
        }
        if (debugfontp_base[2]){
          rttxcb->add_font_pixel(screen_x+4,ys);
        }
      }
      debugfontp_base+=FONT_LINE_WIDTH;
    }
    if (auto_kern && empty_last_column){
      screen_x+=3;
    }else{
      screen_x+=4;
    }
  }while(*++string!=0);
  last_width=screen_x-start_x;
  rttxcb->render_font_pixels();
}


void print_pretty(RTTXCB *rttxcb,int screen_x,int screen_y,const char *string, COLORREF colour_alpha, COLORREF colour_number, COLORREF colour_operators,bool auto_kern,bool no_output){
  int pos=0;
  int start_x=screen_x;
  do{
    int c=*string-32;
    int fontx=c&(CHARS_PER_LINE-1);
    int fonty=c>>POW_CHARS_PER_LINE;
    u8 *debugfontp_base=debugfont+fontx*FONT_CHAR_WIDTH+fonty*FONT_LINE_WIDTH*FONT_CHAR_HEIGHT;
    u8 *debugfontp=debugfontp_base;
    int index=(int)colour_index[c];
    int colour;
    if ((index&3)==1){
      rttxcb->render_font_pixels();
      rttxcb->setup_font_plotting(colour_alpha);
      colour=colour_alpha;
    }else if ((index&3)==2){
      rttxcb->render_font_pixels();
      rttxcb->setup_font_plotting(colour_number);
    }else{
      rttxcb->render_font_pixels();
      rttxcb->setup_font_plotting(colour_operators);
    }
    int screen_y_use=screen_y;
    if (index&4){
      screen_y_use++;
    }
    bool empty_last_column=true;
    if (auto_kern){
      bool empty_first_column=true;
      for(int y=0;y<FONT_CHAR_HEIGHT;y++){
        int ys=y+screen_y_use;
        if (debugfontp[0]!=0){
          empty_first_column=false;
        }
        if (debugfontp[2]!=0){
          empty_last_column=false;
        }
        debugfontp+=FONT_LINE_WIDTH;
      }
      if (empty_first_column){
        screen_x-=1;
      }
    }
    for(int y=0;y<FONT_CHAR_HEIGHT;y++){
      int ys=y+screen_y_use;
      if (!no_output){
        if (debugfontp_base[0]){
          rttxcb->add_font_pixel(screen_x+2,ys);
        }
        if (debugfontp_base[1]){
          rttxcb->add_font_pixel(screen_x+3,ys);
        }
        if (debugfontp_base[2]){
          rttxcb->add_font_pixel(screen_x+4,ys);
        }
      }
      debugfontp_base+=FONT_LINE_WIDTH;
    }
    if (auto_kern && empty_last_column){
      screen_x+=3;
    }else{
      screen_x+=4;
    }
  }while(*++string!=0);
  last_width=screen_x-start_x;
  rttxcb->render_font_pixels();
}



void print_colour_right_justify(RTTXCB *rttxcb,int screen_x,int screen_y,const char *string, COLORREF colour,bool auto_kern){
  print_colour(rttxcb,screen_x,screen_y,string,colour,auto_kern,true);
  screen_x-=last_width;
  print_colour(rttxcb,screen_x,screen_y,string,colour,auto_kern,false);
}

void print_pretty_right_justify(RTTXCB *rttxcb,int screen_x,int screen_y,const char *string, COLORREF colour_alpha, COLORREF colour_number, COLORREF colour_operators, bool auto_kern){
  print_pretty(rttxcb,screen_x,screen_y,string,colour_alpha, colour_number, colour_operators,auto_kern,true);
  screen_x-=last_width;
  print_pretty(rttxcb,screen_x,screen_y,string,colour_alpha, colour_number, colour_operators,auto_kern,false);
}

void print_colour_centered(RTTXCB *rttxcb,int screen_x,int screen_y,const char *string, COLORREF colour,bool auto_kern){
  print_colour(rttxcb,screen_x,screen_y,string,colour,auto_kern,true);
  screen_x-=last_width>>1;
  print_colour(rttxcb,screen_x,screen_y,string,colour,auto_kern,false);
}

void print_pretty_centered(RTTXCB *rttxcb,int screen_x,int screen_y,const char *string, COLORREF colour_alpha, COLORREF colour_number, COLORREF colour_operators, bool auto_kern){
  print_pretty(rttxcb,screen_x,screen_y,string,colour_alpha, colour_number, colour_operators,auto_kern,true);
  screen_x-=last_width>>1;
  print_pretty(rttxcb,screen_x,screen_y,string,colour_alpha, colour_number, colour_operators,auto_kern,false);
}

