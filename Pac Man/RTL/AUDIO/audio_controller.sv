module	audio_controller	(	
			input	logic	clk,
			input	logic	resetN,
			input	logic	keypadValid,  
			input	logic [3:0] keyPad,
			input	logic	collision,
			input	logic	collision_mp,
			input	logic winDR,
			input	logic	loseDR,
			input logic OneSecPulse, 
			
			output logic EnableSound,
			output logic [3:0] frequency
			
);

enum  logic [3:0] {IDLE_ST, // initial state
					key_pressed_ST, // key is pressed 
					Hart_lose_ST, //all life lost,end game
					Hart2_lose_ST,
					Hart3_lose_ST,
					Hart4_lose_ST,
					Coin2_win_ST,
					Coin3_win_ST,
					Coin4_win_ST,
					collision_mp_ST,
					wall_collision_ST,
					Coin_win_ST //all coin finish, win
					}  SM_PS, 
						SM_NS ;
						
logic [0:2] timer_ns, timer_ps;
   
	
 always_ff @(posedge clk or negedge resetN )
		begin : fsm_sync_proc
			if (resetN == 1'b0)  begin 
				SM_PS <= IDLE_ST ;
				timer_ps <= 3'b0;	
				
			end 	
			else begin 
				SM_PS  <= SM_NS ;
				timer_ps <= timer_ns;
			end	 
		end // end fsm_sync
		
always_comb 
begin
	// set default values 
		 SM_NS = SM_PS;
		 timer_ns = timer_ps;		
		 EnableSound = 1'b0;
		 frequency[3:0] = 0;
	case(SM_PS)
//------------
		IDLE_ST: begin
//------------
		
		if (winDR) 
				SM_NS = Coin_win_ST ;
		else if(loseDR)
				SM_NS = Hart_lose_ST ;
		else if(collision_mp)
				SM_NS = collision_mp_ST ;
		else if(collision)
				SM_NS = wall_collision_ST ;		
		else if(keypadValid)
				SM_NS = key_pressed_ST ;
		
 	
	end
	
//------------
		key_pressed_ST:  begin     
//-----------
			EnableSound = keypadValid;
			frequency[3:0] = keyPad[3:0];
         
			if (!keypadValid)						
				SM_NS = IDLE_ST ; 
		 
			
	end 
				

//--------------------
		wall_collision_ST: begin  
//--------------------
			EnableSound = 1'b1;
		   frequency[3:0] = 4;
			
			if(!collision) begin
				if (OneSecPulse) begin 	
						SM_NS = IDLE_ST ;
				end	
					
			end
		end
//------------------------
 		collision_mp_ST : begin  
//------------------------
			EnableSound = 1'b1;
		   frequency[3:0] = 6;
			
			if (OneSecPulse) begin 	
					if (timer_ps == 3'b001) begin
						timer_ns = 3'b0;	
						SM_NS = IDLE_ST ;
					end
					else begin
						timer_ns = timer_ps + 3'b001 ;
					end
			end
		end
//------------------------
		Hart_lose_ST : begin  
//------------------------
			
			timer_ns = 3'b0;
			
				
			SM_NS = Hart2_lose_ST ; 
			
			
			 
			
		end
//------------------------
		Hart2_lose_ST : begin  
//------------------------
			
			EnableSound =1'b1;
			frequency[3:0] = 9;
			if (OneSecPulse) begin 	
					if (timer_ps == 3'b001) begin
						timer_ns = 3'b0;	
						SM_NS = Hart3_lose_ST ;
					end
					else begin
						timer_ns = timer_ps + 3'b001 ;
					end
			end
			
		end
		
		
//------------------------
		Hart3_lose_ST : begin  
//------------------------
			
			EnableSound =1'b1;
			frequency[3:0] = 7;
			
			if (OneSecPulse) begin 	
					if (timer_ps == 3'b001) begin
						timer_ns = 3'b0;
						SM_NS = Hart4_lose_ST ;
					end
					else begin
						timer_ns = timer_ps + 3'b001 ;
					end
			end
			
		end
		
//------------------------
		Hart4_lose_ST : begin  
//------------------------
			
			EnableSound =1'b1;
			frequency[3:0] = 5;
			
			if (OneSecPulse) begin 	
					if (timer_ps == 3'b001) begin
						timer_ns = 3'b0;
						SM_NS = Hart2_lose_ST ;
					end
					else begin
						timer_ns = timer_ps + 3'b001 ;
					end
			end
			
		end
					
//------------------------
		Coin_win_ST : begin  
//------------------------
			timer_ns = 3'b0;
			
				
			SM_NS = Coin2_win_ST ; 
		end
//------------------------
		Coin2_win_ST : begin  
//------------------------
			
			EnableSound =1'b1;
			frequency[3:0] = 5;
			if (OneSecPulse) begin 	
					if (timer_ps == 3'b001) begin
						timer_ns = 3'b0;	
						SM_NS = Coin3_win_ST ;
					end
					else begin
						timer_ns = timer_ps + 3'b001 ;
					end
			end
			
		end
		
		
//------------------------
		Coin3_win_ST : begin  
//------------------------
			
			EnableSound =1'b1;
			frequency[3:0] = 3;
			
			if (OneSecPulse) begin 	
					if (timer_ps == 3'b001) begin
						timer_ns = 3'b0;
						SM_NS = Coin4_win_ST ;
					end
					else begin
						timer_ns = timer_ps + 3'b001 ;
					end
			end
			
		end
		
//------------------------
		Coin4_win_ST : begin  
//------------------------
			
			EnableSound =1'b1;
			frequency[3:0] = 1;
			
			if (OneSecPulse) begin 	
					if (timer_ps == 3'b001) begin
						timer_ns = 3'b0;
						SM_NS = Coin2_win_ST ;
					end
					else begin
						timer_ns = timer_ps + 3'b001 ;
					end
			end
			
		end		
		
endcase  // case 
end		


endmodule	