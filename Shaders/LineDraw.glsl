#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


#ifdef VERTEX_SHADER

attribute vec2 aPosition;


void main()
{
	gl_Position = vec4(aPosition, 0, 1);
}

#endif

#ifdef PIXEL_SHADER

uniform vec4 uColor;

void main()
{
	gl_FragColor = uColor;
}
#endif
