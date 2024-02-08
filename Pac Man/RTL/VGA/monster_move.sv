


module	monster_move	(	
 
					input	logic	clk,
					input	logic	resetN,
					input	logic	startOfFrame,  // short pulse every start of frame 30Hz 
					input	logic	[1:0] direction_key,  //in which direction to move 
					input logic collision,  //collision if monster hits a wall or the background
					input	logic	[3:0] HitEdgeCode, //one bit per edge 

					output	 logic signed 	[10:0]	topLeftX, // output the top left corner 
					output	 logic signed	[10:0]	topLeftY
					
);

  

parameter int INITIAL_X = 500;
parameter int INITIAL_Y = 50;
parameter int INITIAL_X_SPEED = 0;
parameter int INITIAL_Y_SPEED = 0;
parameter int DEFAULT_SPEED = 50;

const int	FIXED_POINT_MULTIPLIER	=	64;


// movement limits 
const int   OBJECT_WIDTH_X = 32;
const int   OBJECT_HIGHT_Y = 32;
const int	SafetyMargin =	27; //just enough to hit the background frame

const int	x_FRAME_LEFT	=	(SafetyMargin)* FIXED_POINT_MULTIPLIER; 
const int	x_FRAME_RIGHT	=	(639 - SafetyMargin - OBJECT_WIDTH_X)* FIXED_POINT_MULTIPLIER; 
const int	y_FRAME_TOP		=	(SafetyMargin) * FIXED_POINT_MULTIPLIER;
const int	y_FRAME_BOTTOM	=	(479 -SafetyMargin - OBJECT_HIGHT_Y ) * FIXED_POINT_MULTIPLIER; //- OBJECT_HIGHT_Y

