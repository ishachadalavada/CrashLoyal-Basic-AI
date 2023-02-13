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

#include "Mob.h"

#include "Constants.h"
#include "Game.h"

#include <algorithm>
#include <vector>
using namespace std;


Mob::Mob(const iEntityStats& stats, const Vec2& pos, bool isNorth) : Entity(stats, pos, isNorth), m_pWaypoint(NULL)
{
    assert(dynamic_cast<const iEntityStats_Mob*>(&stats) != NULL);
}

Vec2& Mob::getPos() {
    return m_Pos;
}
void Mob::tick(float deltaTSec)
{
    // Tick the entity first.  This will pick our target, and attack it if it's in range.
    Entity::tick(deltaTSec);

    // if our target isn't in range, move towards it.
    if (!targetInRange())
    {
        move(deltaTSec);
    }
}

void Mob::move(float deltaTSec)
{
    // If we have a target and it's on the same side of the river, we move towards it.
    //  Otherwise, we move toward the bridge.
    bool bMoveToTarget = false;
    if (!!m_pTarget)
    {    
        bool imTop = m_Pos.y < (GAME_GRID_HEIGHT / 2);
        bool otherTop = m_pTarget->getPosition().y < (GAME_GRID_HEIGHT / 2);

        if (imTop == otherTop)
        {
            bMoveToTarget = true;
        }
    }

    Vec2 destPos;
    if (bMoveToTarget)
    { 
        m_pWaypoint = NULL;
        destPos = m_pTarget->getPosition();
    }
    else
    {
        if (!m_pWaypoint)
        {
            m_pWaypoint = pickWaypoint();
        }
        destPos = m_pWaypoint ? *m_pWaypoint : m_Pos;
    }

    // Actually do the moving
    Vec2 moveVec = destPos - m_Pos;
    float distRemaining = moveVec.normalize();
    float moveDist = m_Stats.getSpeed() * deltaTSec;

    // if we're moving to m_pTarget, don't move into it
    if (bMoveToTarget)
    {
        assert(m_pTarget);
        distRemaining -= (m_Stats.getSize() + m_pTarget->getStats().getSize()) / 2.f;
        distRemaining = std::max(0.f, distRemaining);
    }

    if (moveDist <= distRemaining)
    {
        m_Pos += moveVec * moveDist;
    }
    else
    {
        m_Pos += moveVec * distRemaining;

        // if the destination was a waypoint, find the next one and continue movement
        if (m_pWaypoint)
        {
            m_pWaypoint = pickWaypoint();
            destPos = m_pWaypoint ? *m_pWaypoint : m_Pos;
            moveVec = destPos - m_Pos;
            moveVec.normalize();
            m_Pos += moveVec * distRemaining;
        }
    }
    
    processRiverCollision();
    
    processBuildingCollision();

    // PROJECT 1: This is where your collision code will be called from
    vector<Mob*> otherMob = checkCollision();
    for (Mob* om : otherMob) {
        if (om) {
            processCollision(om, deltaTSec);
        }
    }
    
    
    
}

const Vec2* Mob::pickWaypoint()
{
    float smallestDistSq = FLT_MAX;
    const Vec2* pClosest = NULL;

    for (const Vec2& pt : Game::get().getWaypoints())
    {
        // Filter out any waypoints that are behind (or barely in front of) us.
        // NOTE: (0, 0) is the top left corner of the screen
        float yOffset = pt.y - m_Pos.y;
        if ((m_bNorth && (yOffset < 1.f)) ||
            (!m_bNorth && (yOffset > -1.f)))
        {
            continue;
        }

        float distSq = m_Pos.distSqr(pt);
        if (distSq < smallestDistSq) {
            smallestDistSq = distSq;
            pClosest = &pt;
        }
    }

    return pClosest;
}

// PROJECT 1: 
//  1) return a vector of mobs that we're colliding with
//  2) handle collision with towers & river 
vector<Mob*> Mob::checkCollision()
{

    float mobsize = m_Stats.getSize();
    
    vector<Mob*> collisionvect;

   // iterates through the list of mobs
    for (Mob* pOtherMob : Game::get().getMobs())
    {
        if (this == pOtherMob)
        {
            continue;
        }

        // PROJECT 1: YOUR CODE CHECKING FOR A COLLISION GOES HERE

        // gets size and position of other mob
        const iEntityStats& omob = pOtherMob->m_Stats;
        float omobsize = omob.getSize();
        Vec2& omobpos = pOtherMob->getPos();

        // distance from center (pos) to outside edge
        float sizeboundary = 0.5 * (mobsize + omobsize);
        
        float distance = omobpos.distSqr(this->m_Pos);

        if (distance <= sizeboundary){
            Mob* mobCollision = pOtherMob;
            // for extension aspect
                collisionvect.push_back(pOtherMob);
        }

    }
     return collisionvect;
}

