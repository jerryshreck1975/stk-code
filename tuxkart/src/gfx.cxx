//  $Id: gfx.cxx,v 1.20 2004/08/11 12:33:17 grumbel Exp $
//
//  TuxKart - a fun racing game with go-kart
//  Copyright (C) 2004 Steve Baker <sjbaker1@airmail.net>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef WIN32
#include <unistd.h>
#include <string.h>
#include <sys/io.h>
#include <sys/perm.h>                                                           
#endif

#include "tuxkart.h"
#include "Camera.h"
#include "gfx.h"
#include "TrackData.h"
#include "TrackManager.h"
#include "World.h"

GFX::GFX ( int _mirror )
{
  mirror = _mirror ;

  ssgSetFOV ( 75.0f, 0.0f ) ;
  ssgSetNearFar ( 0.05f, 1000.0f ) ;

  sgCoord cam ;
  sgSetVec3 ( cam.xyz, 0, 0, 0 ) ;
  sgSetVec3 ( cam.hpr, 0, 0, 0 ) ;
  ssgSetCamera ( & cam ) ;
}


void GFX::update ()
{
  TrackData& track_data = track_manager.tracks[World::current()->raceSetup.track];

  ssgGetLight ( 0 ) -> setPosition ( track_data.sun_position ) ;
  ssgGetLight ( 0 ) -> setColour ( GL_AMBIENT , track_data.ambientcol  ) ;
  ssgGetLight ( 0 ) -> setColour ( GL_DIFFUSE , track_data.diffusecol  ) ;
  ssgGetLight ( 0 ) -> setColour ( GL_SPECULAR, track_data.specularcol ) ;

  if ( mirror ) glFrontFace ( GL_CW ) ;

  ssgCullAndDraw ( World::current()->scene ) ;
}



/* Address of the parallel port. */

#define LPBASE 0x378L

/* -1 left-eye, +1 right-eye, 0 center (ie No Stereo) */

static int stereo = 0 ;


int stereoShift ()
{
  return stereo ;
}


void GFX::done ()
{
  swapBuffers();
  
  glBegin ( GL_POINTS ) ;
  glVertex3f ( 0, 0, 0 ) ;
  glEnd () ;
  glFlush () ;

#ifdef UL_LINUX
  static int firsttime = TRUE ;

  if ( firsttime )
  {
    firsttime = FALSE ;

    if ( getenv ( "TUXKART_STEREO" ) == NULL )
    {
      stereo = 0 ;
      return ;
    }

    fprintf ( stderr, "Requesting control of parallel printer port...\n" ) ;
 
    int res = ioperm ( LPBASE, 8, 1 ) ;
 
    if ( res != 0 )
    {
      perror ( "parport" ) ;
      fprintf ( stderr, "Need to run as 'root' to get stereo.\n" ) ;
      stereo = 0 ;
    }
    else
    {
      fprintf ( stderr, "Stereo Enabled!\n" ) ;
      stereo = -1 ;
    }
  }

  if ( stereo != 0 )
  {
    outb ( (stereo==-1) ? ~3 : ~2, LPBASE+2 ) ;
    stereo = -stereo ;
  }
#endif
}


void updateGFX ( GFX *gfx )
{
  TrackData& track_data = track_manager.tracks[World::current()->raceSetup.track];

  glEnable ( GL_DEPTH_TEST ) ;

  if (track_data.use_fog)
    {
      glEnable ( GL_FOG ) ;
      
      glFogf ( GL_FOG_DENSITY, 1.0f / 100.0f ) ;
      glFogfv( GL_FOG_COLOR  , track_data.fog_color ) ;
      glFogf ( GL_FOG_START  , 0.0       ) ;
      glFogf ( GL_FOG_END    , 1000.0      ) ;
      glFogi ( GL_FOG_MODE   , GL_EXP2   ) ;
      glHint ( GL_FOG_HINT   , GL_NICEST ) ;

      /* Clear the screen */
      glClearColor (track_data.fog_color[0], 
                    track_data.fog_color[1], 
                    track_data.fog_color[2], 
                    track_data.fog_color[3]);
    }
  else
    {
      /* Clear the screen */
      glClearColor (track_data.sky_color[0], 
                    track_data.sky_color[1], 
                    track_data.sky_color[2], 
                    track_data.sky_color[3]);
    }

  glClear      ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;

  for ( int i = 0 ; i < Camera::getNumSplits() ; i++ )
    {
      camera [ i ] -> apply () ;
      gfx -> update () ;
    }

  if (track_data.use_fog)
    {
      glDisable ( GL_FOG ) ;
    }

  glViewport ( 0, 0, getScreenWidth(), getScreenHeight() ) ;
}

/* EOF */
