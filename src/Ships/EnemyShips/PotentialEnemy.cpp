#include "PotentialEnemy.h"
#include "SoundManager.h"
using namespace std;

PotentialEnemy::PotentialEnemy(int xpos, int ypos) : 
    EnemyShip(xpos, ypos, 0.7f, 20, 250) {
    amIBoss = false;
    enemyShipSprite.load("CompressedImages/newEnemy-min.png");
    enemyHitBox = new HitBox(pos, enemyShipSprite.getWidth() * 0.05, enemyShipSprite.getHeight() * 0.05);
}

void PotentialEnemy::update(const ofPoint& playerPos) {
    ofVec2f toPlayer = playerPos - pos;
    float angleToPlayer = ofRadToDeg(-atan2(toPlayer.x, toPlayer.y));
    shipOrientation = angleToPlayer;
    pos += toPlayer.getNormalized() * speed;
    enemyHitBox->box.setPosition(pos.x - 15, pos.y - 10);

    if (shotTimer++ % 120 == 0) {
        shoot();
    }
}

void PotentialEnemy::draw() {
    ofPushMatrix();
    ofTranslate(pos.x, pos.y);
    ofRotateDeg(shipOrientation);
    enemyShipSprite.draw(-32, -32, 70, 70);
    ofPopMatrix();
    
    if(showHitboxes) enemyHitBox->draw();
}

void PotentialEnemy::shoot() {
    Projectiles leftShot(pos, shipOrientation + 90 - spreadAngle/2);
    Projectiles rightShot(pos, shipOrientation + 90 + spreadAngle/2);
    
    leftShot.setColors(ofColor::purple, ofColor::magenta);
    rightShot.setColors(ofColor::purple, ofColor::magenta);
    
    enemyBullets.push_back(leftShot);
    enemyBullets.push_back(rightShot);
    
    SoundManager::playSong("bulletSound", false);
}

void PotentialEnemy::takeDamage(int dmg) {
    hitCount++;
    if(hitCount >= requiredHits) {
        health = 0;
        dead = true;
    }
}