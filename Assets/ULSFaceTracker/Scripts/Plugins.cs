using UnityEngine;
using System.IO;
using System.Collections;
using System.Runtime.InteropServices;
using System;
using AOT;

namespace ULSTrackerForUnity
{

	public static class Plugins
	{
		public const int MAX_TRACKER_POINTS = 30;

#if UNITY_STANDALONE || UNITY_EDITOR
		const string dll = "LibTrackerLite";
#else
	#if UNITY_IOS
		const string dll = "__Internal";
	#elif UNITY_ANDROID
		const string dll = "ulsCppTracker_native_lite";
	#endif
#endif

#region --C# Callbacks from Native--
		private static Texture2D _PreviewTexture = null;
		private static bool WaitForFirstFrame = true;

		[MonoPInvokeCallback(typeof(RenderCallback))]
		private static void Render (int request) {
			Dispatch.Dispatch (() => {
				GL.IssuePluginEvent (NativeRendererCallback (), request);
			});
		}

		[MonoPInvokeCallback(typeof(UpdateCallback))]
		private static void Update (int width, int height, IntPtr RGBA32GPUPtr, int rotate) {
			//Dispatch on main thread
			Dispatch.Dispatch(() => {
				if (WaitForFirstFrame) {
					//Initialization
					#if UNITY_STANDALONE || UNITY_EDITOR
					_PreviewTexture = _PreviewTexture ?? new Texture2D(width, height, TextureFormat.RGBA32, false);
					#else
					_PreviewTexture = _PreviewTexture ?? Texture2D.CreateExternalTexture (width, height, TextureFormat.RGBA32, false, false, RGBA32GPUPtr);
					#endif

					WaitForFirstFrame = false;
					if (OnPreviewStart!=null)
						OnPreviewStart(_PreviewTexture, rotate);
				} else {
					#if UNITY_STANDALONE || UNITY_EDITOR
					_PreviewTexture.LoadRawTextureData(RGBA32GPUPtr,width*height*4);
					_PreviewTexture.Apply();
					#else
					_PreviewTexture.UpdateExternalTexture(RGBA32GPUPtr);
					#endif

					if (OnPreviewUpdate!=null) {
						OnPreviewUpdate(_PreviewTexture);
					}
				}
			});
		}
#endregion

		#region ---Native Delegate Callbacks---
		public delegate void RenderCallback (int request);
		public delegate void UpdateCallback (int width, int height, IntPtr RGBA32GPUPtr, int rotate);
		#endregion

		#region --Events--
		public static Action<Texture,int> OnPreviewStart;
		public static Action<Texture> OnPreviewUpdate;
		#endregion

		private static CameraDispatch Dispatch = null;

        // import ULSee FaceTracker functions
#if UNITY_STANDALONE || UNITY_EDITOR

		[DllImport (dll)]
		private static extern int ULS_UnityTrackerInit(string pModelPath, string activateKey, string writable);
		[DllImport (dll)]
		private static extern void ULS_UnitySetSmoothing(bool use_smoothing);
		[DllImport (dll)]
		private static extern bool ULS_UnityGetSmoothing();
		[DllImport (dll)]
		private static extern void ULS_UnityTrackerRelease();

		[DllImport (dll)]
		public static extern IntPtr NativeRendererCallback();
		[DllImport (dll)]
		public static extern int ULS_UnityTrackerUpdate([In, Out] byte[] image, int width, int height);
		[DllImport (dll)]
		public static extern void ULS_UnityRegisterCallbacks (RenderCallback rCallback, UpdateCallback uCallback);
		[DllImport (dll)]
		public static extern int ULS_UnityCreateVideoCapture(int index, int width, int height, int fps, int rotate);//0:CCW, 1:flip, 2:CW
		[DllImport (dll)]
		public static extern void ULS_UnityUpdateVideoCapture();
		[DllImport (dll)]
		public static extern void ULS_UnityCloseVideoCapture();

		//<summary>
		///Initialize face tracker and camera.
		///</summary>
		public static int ULS_UnityTrackerInit() {
			#if UNITY_EDITOR
			string path = Application.dataPath + "/ULSFaceTracker/Plugins/model";
			#elif UNITY_STANDALONE_OSX
			string path = Application.dataPath + "/Plugins/"+dll+".bundle/Contents/Resources/model";
			#else //WINDOWS
			string path = Application.dataPath + "/Plugins/model";
			#endif
			ULS_UnityRegisterCallbacks(Render, Update);
			if (Dispatch == null) {
				Dispatch = new GameObject ("CameraDispatch").AddComponent<CameraDispatch> ();
			}
			int ret = ULS_UnityTrackerInit(path, "ulsee_for_unity", Application.temporaryCachePath);
			ULS_UnitySetSmoothing (true);
			return ret;
		}

