//Shader "Unlit/FaceShader"
//{
//	Properties
//	{
//		_MainTex ("Texture", 2D) = "white" {}
//	}
//	SubShader
//	{
//		Tags { "RenderType"="Opaque" }
//		LOD 100
//
//		Pass
//		{
//			CGPROGRAM
//			#pragma vertex vert
//			#pragma fragment frag
//			// make fog work
//			#pragma multi_compile_fog
//			
//			#include "UnityCG.cginc"
//
//			struct appdata
//			{
//				float4 vertex : POSITION;
//				float2 uv : TEXCOORD0;
//			};
//
//			struct v2f
//			{
//				float2 uv : TEXCOORD0;
//				float4 vertex : SV_POSITION;
//				float2 varTexcoord:TEXCOORD1;
//                float4 color:TEXCOORD2;
//			};
//
//			sampler2D _MainTex;
//			float4 _MainTex_ST;
//			//-----function----
//			vec3 getVector(vec3 point, vec3 pos, vec3 dir , float offset , float radius){
//                      point.z=0.0;
//                      pos.z=0.0;
//                      float a = distance(point,pos);
//                     
//                      if(a>=radius) return vec3(0,0,0);
//                      float w =(radius-a)*offset;
//                      dir.x*=w;
//                      dir.y*=w;
//                      return dir;
//                  }
//                  
//                  vec3 getVectorEye(vec3 point1,vec3 point2, vec3 pos, vec3 dir , float offset , float radius){
//                      point1.z=0.0;
//                      point2.z=0.0;
//                      pos.z=0.0;
//                      float a = distance(point1,pos) + distance(point2,pos);
//                      
//                      if(a>=radius) return vec3(0,0,0);
//                      float w =(radius-a)*offset;
//                      dir.x*=w;
//                      dir.y*=w;
//                      return dir;
//                  }
//                  
//                  vec3 transFace(vec3 pos,int idx){
//                      int faceRect = 17*idx;
//                      vec3 result=pos;
//                      vec3 v0 = vec3(points[0+faceRect].x,points[0+faceRect].y,0);
//                      vec3 v16 = vec3(points[16+faceRect].x,points[16+faceRect].y,0);
//                      vec3 vLeft = v16-v0;
//                      normalize(vLeft);
//                      
//                      vec3 vRight = v0-v16;
//                      normalize(vRight);
//                      for(int i=1;i<16;i++){
//                          if(i==8) continue;
//                          vec3 m1 = points[i+1+faceRect]-points[i+faceRect];
//                          vec3 m2 = points[i-1+faceRect]-points[i+faceRect];
//                          
//                          vec3 dir =  m1+m2;
//                          if(dir.x==0.0 && dir.y==0.0){
//                              dir =i<8 ? vRight : vLeft;
//                          }
//                          vec3 value = getVector(points[i+faceRect],result,dir,weight[i]*deformRatio,faceRadius[idx]);
//                          result = result + value;
//                      }
//                      return result;
//                  }
//                  
//                  vec3 transEye(vec3 d,int idx){
//                      vec3 pos=d;
//                      vec3 tempPos=vec3(0.0,0.0,0.0);
//                      //-------------eye-------------
//                      vec3 dir = pos-leftEyeCenter[idx];
//                      normalize(dir);
//                      tempPos = tempPos+getVectorEye(leftEyeCenter1[idx],leftEyeCenter2[idx],pos,dir,eyeDeformRatio,eyeRadius[idx]*2.0);
//                      
//                      pos = tempPos+pos;
//                      tempPos=vec3(0.0,0.0,0.0);
//                      dir = pos-rightEyeCenter[idx];
//                      normalize(dir);
//                      tempPos = tempPos + getVectorEye(rightEyeCenter1[idx],rightEyeCenter2[idx],pos,dir,eyeDeformRatio,eyeRadius[idx]*2.0);
//                      
//                      pos = tempPos+pos;
//                      //-------------end eye------------
//                      return pos;
//                  }
//			//----end function------
//			v2f vert (appdata v)
//			{
//				v2f o;
//				o.vertex = UnityObjectToClipPos(v.vertex);
//				o.uv = TRANSFORM_TEX(v.uv, _MainTex);
//
//
//				//----------------
//				 o.color = vec4(1.0,1.0,1.0,1.0);
//                      vec3 pos = position.xyz;
//                      vec3 oPos = position.xyz;
//                      pos.x = pos.x / meshHeight;
//                      pos.y = pos.y / meshWidth ;
//                      
//                      float blX =pos.y+0.5;
//                      float blY =pos.x+0.5;
//                      if(blX<0.01 || blX>0.99 || blY<0.01 || blY>0.99) {
//                          pos.x = pos.x * meshHeight;
//                          pos.y = pos.y * meshWidth ;
//                          gl_Position	= MVP_MATRIX * vec4(pos,1);\
//                          varTexcoord = texcoord;\
//                          return;
//                      }
//                     
//                      ////---------------face---------------
//                      if(faceEnable!=0){
//                          for(int i=0;i<faceNum;i++){
//                              pos = transFace(pos,i);
//                          }
//                      }
//                      
//                      if(eyeEnable !=0){
//                          for(int i=0;i<faceNum;i++){
//                              pos = transEye(pos,i);
//                          }
//                      }
//
//                      pos.x = pos.x * meshHeight;
//                      pos.y = pos.y * meshWidth ;
//                      float wp= distance(pos,oPos) * 0.1;
//                      if(wp>=0.01){
//                          o.color = vec4(wp,0.0,1.0-wp,1.0);
//                      }
//                      o.vertex= UnityObjectToClipPos(vec4(pos,1));
//                      o.varTexcoord = texcoord;\
//				//---------------
//				return o;
//			}
//			
//			fixed4 frag (v2f i) : SV_Target
//			{
//				// sample the texture
//				fixed4 col = tex2D(_MainTex, i.uv);
//				// apply fog
//				return col;
//			}
//			ENDCG
//		}
//	}
//}
//
//
//
////                  varying vec2 varTexcoord;\
////                  varying vec4 color;\
//                 
//                  uniform float meshWidth;
//                  uniform float meshHeight;
//                  uniform float videoWidth;
//                  uniform float videoHeight;
//                  uniform float meshScale;
//                  
//                  uniform int faceEnable;
//                  uniform int eyeEnable;
//                  uniform float deformRatio;
//                  uniform float eyeDeformRatio;
//                  uniform float weight[17];
//                  uniform float eyeWeight[6];
//
//                  const int maxFace=MAX_FACE;
//                  
//                  uniform int faceNum;
//                  //----
//                  uniform float faceRadius[maxFace];\
//                  uniform float eyeRadius[maxFace];\
//                  uniform vec3 points[17*maxFace];\
//                  
//                  uniform vec3 leftEyePoints[6*maxFace];\
//                  uniform vec3 leftEyeCenter[maxFace];\
//                  uniform vec3 leftEyeCenter1[maxFace];\
//                  uniform vec3 leftEyeCenter2[maxFace];\
//                  
//                  uniform vec3 rightEyePoints[6*maxFace];\
//                  uniform vec3 rightEyeCenter[maxFace];\
//                  uniform vec3 rightEyeCenter1[maxFace];\
//                  uniform vec3 rightEyeCenter2[maxFace];\
//                  //----
//                  
//
//                  
//                  void main (void)\
//                  {\
//                      color = vec4(1.0,1.0,1.0,1.0);
//                      vec3 pos = position.xyz;
//                      vec3 oPos = position.xyz;
//                      pos.x = pos.x / meshHeight;
//                      pos.y = pos.y / meshWidth ;
//                      
//                      float blX =pos.y+0.5;
//                      float blY =pos.x+0.5;
//                      if(blX<0.01 || blX>0.99 || blY<0.01 || blY>0.99) {
//                          pos.x = pos.x * meshHeight;
//                          pos.y = pos.y * meshWidth ;
//                          gl_Position	= MVP_MATRIX * vec4(pos,1);\
//                          varTexcoord = texcoord;\
//                          return;
//                      }
//                     
//                      ////---------------face---------------
//                      if(faceEnable!=0){
//                          for(int i=0;i<faceNum;i++){
//                              pos = transFace(pos,i);
//                          }
//                      }
//                      
//                      if(eyeEnable !=0){
//                          for(int i=0;i<faceNum;i++){
//                              pos = transEye(pos,i);
//                          }
//                      }
//
//                      pos.x = pos.x * meshHeight;
//                      pos.y = pos.y * meshWidth ;
//                      float wp= distance(pos,oPos) * 0.1;
//                      if(wp>=0.01){
//                          color = vec4(wp,0.0,1.0-wp,1.0);
//                      }
//                      gl_Position	= MVP_MATRIX * vec4(pos,1);\
//                      varTexcoord = texcoord;\
//                  }
