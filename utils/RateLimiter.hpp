#pragma once
// Description:
//   Limits the rate of a loop
//
// Copyright (C) 2012 Adam Winsor
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

class RateLimiter
{
    public:
        RateLimiter();
        RateLimiter(int rate);
        ~RateLimiter();
        void reset(float currTime);
        void limit(float currTime);
        void setRate(int rate)
        {
            this->_timeSlice = 1.0 / rate;
        }
  
        void setEnabled(bool value)
        {
            this->_enabled = value;
        }
  
        bool isEnabled()
        {
            return this->_enabled;
        }

    private:
      float _timeSlice;
      float _sliceStartTime;
      bool _enabled;
};
