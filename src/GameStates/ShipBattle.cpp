#include "ShipBattle.h"
#include "EnemyManager.h"
#include "Boss.h"
#include "playerShip.h"
using namespace std;
// ====================================
// Constructor & Destructor Section
// ====================================

ShipBattle::ShipBattle() {
    this->player = new Player(ofGetWidth() / 2, ofGetHeight() / 2);
    this->playerScore = 0;
    this->killspreeTimer = 0;
    
    font.load("Fonts/Orbitron.ttf", 20, true);
    indicatorFont.load("Fonts/Orbitron.ttf", 10, true);
    backgroundImage.load("Menu_Images/BattleArea.jpg");
}

// ====================================
// Core Functionality Section
// ====================================


// Update Method
void ShipBattle::update() {
    if (EnemyManager::getEnemiesKilledThisFrame() > 0) {
        player->increaseShieldEnergy(5.0f * EnemyManager::getEnemiesKilledThisFrame());
    }
    player->updateShield(ofGetLastFrameTime());
    

    // Boss spawn logic
    if (EnemyManager::isBossSpawning()) {
        displayBossWarning = true;
        SoundManager::stopSong("battle");
    } 
    else {
        displayBossWarning = false;
    }

    // Update enemies and player
    EnemyManager::updateEnemies(player);
    this->player->processPressedKeys();
    this->player->update();
    wrapCoords(this->player->pos);

    // Update scoring and kill spree timer
    playerScore += EnemyManager::getPointsScored() * scoreMultiplier() * killSpreeMode();
    this->player->setScore(playerScore);
    killspreeTimer = EnemyManager::getKillSpreeTimer();

    // Update player projectiles
    if (!this->player->bullets.empty()) {
        updateBullets();
    }

    // State switching logic for when the player dies
    if (this->player->health <= 0) {
        player->loseLife();

        if(player->getLives() <= 0){
        this->setNextState("GameOverState");
        SoundManager::stopSong("battle");
        if(EnemyManager::getSpawningBossType() != ""){
            SoundManager::stopSong(EnemyManager::getSpawningBossType());
        }
            // Write the current score to a file
            ofstream scoreFile("currentScore.txt");
            if (scoreFile.is_open()) {
                scoreFile << playerScore;
                scoreFile.close();
            }
            this->setFinished(true);
        } else {
            EnemyManager::cleanUp();
            player->pos.set(ofGetWidth()/2, ofGetHeight()/2);
            player->health = 100;
            player->bullets.clear();
            ofSetColor(ofColor::red);
            font.drawString("LIFE LOST!" , player->pos.x - 30, player->pos.y - 40);
            SoundManager::playSong("button", false);
            if (EnemyManager::getEnemiesKilledThisFrame() > 0) {
                player->increaseShieldEnergy(5.0f * EnemyManager::getEnemiesKilledThisFrame()); 
            }
            
            player->updateShield(ofGetLastFrameTime());
        }
    }
}


//====== Draw Method ====== 
void ShipBattle::draw() {
    ofSetBackgroundColor(ofColor::black);
    backgroundImage.draw(0, 0, ofGetWidth(), ofGetHeight());
    
    // Draw the score
    ofSetColor(ofColor::white);
    font.drawString("SCORE " + to_string(playerScore), ofGetWidth() / 2 - 50, 50);
    float livesTextWidth = font.getStringBoundingBox("LIVES: ", 0 , 0).width;
    indicatorFont.drawString("LIVES: " + to_string(player->getLives()), 15, 30);

    // Draw enemies and player
    EnemyManager::drawEnemies();
    player->draw();
    draw_bullets();
    
    // Draw boss warning if needed
    if (displayBossWarning) {
        // Semi-transparent red screen tint
        ofSetColor(255, 0, 0, 128);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

        // Warning text
        ofSetColor(255);
        font.drawString("BOSS APPROACHING!", ofGetWidth() / 2 - 100, ofGetHeight() / 2);
    }

    // Draw UI elements
    healthBar(player->health, 100);
    ofSetColor(ofColor::red);
    float circleStartX = 15 + livesTextWidth + 10;
    for(int i = 0; i < player->getLives(); i++) {
        ofDrawCircle(circleStartX + (i * 25), 25, 8);
    }
    killSpreeTimer(this->killspreeTimer, 150);
    
    //Draw a mini box for the bomb. Make sure to draw the bomb inside this box.
        ofNoFill();
        ofDrawRectangle(ofGetWidth() - 150, 30, 50, 50);
        ofFill();
        if (player->getBombs() > 0) {
            player->bombImage.draw(ofGetWidth() - 150, 30, 50, 50);
            ofSetColor(ofColor::white);
            font.drawString(ofToString(player->getBombs()), ofGetWidth() - 130, 60);
        }
        shieldEnergyBar(player->getShieldEnergy(), Player::SHIELD_MAX_ENERGY); 

}




