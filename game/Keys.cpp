// Description:
//   Keyboard/Trigger helpers.
//
// Copyright (C) 2007 Frank Becker
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation;  either version 2 of the License,  or (at your option) any  later
// version.
//
// This program is distributed in the hope that it will be useful,  but  WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details
//
#include "Keys.hpp"

#include <sstream>
#include <SDL2/SDL_keycode.h>

#include "Trace.hpp"

using namespace std;

bool Keys::convertStringToTrigger( string & keyname, Trigger & trigger)
{
//    XTRACE();
    if( keyname == "MOTION")
    {
        trigger.type = eMotionTrigger;
        trigger.data1= 0;
        trigger.data2= 0;
        trigger.data3= 0;
        return true;
    }

    if( keyname.substr(0,11) == "MOUSEBUTTON")
    {
        trigger.type = eButtonTrigger;
        if( keyname.length() == 12)
        {
	    trigger.data1= keyname[11] - '0'; 
        }
        else
        {
	    trigger.data1= 1; //default to button 1
        }
//        trigger.data1= atoi( keyname.substr(11,1).c_str());
        trigger.data2= 0;
        trigger.data3= 0;
        return true;
    }

    trigger.type = eKeyTrigger;
    SDL_Keycode k = SDL_GetKeyFromName(keyname.c_str());
    if(k == SDLK_UNKNOWN)
    {
      return false;
    }

    trigger.data1 = k; 
    trigger.data2 = 0;
    trigger.data3 = 0;

    return true;
}

string Keys::convertTriggerToString( const Trigger & trigger)
{
//    XTRACE();
    string triggerName;

    switch( trigger.type)
    {
        case eKeyTrigger:
            //triggerName = _symmap[ trigger.data1];
            triggerName = SDL_GetKeyName(trigger.data1);
            break;
        case eButtonTrigger:
            {
                ostringstream ost;
                ost << "MOUSEBUTTON" << trigger.data1;
		triggerName = ost.str();
            }
            break;
        case eMotionTrigger:
            triggerName = "MOTION";
            break;
        case eUnknownTrigger:
        default:
            triggerName = "UNKNOWN_";
            break;
    }

    return triggerName;
}

Keys::Keys( void)
{
    XTRACE();
}

Keys::~Keys()
{
    XTRACE();
}
