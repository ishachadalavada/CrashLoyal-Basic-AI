// MIT License
// 
// Copyright(c) 2020 Arthur Bacon and Kevin Dill
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "Entity.h"
using namespace std;

struct Waypoint;

class Mob : public Entity {

public:
    Mob(const iEntityStats& stats, const Vec2& pos, bool isNorth);
    
    Vec2& getPos();

    virtual void tick(float deltaTSec);

protected:
    void move(float deltaTSec);
    const Vec2* pickWaypoint();
    vector<Mob*> checkCollision();
    bool checkRiverCollision();
    void processRiverCollision();
    bool checkBuildingCollision();
    void processBuildingCollision();
    void processCollision(Mob* otherMob, float deltaTSec);


private:
    const Vec2* m_pWaypoint;
};
