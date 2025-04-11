#include "playerShip.h"
const float Player::SHIELD_MAX_ENERGY = 100.0f;


//Parametrized Constructor for the playerShip
Player::Player(int Xposition, int Yposition){
    pos.x = Xposition;
    pos.y = Yposition;
    health = 100;
    lives = 3;
    velocity.set(0, 0);
    this->shipSprite.load("ShipModels/shipModel2.png");

    this->shipOrientation = 0;
    accelerationAmount = 5.0; // Adjust the value as needed
    score = 0;
    hitBox =  HitBox(pos, shipSprite.getWidth() * 0.25, shipSprite.getHeight() * 0.15);
            
    lastShotTime = 0;
    shotCooldown = 0.1;  // Set the cooldown duration to 0.5 seconds (adjust as needed)
    normalSpeed = 5.0f;
    sprintSpeed = 10.0f;
    isSprinting = false;
    maxSpeed = normalSpeed;
    shieldActive = false;
    shieldEnergy = 0.0f;
    bombs = 0;
    weaponsUpgraded = false;
    shipUpgraded = false;
    this->secondShipSprite.load("ShipModels/secondShip.png");
    this->shieldSprite.load("CompressedImages/ForceShield.png");
    this->bombImage.load("CompressedImages/Bomb-min.png");
}

Player::Player(){
    Player(ofGetWidth()/2, ofGetHeight()/2);
}

int Player::getScore() { return score; }
void Player::setScore(int score) { this->score = score; }

void Player::draw() {
        // Draw the ship sprite with the calculated rotation
            ofPushMatrix();
            ofTranslate(this->pos.x, this->pos.y);
            ofRotateDeg(shipOrientation);
            if (isSprinting) {
                ofSetColor(ofColor::yellow, 100); 
                ofDrawCircle(0, 0, 30); 
                ofSetColor(ofColor::blue);
                ofSetColor(ofColor::white); 
            }

            if(shipUpgraded){
                this->secondShipSprite.draw(-20, -20, 45, 45);
            }else{
            this->shipSprite.draw(-20, -20, 45, 45);
            }
            if (shieldActive) {
                ofSetColor(255, 255, 255, 180);
                this->shieldSprite.draw(-35, -35, 70, 70);
                ofSetColor(ofColor::white);
            }
            ofPopMatrix();
                
        // Draw the hitbox around the player ship. Uncomment this line for testing purposes
            if(showHitbox)  this->hitBox.draw();
}

void Player::update() {

    processPressedKeys();  // Process the pressed keys and calculate orientation change
    increaseShieldEnergy(5.0f * ofGetLastFrameTime());

    velocity.limit(maxSpeed); // Limit the velocity to the maximum speed
            
    pos += velocity; // Update position based on velocity
    this->hitBox.box.setPosition(pos.x - 15, pos.y - 15);
            
    velocity *= damping; // Apply damping to slow down the ship

    draw();  // Draw the ship

    if (shieldActive) {
        shieldTimer -= ofGetLastFrameTime();  
        if (shieldTimer <= 0)
         {
            shieldActive = false;  
            shieldEnergy = 0;     
        }
        decreaseShieldEnergy(SHIELD_MAX_ENERGY / 10.0f * ofGetLastFrameTime());
    }
    updateShield(ofGetLastFrameTime());
}


void Player::shoot() { 
    // Calculate the current time
    float currentTime = ofGetElapsedTimef();

    // Check if enough time has passed since the last shot
 
    if (currentTime - lastShotTime >= shotCooldown) {
        Projectiles p = Projectiles(ofPoint(this->pos.x, this->pos.y), this->shipOrientation);
        
        if (weaponsUpgraded) {
            p.setColors(ofColor::orangeRed, ofColor::gold);
            p.setDamage(p.getDamage() * 2); 
        } else {
            p.setColors(ofColor::azure, ofColor::blueViolet);
        }
        
        this->bullets.push_back(p);
        SoundManager::playSong("Beam", false);
        lastShotTime = currentTime;
    }
}

void Player::setShotCooldown(float shotCooldown) { this->shotCooldown = shotCooldown; }

void Player::removeMarkedBullets(){
    bullets.erase(remove_if(bullets.begin(), bullets.end(),
                        [](const Projectiles& p) { return p.markedForDeletion; }),
    bullets.end());
}

