uniform vec2 pos;
uniform vec2 size;

bool good(in vec2 u){
	
}

void main(){
	vec2 u = gl_TexCoord[0].xy * size;
	if(good(u)){
		gl_FragColor = vec4(1, 1, 0, 1);
	}else{
		gl_FragColor = vec4(0, 1, 0, 1);
	}
}