// ====================================
// Input Handling Section
// ====================================

void ShipBattle::keyPressed(int key) {
    if (key == OF_KEY_LEFT_SHIFT || key == OF_KEY_RIGHT_SHIFT) {
        player->setSprinting(true);
        return; 
    }
    
    player->addPressedKey(key);
    
    
    if(key == 'k') {
        EnemyManager::toggleHitBoxes = !EnemyManager::toggleHitBoxes; 
        player->showHitbox = !player->showHitbox;
    }
    if(key == 'o')  player->health = 100;
    if(key == 'p')  playerScore += 10000;
    if (key == 'q' || key == 'Q') 
    {
        if (player->getShieldEnergy() >= Player::SHIELD_MAX_ENERGY)
         {
        player->activateShield();
        }
}

    if (key == 'e' || key == 'E') {
        if (player->useBomb()) {
            EnemyManager::destroyAllRegularEnemies();
            SoundManager::playSong("shipDestroyed", false);
        }
    }
}


void ShipBattle::keyReleased(int key) {
   
    if (key == OF_KEY_LEFT_SHIFT || key == OF_KEY_RIGHT_SHIFT) {
        player->setSprinting(false);
        return; 
    }
    
    key = tolower(key);
    this->player->removePressedKey(key);
}

void ShipBattle::mousePressed(int x, int y, int button) {
    // Implementation for mousePressed event in case you want to do something with this
}

// ====================================
// Helper Methods Section
// ====================================

void ShipBattle::wrapCoords(ofPoint &currentPos) {
    // Screen wrapping logic for player ship
    if (currentPos.x < 0.0) currentPos.x = ofGetWidth() - 10;
    if (currentPos.x >= ofGetWidth()) currentPos.x = 10;
    if (currentPos.y < 0.0) currentPos.y = ofGetHeight() - 10;
    if (currentPos.y >= ofGetHeight()) currentPos.y = 10;
}

void ShipBattle::draw_bullets() {
    // Draw all player bullets
    for (auto &bullet : this->player->bullets) {
        bullet.draw();
    }
}

void ShipBattle::updateBullets() {
    // Update logic for player bullets
    for (auto &bullet : this->player->bullets) {
        bullet.update();
    }
}

// ====================================
// UI and Feedback Methods Section
// ====================================
void ShipBattle::healthBar(int currHealth, int maxHealth) {
    int barX = 85;
    int barY = 50;
    indicatorFont.drawString("HEALTH", 10, barY + 15);
    ofNoFill();
    ofDrawRectangle(barX, barY, maxHealth *2, 20);
    ofFill();
    ofSetColor(ofColor::green);
    ofDrawRectangle(barX, barY, currHealth *2, 20);
    ofSetColor(ofColor::white);
}

void ShipBattle::killSpreeTimer(int currTimer, int maxTimer) {
    int barX = 110;
    int barY = 80;
    indicatorFont.drawString("KILL SPREE", 10, barY + 10);  
    ofNoFill();
    ofDrawRectangle(barX, barY, maxTimer, 10);
    ofFill();
    ofSetColor(ofColor::red);
    ofDrawRectangle(barX, barY, currTimer, 10);
    ofSetColor(ofColor::white);
}

// ====================================
// Game Mechanics Methods Section
// ====================================

double ShipBattle::killSpreeMode() {
    // Logic for kill spree mode bonus
    return this->killspreeTimer > 0 ? 1.5 : 1.0;
}

double ShipBattle::scoreMultiplier() {
    // Logic for score multiplier based on current score
    if (playerScore >= 350) return 3.5;
    else if (playerScore >= 200) return 2.5;
    else if (playerScore >= 100) return 1.5;
    else return 1.0; // Default multiplier
}

void ShipBattle::reset(){
    delete player;
    player = new Player(ofGetWidth() / 2, ofGetHeight() / 2);

    playerScore = 0;
    killspreeTimer = 0;

    EnemyManager::cleanUp();
    playerScore = 0;
    killspreeTimer = 0;


    setFinished(false);
    setNextState("");
}
void ShipBattle::shieldEnergyBar(float currEnergy, float maxEnergy) {
    int barX = 85;
    int barY = 110;
    indicatorFont.drawString("SHIELD", 10, barY + 15);
    ofNoFill();
    ofDrawRectangle(barX, barY, maxEnergy * 2, 20);    
    ofFill();
    ofSetColor(ofColor::cyan);
    ofDrawRectangle(barX, barY, 200 * (currEnergy / maxEnergy), 20);
    ofSetColor(ofColor::white);
}


                                        
