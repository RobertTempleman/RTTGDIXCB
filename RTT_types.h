#ifndef INCLUDED_RTT_TYPES_HEADER
#define INCLUDED_RTT_TYPES_HEADER

#include "RTTGDIXCB.h"
#include "RTT_vector.h"
#include <vector>
#include <string>
#include <queue>
#include <list>
//#include "RTT_colours.h"
//#include "RTT_text_edit_box.h"
//#include "RTT_button.h"
//#include "RTT_pane.h"

#define RTT_PI 3.14159265359f
#define RTT_2PI (2.0f*RTT_PI)
#define RTT_RADS_TO_DEGREES (360.0f/RTT_2PI)

using namespace std;
class rtt_pane;

typedef std::function<void(rtt_pane &p)> paintfn_type;
typedef std::function<void(rtt_pane &p,s32 butt,s32 x_click,s32 y_click)> clickfn_type;
typedef std::function<void(rtt_pane &p,s32 x_move_to,s32 y_move_to)> mouse_move_fn_type;

#endif
