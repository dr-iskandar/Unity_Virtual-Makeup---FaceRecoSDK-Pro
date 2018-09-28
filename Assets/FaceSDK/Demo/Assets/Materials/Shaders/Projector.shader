// Upgrade NOTE: replaced 'mul(UNITY_MATRIX_MVP,*)' with 'UnityObjectToClipPos(*)'

// Upgrade NOTE: replaced '_Projector' with 'unity_Projector'
// Upgrade NOTE: replaced '_ProjectorClip' with 'unity_ProjectorClip'

Shader "VacuumShaders/Projector"
{
	Properties 
	{
		//Tag   
		[V_WIRE_Tag] _V_WIRE_Tag("", float) = 0 

		[HideInInspector] _Color("Color (RGB)", color) = (1, 1, 1, 1)
		[HideInInspector] _MainTex("Base (RGB)", 2D) = "white"{}		

		//Visual Options
		[V_WIRE_Title] _V_WIRE_Title_V_Options("Default Visual Options", float) = 0  

		[NoScaleOffset] _ShadowTex ("Color (RGB) Mask (A)", 2D) = "white" {}
		[NoScaleOffset] _FalloffTex ("FallOff (A)", 2D) = "" {}


		//Wire Options
		[V_WIRE_Title] _V_WIRE_Title_W_Options("Wire Visual Options", float) = 0  

		[Enum(Screen Space,0,Fixed,1)] _V_WIRE_FixedSize("Size", float) = 0
		[V_WIRE_PositiveFloat] _V_WIRE_Size("    Value", Float) = 0.5
		[HDR]_V_WIRE_Color("Color (HDR)", color) = (0, 0, 0, 1)
	}
	
	Subshader 
	{
		Tags { "Queue"="Transparent" } 
		 
		Pass 
		{
			ZWrite Off
			ColorMask RGB
			Blend SrcAlpha OneMinusSrcAlpha 
			Offset -1, -1
	
			CGPROGRAM
			#pragma vertex vert
			#pragma geometry geom
			#pragma fragment frag
			#pragma multi_compile_fog
			#include "UnityCG.cginc"
			
			#include "cginc/Wireframe_GS_Platform.cginc"


			sampler2D _ShadowTex;
			sampler2D _FalloffTex;
			float4x4 unity_Projector;
			float4x4 unity_ProjectorClip;
						
			
			#include "cginc/Wireframe_Core.cginc"


			struct vInput
			{
				float4 vertex : POSITION;
				half4 texcoord : TEXCOORD0;				
			};

			struct v2f_surf
			{
				float4 pos : SV_POSITION;
				float4 uvShadow : TEXCOORD0;
				float4 uvFalloff : TEXCOORD1;

				fixed3 custompack2 : TEXCOORD2;

				UNITY_FOG_COORDS(3)				
			};
			

			#include "cginc/Wireframe_GS.cginc"

			
			v2f_surf vert (vInput v)
			{
				v2f_surf o;


//Curved World Compatibility
//V_CW_TransformPoint(v.vertex);

				
				o.pos = UnityObjectToClipPos (v.vertex);
				o.uvShadow = mul (unity_Projector, v.vertex);
				o.uvFalloff = mul (unity_ProjectorClip, v.vertex);
				o.custompack2 = 0;

				UNITY_TRANSFER_FOG(o,o.pos);
				
				return o;
			}
			
			
			
			fixed4 frag (v2f_surf i) : SV_Target
			{				

				fixed4 projT = tex2Dproj (_ShadowTex, UNITY_PROJ_COORD(i.uvShadow));
				projT.a *= tex2Dproj (_FalloffTex, UNITY_PROJ_COORD(i.uvFalloff)).a;
								
				half value = ExtructWireframeFromMass(i.custompack2.xyz, 1);
				
				half4 res = projT * _V_WIRE_Color;
				res.a = lerp(projT.a * _V_WIRE_Color.a, 0, value);

			
				UNITY_APPLY_FOG(i.fogCoord, res);			


				return res;
			}
			ENDCG
		} 
	}
}
