#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#define field_height 40
#define field_width 158
#define true 1
#define false 0
#define SNAKE_CHAR '0'
#define FOOD_CHAR '+'
#define BORDER_CHAR '#'
#define DEFAULT_SPEED 300
#define MAX_SPEED 10
#define BOOST_SPEED 5
#define MIN_SPEED 1000

char field[field_height][field_width];
char direction;
int endGame = false;
int snakeSpeed = DEFAULT_SPEED;
int score = 0;
int gtime = 0;

// snake body structure

typedef struct Snake{
    int coord_x, coord_y;
    struct Snake* next;
} Snake;

Snake *tail, *head;


void initializeField();
void drawField();
void initializeSnake();
void moveSnake();
void getKey();
void generateFood();
void gameOverScreen();
void printStats();

int main(){

    initscr();
    keypad(stdscr, true);
    nodelay(stdscr, true);
    cbreak();
    noecho();
    srand(time(NULL));
    curs_set(0);
    
    initializeField();
    initializeSnake();
    generateFood();
    drawField();
    
    
   
    while (!endGame){
		getKey();
		moveSnake();
		clear();	
		drawField();
        napms(snakeSpeed);
        gtime+=snakeSpeed;
    }
	
	nodelay(stdscr, false);
    gameOverScreen();
    endwin();


}

// init

void initializeField(){
    for (int i = 0; i < field_height; i++) {
        for (int j = 0; j < field_width; j++) {
            if ((i == 0) || (i == field_height - 1) || (j == 0) || (j == field_width - 1)){
                field[i][j] = BORDER_CHAR;
            } else{
                field[i][j] = ' ';
            }
        }
    }
};

// drawField :)

void drawField(){
    for (int i = 0; i < field_height; i++) {
        for (int j = 0; j < field_width; j++) {
            printw("%c", field[i][j]);
        }
        printw("\n");
    };
    printw("|| Time: %d%d:%d%d || Score: %d || Use arrow keys to change direction or speed up/slow down. Press \"q\" to exit.", (gtime / 60000) / 10, 
    (gtime / 60000) % 10, ((gtime / 1000) % 60) / 10, ((gtime / 1000) % 60) % 10, score);
};

// creating snake

void initializeSnake(){
    tail = (Snake*)malloc(sizeof(Snake));
    tail->coord_x = field_width / 2;
    tail->coord_y = field_height / 2;
    field[tail->coord_y][tail->coord_x] = SNAKE_CHAR;
    head = (Snake*)malloc(sizeof(Snake));
    tail->next = head;
    head->next = NULL;
    head->coord_x = tail->coord_x + 1;
    head->coord_y = tail->coord_y;
    field[head->coord_y][head->coord_x] = SNAKE_CHAR;
    direction = 'r';
};

// changing snake coordinates

void moveSnake(){

	Snake* temp = tail;	
	int bufx = tail->coord_x;
	int bufy = tail->coord_y;

    while (temp != head){
        temp->coord_x = temp->next->coord_x;
        temp->coord_y = temp->next->coord_y;
        temp = temp->next;
    }

    switch (direction){
        case 'u':
            head->coord_y-=1;
            if (field[head->coord_y][head->coord_x] == BORDER_CHAR){
                head->coord_y = field_height - 2;
            }
            break;
        case 'd':
            head->coord_y+=1;
            if (field[head->coord_y][head->coord_x] == BORDER_CHAR){
                head->coord_y = 1;
            }
            break;
        case 'l':
            head->coord_x-=1;
            if (field[head->coord_y][head->coord_x] == BORDER_CHAR){
                head->coord_x = field_width - 2;
            }
            break;
        case 'r':
            head->coord_x+=1;
            if (field[head->coord_y][head->coord_x] == BORDER_CHAR){
                head->coord_x = 1;
            }
            break;
        default:
            printw("Unexpected error!");
    };
    
    if (field[head->coord_y][head->coord_x] == FOOD_CHAR){
		temp = (Snake*)malloc(sizeof(Snake));
		temp->coord_y = bufy;
		temp->coord_x = bufx;
		temp->next = tail;
		tail = temp;	
		generateFood();
		score++;
		if (snakeSpeed > MAX_SPEED){
			snakeSpeed-=BOOST_SPEED;
		};
	} else if (field[head->coord_y][head->coord_x] == SNAKE_CHAR){ 
		endGame = true;
	} else {
		field[bufy][bufx] = ' ';	
	}
	
    field[head->coord_y][head->coord_x] = '0';
    
    
}

