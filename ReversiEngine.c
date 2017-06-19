#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


struct rusage usage; // a structure to hold "resource usage" (including time)
struct timeval start, end; // will hold the start and end times

int findSmarterMove(char board[][26], int n, char colour, int *row, int *col);
int findSmartestMove(char board[][26], int n, char colour, int *row, int *col);

//creates a starting board
void generateStartBoard(int n, char newBoard[][26]){
   int row,col;
   for(row = 0; row < n; ++row){
      for( col = 0; col < n; ++col){
         //first special row
         if((row == n/2 -1 && col == n/2 )|| (row == n /2 && col == n/2-1)){
            newBoard[row][col] = 'B';
         }
         else if ((row == n/2 && col == n/2) || (row == n/2-1 && col == n/2 -1))
            newBoard[row][col] = 'W';
         else
            newBoard[row][col] = 'U';
         }
   }
}


//prints board
void printBoard(char board[][26], int n){
   int row, col,i;
   printf("  ");
   for(i = 0; i < n; ++i){
      printf("%c", 'a'+ i);
   }
   printf("\n");
   for(row = 0; row < n; ++row){
      printf("%c ", 'a' + row);
      for(col = 0; col < n; ++col){
         printf("%c", board[row][col]);
      }
      printf("\n");//new line so board looks like a board
  }
}

//returns opposite colour W->B B->W
char oppositeColour(char colour){
   if(colour == 'W')
      return 'B';
   return 'W';
}

//checks to see if (row,col) is in bounds
bool positionInBounds(char board[][26], int n, char row, char col){
  if((row-'a')>=0 && (col-'a') >=0 && (row-'a') < n && (col - 'a') < n){
     return true;
  }
  return false;
}


//checks to see if move is legal in specified direction
bool checkLegalInDirection(char board[][26], int n, char row, char col,
  char colour, int deltaRow, int deltaCol){
    
    int i = deltaRow;
    int j = deltaCol;
    if(board[row-'a'][col-'a'] == 'U' && board[row-'a' +i][col-'a'+j] == oppositeColour(colour)){
    while(positionInBounds(board,n,row+i,col+j)){
          if(board[row-'a'][col-'a'] == oppositeColour(colour) &&
            board[row-'a'+deltaRow][col-'a'+deltaCol] == colour){
               return true;
           }
          if(board[row-'a'+deltaRow][col-'a'+deltaCol]=='U'){
              return false; 
          }
          row+=deltaRow;
          col+=deltaCol;
       }
    }
        return false;
   
}
    


//checks to see if a move is valid
bool isValidMove(char board[][26], int n, char colour, char row, char col){
  if(board[row-'a'][col-'a'] == 'U' &&(
     checkLegalInDirection(board, n, row, col, colour,1,0)
  || checkLegalInDirection(board, n, row, col, colour,-1,0)
  || checkLegalInDirection(board, n, row, col, colour,0,1)
  || checkLegalInDirection(board, n, row, col, colour,0,-1)
  || checkLegalInDirection(board, n, row, col, colour,1,1)
  || checkLegalInDirection(board, n, row, col, colour,-1,1)
  || checkLegalInDirection(board, n, row, col, colour,1,-1)
  || checkLegalInDirection(board, n, row, col, colour,-1,-1))){
    return true;
  }
  return false;
}

//gets a list of valid moves for a board and for player of specified colour
char** getValidMoves(char board[][26], int n, char colour, int * validMoves){
   int numValidMoves = 0;
   char i,j;
   char** moveList = malloc(sizeof(char) * numValidMoves);
   for(i = 'a'; i < n+'a'; ++i){
      for(j = 'a'; j < n+'a'; ++j){
         if(isValidMove(board,n,colour, i,j)){

            numValidMoves++;
            moveList = realloc(moveList, sizeof(char*) * numValidMoves);
            if(moveList == NULL){
               printf("moveList has failed to reallocate\n");
            }
            else{
               //printf("hi\n");
               moveList[numValidMoves - 1] = malloc(sizeof(char)*2);
               moveList[numValidMoves-1][0] = i;
               moveList[numValidMoves-1][1] = j;
             }
         }
      }
   }
   *validMoves = numValidMoves;
   return moveList;
}

//switches tiles where move is excecuted 
void switchTiles(char board[][26], int n, char colour, char row, char col,
int deltaRow, int deltaCol){
   int i = row - 'a'+deltaRow;
   int j = col - 'a'+deltaCol;
   while(positionInBounds(board,n, i+'a',j+'a')){
      board[i][j] = colour;
      i += deltaRow;
      j += deltaCol;
      if(board[i][j] ==colour){
        break;
      }
   }
}

