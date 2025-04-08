#pragma once
#include "Boss.h"
#include "ofMain.h"
class CassandraVexBoss : public Boss {
public:
    CassandraVexBoss(int xpos, int ypos, string name);
    void update(const ofPoint& playerPos) override;
    void draw() override;
    void shoot() override;
    
private:
    float vortexAngle = 0.0f;         
    const float vortexSpeed = 7.0f;    
    const int projectilesPerWave = 12; 
};