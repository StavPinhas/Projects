module time_controller (


  input  logic clk,
  input  logic resetN,
  input logic [7:0] Time,
  output logic monster_type, //0 red, 1 for green
  output logic time_pulse

);

int flag;
int reg_pulse;

always_ff @( posedge clk or negedge resetN ) begin
	
		if ( !resetN ) begin
			flag <= 0;
			reg_pulse <= 0;	
		end // if reset
		
		else begin
			
			if (((Time[3:0] == 4'b0101) || (Time[3:0] == 4'b0)) && (flag == 0)) begin //pulse every 5 seconds
				reg_pulse <= 1;
				flag <= 1;
			end
				
			else begin
				reg_pulse <= 0;
				flag <= 0;
			end
			
		end
				
end


assign time_pulse = reg_pulse;

assign monster_type = (Time[7:4] == 4'b0101) ? 1'b1 : 1'b0 ; //monster is green every last 10 seconds of a minute

endmodule 