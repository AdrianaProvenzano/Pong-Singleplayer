// methods 

void drawGameField(void); 

void startGame (void);

void endGame (void); 

void resumeGame(void); 

void pauseGame(void); 

void mooveBall (void);

void moovePaddle(unsigned short, unsigned short );

void increaseScore(void); 

void hitWall(void); 

void hitPaddle(void); 

void printWelcomeScreen(void); 

enum GameStatus{NotStarted, Started, Paused, Lost};
