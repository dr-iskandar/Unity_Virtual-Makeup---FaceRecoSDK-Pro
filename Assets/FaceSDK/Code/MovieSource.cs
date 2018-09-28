using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace milook{
	/// <summary>
	/// Get pixels from renderTexture.
	/// </summary>
	public class MovieSource : BasicSource {
		private Texture2D text2d;
		public RenderTexture movieRenderTexture;
		public MeshRenderer displayTextrue;
		void Awake () {
			text2d =  new Texture2D(movieRenderTexture.width, movieRenderTexture.height);
			displayTextrue.material.mainTexture = text2d;
			this.width = text2d.width;
			this.height = text2d.height;
		}
		void Update () {
			RenderTexture currentActiveRT = RenderTexture.active;
			RenderTexture.active = movieRenderTexture;
			text2d.ReadPixels(new Rect(0,0, movieRenderTexture.width, movieRenderTexture.height),0, 0);
			RenderTexture.active = currentActiveRT;
			text2d.Apply ();

			Color32[] p = text2d.GetPixels32 ();
			target.UpdateWithSource (p);
		}
		protected override int platformId {
			get {
				return 4;
			}
		}
	}
}
