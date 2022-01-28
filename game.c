#include "game.h"
#include "LPC17xx.h"                    // Device header
#include "timer/timer.h"								
#include "GLCD/GLCD.h" 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum GameStatus game_status;				// stato del gioco (NotStarted, Started, Paused, Lost)

int score = 0; 											// punteggo
char score_string[10]=""; 
int record = 100; 									// record 
char new_record_string[25]=""; 

unsigned short paddle_last = 0;     // ultima posizione della barra
unsigned short paddle_curr = 0;			// posizione corrente della barra 
int ball_x; 												// posizione della pallina, asse x
int ball_y; 												// posizione della pallina, asse y
int ball_x_incr = 0; 								// incremento della posizione della pallina, asse x
int ball_y_incr = 0; 								// incremento della posizione della pallina, asse y
int right; 													// flag 1 se la pallina va verso destra 
int down; 													// flag 1 se la pallina va verso giù
int i; 															// indice per cicli for 
int j; 															// indice per cicli for 

void startGame(){
	// il gioco è iniziato
	game_status = Started; 

	// disegno il campo di gioco
	LCD_Clear(Black);
	drawGameField(); 
	
	// inizializzo la posizione della pallina e di quanto viene 
	// incrementata la sua posizione muovendosi
	ball_x = 225; 
	ball_y = 161;
	ball_x_incr = 5; 
	ball_y_incr = 5; 
	
	// inizializzo la direzione della pallina 
	down = 1; 
	right = 0; 
	
	// inizializzo il punteggio 
	score=0;
	
	// stampo il punteggio
	sprintf(score_string,"%d", score); 
	GUI_Text(7, 160, (uint8_t *) score_string, White, Black);
	
	// disegno la pallina
	for ( i=0; i<5 ; i++ ) 
		for ( j = 0; j< 5; j++) 
			LCD_SetPoint(ball_x+i, ball_y+j, Green); 
			
	// faccio partire i timer
	enable_timer(0); 		// per la barra (ADC)
	enable_timer(1);	 	// per la pallina
}

void endGame(){
	// il gioco è terminato
	game_status=Lost; 		
	
	reset_timer(1); 			
	disable_timer(1); 		// disablilito timer pallina
	
	reset_timer(0); 
	disable_timer(0); 		// disabilito timer paddle 
	
	// cancello la pallina
	for ( i=0; i<5 ; i++ ) 
		for ( j = 0; j< 5; j++) 
			LCD_SetPoint(ball_x+i, ball_y+j, Black); 
	
	// cancello la barra
	for ( i=0; i<10 ; i++ ) {
		LCD_DrawLine(paddle_curr, 276+i, paddle_curr+50, 276+i, Black);	// orizzontale in alto
	}
	
	// cancello il punteggio
	for ( i=0; i<17 ; i++ ) {
		LCD_DrawLine(6, 159+i, 35, 159+i, Black);	
	}
	
	// cancello l'eventuale record
	if(score==record && score>100){
		for ( i=0; i<20 ; i++ ) {
			LCD_DrawLine(100, 8+i, 230, 8+i, Black);	
		}
	}

	// stampo messaggi di fine partita 
	GUI_Text(80, 50, (uint8_t *) " YOU LOSE ", Red, Black);
	GUI_Text(20, 120, (uint8_t *) "Press INT0 and then KEY1", White, Black);
	GUI_Text(40, 140, (uint8_t *) "to start a new game", White, Black);
	sprintf(score_string,"YOUR SCORE: %d points", score); 
	GUI_Text(30, 220, (uint8_t *) score_string, White, Black);
	
	if(score==record && score>100) {
		sprintf(new_record_string, "NEW RECORD: %d points!", record); 
		GUI_Text(30, 240, (uint8_t *) new_record_string, Yellow, Black);
	}
	return; 
}

void pauseGame(void){
	game_status=Paused;			// il gioco è in pausa 
	disable_timer(0);				// disabilito timer per il paddle (ADC)
	disable_timer(1); 			// disabilito timer per la pallina 
	return; 
}

void resumeGame(void){
	game_status=Started; 		// il gioco ricomincia
	enable_timer(0); 				// riabilito timer per il paddle (ADC) 
	enable_timer(1); 				// riabilito timer per la pallina
	// cancello "GAME PAUSED" 
	for ( i=0; i<30 ; i++ ) {
		LCD_DrawLine(0, 290+i, 120, 290+i, Black);	
	}
}

void printWelcomeScreen(void){
	// disabilito timer nel caso in cui premo INT0
	// direttamente mentre sto giocando una partita
	reset_timer(1); 			
	disable_timer(1); 		// disablilito timer pallina
	
	reset_timer(0); 
	disable_timer(0); 		// disabilito timer paddle 
	
	// schermata iniziale con istruzioni e record attuale
	LCD_Clear(Black);

	GUI_Text(30, 50, (uint8_t *) "WELCOME PLAYER: Adriana", White, Black);
	GUI_Text(40, 120, (uint8_t *) "Press KEY1 to start", White, Black);
	GUI_Text(40, 150, (uint8_t *) "Press KEY2 to pause", White, Black);
	GUI_Text(40, 180, (uint8_t *) "Press INT0 to reset", White, Black);
	
	sprintf(new_record_string,"Actual record: %d points", record); 
	GUI_Text(20, 250, (uint8_t *) new_record_string, White, Black);
	
	game_status = NotStarted; 
}

