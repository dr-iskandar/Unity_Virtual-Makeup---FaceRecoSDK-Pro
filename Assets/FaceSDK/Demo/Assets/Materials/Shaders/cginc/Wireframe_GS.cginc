#ifndef VACUUM_WIREFRAME_GS_CGINC
#define VACUUM_WIREFRAME_GS_CGINC

[maxvertexcount(3)]
void geom(triangle v2f_surf input[3], inout TriangleStream<v2f_surf> triStream)
{
	input[0].custompack2.xyz = fixed3(1, 0, 0);
	input[1].custompack2.xyz = fixed3(0, 1, 0);
	input[2].custompack2.xyz = fixed3(0, 0, 1);


	triStream.Append(input[0]);
	triStream.Append(input[1]);
	triStream.Append(input[2]);

	triStream.RestartStrip();
}

#endif
