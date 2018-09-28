//#define USE_UNITY_WEBCAM

using UnityEngine;
using System;
using System.Collections;
using System.Threading;
using System.Runtime.InteropServices;
using Queue = System.Collections.Generic.Queue<System.Action>;

namespace ULSTrackerForUnity {
	
	[StructLayout(LayoutKind.Explicit)]
	public struct Color32Bytes
	{
		[FieldOffset(0)]
		public byte[] byteArray;

		[FieldOffset(0)]
		public Color32[] colors;
	}

	public sealed class CameraDispatch: MonoBehaviour {

		public bool isRunning {get {return running;}}

		private Thread mainThread;
		private Queue invocation, execution;

		private readonly object queueLock = new object();
		private volatile bool running;

		public System.Action<bool> SetApplicationFocus = null;

#if UNITY_STANDALONE || UNITY_EDITOR

		#if USE_UNITY_WEBCAM
		WebCamTexture camTexture;
		WebCamDevice camDevice;
		Color32Bytes colorData;
		bool WaitForFirstFrame = true;
		#endif//USE_UNITY_WEBCAM

		void Start() {
			#if USE_UNITY_WEBCAM
			colorData = new Color32Bytes ();
			camTexture = PrepareWebCamTexture (ref camDevice, true, 640, 480);
			camTexture.Play ();
			#else
			int ret = Plugins.ULS_UnityCreateVideoCapture (0, 640, 480, 60, -1);
			Debug.Log ("Initialize Desktop Camera: "+ret);
			#endif//USE_UNITY_WEBCAM
		}
		private void OnDisable() {
			#if !USE_UNITY_WEBCAM
			Plugins.ULS_UnityCloseVideoCapture ();
			#endif
		}

#endif//UNITY_STANDALONE || UNITY_EDITOR

		void OnApplicationPause (bool paused) {
			if (SetApplicationFocus!=null)
				SetApplicationFocus(paused);
		}

		public void SetRunnig(bool enable) {
			running = enable;
		}

		#region --Ctor & Dtor--

		//private CameraDispatch () {}
		public CameraDispatch() {
			mainThread = Thread.CurrentThread;
			invocation = new Queue ();
			execution = new Queue ();
			running = true;
		}

		~CameraDispatch () {
			running = false;

			invocation.Clear(); 
			execution.Clear();

			invocation = execution = null;
			mainThread = null;
		}

		#endregion

		#region --Dispatching--

		public void Clear() {
			invocation.Clear(); 
			execution.Clear();
		}

		public void Dispatch (Action action) {
			//Check that we aren't already on the target thread
			if (Thread.CurrentThread.ManagedThreadId == mainThread.ManagedThreadId) {
				//Debug.Log("Dispatch Execute");
				action();
			}
			//Enqueue
			else lock (queueLock) {
				execution.Enqueue(action);
			}
		}
			
		//private void Update (Camera unused) {
		void Update() {
			//Lock
			lock (queueLock) {
				while (execution.Count>0) {
					var exe = execution.Dequeue();
					exe ();
				}
			}

			#if UNITY_STANDALONE || UNITY_EDITOR
			#if USE_UNITY_WEBCAM
			if (camTexture.didUpdateThisFrame) {
				if (WaitForFirstFrame) {
					WaitForFirstFrame = false;
					colorData.colors = new Color32[camTexture.width * camTexture.height];
					camTexture.GetPixels32 (colorData.colors);
					Plugins.ULS_UnityTrackerUpdate (colorData.byteArray, camTexture.width, camTexture.height);
					if (Plugins.OnPreviewStart!=null)
						Plugins.OnPreviewStart(camTexture, camTexture.videoRotationAngle);
				} else {
					camTexture.GetPixels32 (colorData.colors);
					Plugins.ULS_UnityTrackerUpdate (colorData.byteArray, camTexture.width, camTexture.height);
					if (Plugins.OnPreviewUpdate!=null)
						Plugins.OnPreviewUpdate(camTexture);
				}
			}
			#else
			if (isRunning) {
				Plugins.ULS_UnityUpdateVideoCapture ();
			}
			#endif//USE_UNITY_WEBCAM
			#endif//UNITY_STANDALONE || UNITY_EDITOR
		}

		#endregion

		#region ---Camera Utility---

		public static WebCamTexture PrepareWebCamTexture (ref WebCamDevice device, bool frontFace, int width, int height)
		{
			WebCamTexture texture = null;
			// Checks how many and which cameras are available on the device
			for (int cameraIndex = 0; cameraIndex < WebCamTexture.devices.Length; cameraIndex++) {
				if (WebCamTexture.devices [cameraIndex].isFrontFacing == frontFace) {
					Debug.Log (cameraIndex + " name " + WebCamTexture.devices [cameraIndex].name
						+ " isFrontFacing " + WebCamTexture.devices [cameraIndex].isFrontFacing);

					device = WebCamTexture.devices [cameraIndex];
					texture = new WebCamTexture (device.name, width, height);
					break;
				}
			}
			// fallback
			if (texture == null) {
				device = WebCamTexture.devices[0];
				texture = new WebCamTexture (device.name, width, height);
			}
			Debug.Log ("width " + texture.width + " height " + texture.height + " fps " + texture.requestedFPS);
			return texture;
		}
		#endregion
	}
}
