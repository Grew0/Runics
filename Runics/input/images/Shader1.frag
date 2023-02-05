uniform vec2 pl;
uniform vec2 coord;
uniform vec2 size;
uniform vec2 sizeof;
uniform sampler2D texture;

void main(){
	vec2 u = gl_TexCoord[0].xy;
	u *= sizeof;
	float r = length(u + coord - pl);
	r = min(max(0.0, -(100.0 - r) / 300.0), 1);
	u = gl_TexCoord[0].xy * sizeof + coord * 0.5;
	u.x = int(u.x) % int(size.x);
	u.y = int(u.y) % int(size.y);
	u /= size;
	vec4 px = texture2D(texture, u);
	px += (vec4(vec3(0), 1) - px) * r;
	gl_FragColor = px;
}