enum  logic [2:0] {IDLE_ST, // initial state
					MOVE_ST, // moving no colision 
					WAIT_FOR_EOF_ST, // change speed done, wait for startOfFrame  
					POSITION_CHANGE_ST,// position interpolate 
					POSITION_LIMITS_ST //check if inside the frame  
					}  SM_PS, 
						SM_NS ;

 int Xspeed_PS,  Xspeed_NS  ; // speed    
 int Yspeed_PS,  Yspeed_NS  ; 
 int Xposition_PS, Xposition_NS ; //position   
 int Yposition_PS, Yposition_NS ;  


 //---------
 
 always_ff @(posedge clk or negedge resetN)
		begin : fsm_sync_proc
			if (resetN == 1'b0) begin 
				SM_PS <= IDLE_ST ; 
				Xspeed_PS <= 0   ; 
				Yspeed_PS <= 0  ; 
				Xposition_PS <= INITIAL_X  ; 
				Yposition_PS <= INITIAL_Y   ;
			
			end 	
			else begin 
				SM_PS  <= SM_NS ;
				Xspeed_PS   <= Xspeed_NS    ; 
				Yspeed_PS    <=   Yspeed_NS  ; 
				Xposition_PS <=  Xposition_NS    ; 
				Yposition_PS <=  Yposition_NS    ;  
			end ; 
		end // end fsm_sync

 
 ///-----------------
 
 
always_comb 
begin
	// set default values 
		 SM_NS = SM_PS  ;
		 Xspeed_NS  = Xspeed_PS ; 
		 Yspeed_NS  = Yspeed_PS  ; 
		 Xposition_NS =  Xposition_PS ; 
		 Yposition_NS  = Yposition_PS  ;


	case(SM_PS)
//------------
		IDLE_ST: begin
//------------
		 Xspeed_NS  = INITIAL_X_SPEED ; 
		 Yspeed_NS  = INITIAL_Y_SPEED  ; 
		 Xposition_NS = INITIAL_X * FIXED_POINT_MULTIPLIER ; 
		 Yposition_NS = INITIAL_Y * FIXED_POINT_MULTIPLIER ; 

		 if (startOfFrame) 
				SM_NS = MOVE_ST ;
 	
	end
	
//------------
		MOVE_ST:  begin     // moving no colision 
//------------
	if(!collision) begin	
		if (direction_key == 2'b00) begin//  while moving up
			
				
			if((Yspeed_PS < 0 )) begin
				Yspeed_NS = Yspeed_PS  ; //keep moving up
				Xspeed_NS = 0  ;
			end
			
			else begin
				Yspeed_NS = -DEFAULT_SPEED; //change to up direction
				Xspeed_NS= 0;
			end
		end 
		
		if (direction_key == 2'b01) begin//  while moving down
			
			if((Yspeed_PS > 0 )) begin
					Yspeed_NS = Yspeed_PS  ; //keep moving down
					Xspeed_NS = 0  ;
			end	
			
			else begin
				Yspeed_NS = DEFAULT_SPEED; //change to down direction
				Xspeed_NS= 0;
			end	
		end
		
     if (direction_key == 2'b10) begin//  while moving right
		
			if((Xspeed_PS > 0 )) begin
					Yspeed_NS = 0  ;
					Xspeed_NS = Xspeed_PS  ; //keep moving right
			end
			
			else begin
				Yspeed_NS = 0;
				Xspeed_NS= DEFAULT_SPEED; //change to right direction
			end	
		end
		
		if (direction_key == 2'b11) begin//  while moving left
		 
			if((Yspeed_PS < 0 )) begin
				Yspeed_NS = 0  ;
				Xspeed_NS = Xspeed_PS  ; //keep moving left
						
			end
			
			else begin
				Yspeed_NS = 0;
				Xspeed_NS= -DEFAULT_SPEED; //change to left direction
			end	
		end 
	end	
	if (collision) begin  //any collision was detected 
				
			if (HitEdgeCode [2] == 1 ) begin  // hit top border of brick  
				if (Yspeed_PS < 0) // while moving up
						Yspeed_NS = DEFAULT_SPEED ; 
			end
			
			if ( HitEdgeCode [0] == 1 ) begin	// hit bottom border of brick  
				if (Yspeed_PS > 0 )//  while moving down
						Yspeed_NS = -DEFAULT_SPEED ; 
			end
								
			if (HitEdgeCode [3] == 1) begin   // hit left border of brick
				if (Xspeed_PS < 0 ) // while moving left
						Xspeed_NS = DEFAULT_SPEED ; // positive move right
			end	
									
			if ( HitEdgeCode [1] == 1 ) begin   // hit right border of brick  
					if (Xspeed_PS > 0 ) //  while moving right
						Xspeed_NS = -DEFAULT_SPEED  ;  // negative move left   
			end
			
			SM_NS = WAIT_FOR_EOF_ST ; 
	end 
	
		if (startOfFrame) 
			SM_NS = POSITION_CHANGE_ST ; 
end 
				
//--------------------
		WAIT_FOR_EOF_ST: begin  // change speed already done once, now wait for EOF 
//--------------------
									
			if (startOfFrame) 
				SM_NS = POSITION_CHANGE_ST ; 
		end 

//------------------------
 		POSITION_CHANGE_ST : begin  // position interpolate 
//------------------------
	
			 Xposition_NS =  Xposition_PS + Xspeed_PS; 
			 Yposition_NS  = Yposition_PS + Yspeed_PS ;
			 
	    
				SM_NS = POSITION_LIMITS_ST ; 
		end
		
		
//------------------------
		POSITION_LIMITS_ST : begin  //check if still inside the frame 
//------------------------
		
		
				 if (Xposition_PS < x_FRAME_LEFT ) 
						begin  
							Xposition_NS = x_FRAME_LEFT; 

						end ; 
	
				 if (Xposition_PS > x_FRAME_RIGHT) 
						begin  
							Xposition_NS = x_FRAME_RIGHT; 
 
						end ; 
							
				if (Yposition_PS < y_FRAME_TOP ) 
						begin  
							Yposition_NS = y_FRAME_TOP; 

						end ; 
	
				 if (Yposition_PS > y_FRAME_BOTTOM) 
						begin  
							Yposition_NS = y_FRAME_BOTTOM; 
						end ;

			SM_NS = MOVE_ST ; 
			
		end
		
endcase  // case 
end		
//return from FIXED point  trunc back to prame size parameters 
  
assign 	topLeftX = Xposition_PS / FIXED_POINT_MULTIPLIER ;    
assign 	topLeftY = Yposition_PS / FIXED_POINT_MULTIPLIER ;
  

	

endmodule	
//---------------
 
