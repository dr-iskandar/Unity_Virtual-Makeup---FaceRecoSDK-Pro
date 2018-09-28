// Copyright (c) 2017 
//
// Permission is hereby granted,to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

using UnityEngine;
using System.Collections;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Resources;
using System.IO;
namespace milook{
	public class MilookEngine : MonoBehaviour {
		// Expression index
		readonly static int NUM_FFT_26		  =	26;		// output 26 facial feature points
		readonly static int NUM_POSE_6		  =	 6;		// size of face pose array
		readonly static int NUM_FACE_PROPERTY =	 6;		// face property: x,y,z, ax,ay,az
		readonly static int NUM_EXPRESSION	  =	18;		// number of expressions
		readonly static int NUM_FFT_68		  =	68;		// output 68 facial feature points
		readonly static int NUM_MASK_MODEL_78 =	78;		// 78 points 3d mask model
		readonly static int EM_NUM_TOTAL	  = 21;
		readonly static int NUM_MASK_VERTEX	  = 112;
			
		public enum FaceEX{
			 EX_E_CLOSE_SYM	=	0,		// EX1 - Symmetric eye close
			 EX_E_CLOSE_R	=	1,		// EX2 - Right eye close
			 EX_E_CLOSE_L	=	2,		// EX3 - Left eye close
			 EX_E_OPEN_SYM	=	3,		// EX4 - Symmetric wide eye open
			 EX_B_RAISE_SYM	=	4,		// EX5 - Symmetric eyebrow raise
			 EX_B_RAISE_R	=	5,		// EX6 - Right eyebrow raise
			 EX_B_RAISE_L	=	6,		// EX7 - Left eyebrow raise
			 EX_B_FURROW_SYM=	7,		// EX8 - Symmetric eyebrow furrow
			 EX_M_AH		=	8,		// EX9 - Ah-shape mouth open
			 EX_M_DIS		=	9,		// EX10 - Disgusted mouth shape
			 EX_M_DOWN		=	10,		// EX11 - Downward displacement of the mouth
			 EX_M_OH		=	11,		// EX12 - Oh-shaped mouth
			 EX_M_EH		=	12,		// EX13 - Eh-shaped mouth
			 EX_M_CLOSE_SMILE=	13,		// EX14 - Mouth-closed smile
			 EX_M_OPEN_SMILE =	14,		// EX15 - Mouth-open smile
			 EX_M_FROWN		 =	15,		// EX16 - Frown mouth shape
			 EX_M_PULL_RIGHT =	16,		// EX17 - Pull of the right mouth corner
			 EX_M_PULL_LEFT	 =	17,		// EX18 - Pull of the left mouth corner
		}
		// typedef
		// integer rectangle in an image
		public struct YSRect
		{ 
			public	int 	xMin;			/* x position top-left corner */
			public	int 	yMin;			/* y position top-left corner */
			public	int 	xMax;			/* x position bottom-right corner */
			public	int 	yMax;			/* y position bottom-right corner */
		}
		public struct YSNode
		{
			public	float	x;				/* x position */
			public	float	y;				/* y position */
			public	float	z;				/* z position, future use */
			public	int		id;				/* node ID */
			public	float	confidence;		/* confidence value */
		}

		//! face info in tracking
		[StructLayout(LayoutKind.Sequential)]
		public struct stFaceInfo
		{
			public	int			nIdFace;						//!< face id
			public	YSRect		rectFace;						//!< face rect
			public	float		fEyeDistance;					//!< distance of two pupils
			public	float		fConfidence;					//!< confidence value
			public	YSNode		nodeLP;							//!< left pupil point
			public	YSNode		nodeRP;							//!< right pupil point
			[MarshalAs(UnmanagedType.ByValArray, SizeConst = 78)]
			public	YSNode[]	pNodes;							//!< 78 facial feature points
			[MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
			public	float[]		pPose;							//!< face pose: Position: x, y, z, Euler angles: angleX, angleY, angleZ
			[MarshalAs(UnmanagedType.ByValArray, SizeConst = 21)]
			public	float[]		pExpressions;					//!< facial expressions
			[MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
			public	YSNode[]	p3dUserFace;					//!< 3d user face model
			public	int			nStatus;						//!< status are: 0 - FD result; 1 - FFD result; 2 - FFT result
		};

		public struct Vertex3
		{
			public	float	x;				/* x position */
			public	float	y;				/* y position */
			public	float	z;				/* z position, future use */
		}

		public struct TrackerData{
			public List<Vector3> node;
			public float[] facePose;
			public float[] exp;
			public List<Vector3> p3dnode;
			public List<Vector3> maskVertexes;

			public List<int> maskIndexs;
			public int mirror;
			public int videoRotation;
			public int error;
			public void reset(){
				node.Clear ();
				p3dnode.Clear ();
				maskVertexes.Clear ();
				maskIndexs.Clear ();
			}
		}
		public static bool isInit;
		public List<GameObject> listenerList;
		public delegate string callbackDelegate(IntPtr stFaceInfo,int num);
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport ("__Internal")]
		#else
		[DllImport ("MiloFace")]
		#endif
		private static extern int init( string path,int width,int height,int rotation,int mirror);

		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport ("__Internal")]
		#else
		[DllImport ("MiloFace")]
		#endif
		private static extern IntPtr updateTrackingData(ref Color32 i1, int i2);

		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport ("__Internal")]
		#else
		[DllImport ("MiloFace")]
		#endif
		private static extern int getFaceNum();

