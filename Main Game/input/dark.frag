uniform vec2 pl;
uniform vec2 xy;
uniform vec2 wh;
uniform float radius;

void main(){
	vec2 u = gl_TexCoord[0].xy;
	float r = length(u * wh + xy - pl);
	r = (radius - r) / radius;
	r = min(max(0.2, r), 1);
	gl_FragColor = vec4(0, 0, 0, 1-r);
}