bool Mob::checkRiverCollision() {
    
    float x = m_Pos.x;
    
    // when in the range of the river
    if ((m_Pos.y >= RIVER_TOP_Y) && (m_Pos.y <= RIVER_BOT_Y)) {
        cout << "on river area" << endl;
        
        // if not on bridge
        if ((!((x >= LEFT_BRIDGE_CENTER_X - (BRIDGE_WIDTH/2)) && (x <= (LEFT_BRIDGE_CENTER_X + BRIDGE_WIDTH/2)))) && (!((x >= RIGHT_BRIDGE_CENTER_X - (BRIDGE_WIDTH/2)) && (x <= (RIGHT_BRIDGE_CENTER_X + BRIDGE_WIDTH/2))))){
            
            cout << "bridge" << LEFT_BRIDGE_CENTER_X << endl;
            cout << "position " << m_Pos.x << endl;
            
            return true;
            }

        }
    return false;
}

bool Mob::checkBuildingCollision() {
    float mobsize = m_Stats.getSize();


   // iterates through the list of mobs
    for (Entity* otherBuilding : Game::get().getBuildings())
    {

        // gets size and position of other mob
        const iEntityStats& obuild = otherBuilding->getStats();
        float obuildsize = obuild.getSize();
        const Vec2& obuildpos = otherBuilding->getPosition();

        // distance from center (pos) to outside edge
        float sizeboundary = (mobsize + obuildsize);

        float distance = obuildpos.distSqr(m_Pos);


        if (distance <= sizeboundary){
            // for extension aspect
            return true;
        }

    }
    return false;
}

// moves according to position on screen/if a collision is happening
void Mob:: processBuildingCollision() {
    bool collision = checkBuildingCollision();

    if (collision) {
        Vec2 right = Vec2(0.0f, 0.1f);
        Vec2 left = Vec2(0.0f, -0.1f);
        Vec2 top = Vec2(-0.1f, 0.0f);
        Vec2 bottom =Vec2(0.1f, 0.0f);
        if (m_Pos.x >= GAME_GRID_WIDTH / 2) {
            m_Pos += left;
        }
        else {
            m_Pos += right;
        }
//        collision = checkBuildingCollision();
    }

}

void Mob::processRiverCollision() {
    bool inRiver = checkRiverCollision();
    while (inRiver) {
        if (m_Pos.y >= (RIVER_TOP_Y + BRIDGE_HEIGHT/2)){
            Vec2 collisionHandle = Vec2(0.0f, -0.1f);
            m_Pos += collisionHandle;
        }
        if (m_Pos.y <= (RIVER_TOP_Y + BRIDGE_HEIGHT/2)){
            Vec2 collisionHandle = Vec2(0.0f, 0.1f);
            m_Pos += collisionHandle;
        }
        inRiver = checkRiverCollision();
    }
}


void Mob::processCollision(Mob* otherMob, float deltaTSec) 
{
    // PROJECT 1: YOUR COLLISION HANDLING CODE GOES HERE
    bool collision = true;
    
    // while this object is still in collision, moves object slightly away from each other
    while (collision) {
        Vec2 left = Vec2(0.1f, 0.0f);
        Vec2 right = Vec2(-0.1f, 0.0f);

            if (m_bNorth == true) {
                
                // to ensure that they have a little bit of a buffer between
                if (m_Pos.x + 1 <= GAME_GRID_WIDTH) {
                    m_Pos += left;
                }
                else {
                    m_Pos += right;
                }
            }
            else {
                if (m_Pos.x - 1 >= 0) {
                    m_Pos += right;
                }
                else {
                    m_Pos += left;
                }
            }

        
//        cout << "After: " << m_Pos.x << endl;
        
        
        //gets necessary info to check if collision is still true
        const iEntityStats& omob = otherMob->m_Stats;
        float omobsize = omob.getSize();
        Vec2& omobpos = otherMob->getPos();
        float mobsize = m_Stats.getSize();
        
//        cout << "this size: " << mobsize << " other size: " << omobsize << endl;
        
    
        // distance from center (pos) to outside edge
        float sizeboundary = 0.5 * (mobsize + omobsize);
        float distance = omobpos.dist(m_Pos);
//        cout << "size boundary: " << sizeboundary << endl;
//        cout << "Distance: " << distance << endl;
        if (distance > sizeboundary){
            collision = false;
        }
    }
    

    /*
     - checks if otherMob is in list of collisions
     - if it is, tries to move in another direction -- how to stop normal directions and change pos how you want it
     */
    
}

