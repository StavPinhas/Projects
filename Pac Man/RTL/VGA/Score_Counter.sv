module Score_Counter (

  input  logic clk,
  input  logic resetN,
  input	logic stop_gameN,
  input  logic trigger,
  output logic [11:0] Score

);
	parameter DIGIT_CHANGE = 9; 
	parameter SECOND_DIGIT_CHANGE = 9; //time is 5 or score is 9
	int Score_reg1;
	int Score_reg2;
	int Score_reg3;


	always_ff @( posedge clk or negedge resetN or negedge stop_gameN) begin
	
		if ( (!resetN) || (!stop_gameN)) begin
			if(!resetN) begin
				Score_reg1 = 0;
				Score_reg2 = 0;
				Score_reg3 = 0;	
			end // if reset
		end
		
		else begin
				
			if (trigger == 1'b1) begin //pacman collected a coin or 1 second have passed
				
				if(Score_reg1 == DIGIT_CHANGE) begin 
				
					Score_reg1 <= 0;

					if (Score_reg2 == SECOND_DIGIT_CHANGE) begin
					
						Score_reg2 <= 0;
						Score_reg3 <= Score_reg3+1;
						
						if(Score_reg3 == DIGIT_CHANGE) begin
							Score_reg3 <= 0;
						end //3rd digit max
						
					end //2nd digit max
					
					else begin
						Score_reg2 <= Score_reg2+1;
					end //2rd digit regular
					
				end//1st digit max
				
				else begin
					Score_reg1 <= Score_reg1+1;
				end //1st digit regular	
				
		  end	//collision protocol
		end //resetN else
	end //always ff
	
   assign Score[3:0] = Score_reg1; //1st digit
	assign Score[7:4] = Score_reg2; //2nd digit
	assign Score[11:8] = Score_reg3; //3rd digit
	
  initial begin
    Score_reg1 = 0;
	 Score_reg2 = 0;
	 Score_reg3 = 0;
  end

endmodule






