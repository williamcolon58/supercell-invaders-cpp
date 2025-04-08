#pragma once
#include "enemy.h"
#include "ofMain.h" 

class PotentialEnemy : public EnemyShip {
public:
    PotentialEnemy(int xpos, int ypos);
    void update(const ofPoint& playerPos) override;
    void draw() override;
    void shoot() override;
    void takeDamage(int dmg) override;
    
private:
    int hitCount = 0;
    const int requiredHits = 2;       
    const float spreadAngle = 10.0f;  
};