#ifndef MAIN_H
#define MAIN_H

#define WHITE 1
#define BLACK -1

#define PAWN 1
#define ROOK 2
#define KNIGHT 3
#define BISHOP 4
#define QUEEN 5
#define KING 6

#define PIECE(type, color) (color << type)

#define DIAGONAL (PIECE(BISHOP, 1) | PIECE(QUEEN, 1) | PIECE(KING, 1))
#define SIDE (PIECE(ROOK, 1) | PIECE(QUEEN, 1) | PIECE(KING, 1))

#define MIN_DRAGG_PIXELS 5
#define CIRCLE_SEGMENTS 360

static const char* ALGEBRAIC_POSITION_ROW = "abcdefgh";
static const char* ALGEBRAIC_POSITION_COLUMN = "87654321";

// Boost libraries
#include <boost/utility.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <stack>
#include <thread>

#include <wx/defs.h>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#   include <wx/wx.h>
#endif
#include <wx/thread.h>
#include <wx/utils.h>
#include <wx/progdlg.h>
#include <wx/glcanvas.h>
#include <wx/debugrpt.h>
#include <wx/minifram.h>
#include <wx/gbsizer.h>
#include <wx/choicebk.h>
#include <wx/tglbtn.h>
#include <wx/dcbuffer.h>
#include <wx/aui/aui.h>
#include <wx/cmdline.h>
#include <wx/filename.h>
#include <wx/filepicker.h>
#include <wx/arrstr.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/wxhtml.h>
#include <wx/vlbox.h>
#include <wx/stdpaths.h>
#include <wx/url.h>
#include <wx/ipc.h>
#include <wx/grid.h>
#include <wx/clipbrd.h>

#endif