//excecutes move at (row,col)
void executeMove(char board[][26], int n, char colour, char row, char col){
  int deltaRow, deltaCol;
  for(deltaRow = -1; deltaRow <= 1; ++deltaRow){
     for(deltaCol = -1; deltaCol <= 1; ++deltaCol){
        if(!(deltaCol ==0 && deltaRow ==0)){
           if(checkLegalInDirection(board, n, row, col, colour,deltaRow,deltaCol)){
              switchTiles(board, n, colour, row, col,deltaRow, deltaCol);
           }
         }
      }
    }
  board[row-'a'][col-'a'] = colour;
}

//checks to see if board is full
bool boardIsFull(char board[][26], int n){
   int i,j;
   for(i = 0; i < n; ++i){
      for(j = 0; j < n; ++j){
         if(board[i][j] == 'U'){
           return false;
         }
      }
   }
   return true;
}

//calculates a weighted score of tiles across board
int getScore(char board[][26], int n, char colour, bool gameOver){
   int score = 0;
   int i,j;
   for( i = 0; i < n; ++i){
      for(j = 0; j < n; ++j){
        
         if(board[i][j] == colour){
            if((i == 0 && j ==0) || (i ==0 && j == n-1)
            ||(i == n-1 && j == 0)||(i ==n-1 && j == n-1) && ! gameOver){//corners
               score += n*n*n*n;
            }
            else if((i == 1 && j ==1) || (i ==1 && j == n-2)
            ||(i == n-2 && j == 1)||(i ==n-2 && j == n-2)){
               score -= n*n*n;
            }
            
            else if((i == 0 || j == 0 || i == n-1 || j == n -1) && n>=16){ 
               score += n;
            }
            else if((i == 0 || j == 0 || i == n-1 || j == n -1)){ 
               score += n;
            }
            
            
            else if((i == 1 || j == 1 || i == n-2 || j == n -2) && n >= 8 ){//&& n > 14){ //&& n <= 8){
              score += n/2;
            }
          
            else{//normal piece
               score += 1;
            }
         }
      }
      
   }
   return score;
}

//counts real score value of tiles across a board
int getWinner(char board[][26], int n, char colour){
  int i ,j;
  int score = 0;
  for( i = 0; i < n; ++i){
     for(j = 0; j < n; ++j){
        if(board[i][j] == colour) score +=1;
      }
    }
    return score;
}

//returns max of two values
int max(int a, int b){
  if(a>b){
     return a;
  }
  else
     return b;
}

//returns min of two values
int min(int a, int b){
   if(a < b){
     return a;
   }
   return b;
}

//generates a new board state
void generateState(char board[][26], char newBoard[][26],int n , char row, char col,char colour){


   char i,j;
   for(i = 'a'; i < n +'a'; ++i){
      for(j = 'a'; j < n +'a'; ++j){
         if(!(i == row && j == col)){
            newBoard[i-'a'][j-'a'] = board[i-'a'][j-'a'];
         }
      }
   }
   executeMove(newBoard, n, colour,row,col);


}

//finds the optimal move at specified depth, is actually alphaBeta. Name is kinda a misnomer (sorry!)
int miniMax(char boardState[][26], int n, int depth, int startDepth, int alpha, int beta,char colour,bool isMaxing
  , char* moveRow, char* moveCol){

   char row, col;
   int i ,numValidMoves,numValidMovesOpp;
   char** moveList = getValidMoves(boardState, n, colour, &numValidMoves);
   getValidMoves(boardState, n, colour, &numValidMovesOpp);
   if(numValidMoves > 27 && n > 16){//cut off to ensure time is okay
       numValidMoves = 27;
    }
   else if(numValidMoves > 24 && n > 20){
       numValidMoves = 24;
   }
   
   
   if(depth == 0 && startDepth % 2 != 0){

      return
      getScore(boardState,n,oppositeColour(colour),false)
      -getScore(boardState,n,colour, false);//fix gameIsOver
   }
   
   else if (isMaxing){
      //int numValidMoves, i;
      int maxScore = -10000000;
      int prevScore;


      for(i = 0 ; i < numValidMoves; ++i){

         row = moveList[i][0];
         col = moveList[i][1];
         char newBoard[n][26];
         generateState(boardState, newBoard,n, row, col, colour);//generate new board
         prevScore = maxScore;
         maxScore = max(maxScore,
           miniMax(newBoard, n, depth - 1, startDepth, alpha,beta,oppositeColour(colour), false, moveRow, moveCol));
         
         alpha = max(alpha, maxScore);
            if(depth == startDepth && maxScore != prevScore){//possible highest move//makes next move maximal move
               *moveRow = row;
               *moveCol = col;
            }
         if(beta <= alpha){//checks to see if number is out of range
             return beta;
         }
      }
      free(moveList);//free memory
      return maxScore;
    }
      
   
   else{
     int minScore = 10000000;



     for(i = 0 ; i < numValidMoves; ++i){
        row = moveList[i][0];
        col = moveList[i][1];
        char newBoard[n][26];
        generateState(boardState, newBoard,n, row, col, colour);
        minScore = min(minScore, miniMax(newBoard, n, depth - 1, startDepth,alpha, beta,oppositeColour(colour), true, moveRow, moveCol));
        beta = min(beta , minScore);
        if(beta <= alpha){
            return alpha;
        }
     }
     free(moveList);
     return minScore;
   }



}