		//<summary>
		///Change camera setting.
		///</summary>
		///<param name="width"> camera width </param>
		///<param name="height"> camera height </param>
		///<param name="fps"> frame rate per second </param>
		///<param name="frontal"> front camera or rear camera </param>
		public static void ULS_UnitySetupCamera(int width, int height, int fps, bool frontal) {
			if (0 == Plugins.ULS_UnityCreateVideoCapture (0, width, height, fps, -1)) {
				Dispatch.Clear();
				_PreviewTexture = null;
				WaitForFirstFrame = true;
			}
		}

		//<summary>
		///Enable camera flash light.
		///</summary>
		///<param name="enable"> enable flash light </param>
		public static void ULS_UnitySetFlashLight(bool enable) {
		}

		//<summary>
		///Get status of camera flash light.
		/// True: flash light is enabled.
		/// False: flash light is disabled.
		///</summary>
		public static bool ULS_UnityGetFlashLight() {
			return false;
		}

		//<summary>
		///Pause camera feed.
		///</summary>
		///<param name="paused"> enable pausing </param>
		public static void ULS_UnityPauseCamera (bool paused) {
			Dispatch.SetRunnig (!paused);
		}

		//<summary>
		///Terminate face tracker and camera.
		///</summary>
		public static void ULS_UnityTrackerTerminate() {
			ULS_UnityTrackerRelease();
			Dispatch.Clear();
			Dispatch = null;
			_PreviewTexture = null;
			WaitForFirstFrame = true;
		}

#elif UNITY_IOS

		[DllImport (dll)]
		private static extern void ULS_UnityRegisterCallbacks (RenderCallback rCallback, UpdateCallback uCallback);
		[DllImport (dll)]
		private static extern int ULS_UnityTrackerInitWithKey(string activateKey);
		[DllImport (dll)]
		private static extern void ULS_UnityDeleteTracker();
		[DllImport (dll)]
		private static extern void NativeCameraSetting(int w, int h, int fps, bool front);

		[DllImport(dll)]
		public static extern IntPtr NativeRendererCallback ();

		//<summary>
		///Initialize face tracker and camera.
		///</summary>
		public static int ULS_UnityTrackerInit() {
			if (Dispatch == null) {
				Dispatch = new GameObject ("CameraDispatch").AddComponent<CameraDispatch> ();
			}
			ULS_UnityRegisterCallbacks(Render, Update);
			int ret = ULS_UnityTrackerInitWithKey("ulsee_for_unity");
			return ret;
		}

		//<summary>
		///Pause camera feed.
		///</summary>
		///<param name="enable"> enable pausing </param>
		[DllImport (dll)]
		public static extern void ULS_UnityPauseCamera(bool enable);

		//<summary>
		///Enable camera flash light.
		///</summary>
		///<param name="enable"> enable flash light </param>
		[DllImport (dll)]
		public static extern void ULS_UnitySetFlashLight(bool enable);

		//<summary>
		///Get status of camera flash light.
		/// True: flash light is enabled.
		/// False: flash light is disabled.
		///</summary>
		[DllImport (dll)]
		public static extern bool ULS_UnityGetFlashLight();

		//<summary>
		///Change camera setting.
		///</summary>
		///<param name="width"> camera width </param>
		///<param name="height"> camera height </param>
		///<param name="fps"> frame rate per second </param>
		///<param name="frontal"> front camera or rear camera </param>
		public static void ULS_UnitySetupCamera(int width, int height, int fps, bool frontal) {
			NativeCameraSetting(width, height, fps, frontal);
			Dispatch.Clear();
			_PreviewTexture = null;
			WaitForFirstFrame = true;
		}

		//<summary>
		///Terminate face tracker and camera.
		///</summary>
		public static void ULS_UnityTrackerTerminate() {
			ULS_UnityDeleteTracker();
			Dispatch.Clear();
			_PreviewTexture = null;
			WaitForFirstFrame = true;
		}

#elif UNITY_ANDROID

		[DllImport("ulsCppTracker_unity_lite")]
		public static extern IntPtr NativeRendererCallback ();

		[DllImport("ulsCppTracker_unity_lite")]  
		private static extern void RegisterCallbacks(RenderCallback cb, UpdateCallback cb2);

		public static AndroidJavaObject A {
			get {
				if (NCNA == null) {
					using (AndroidJavaClass NCNAClass = new AndroidJavaClass("com.uls.multifacetrackerlib.UlsCamera")) {
						if (NCNAClass != null) {
							NCNA = NCNAClass.CallStatic<AndroidJavaObject>("instance");
						}
					}
				}
				return NCNA;
			}
		}

		private static AndroidJavaObject NCNA;
		private static int tracker_init = 0;

		//<summary>
		///Pause camera feed.
		///</summary>
		///<param name="enable"> enable pausing </param>
		public static void ULS_UnityPauseCamera (bool paused) {
			if (tracker_init == 0)
				return;
			if (paused) A.Call("SuspendProcess");
			else A.Call("ResumeProcess", true);
		}