		//===========Mask=================
		#if UNITY_IPHONE && !UNITY_EDITOR
		[DllImport ("__Internal")]
		#else
		[DllImport ("MiloFace")]
		#endif
		private static extern IntPtr getMaskData();
		//===========Mask End============
		public static TrackerData trackerData;
		public const string TRACK_EVENT="OnTrack";
		private string configFileName = "model.bin";
		private static int staticVideoWidth,staticVideoHeight;
		public static MilookEngine instance; 
		private Color32[] pixels;
		public BasicSource source;
		void Start () {
			#if UNITY_EDITOR
			if (!File.Exists (getConfigURL())) {
				if(!Directory.Exists(Application.streamingAssetsPath)){
					Directory.CreateDirectory(Application.streamingAssetsPath);
				}
				File.Copy(Application.dataPath+"/FaceSDK/"+configFileName,Application.streamingAssetsPath+"/"+configFileName);
			}
			#endif
			if (instance != null) {
				Debug.LogError ("Repeat to create");
				GameObject.DestroyImmediate (gameObject);
				return;
			}
			instance = this;
			source.target = this;
			staticVideoWidth = source.GetWidth();
			staticVideoHeight = source.GetHeight();
			trackerData = new TrackerData ();
			trackerData.node = new List<Vector3> ();
			trackerData.facePose = new float[6];
			trackerData.exp = new float[21];
			trackerData.p3dnode = new List<Vector3>();
			trackerData.maskVertexes = new List<Vector3> ();
			trackerData.maskIndexs = new List<int> ();

			#if UNITY_ANDROID && !UNITY_EDITOR
				if (!File.Exists (Application.persistentDataPath + "/"+configFileName)) {
					StartCoroutine(load());
				}else{
					StartWork();
				}
			#else
				StartWork();
			#endif
		}
		/// <summary>
		/// Get video Height.
		/// </summary>
		public int GetHeight(){
			return staticVideoHeight;
		}
		/// <summary>
		/// Get video Width.
		/// </summary>
		public int GetWidth(){
			return staticVideoWidth;
		}
		/// <summary>
		/// Start.
		/// </summary>
		private void StartWork(){
			if (isInit) return;
			isInit = true;
			init (getConfigURL(), staticVideoWidth,  staticVideoHeight,source.getRotation(),source.getMirror() );
			Debug.Log ("Param:"+staticVideoWidth + " , "+  staticVideoHeight + ","+source.getRotation()+","+source.getMirror());

		}

		private IEnumerator load(){
			WWW w = new WWW ("jar:file://" + Application.dataPath + "!/assets/"+configFileName);
			yield return w;
			FileStream f =  File.Create(Application.persistentDataPath + "/"+configFileName);
			f.Write (w.bytes,0,w.bytes.Length);
			f.Close ();
			StartWork ();
		}


		
		/// <summary>
		/// Update engine pixel catche with pixels. 
		/// </summary>
		/// <param name="pixels">
		/// pixels data
		/// </param>
		public void UpdateWithSource(Color32[] pixels){
				IntPtr r = updateTrackingData (ref pixels [0], pixels.Length * 4);
				int faceNum = getFaceNum ();
				trackerData.error = faceNum > 0 ? 0 : 1;
				Debug.Log (faceNum+","+pixels[800]+","+staticVideoWidth+","+staticVideoHeight+","+pixels.Length * 4);
				OnTrackCall (r, faceNum);
				gameObject.BroadcastMessage (TRACK_EVENT, trackerData, SendMessageOptions.DontRequireReceiver);

				for (int i = 0; i < listenerList.Count; i++) {
					if (listenerList [i] == null) continue;
					listenerList [i].BroadcastMessage (TRACK_EVENT, trackerData, SendMessageOptions.DontRequireReceiver);
				}
		}
		
