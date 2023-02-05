uniform vec4 col;

void main(){
	float r = (length(gl_TexCoord[0].xy - vec2(0.5, 0.5))) * 2.0;	
	col.a *= 1 - r;
	gl_FragColor = col;
}