//-- feb 2021 add all colors square 
// (c) Technion IIT, Department of Electrical Engineering 2021


module	back_ground_draw	(	

					input	logic	clk,
					input	logic	resetN,
					input 	logic	[10:0]	pixelX,
					input 	logic	[10:0]	pixelY,

					output	logic	[7:0]	BG_RGB,
					output	logic		boardersDrawReq 
);

const int	xFrameSize	=	640;
const int	yFrameSize	=	480;
const int	bracketOffset =	32;
const int   COLOR_MARTIX_SIZE  = 16*8 ; // 128 

logic [2:0] redBits;
logic [2:0] greenBits;
logic [1:0] blueBits;
logic [10:0] shift_pixelX;


localparam logic [2:0] DARK_COLOR = 3'b111 ;// bitmap of a dark color
localparam logic [2:0] LIGHT_COLOR = 3'b000 ;// bitmap of a light color

 
localparam  int RED_TOP_Y  = 156 ;
localparam  int RED_LEFT_X  = 256 ;
localparam  int GREEN_RIGHT_X  = 32 ;
localparam  int BLUE_BOTTOM_Y  = 300 ;
localparam  int BLUE_RIGHT_X  = 200 ;
 
parameter  logic [10:0] COLOR_MATRIX_TOP_Y  = 100 ; 
parameter  logic [10:0] COLOR_MATRIX_LEFT_X = 100 ;

 
 
always_ff@(posedge clk or negedge resetN)
begin
	if(!resetN) begin
				redBits <= DARK_COLOR ;	
				greenBits <= DARK_COLOR  ;	
				blueBits <= DARK_COLOR ;	 
	end 
	else begin

	// defaults 
		greenBits <= 3'b111 ; 
		redBits <= 3'b111 ;
		blueBits <= 2'b11;//LIGHT_COLOR;
		boardersDrawReq <= 	1'b0 ; 
		
		if (((pixelX == bracketOffset) && (pixelY >= bracketOffset) && (pixelY <= yFrameSize-bracketOffset))||
						((pixelY == bracketOffset) && (pixelX >= bracketOffset) && (pixelX <= xFrameSize-bracketOffset)) ||
						(pixelX == (xFrameSize-bracketOffset) && (pixelY >= bracketOffset) && (pixelY <= yFrameSize-bracketOffset)) || 
						((pixelY == (yFrameSize-bracketOffset)) && (pixelX >= bracketOffset) && (pixelX <= xFrameSize-bracketOffset)))
			begin 
					redBits <= DARK_COLOR ;	
					greenBits <= DARK_COLOR  ;	
					blueBits <= DARK_COLOR ;
					 boardersDrawReq <= 	1'b1; // pulse if drawing the boarders 
			end
	
		
	BG_RGB =  {redBits , greenBits , blueBits} ; //collect color nibbles to an 8 bit word 
			


	end; 	
end 
endmodule