// key processing

void getKey(){

    int keyPressed = getch();
    switch (keyPressed){
        case KEY_UP:
            if (direction == 'u'){
            	if (snakeSpeed > MAX_SPEED){
            		snakeSpeed-=BOOST_SPEED;
            	};
            } else if (direction == 'd'){
            	if (snakeSpeed < MIN_SPEED){
            		snakeSpeed+=BOOST_SPEED;
            	};
            } else {
            	direction = 'u';
            };
            break;
        case KEY_DOWN:
            if (direction == 'd'){
            	if (snakeSpeed > MAX_SPEED){
            		snakeSpeed-=BOOST_SPEED;
            	};
            } else if (direction == 'u'){
            	if (snakeSpeed < MIN_SPEED){
            		snakeSpeed+=BOOST_SPEED;
            	};
            } else {
            	direction = 'd';
            };
            break;
        case KEY_LEFT:
            if (direction == 'l'){
            	if (snakeSpeed > MAX_SPEED){
            		snakeSpeed-=BOOST_SPEED;
            	};
            } else if (direction == 'r'){
            	if (snakeSpeed < MIN_SPEED){
            		snakeSpeed+=BOOST_SPEED;
            	};
            } else {
            	direction = 'l';
            };
            break;
        case KEY_RIGHT:
            if (direction == 'r'){
            	if (snakeSpeed > MAX_SPEED){
            		snakeSpeed-=BOOST_SPEED;
            	};
            } else if (direction == 'l'){
            	if (snakeSpeed < MIN_SPEED){
            		snakeSpeed+=BOOST_SPEED;
            	};
            } else {
            	direction = 'r';
            };
            break;
        case 'q':
        	endGame = true;
    }
	
};

// food generation

void generateFood(){
	int generated = false;
	while (!generated){
		int i = 1 + rand() % (field_height - 2);
		int j = 1 + rand() % (field_width - 2);
		if (!(field[i][j] == '0')){
			generated = true;
			field[i][j] = FOOD_CHAR;
		}
	}
}

// some mad art skills

void gameOverScreen(){
	clear();
	printw("##############################################################################################################################################################\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                          ################         ########         #             #     ##############         #########    #         #                     #\n");
	printw("#                          #                       #        #        ##           ##     #                      #       #     #       #                      #\n");
	printw("#                          #                      #          #       # #         # #     #                      #       #      #     #                       #\n");
	printw("#                          #                     #            #      #  #       #  #     #                      #       #       #   #                        #\n");
	printw("#                          #                    #              #     #   #     #   #     #                      #       #        # #                         #\n");
	printw("#                          #                    #              #     #    #   #    #     #                      #########         #                          #\n");
	printw("#                          #                    #              #     #     # #     #     ##############                                                      #\n");
	printw("#                          #     ##########     ################     #      #      #     #                      #########    ###########                     #\n");
	printw("#                          #     #        #     #              #     #             #     #                      #            #         #                     #\n");
	printw("#                          #              #     #              #     #             #     #                      #            #         #                     #\n");
	printw("#                          #              #     #              #     #             #     #                      #########    ###########                     #\n");
	printw("#                          #              #     #              #     #             #     #                      #            ###                             #\n");
	printw("#                          ################     #              #     #             #     ##############         #            #  ##                           #\n");
	printw("#                                                                                                               #########    #    ######                     #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("#                                                                                                                                                            #\n");
	printw("##############################################################################################################################################################\n");
	printw("Score: %d", score);
	getch();
}

