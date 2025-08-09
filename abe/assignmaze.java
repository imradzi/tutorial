/*
  NAME          : Ahmad Tarmizi bin Mohd Radzi
  MATRIC NUMBER : 2019285
  SECTION       : 2

  INDIVIDUAL ASSIGNMENT - Maze Generator
*/
package assignment;
import java.util.Stack;
import java.util.Random;

public class assignmaze{
    private final static int rows = 9;
    private final static int cols = 9;
    private Random random = new Random();
    private Cell[][] cell;
    private Stack<Cell> path = new Stack<Cell>();
    
    public void printMaze() throws Exception{
        generateMaze();
        for(int x = 0; x < rows; x++) {
        	for(int y = 0; y < cols; y++) {
        		
        	}
        }
    }

    public void generateMaze(){
        cell = new Cell[rows][cols];
        int i, j;
        String dir = "nsew";
        System.out.print(path.size());
        
        //assigning coordinates in each cell
        for(i = 0; i < rows; i++){
            for(j = 0; j < cols; j++){
                cell[i][j] = new Cell(i, j);
            }
        }
        
        //Randomising starting cell
        i = random.nextInt(rows); 
        if(i > 0 && i < rows-1){
            do
                j = random.nextInt(cols);
            while(j > 0 && j < cols-1);

        }else
            j = random.nextInt(cols);
            
        cell[i][j].setFirst(true);
        cell[i][j].setVisited(true);        
        path.push(cell[i][j]);
        
        //carving a path
		while (!path.empty()) {
        do {
        	switch(dir.charAt(random.nextInt(dir.length()))) {
        		case 'n': //up = 0
        			if(j < cols-1 && !cell[i][j+1].getVisited()) {
        				j+=1;
        				//cell[i][j].setWall(0, false);
        			}
        			break;
        		case 's': //down = 2
        			if(j > 0 && !cell[i][j-1].getVisited()) {
        				j-=1;
        				//cell[i][j].setWall(2, false);
        			}
        			break;
        		case 'e'://right = 1
        			if(i < rows-1 && !cell[i+1][j].getVisited()) {
        				i+=1;
        				//cell[i][j].setWall(1, false);
        			}
        			break;
        		case 'w': //left = 3
        			if(i > 0 && !cell[i-1][j].getVisited()) {
        				i-=1;
        				//cell[i][j].setWall(3, false);
        			}
        			break;
        	}
        	cell[i][j].setVisited(true);
        	path.push(cell[i][j]);
        	
        }while(path.size()<cols*rows);
		}
    }

    class Cell{
        private int x, y;
        private boolean visited = false, first = false;
        private boolean[] wall = {true, true, true, true};
        
        Cell(){
        	this.setX(0);
        	this.setY(0);
        }
        
        Cell(int x, int y){
        	this.setX(x);
        	this.setY(y);
        }
        
        public boolean getVisited() {
        	return visited;
        }
        
        public boolean getFirst() {
        	return first;
        }
        
        public boolean getWall(int i) {
        	return wall[i];
        }
        
        public void setVisited(boolean visited) {
        	this.visited = visited;
        }
        
        public void setFirst(boolean first) {
        	this.first = first;
        }
        
        public void setWall(int i, boolean wall) {
        	this.wall[i] = wall;
        }

		public Integer getX() {
			return x;
		}

		public void setX(Integer x) {
			this.x = x;
		}

		public Integer getY() {
			return y;
		}

		public void setY(Integer y) {
			this.y = y;
		}
		
		public boolean checkAdjacent(int x, int y) {
			boolean n = false, e = false, s = false, w = false;
			
			if(x == 0) {
				if(y ==  0){
					e = cell[x + 1][y].getVisited();
					w = cell[x - 1][y].getVisited();
					n = cell[x][y + 1].getVisited();
					s = cell[x][y - 1].getVisited();
				}else if(y == cols){
					e = cell[x + 1][y].getVisited();
					w = cell[x - 1][y].getVisited();
					n = cell[x][y + 1].getVisited();
					s = cell[x][y - 1].getVisited();
				}else {
					
				}
			}else if(x == rows){
				w = cell[x - 1][y].getVisited();
				n = cell[x][y + 1].getVisited();
				s = cell[x][y - 1].getVisited();
			}else {
				e = cell[x + 1][y].getVisited();
				w = cell[x - 1][y].getVisited();
				n = cell[x][y + 1].getVisited();
				s = cell[x][y - 1].getVisited();
			}
			return false;
		}
    }

    public static void main (String[] args) throws Exception{
        assignmaze myMaze = new assignmaze();
		System.out.println("Printing");
        myMaze.printMaze();
    }
}



