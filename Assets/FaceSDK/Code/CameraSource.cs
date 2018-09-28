using System.Collections;
using System.Collections.Generic;
using UnityEngine;
namespace milook{
	/// <summary>
	/// Get pixels from camera device.
	/// </summary>
	public class CameraSource : BasicSource {
		private WebCamTexture webTex; 
		public int cameraID=1;
		public bool isPause;
		public Renderer videoRender;//display vidieo Renderer (optional)
		public int videoWidth=1280;
		public int videoHeight=720;
		public int videoFPS = 24;
		private string deviceName;
		WaitForSeconds updateTime;
		void Awake () {
			updateTime = new WaitForSeconds (1.0f/(float)videoFPS);
			StartCoroutine (CameraStart());
			StartCoroutine (OnUpdate());

			this.width = videoWidth;
			this.height = videoHeight;
		}

		/// <summary>
		/// Stop camera.
		/// </summary>
		public void Stop(){
			if (webTex != null)
				webTex.Stop ();
		}
		/// <summary>
		/// Pause.
		/// </summary>
		public void Pause(){
			if (webTex != null)
				webTex.Pause ();
		}
		/// <summary>
		/// Play.
		/// </summary>
		public void Play(){
			if (webTex != null)
				webTex.Play ();
		}
		IEnumerator OnUpdate () {
			while (true) {
				yield return updateTime;
				if (webTex == null || webTex.width<100)
					continue;
				if (!webTex.isPlaying && !isPause)
					continue;

				if (!isPause ) {
					pixels = webTex.GetPixels32 ();
				}

				target.UpdateWithSource (pixels);
			}
		}

		IEnumerator CameraStart()    
		{
			yield return Application.RequestUserAuthorization(UserAuthorization.WebCam);    
			if(Application.HasUserAuthorization(UserAuthorization.WebCam))    
			{    
				WebCamDevice[] devices = WebCamTexture.devices;   
				if (cameraID<devices.Length) {
					deviceName = devices [cameraID].name;    
				} else {
					deviceName = devices [0].name;    
				}
				webTex=new WebCamTexture(deviceName,videoWidth,videoHeight,videoFPS); 
				webTex.Play(); 
				if (videoRender != null) {
					videoRender.material.mainTexture = webTex;
				}
			}    
		}
	}
}