void drawGameField(){
	// il campo è rappresentato da tre linee
	// una orizzontale in alto e due verticali, a destra e a sinistra
	for ( i=0; i<5 ; i++ ) {
		LCD_DrawLine(0, 0+i, 240, 0+i, Red);			// orizzontale in alto
		LCD_DrawLine(0+i, 0, 0+i, 276, Red);			// verticale sinistra 
		LCD_DrawLine(235+i, 0, 235+i, 276, Red); 	// verticale destra 
	}
	return;
}

void mooveBall(){
	if(game_status!=Started)
		return; 
	
	// cancelliamo la pallina nella vecchia posizione 
	for ( i=0; i<5 ; i++ ) 
		for ( j = 0; j< 5; j++) {
			if(ball_x+i>6 && ball_x+i<35 && ball_y+j>159 && ball_y+j<175){
				// la pallina era sopra il punteggio 
					; 
			}
			else if(record==score && score>100 && ball_x+i>100 && ball_x+i<230 && ball_y+j>8 && ball_y+j<28){
				// la pallina era sopra il record 
				;
			}
			else{
					LCD_SetPoint(ball_x+i, ball_y+j, Black); 
				}
			}

	// ha incontrato un muro ?
	if(ball_x<=230 && ball_x>=226 && right){				// muro a destra
		right = 0; 
		hitWall(); 		 
	}
	else if (ball_x>=5 && ball_x<=9 && !right){		// muro a sinistra
		right = 1; 
		hitWall(); 		
	}
	
	if(!down && ball_y==6){													// muro sopra
		// se stava andando a destra rimbalza a destra
		// idem se a sinistra, quindi non modifico right ma solo down
		down = 1;  
		hitWall(); 		 
	}
	
	// ha incontrato la barra ? 
	if(ball_y==271 && down) {
		// si trova all'altezza della barra 
		if(ball_x>paddle_curr && ball_x<paddle_curr+50){
			// si trova nella lunghezza della barra
			if(ball_x>paddle_curr+16 && ball_x<paddle_curr+34)
				// si trova al centro della barra
				ball_x_incr = 1; 
			else if ((ball_x>paddle_curr && ball_x<paddle_curr+8)||
				(ball_x>paddle_curr+42 && ball_x<paddle_curr+50))
				// si trova all'estremità della barra 
				ball_x_incr = 5;
			else 
				ball_x_incr = 3; 
			down=0; 
			increaseScore();
			for ( i=0; i<5 ; i++ ) 
				for ( j = 0; j< 5; j++) {
					LCD_SetPoint(ball_x+i, ball_y+j, Green); 
				}
			return; 
		} 
	}else if (ball_y>271){
		endGame();	// il giocatore ha perso 
		return; 
	} 
	
	// aggiorno la posizione della pallina 
	if(down){
		ball_y = ball_y + ball_y_incr; 
	} else{
		ball_y = ball_y - ball_y_incr; 
	}
	if(right){
		ball_x = ball_x + ball_x_incr; 
	}else{
		ball_x = ball_x - ball_x_incr; 
	}
	
	for ( i=0; i<5 ; i++ ) 
			for ( j = 0; j< 5; j++) {
				if(ball_x+i>6 && ball_x+i<35 && ball_y+j>159 && ball_y+j<175){
					// la pallina passa sopra il punteggio 
					; 
				}else if(score==record && score>100 && ball_x+i>100 && ball_x+i<230 && ball_y+j>8 && ball_y+j<28){
					// la pallina passa sopra il record
					;
				}
				else{
					LCD_SetPoint(ball_x+i, ball_y+j, Green); 
				}
			}	
	
	return; 
}


void moovePaddle(unsigned short last, unsigned short curr){
	if(game_status!=Started)
		return;
	
	paddle_last = last; 
	paddle_curr = curr; 
	
	// cancello la barra nell'ultima posizione 
	for ( i=0; i<10 ; i++ ) {
		LCD_DrawLine(paddle_last, 276+i, paddle_last+50, 276+i, Black);	
	}
	// disegno la barra nella nuova posizione 
	for ( i=0; i<10 ; i++ ) {
		LCD_DrawLine(paddle_curr, 276+i, paddle_curr+50, 276+i, Green);	
	}
	return;
}

void hitWall(void){
	init_timer(2, 2120); 		// produco tono basso
	enable_timer(2); 
}

void hitPaddle(void){
	init_timer(2, 1062); 		// producto tono alto
	enable_timer(2); 
}

void increaseScore(){
	// incremento punteggio
	if(score>=100){
		score = score+10; 
	}else{
		score = score + 5; 
	}
	
	// stampo nuovo punteggio 
	sprintf(score_string,"%d", score); 
	GUI_Text(7, 160, (uint8_t *) score_string, White, Black);
	
	// produco suono
	hitPaddle(); 
	
	if(score>record){
		// aggiorno record 
		record = score; 
		sprintf(new_record_string,"NEW RECORD: %d", record); 
		GUI_Text(100, 8, (uint8_t *) new_record_string, White, Black);
	}
}