		/// <summary>
		/// Call back from c++ native code.
		/// </summary>
		/// <param name="stFaceInfo">
		/// face infomation.
		/// </param>
		/// <param name="num">
		/// face num.
		/// </param>
		string OnTrackCall(IntPtr stFaceInfo,int num)
		{
			if (num == 0) return "";
			IntPtr faceInfo = (IntPtr)Marshal.PtrToStructure (stFaceInfo, typeof(IntPtr));
			IntPtr newPtr = faceInfo;// new IntPtr (faceInfo.ToInt64 () + 0);
			stFaceInfo face = (stFaceInfo)Marshal.PtrToStructure (newPtr, typeof(stFaceInfo));
			int size= Marshal.SizeOf(face);
			for (int i = 0; i < num; i++) {
				newPtr = new IntPtr (faceInfo.ToInt64 () + i*size);
				face = (stFaceInfo)Marshal.PtrToStructure (newPtr, typeof(stFaceInfo));
				Analysis (face.pNodes, face.pPose, face.pExpressions, face.p3dUserFace);
			}

			//Mask
			IntPtr tempAddrName = getMaskData ();
			Vertex3 mask = (Vertex3)Marshal.PtrToStructure (tempAddrName, typeof(Vertex3));
			size= Marshal.SizeOf(mask);
			IntPtr ptr;
			float rate = 10f;
			for (int i = 0; i < NUM_MASK_VERTEX; i++) {
				ptr = new IntPtr (tempAddrName.ToInt64 () + i*size);
				mask = (Vertex3)Marshal.PtrToStructure (ptr, typeof(Vertex3));
				trackerData.maskVertexes.Add (new Vector3 ((mask.x-staticVideoWidth/2)*rate, mask.z,(staticVideoHeight/2-mask.y)*rate));
			}
			return "0";
		}
		
		/// <summary>
		/// Format The original data from engine
		/// </summary>
		private string Analysis( YSNode[] node,float[] facePose   ,float[] exp  ,YSNode[] unity_p3dUserFace)
		{
			trackerData.reset ();
			trackerData.mirror = source.getMirror ();// mirror[platformId];
			trackerData.videoRotation= source.getRotation();// trackRotation[platformId];
			for (int i = 0; i < NUM_MASK_MODEL_78; i++) {
				Vector3 temp = new Vector3 (node[i].x / staticVideoWidth, node[i].y / staticVideoHeight, 0);//node[i].z
				trackerData.node.Add(temp);
			}
			for (int i = 0; i < NUM_POSE_6; i++) {
				trackerData.facePose [i] = facePose[i];
			}
		
			for (int i = 0; i < EM_NUM_TOTAL; i++) {
				trackerData.exp [i] = trackerData.exp[i] * 0.7f + exp[i]*0.2f;
			}

			for (int i = 0; i < NUM_FACE_PROPERTY; i++) {
				trackerData.p3dnode.Add(new Vector3(unity_p3dUserFace[i].x,unity_p3dUserFace[i].y,unity_p3dUserFace[i].z));
			}
			return "0";
		}

		/// <summary>
		/// Transform coordinates
		/// </summary>
		void transformNode(){
			float offset =  Mathf.Max(staticVideoWidth, staticVideoHeight);
			trackerData.facePose[4] = -trackerData.facePose[4];
			trackerData.facePose[4] =  trackerData.facePose[2];

			trackerData.facePose[0] = trackerData.facePose[0] *offset;
			trackerData.facePose[1] = trackerData.facePose[1] *offset;
			trackerData.facePose[2] = trackerData.facePose[2] *offset;
		}

		/// <summary>
		/// Get the configure file url
		/// </summary>
		string getConfigURL(){
			if (Application.isEditor) {
				return  Application.dataPath +"/StreamingAssets/"+configFileName;
			}
			#if UNITY_IOS
			string filepath = Application.dataPath +"/Raw/"+configFileName;
			#elif UNITY_ANDROID
			string filepath = Application.persistentDataPath + "/"+configFileName;// "/storage/emulated/0/chiwell/milo_data.bin";// "jar:file://" + Application.dataPath + "!/assets/"+configFileName;
			#else
			string filepath = Application.streamingAssetsPath +"/"+configFileName;
			#endif
			return  filepath;
		}
		void OnDestroy(){
				instance = null;
			Debug.Log ("Destroy!");
		}
	}
}