
// (c) Technion IIT, Department of Electrical Engineering 2021 
//-- Alex Grinshpun Apr 2017
//-- Dudy Nov 13 2017
// SystemVerilog version Alex Grinshpun May 2018
// coding convention dudy December 2018

//-- Eyal Lev 31 Jan 2021

module	objects_mux	(	
//		--------	Clock Input	 	
					input		logic	clk,
					input		logic	resetN,
					
				//win	
					input		logic	winDR, // two set of inputs per unit
					input		logic	[7:0] win_RGB, 
					
				//lose
					input		logic	loseDR,
					input		logic	[7:0] lose_RGB,
					
		   // pacman
					input		logic	pacmanDrawingRequest, 
					input		logic	[7:0] pacmanRGB, 
					     
		  //monster
					input		logic monsterDrawingRequest,
					input		logic [7:0] RGBMonster,
					
			//hearts
					input    logic HartDrawingRequest,
					input		logic	[7:0] hartRGB,
					
			//hammers
					input    logic HammerDR, 
					input		logic	[7:0] hammerRGB,	
					
			//score
					input		logic	[2:0]		scoreDrawingRequest,
					input		logic [23:0]	scoreRGB,
					
			//time
					input		logic	[2:0]		timeDR,					
					input		logic [23:0]	timeRGB,					
			
		
		  // walls 
					input    logic WallDrawingRequest,
					input		logic	[7:0] wallRGB,
		
		 // coins
					input    logic coinDrawingRequest, 
					input		logic	[7:0] coinRGB,
		

		//background
					input		logic	[7:0] backGroundRGB, 
					input		logic	BGDrawingRequest, 
					input		logic	[7:0] RGB_MIF,
		
		
					
			  
				   output	logic	[7:0] RGBOut
);

always_ff@(posedge clk or negedge resetN) begin
	if(!resetN) begin
			RGBOut	<= 8'b0;
	end
	
	else begin
	
		if (winDR == 1'b1 )   
			RGBOut <= win_RGB;  //first priority 


		else if (loseDR == 1'b1)
			RGBOut <= lose_RGB;
	
	
	
		else if (pacmanDrawingRequest == 1'b1 )   
			RGBOut <= pacmanRGB;


		 else if (monsterDrawingRequest == 1'b1)
				RGBOut <= RGBMonster;
		
 		else if (HartDrawingRequest == 1'b1)
				RGBOut <= hartRGB;
				
		else if (HammerDR == 1'b1)
			RGBOut <= hammerRGB;
				
		//digit after digit		
		else if ( scoreDrawingRequest[0] == 1'b1)
			RGBOut <= scoreRGB[7:0];
			
		else if ( scoreDrawingRequest[1] == 1'b1)
			RGBOut <= scoreRGB[15:8];
			
		else if ( scoreDrawingRequest[2] == 1'b1)
			RGBOut <= scoreRGB[23:16];
			
		else if ( timeDR[0] == 1'b1)
			RGBOut <= timeRGB[7:0];
			
		else if ( timeDR[1] == 1'b1)
			RGBOut <= timeRGB[15:8];
			
		else if ( timeDR[2] == 1'b1)
			RGBOut <= timeRGB[23:16];
				
		else if (WallDrawingRequest == 1'b1)
			RGBOut <= wallRGB;
						
		else if ( coinDrawingRequest ==1'b1)
			RGBOut <= coinRGB;
			
		else if (BGDrawingRequest == 1'b1)
				RGBOut <= backGroundRGB ;
				
		else 
			RGBOut <= RGB_MIF ;// last priority 
	end 
end

endmodule