		//<summary>
		///Initialize face tracker and camera.
		///</summary>
		public static int ULS_UnityTrackerInit() {
			if (Dispatch == null) {
				Dispatch = new GameObject ("CameraDispatch").AddComponent<CameraDispatch> ();
				Dispatch.SetApplicationFocus = (ULS_UnityPauseCamera);
			}

			Dispatch.Clear();
			// change default camera setting
			//A.Call<bool> ("SetupCamera", 1280, 720, 30, true);

			_PreviewTexture = null;
			WaitForFirstFrame = true;
			RegisterCallbacks (Render, Update);
			tracker_init = A.Call<int> ("initialize", "ulsee_for_unity");

			A.Call ("PlayPreview");
			return tracker_init;
		}

		//<summary>
		///Change camera setting.
		///</summary>
		public static void ULS_UnitySetupCamera(int width, int height, int fps, bool frontal) {
				if (A.Call<bool> ("SetupCamera", width, height, fps, frontal)) {
					if (OnPreviewStart!=null)
						OnPreviewStart(Texture2D.whiteTexture, 0);
					Dispatch.Clear();
					_PreviewTexture = null;
					WaitForFirstFrame = true;
					A.Call ("PlayPreview");
				}
		}

		//<summary>
		///Terminate face tracker and camera.
		///</summary>
		public static void ULS_UnityTrackerTerminate() {
				A.Call("SuspendProcess");
				if (OnPreviewStart!=null)
					OnPreviewStart(Texture2D.whiteTexture, 0);
				Dispatch.Clear();
				_PreviewTexture = null;
				WaitForFirstFrame = true;
				A.Call("TerminateOperations");
				//Dispatch = null;
				NCNA = null;
		}

		//<summary>
		///Enable camera flash light.
		///</summary>
		///<param name="enable"> enable flash light </param>
		public static void ULS_UnitySetFlashLight(bool enable) {
			A.Call("SetFlashLight",enable);
		}

		//<summary>
		///Get status of camera flash light.
		/// True: flash light is enabled.
		/// False: flash light is disabled.
		///</summary>
		public static bool ULS_UnityGetFlashLight() {
			return A.Call<bool>("GetFlashLight");
		}

#endif

        [DllImport (dll)]
		public static extern int ULS_UnityGetPoints([In, Out] float[] points2d);

		[DllImport (dll)]
		public static extern int ULS_UnityGetConfidence( [In, Out] float[] conf );

		[DllImport (dll)]
		public static extern float ULS_UnityGetPitchRadians ();

		[DllImport (dll)]
		public static extern float ULS_UnityGetYawRadians ();

		[DllImport (dll)]
		public static extern float ULS_UnityGetRollRadians ();

		[DllImport (dll)]
		public static extern float ULS_UnityGetScaleInImage();

		//<summary>
		///Get current tracker points. When no face is detected, return 0.
		///</summary>
		[DllImport (dll)]
		public static extern int  ULS_UnityGetTrackerPointNum();

		//<summary>
		///Get transform matrix to align 3D tracking points with 2D tracking points.
		///</summary>
		///<param name="transform"> Output Trnasform matrix</param>
		///<param name="intrinsic_camera_matrix">
		/// Input intrinsic Camera Matrix,
		/// [focal_length_x, 0, image_center_x,
		///  0, focal_length_y, image_center_y,
		///  0,              0,              1]
		///</param>
		///<param name="distort_coeffs">
		/// Input vector of distortion coefficients (k_1, k_2, p_1, p_2[, k_3[, k_4, k_5, k_6]]) of 4, 5, or 8 elements. 
		/// If the vector is NULL/empty, the zero distortion coefficients are assumed.
		///</param>
		[DllImport (dll)]
		public static extern void  ULS_UnityGetTransform([In, Out] float [] transform, [In] float[] intrinsic_camera_matrix, [In] float[] distort_coeffs);

		//<summary>
		///Get 3d scale vector compare to original 3d model of tracker
		///</summary>
		///<param name="x"> X axis of scale vector. </param>
		///<param name="y"> Y axis of scale vector. </param>
		///<param name="z"> Z axis of scale vector. </param>
		[DllImport (dll)]
		public static extern void ULS_UnityGetScale3D([In, Out] float [] x, [In, Out] float [] y, [In, Out] float [] z);
		
		//<summary>
		///Get Field of View.
		///</summary>
		///<param name="intrinsic_camera_matrix">
		/// Input intrinsic Camera Matrix,
		/// [focal_length_x, 0, image_center_x,
		///  0, focal_length_y, image_center_y,
		///  0,              0,              1]
		///</param>
		///<param name="image_width"> Input image width. </param>
		///<param name="image_height"> Input image height. </param>
		///<param name="fovx"> Output field of view in degrees along the horizontal sensor axis. </param>
		///<param name="fovy"> Output field of view in degrees along the vertical sensor axis. </param>
		[DllImport (dll)]
		public static extern void ULS_UnityCalibration([In] float[] intrinsic_camera_matrix, float image_width, float image_height,
		[In, Out] float[] fovx, [In, Out] float[] fovy);

		//<summary>
		///Enable Tracker.
		///</summary>
		///<param name="isTracking"> Input enable tracker. </param>
		[DllImport (dll)]
		public static extern void ULS_UnityTrackerEnable ([In] bool isTracking);
	}
}