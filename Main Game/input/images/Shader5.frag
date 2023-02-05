uniform vec2 pl;
uniform vec2 coord;
uniform vec2 size;
uniform sampler2D texture;

void main(){
	vec2 u = gl_TexCoord[0].xy;
	//u.x *= size.x;
	//u.y *= size.y;
	//float r = length(u + coord - pl);
	//r = pow(max(0, (100 - r) / 150.0), 2);
	//vec4 px = texture2D(texture, gl_TexCoord[0].xy);
	//px = vec4(min(1.0, px.x+r), min(1.0, px.y+r), min(1.0, px.z+r), 1.0);
	//gl_FragColor = px;
    gl_FragColor = vec4(u.xy, 0, 1);
}
