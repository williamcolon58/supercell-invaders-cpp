#include "CassandraVexBoss.h"

CassandraVexBoss::CassandraVexBoss(int xpos, int ypos, string name) : 
    Boss(xpos, ypos, 0.0f, 600, name) {
    enemyShipSprite.load("ShipModels/Space_Station.png");
    enemyHitBox = new HitBox(pos.x - 60, pos.y - 60, 120, 120);
}

void CassandraVexBoss::update(const ofPoint& playerPos) {
    pos.x = ofGetWidth() / 2;
    pos.y = ofGetHeight() / 2;
    enemyHitBox->box.setPosition(pos.x - 60, pos.y - 60);

    if (shotTimer++ % 10 == 0) {
        shoot();
    }
}

void CassandraVexBoss::draw() {
    ofPushMatrix();
    ofTranslate(pos.x, pos.y);
    ofRotateDeg(vortexAngle * 0.5f); 
    enemyShipSprite.draw(-80, -80, 160, 160);
    ofPopMatrix();
    
    if(showHitboxes) enemyHitBox->draw();
    showBossHealth();
}

void CassandraVexBoss::shoot() {
    
        float angle = vortexAngle;
        if (angle >= 360.0f) angle -=360.0f;
        Projectiles vortexProjectile(pos, angle);
        vortexProjectile.setSpeed(5);
        vortexProjectile.setColors(ofColor::cyan, ofColor::blue);
        enemyBullets.push_back(vortexProjectile);
    

    vortexAngle += vortexSpeed;
    if(vortexAngle >= 360.0f) vortexAngle -= 360.0f;

    SoundManager::playSong("bulletSound", false);
}