void Player::addPressedKey(int key) {
    if (key == OF_KEY_LEFT_SHIFT || key == OF_KEY_RIGHT_SHIFT) return;
    
    key = tolower(key);
    keyMap[key] = true;
    isMoving = true;
  
}

void Player::processPressedKeys() {
    maxSpeed = isSprinting ? sprintSpeed : normalSpeed;
    
    if(keyMap['w']) movement('w');
    if(keyMap['s']) movement('s');
    if(keyMap['d']) movement('d');
    if(keyMap['a']) movement('a');

    if(keyMap[' ']) shoot();
   

    if (!isMoving) {
        velocity *= damping; 
    }
}     

void Player::removePressedKey(int key) {
    key = tolower(key);
    keyMap[key] = false;
}

void Player::movement(char keyPressed) {
    ofVec2f acceleration;

    if (keyPressed == 'w') {
        // Apply acceleration in the direction of the ship's orientation
        acceleration.set(cos(ofDegToRad(shipOrientation)) * accelerationAmount, sin(ofDegToRad(shipOrientation)) * accelerationAmount);
        }
    if (keyPressed == 's') {
        // Apply acceleration in the opposite direction of the ship's orientation
        acceleration.set(-cos(ofDegToRad(shipOrientation)) * accelerationAmount, -sin(ofDegToRad(shipOrientation)) * accelerationAmount);
        }

    // Apply force to velocity
    velocity += acceleration;
        
    if (keyPressed == 'a') {
        // Rotate the ship counterclockwise
        shipOrientation -= rotationSpeed;
        }
    if (keyPressed == 'd') {
        // Rotate the ship clockwise
        shipOrientation += rotationSpeed;
        }
    }   
void Player::loseLife() {
    SoundManager::playSong("shipDestroyed", false);
    lives--;
    if(lives > 0) {
        health = 100;
        pos.x = ofGetWidth()/2;
        pos.y = ofGetHeight()/2;
    }
}
void Player::reset() {
    health = 100;
    lives = 3;
    score = 0;
    pos.x = ofGetWidth()/2;
    pos.y = ofGetHeight()/2;
    velocity.set(0, 0);
    bullets.clear();
}
void Player::setSprinting(bool sprinting) {
    isSprinting = sprinting;
    maxSpeed = isSprinting ? sprintSpeed : normalSpeed;
}
void Player::activateShield() {
    if (shieldEnergy >= SHIELD_MAX_ENERGY && !shieldActive) {
        shieldActive = true;
        shieldEnergy = SHIELD_MAX_ENERGY;
        shieldTimer = 10.0f;
        SoundManager::playSong("Beam", false);
    }
}
void Player::updateShield(float deltaTime) {
    if (shieldActive) {
        shieldEnergy -= 20.0f * deltaTime;
        if (shieldEnergy <= 0) {
            shieldActive = false;
            shieldEnergy = 0;
        }
    }
}
void Player::increaseShieldEnergy(float amount) {
    if (!shieldActive && shieldEnergy < SHIELD_MAX_ENERGY) {
        shieldEnergy = min(shieldEnergy + amount, SHIELD_MAX_ENERGY);
    }
}

bool Player::isShieldActive() const {
    return shieldActive;
}

float Player::getShieldEnergy() const {
    return shieldEnergy;
}

void Player::addBomb() {
    bombs = min(bombs + 1, 3);
    SoundManager::playSong("button", false);
}

bool Player::useBomb() {
    if (bombs > 0) {
        bombs--;
        SoundManager::playSong("shipDestroyed", false);
        return true;
    }
    return false;
}

int Player::getBombs() const {
    return bombs;
}

void Player::upgradeWeapons() {
    weaponsUpgraded = true;
    shotCooldown *= 0.7f;
    SoundManager::playSong("button", false);
}

void Player::upgradeShip() {
    shipUpgraded = true;
    normalSpeed *= 1.2f;
    sprintSpeed *= 1.2f;
    SoundManager::playSong("button", false);
}

bool Player::hasUpgradedWeapons() const {
    return weaponsUpgraded;
}

bool Player::hasUpgradedShip() const {
    return shipUpgraded;
}

void Player::takeDamage(float damage) {
    if (!isShieldActive())
    {
        health = max(health-damage, 0.0f);
    }
}

void Player::decreaseShieldEnergy(float amount){
    if (shieldActive) {  
        shieldEnergy -= amount;
        if (shieldEnergy <= 0) {
            shieldActive = false;  
            shieldEnergy = 0;
        }
    }
}