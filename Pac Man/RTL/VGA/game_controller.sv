

module	game_controller	(	
			input	logic	clk,
			input	logic	resetN,
			input	logic	startOfFrame,  // short pulse every start of frame 30Hz 
			input	logic	pacman_DR,
			input	logic	monster_DR,
			input	logic	wall_DR,
			input	logic	coin_DR,
			input	logic	Boarders_DR,
			input	logic	win_DR,
			input	logic	lose_DR,
			input	logic keypadValid,
			
			output logic collision,
	    	output logic collision_m,
		   output logic collision_mp,
			output logic collision_pc,
			output logic SingleHitPulse_m,
			output logic SingleHitPulse_mp,
			output logic SingleHitPulse_pc,
			output logic collision_wall,
			output logic stop_gameN
			
);

logic flag ; // a semaphore to set the output only once per frame / regardless of the number of collisions

 
enum  logic  [2:0] {IDLE_ST,
						  PLAYING_ST,
						  STOP_ST
						 }	SM_PS,
							SM_NS ;


always_ff @(posedge clk or negedge resetN) begin

	if(!resetN)	begin
		flag	<= 1'b0;
		SingleHitPulse_m <= 1'b0 ;
		SingleHitPulse_mp <= 1'b0 ;
		SingleHitPulse_pc <= 1'b0;
		SM_PS <= IDLE_ST;
	end 
	
	else begin 
			SM_PS  <= SM_NS ;
			SingleHitPulse_m <= 1'b0 ; // default
			SingleHitPulse_mp <= 1'b0 ;// default
			SingleHitPulse_pc <= 1'b0 ;// default
			
			if(startOfFrame) 
				flag <= 1'b0 ; // reset for next time 


		
			if ((collision_m)  && (flag == 1'b0)) begin 
				flag	<= 1'b1; // to enter only once 
				SingleHitPulse_m <= 1'b1 ; 
			end ; 
			
			if ((collision_mp)  && (flag == 1'b0)) begin 
				flag	<= 1'b1; // to enter only once 
				SingleHitPulse_mp <= 1'b1 ; 
			end ;
			
			if ((collision_pc)  && (flag == 1'b0)) begin 
				flag	<= 1'b1; // to enter only once 
				SingleHitPulse_pc <= 1'b1 ; 
			end ; 
			
	end 
end

 
always_comb begin

	// set default values 
		 SM_NS = SM_PS;
		 
		collision_mp = 0;
		collision = ( pacman_DR &&  wall_DR ) || ( pacman_DR &&  Boarders_DR ) ;
		collision_m = ( monster_DR &&  wall_DR ) || ( monster_DR &&  Boarders_DR ) ;
		collision_pc = ( pacman_DR &&  coin_DR );
		collision_wall = 	pacman_DR &&  wall_DR; //for wall breaking
		 
		 case(SM_PS)
//------------		 
		IDLE_ST: begin
//------------
		stop_gameN = 1'b1;	//don't stop

		
		if(keypadValid) //started playing
			SM_NS = PLAYING_ST;

	end

//------------
		PLAYING_ST: begin
//------------
		stop_gameN = 1'b1;
		
		collision_mp = ( monster_DR &&  pacman_DR );
			
		if( win_DR || lose_DR) //if player won or lost
			SM_NS = STOP_ST;
			
		else if (collision_mp) //so the pacman won't die more than 1 time in starting position
			SM_NS = IDLE_ST;
	end
	
//------------
		STOP_ST:  begin     // end of game
		
//-----------
		stop_gameN = 1'b0;
	

	end
	
	endcase

end

endmodule
