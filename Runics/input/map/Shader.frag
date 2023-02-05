uniform vec2 view;
uniform vec2 coord;
uniform vec2 size;
uniform vec2 sizeof;
uniform sampler2D texture;
uniform float coof;

void main(){
	vec2 u = gl_TexCoord[0].xy;
	u *= sizeof;
	float r = length(u + coord - view);
	r = min(max(0.0, -(100.0 - r) / 300.0), 1);
	u = gl_TexCoord[0].xy * sizeof + coord * coof;
	u.x = int(u.x) % int(size.x);
	u.y = int(u.y) % int(size.y);
	u /= size;
	gl_FragColor = texture2D(texture, u);

}