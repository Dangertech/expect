#version 130

uniform sampler2D tex;
uniform float strength;
uniform float separation;

void main()
{
	vec2 texSize = textureSize(tex,0).xy;
	vec2 texCoord = gl_FragCoord.xy / texSize;
	if (strength <= 0.f)
	{
		gl_FragColor = texture(tex, texCoord);
		return;
	}
	 

	float sep = max(separation, 1);
	/*
	vec4 col = texture(tex, gl_TexCoord[0].xy);
	*/
	vec4 col = vec4(0,0,0,0);
	float count = 0.0;
	for (float i = -strength; i <= strength; i += 1.0)
	{
		for (float j = -strength; j <= strength; j += 1.0) 
		{
			col +=
				texture(tex,(gl_FragCoord.xy + (vec2(i, j) * sep)) / texSize);
			count += 1.0;
		}
	}
	/*
	col /= pow(strength*2.f+1.f, 2.f);
	*/
	col /= count;
	gl_FragColor = col;
}
