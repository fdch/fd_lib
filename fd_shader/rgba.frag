uniform float K1;
uniform float K2;
uniform float K3;
uniform float K4;
void main (void) {
  vec4 c;
  c.r = K1;
  c.g = K2;
  c.b = K3;
  c.a = K4;
  gl_FragColor = c;
}