//Process computer takes to make a move
void computerMove(char board[][26], int n, char colour, bool* computerDone){
 
  int numValidMovesComp;
  char compMoveRow, compMoveCol;
  int depth = 3;//depth changes for minimax based on amount of time I can afford
  if(n < 8 ){
      depth = 3;
  }
  else if(n < 10){
      depth = 5;
  }
  
  /*getrusage(RUSAGE_SELF, &usage);
  start = usage.ru_utime;
  double timeStart = start.tv_sec +
  start.tv_usec / 1000000.0; // in seconds*/
  getValidMoves(board,n,colour,&numValidMovesComp);//get number of valid moves
  if(numValidMovesComp > 0){
      

     //if you have a move exceute minimax and make a move
     miniMax(board,n,depth,depth,-100000,100000,colour,true, &compMoveRow, &compMoveCol);
     printf("Computer places %c at %c%c.\n", colour, compMoveRow,compMoveCol);
     executeMove(board, n, colour, compMoveRow, compMoveCol);
     printBoard(board,n);
  }
  else{
     printf("%c player has no valid move.\n", colour);
     *computerDone = true;

   }
}


//allows the player to make a move
void playerMove(char board[][26], int n, char colour, bool* playerDone, bool* stop){
  int numValidMovesPlayer = 0;
  int playerMoveRow, playerMoveCol;
  
  getValidMoves(board,n,colour,&numValidMovesPlayer);
  if(numValidMovesPlayer > 0){
     
      printf("Enter move for colour %c (RowCol): ", colour);
     char inputStr[2];
     scanf("%s", inputStr);
     playerMoveRow = inputStr[0];
     playerMoveCol = inputStr[1];

  
     if(!(isValidMove(board,n,colour,playerMoveRow,playerMoveCol))){
         printf("Invalid move.\n");
         *stop = true;

     }
     else{
         executeMove(board,n,colour, playerMoveRow,playerMoveCol);
         printBoard(board,n);
     }
    }
   else{
      printf("%c player has no valid move.\n", colour);
      *playerDone = true;
     }
   
}

int main(void){
  int n;
  char colour;
  bool computerFirst = false;
  bool computerDone = false;
  bool playerDone = false;
  bool stop = false;



  printf("Enter the board dimension: ");
  scanf("%d", &n);
  printf("Computer plays (B/W) : ");
  scanf(" %c", &colour);
  if(colour == 'B'){
    computerFirst = true;
  }

  char board[n][26];

  generateStartBoard(n,board);
  printBoard(board,n);

  //the game
  
    while(!(boardIsFull(board,n)) && !(computerDone && playerDone) &&!stop){



     if(computerFirst){
        if(!stop){
        computerMove(board,n,colour, &computerDone);
      }
        playerMove(board,n,oppositeColour(colour), &playerDone,&stop);
         }
     else{
       playerMove(board,n,oppositeColour(colour), &playerDone,&stop);
       if(!stop && !(boardIsFull(board,n))){
       computerMove(board,n,colour, &computerDone);
        }
         }
      if(boardIsFull(board,n)){
        computerDone = true;
        playerDone == true;
      }

    }



  //win conditions
  if(stop){
     printf("%c player wins.\n", colour);
  }
  else if (getWinner(board, n, colour) >
  getWinner(board, n, oppositeColour(colour))){
     printf("%c player wins.\n", colour);
  }
  else if(getWinner(board, n, colour) <
  getWinner(board, n, oppositeColour(colour))){
     printf("%c player wins.\n", oppositeColour(colour));
  }
  else{
      printf("Draw!\n");
  } 
}
