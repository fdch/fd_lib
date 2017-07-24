uniform sampler2D background;
uniform sampler2D layer;
uniform float threshold, contrast;
uniform int type, showback;

varying vec2 texcoord1;
varying vec2 texcoord2;

void main (void) {
  vec4 back = texture2D(background, texcoord1);
  vec4 front = texture2D(layer, texcoord2); 
  vec3 grey = vec3(0.299, 0.587, 0.114);
  vec3 grey2 = vec3(0.2125,  0.7154, 0.0721);
  vec4 gray = vec4(dot(front.rgb,grey));
  front = mix(gray, front, contrast+1.);
  vec4 result;
  if (showback == 1) {
    result = back;
  } else {
  if (type == 0) {
    if ( (front.r > (back.r-threshold/2.))&&(front.r<(back.r+threshold/2.)) ) {
      result = vec4(0.);
    } else {
      result = front;
    }
  } else if (type == 1) {
    result = clamp(front,vec4(threshold),back);
  } else if (type == 2) {
    result = smoothstep(back*threshold,front, vec4(threshold));
  } else if (type == 3) {
    if ( front.r >= threshold ) {
      result = vec4(0.);
    } else {
      result = abs(back/front);
    }
  } else if (type == 4) {
    if ( (front.r > (back.r-threshold/2.))&&(front.r<(back.r+threshold/2.)) ) {
      result = vec4(0.);
    } else {
      result = vec4(vec3(dot(front.rgb, grey)),1.);
    }
  } else if (type == 5) {
    if ((back.r <= threshold)&&(front.r <= threshold)) {
      result = vec4(0.);
    } else {
      result = vec4(vec3(dot(front.rgb, grey)),1.)*contrast;
    }
  } else {
    result = front;
  }
  }
  gl_FragColor = result;
}